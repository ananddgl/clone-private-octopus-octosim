#include <stdlib.h>
#include <algorithm>
#include "SimulationLoop.h"
#include "TcpSim.h"



TcpSim::TcpSim(SimulationLoop* loop)
    :
    retransmitQueue(NULL),
    retransmitQueueLast(NULL),
    reorderQueue(NULL),
    last_sequence_number_sent(0),
    ack_received(0),
    last_sequence_received(0),
    last_sequence_processed(0),
    tcp_idle_timeout(20000000ull),
    rtt(1000000ull),
    rtt_dev(0ull),
    last_transmit_or_receive(0),
    nb_packets_deleted(0),
    state(idle),
    ITransport(loop)
{
}


TcpSim::~TcpSim()
{
    /* empty the reorder queue */
    /* empty the retransmit queue */
}

/*
 * Note well: Application Input processing should not "call back" into
 * the transport. This could cause unpredictable behavior.
 */
void TcpSim::ApplicationInput(ISimMessage * message)
{
    /* Update the last interaction time */
    last_transmit_or_receive = GetLoop()->SimulationTime();
    /* Create a TCP Message based on the application */
    TcpMessage * tm = new TcpMessage(message);
    /* Derefence the message, since we will only handle the copy */
    if (message->Dereference())
    {
        /* This should never happen */
        delete message;
    }

    if (tm != NULL)
    {
        /* Assign a number, etc. */
        last_sequence_number_sent++;
        tm->sequence = last_sequence_number_sent;
        /* Add at the end of the queue */
        if (retransmitQueueLast == NULL)
        {
            retransmitQueue = tm;
            retransmitQueueLast = tm;
        }
        else
        {
            retransmitQueueLast->next_in_reorder = tm;
            retransmitQueueLast = tm;
        }
        /* If the connection is not active, send a SYN */
        if (state != connected)
        {
            if (state == idle)
            {
                SendControlMessage(syn);
                state = syn_sent;
            }
            else
            {
                /* wait for the next message that will create a state change */
            }
            /* start a timer to ensure we get connection progress */
            ResetTimer(rtt + 2 * rtt_dev);
        }
        else
        {
            /* We send a copy of the message, to avoid creating a side channel */
            SendCopyOfMessage(tm);
        }
    }
}

void TcpSim::Input(ISimMessage * message)
{
    TcpMessage * tm = dynamic_cast<TcpMessage *>(message);
    unsigned long long current_time = GetLoop()->SimulationTime();

    last_transmit_or_receive = current_time;

    if (tm != NULL)
    {
        RttUpdate(tm->transmit_time, current_time);

        if (tm->flags == syn)
        {
            /* TODO: If this is a SYN, respond with SYNACK. Transmit pending packets. */
            if (state != rst_sent)
            {
                state = connected;
                SendControlMessage(syn_ack);
            }
        }
        else if (tm->flags == syn_ack)
        {
            /* If this is a SYNACK, transmit pending packets. */
            if (state != idle && state != rst_sent)
            {
                if (state != connected)
                {
                    TcpMessage * next = retransmitQueue;

                    while (next != NULL)
                    {
                        SendCopyOfMessage(next);
                        next = next->next_in_transmit;
                    }

                }
                state = connected;
            }
        }
        else if (tm->flags == rst)
        {
            /* Reset. Respond with reset for sync. */
            if (state != rst_sent)
            {
                SendControlMessage(rst);
            }
            state = idle;
            /* Reset sequence number of packets in transmit queue? */
            /* Arm short timer to start sending SYN if packets are queued */
            ResetTimer(rtt / 16);
        }
        else
        {
            /* If this is data, insert in receive queue */
            if (tm->payload != NULL && tm->sequence > last_sequence_processed)
            {
                TcpMessage * next = reorderQueue;
                TcpMessage ** p_previous = &reorderQueue;

                while (next != NULL)
                {
                    if (next->sequence == tm->sequence)
                    {
                        /* already present*/
                        break;
                    }
                    else if (next->sequence > tm->sequence)
                    {
                        /* insert a copy here */
                        tm->next_in_reorder = next;
                        *p_previous = tm;
                        tm->Reference();
                        break;
                    }
                    else
                    {
                        /* progress */
                        p_previous = &next->next_in_reorder;
                        next = next->next_in_reorder;
                    }
                }

                if (next == NULL)
                {
                    /* packet is larger than last in queue, or queue is empty */
                    tm->next_in_reorder = *p_previous;
                    *p_previous = tm;
                    tm->Reference();
                }
            }

            /* If receive queue is ordered, pass the packets to the application */
            while (reorderQueue != NULL && reorderQueue->sequence == last_sequence_processed + 1)
            {
                TcpMessage * current = reorderQueue;
                reorderQueue = reorderQueue->next_in_reorder;
                current->next_in_reorder = NULL;

                GetApplication()->Input(current->payload);
                current->payload = NULL;
                last_sequence_processed = current->sequence;

                if (current->Dereference())
                {
                    delete current;
                }
            }

            if (tm->flags == ack)
            {
                TcpMessage * next; 

                /* If this contains ACK/NACK, dequeue ACKed */
                if (ack_received < tm->ack_number)
                {
                    ack_received = tm->ack_number;
                }

                while (retransmitQueue && retransmitQueue->sequence <= ack_received)
                {
                    TcpMessage * current = retransmitQueue;
                    retransmitQueue = retransmitQueue->next_in_transmit;
                    current->next_in_transmit = NULL;

                    if (current->Dereference())
                    {
                        delete current;
                    }
                }

                if (retransmitQueue == NULL)
                {
                    retransmitQueueLast = NULL;
                }

                /* Now, schedule the NAK values for retransmission, if they were not
                 * recently retransmitted */
                for (unsigned int i = 0; i < tm->nb_nack; i++)
                {
                    next = retransmitQueue;

                    while (next != NULL && next->sequence < tm->nack[i])
                    {
                        next = next->next_in_transmit;
                    }

                    if (next != NULL && next->sequence == tm->nack[i] &&
                        next->transmit_time + rtt >= current_time)
                    {
                        SendCopyOfMessage(next);
                    }
                }
            }  
        }
    }

    /* The message will be deleted if it was not copied in the reorder queue */
    if (message->Dereference())
    {
        delete message;
    }
}

