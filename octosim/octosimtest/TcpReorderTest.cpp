#include <stdlib.h>
#include "../octosimlib/IModels.h"
#include "../octosimlib/SimulationLoop.h"
#include "../octosimlib/TcpSim.h"
#include "TestModels.h"
#include "TcpReorderTest.h"



TcpReorderTest::TcpReorderTest()
{
}


TcpReorderTest::~TcpReorderTest()
{
}

bool TcpReorderTest::TcpReorderDoTest()
{
    bool ret = true;
    TcpSimReorderQueue tsrq;
    TestMessage * testMessage[128];
    bool testMessageReceived[128];
    bool testMessageDelivered[128];
    TcpMessage * sendTest = new TcpMessage(NULL);

    /* Create a series of ordered messages -- we do not use index = 0 */
    for (int i = 1; i < 128; i++)
    {
        testMessageReceived[i] = false;
        testMessageDelivered[i] = false;
        testMessage[i] = new TestMessage(i);
        if (testMessage[i] == NULL)
        {
            ret = false;
        }
    }

    if (sendTest == NULL)
    {
        ret = false;
    }

    /* Simulate reception of all even messages */
    for (int i = 2; ret && i < 128; i += 2)
    {
        TcpMessage * tm = new TcpMessage(testMessage[i]);
        if (tm == NULL)
        {
            ret = false;
        }
        else
        {
            testMessageReceived[i] = true;
            tm->sequence = i;
            if (!tsrq.Insert(tm))
            {
                delete tm;
                ret = false;
            }
            else
            {
                ret = CheckDelivery(128, &tsrq, testMessage, testMessageReceived, testMessageDelivered);

                if (ret)
                {
                    ret = CheckAcks(128, &tsrq, sendTest, testMessageReceived);
                }
            }
        }
    }

    /* Simulate duplication of some even messages */
    for (int i = 2; ret && i < 128; i += 62)
    {
        TcpMessage * tm = new TcpMessage(testMessage[i]);
        if (tm == NULL)
        {
            ret = false;
        }
        else
        {
            tm->sequence = i;
            if (tsrq.Insert(tm))
            {
                ret = false;
            }
            else
            {
                delete tm;
            }
        }
    }

    /* Simulate reception of half the odd messages */
    for (int i = 1; ret && i < 128; i += 4) 
    {
        if (!testMessageReceived[i])
        {
            TcpMessage * tm = new TcpMessage(testMessage[i]);
            if (tm == NULL)
            {
                ret = false;
            }
            else
            {
                testMessageReceived[i] = true;
                tm->sequence = i;
                if (!tsrq.Insert(tm))
                {
                    delete tm;
                    ret = false;
                }
                else
                {
                    ret = CheckDelivery(128, &tsrq, testMessage, testMessageReceived, testMessageDelivered);
                    if (ret)
                    {
                        ret = CheckAcks(128, &tsrq, sendTest, testMessageReceived);
                    }
                }
            }
        }
    }

    /* Simulate reception of some the odd messages left */
    for (int i = 3; ret && i < 128; i += 8)
    {
        if (!testMessageReceived[i])
        {
            TcpMessage * tm = new TcpMessage(testMessage[i]);
            if (tm == NULL)
            {
                ret = false;
            }
            else
            {
                testMessageReceived[i] = true;
                tm->sequence = i;
                if (!tsrq.Insert(tm))
                {
                    delete tm;
                    ret = false;
                }
                else
                {
                    ret = CheckDelivery(128, &tsrq, testMessage, testMessageReceived, testMessageDelivered);

                    if (ret)
                    {
                        ret = CheckAcks(128, &tsrq, sendTest, testMessageReceived);
                    }
                }
            }
        }
    }

    /* Simulate reception of all the missing messages */
    for (int i = 127; ret && i >= 1; i--)
    {
        if (!testMessageReceived[i])
        {
            TcpMessage * tm = new TcpMessage(testMessage[i]);
            if (tm == NULL)
            {
                ret = false;
            }
            else
            {
                testMessageReceived[i] = true;
                tm->sequence = i;
                if (!tsrq.Insert(tm))
                {
                    delete tm;
                    ret = false;
                }
                else
                {
                    ret = CheckDelivery(128, &tsrq, testMessage, testMessageReceived, testMessageDelivered);

                    if (ret)
                    {
                        ret = CheckAcks(128, &tsrq, sendTest, testMessageReceived);
                    }
                }
            }
        }
    }
    /* At this point, all messages should have been delivered */
    if (ret && tsrq.last_sequence_processed != 127)
    {
        ret = false;
    }

    /* Clean the queue */
    tsrq.Clean();

    /* Delete the left over messages */
    for (int i = 1; i < 128; i++)
    {
        if (testMessage[i] != NULL)
        {
            delete testMessage[i];
            testMessage[i] = NULL;
        }
    }

    return ret;
}

