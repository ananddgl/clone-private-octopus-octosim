#include <stdlib.h>
#include "../octosimlib/IModels.h"
#include "../octosimlib/SimulationLoop.h"
#include "../octosimlib/TcpSim.h"
#include "TestModels.h"
#include "TcpRetransmitTest.h"



TcpRetransmitTest::TcpRetransmitTest()
{
}


TcpRetransmitTest::~TcpRetransmitTest()
{
}

bool TcpRetransmitTest::TcpRetransmitDoTest()
{
    bool ret = true;
    TcpSimRetransmitQueue tsrq;
    TestMessage * message;
    TcpMessage * tm;
    TcpMessage ack (NULL);
    TcpMessage * retransmit;
    unsigned long long delta_t = 10000;
    unsigned long long rtt = 100000;
    unsigned long long first_sack[1] = { 5 };
    unsigned long long second_sack[5] = { 5, 7, 8, 11, 13 };
    unsigned long long third_sack[3] = { 8, 14, 14 };
    unsigned long long fourth_sack[1] = { 15 };

    unsigned long long first_batch_retransmit[3] = { 6, 9, 10 };
    unsigned long long second_batch_retransmit[3] = { 9, 10, 15 };

    /* Add a series of 15 messages to the queue.
     * Stagger the transmit times, so we can use that later. */
    for (int i = 1; i < 16; i++)
    {
        message = new TestMessage(i);
        tm = new TcpMessage(message);
        tsrq.AddNewMessage(tm);
        tm->transmit_time = i * delta_t;
    }

    /* Send a pure serial ACK for #5. Verify the new range */
    ret = AckAndCheck(&tsrq, 5 * delta_t, rtt, &ack, 1, first_sack);

    /* Send a SACK, with adequate holes: ack 5, sack 7-8, 11-13 */
    if (ret)
    {
        ret = AckAndCheck(&tsrq, 13 * delta_t, rtt, &ack, 5, second_sack);
    }
    /* Get the next retransmit messages. Should be 6, 9, 10 */
    if (ret)
    {
        ret = CheckRetransmit(&tsrq, 3, first_batch_retransmit, ack.ack_time + rtt);
    }

    /* Send an ACK for some of the retransmit messages. Ack 8, 14 */
    if (ret)
    {
        ret = AckAndCheck(&tsrq, ack.ack_time + rtt, rtt, &ack, 3, third_sack);
    }

    /* Get the retransmit messages. Should be 9, 10, 15 */
    if (ret)
    {
        ret = CheckRetransmit(&tsrq, 3, second_batch_retransmit, ack.ack_time + rtt);
    }

    /* Set ACK to 15. Queue should then be empty. */
    if (ret)
    {
        ret = AckAndCheck(&tsrq, ack.ack_time + rtt, rtt, &ack, 1, fourth_sack);
    }

    if (ret && !tsrq.IsEmpty())
    {
        ret = false;
    }

    return ret;
}

bool TcpRetransmitTest::CheckRetransmit(
    TcpSimRetransmitQueue * tsrq, 
    int nb_in_batch, unsigned long long * retransmit_batch,
    unsigned long long current_time)
{
    bool ret = true;
    int nb_retransmit = 0;
    TcpMessage * retransmit;

    while (ret && ((retransmit = tsrq->NextToRetransmit()) != NULL))
    {
        if (nb_retransmit > nb_in_batch)
        {
            ret = false;
        }
        else if (retransmit->sequence != retransmit_batch[nb_retransmit])
        {
            ret = false;
        }
        else
        {
            retransmit->transmit_time = current_time;
            nb_retransmit++;
        }
    }
    if (ret && nb_retransmit != nb_in_batch)
    {
        ret = false;
    }

    return ret;
}

bool TcpRetransmitTest::AckAndCheck(TcpSimRetransmitQueue * tsrq, unsigned long long ack_time, unsigned long long rtt, TcpMessage * ack, int nb_numbers, unsigned long long * ack_and_sack)
{
    bool ret = true;

    if ((nb_numbers & 1) == 0)
    {
        ret = false;
    }
    else
    {

        ack->ack_number = ack_and_sack[0];
        ack->nb_nack = (nb_numbers - 1) / 2;
        for (unsigned int i = 0; i < ack->nb_nack; i++)
        {
            ack->ack_range_first[i] = ack_and_sack[1 + 2 * i];
            ack->ack_range_last[i] = ack_and_sack[2 + 2 * i];
        }

        ack->ack_time = ack_time;
        ack->transmit_time = ack_time + rtt / 2;

        tsrq->ApplyAck(ack);

        if (tsrq->ack_received != ack->ack_number ||
            tsrq->last_transmit_time_acked != ack_time)
        {
            ret = false;
        }
    }

    return ret;
}