void TcpSim::TimerExpired(unsigned long long simulationTime)
{
    if (state == connected)
    {
        /* If packets due for retransmission, do it */
        /* TODO: should not retransmit messages that were ACKED! */
        TcpMessage * next = retransmitQueue;
        while (next)
        {
            if (next->transmit_time + rtt + 2 * rtt_dev <= simulationTime)
            {
                SendCopyOfMessage(next);
            }
            else
            {
                next = next->next_in_transmit;
            }
        }

        /* If nothing in queues, check whether to hang the connection */
        if (retransmitQueue == NULL &&
            reorderQueue == NULL &&
            simulationTime > last_transmit_or_receive + tcp_idle_timeout)
        {
            /* Reset because waited too long */
            state = rst_sent;
            SendControlMessage(rst);
        }
    }
    else if (state == syn_sent)
    {
        if (simulationTime > last_transmit_or_receive + rtt + 2 * rtt_dev)
        {
            /* Resend the SYN packet */
            SendControlMessage(syn);
        }
    }
    else if (state == rst_sent)
    {
        /* Resend the RST packet */
        if (simulationTime > last_transmit_or_receive + rtt + 2 * rtt_dev)
        {
            /* Resend the SYN packet */
            SendControlMessage(rst);
        }
    }
    else if (state == idle)
    {
        while (reorderQueue != NULL)
        {
            /* clean up the reorder queue */
            TcpMessage * current = reorderQueue;
            reorderQueue = current->next_in_reorder;

            if (current->Dereference())
            {
                delete current;
            }
        }
        
        if (retransmitQueue != NULL)
        {
            /* Pick a sequence number of the SYN */
            /* Reset the sequence numbers */
            TcpMessage * next = retransmitQueue;
            unsigned long long newSeq = ++last_sequence_number_sent;
            while (next != NULL)
            {
                newSeq++;
                next->sequence = newSeq;
                next = next->next_in_transmit;
            }
            /* Send a SYN packet */
            SendControlMessage(syn);
        }
    }

}

void TcpSim::SendControlMessage(TcpMessageCode code)
{
    TcpMessage * ctrl = new TcpMessage(NULL);
    last_transmit_or_receive = GetLoop()->SimulationTime();
    if (ctrl != NULL)
    {
        ctrl->flags = code;
        GetPath()->Input(ctrl);
        ResetTimer(rtt + 2 * rtt_dev);
    }
}

void TcpSim::FillAckData(TcpMessage * tm)
{
    int nack_number = 0;
    unsigned long long expected_sequence = last_sequence_processed + 1;
    TcpMessage * next = reorderQueue;

    tm->ack_number = last_sequence_processed;

    while (next != NULL && nack_number < 16)
    {
        while (expected_sequence < next->sequence && nack_number < 16)
        {
            tm->nack[nack_number] = expected_sequence;
            expected_sequence++;
            nack_number++;
        }
        expected_sequence = next->sequence + 1;
        next = next->next_in_reorder;
    }
    tm->nb_nack = nack_number;
}

void TcpSim::SendCopyOfMessage(TcpMessage * tm)
{
    TcpMessage * temp = tm->Copy();

    tm->transmit_time = GetLoop()->SimulationTime();
    last_transmit_or_receive = tm->transmit_time;

    if (temp != NULL)
    {
        FillAckData(temp);
        temp->flags = ack;
        GetPath()->Input(temp);

        ResetTimer(rtt + 2 * rtt_dev);
    }
}

TcpMessage::TcpMessage(ISimMessage * payload)
    :
    next_in_transmit(NULL),
    next_in_reorder(NULL),
    payload(payload),
    flags(0),
    sequence(0),
    ack_number(0),
    nb_nack(0),
    transmit_time(0),
    ISimMessage()
{
    memset(nack, 0, sizeof(nack));
    if (payload != NULL)
    {
        payload->Reference();
    }
}

TcpMessage::~TcpMessage()
{
    if (payload != NULL && payload->Dereference())
    {
        delete payload;
    }
}

TcpMessage * TcpMessage::Copy()
{
    TcpMessage * tm = new TcpMessage(payload);
    if (tm != NULL)
    {
        tm->sequence = sequence;
        tm->transmit_time = transmit_time;
    }
    return tm;
}