bool TcpReorderTest::CheckDelivery(
    int range_last,
    TcpSimReorderQueue * tsrq,
    TestMessage ** testMessage,
    bool *testMessageReceived,
    bool * testMessageDelivered)
{
    bool ret = true;
    int expected_next;
    ISimMessage * sm;

    /* Process all messages received in order */
    while (ret && (sm = tsrq->DequeueInOrder()) != NULL)
    {
        TestMessage * tm = dynamic_cast<TestMessage *>(sm);

        if (tm == NULL || tm->message_number >= range_last || !testMessageReceived[tm->message_number])
        {
            /* Message not expected */
            ret = false;
        }
        else if (testMessageDelivered[tm->message_number])
        {
            /* Message delivered twice */
            ret = false;
        }
        else
        {
            testMessageDelivered[tm->message_number] = true;
            testMessage[tm->message_number] = NULL;
        }

        delete sm;
    }

    /* Check the current delivery status */
    for (int i = 1; ret && i <= tsrq->last_sequence_processed; i++)
    {
        ret = testMessageReceived[i];
    }
    /* Check that the next in line is missing */
    expected_next = (int)(tsrq->last_sequence_processed + 1);
    ret &= (expected_next >= range_last || !testMessageReceived[expected_next]);

    return ret;
}

bool TcpReorderTest::CheckAcks(int range_last, 
    TcpSimReorderQueue * tsrq, TcpMessage * tm, bool * testMessageReceived)
{
    bool ret = true;
    unsigned long long max_nack = tsrq->last_sequence_processed + 1;

    tsrq->FillAckData(tm, tm->transmit_time);

    if (tm->ack_number != tsrq->last_sequence_processed)
    {
        ret = false;
    }

    if (ret && tm->nb_nack > 16 )
    {
        ret = false;
    }

    for (unsigned int i = 0; ret && i < tm->nb_nack; i++)
    {
        unsigned long long acked_first = tm->ack_range_first[i];
        unsigned long long acked_last = tm->ack_range_last[i];

        if (acked_first < max_nack || acked_first >= range_last || 
            acked_last < acked_first || acked_last >= range_last)
        {
            ret = false;
        }
        else 
        {
            while (ret && max_nack < acked_first)
            {
                if (testMessageReceived[max_nack])
                {
                    ret = false;
                }
                else
                {
                    max_nack++;
                }
            }

            while (ret && max_nack <= acked_last)
            {
                if (!testMessageReceived[max_nack])
                {
                    ret = false;
                }
                else
                {
                    max_nack++;
                }
            }
        }
    }

    if (ret && tm->nb_nack < 16 && max_nack < range_last && !testMessageReceived[max_nack])
    {
        /* if there is another hole, there should be another nack */
        for (unsigned long long i = max_nack; ret && i < range_last; i++)
        {
            if (testMessageReceived[i])
            {
                ret = false;
            }
        }
    }

    return ret;
}
