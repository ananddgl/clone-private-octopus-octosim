#include <stdlib.h>
#include <algorithm>
#include "SimulationLoop.h"
#include "TcpSim.h"

TcpSim::TcpSim(SimulationLoop* loop, bool quic_mode, bool zero_rtt)
    :
    quic_mode(quic_mode),
    zero_rtt(zero_rtt),
    tcp_idle_timeout((zero_rtt)?20000000000ull:20000000ull),
    last_transmit_time(0),
    last_remote_transmit_time(0),
    last_received_time(0),
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
        retransmitQueue.AddNewMessage(tm);

        /* If the connection is not active, send a SYN */
        if (state != connected)
        {
            if (state == idle)
            {
                state = syn_sent;
                SendControlMessage(syn);
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

    if (tm != NULL)
    {
        last_remote_transmit_time = tm->transmit_time;
        last_received_time = current_time;
        if (tm->flags == syn)
        {
            /* TODO: If this is a SYN, respond with SYNACK. Transmit pending packets. */
            if (state == syn_sent || state == idle)
            {
                /* Send the syn ack, move to connected state, send pending data */
                TcpMessage * next;
                state = connected;
                SendControlMessage(syn_ack);

                while ((next = retransmitQueue.NextToRetransmit(true)) != NULL)
                {
                    SendCopyOfMessage(next);
                }
            }
            else if (state != rst_sent)
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
                    RttUpdate(tm->ack_time + tm->time_since_last_received, current_time);

                    TcpMessage * next;

                    while (next = retransmitQueue.NextToRetransmit(true))
                    {
                        SendCopyOfMessage(next);
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
        }
        else
        {
            /* If this is data, insert in receive queue */
            RttUpdate(tm->ack_time + tm->time_since_last_received, current_time);

            if (reorderQueue.Insert(tm)) {
                ISimMessage * ism;

                if (quic_mode)
                {
                    ISimMessage * ism = tm->payload;
                    tm->payload->Reference();
                    GetApplication()->Input(ism);
                }
                
                while ((ism = reorderQueue.DequeueInOrder()) != NULL)
                {
                    if (!quic_mode)
                        GetApplication()->Input(ism);
                }
            }

            if (tm->flags == ack)
            {
                TcpMessage * next;

                retransmitQueue.ApplyAck(tm);
                /* Now, schedule the NAK values for retransmission, if they were not
                 * recently retransmitted */
                while ((next = retransmitQueue.NextToRetransmit(false)) != NULL)
                {
                    SendCopyOfMessage(next);
                }
            }

            /* TODO: if ACK is needed, send it! */
            if (reorderQueue.IsAckNeeded(last_received_time))
            {
                SendControlMessage(ack);
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
    if (nb_timers_outstanding > 0)
    {
        nb_timers_outstanding--;
    }

    if (GetLoop()->LogFile != NULL)
    {
        fprintf(GetLoop()->LogFile, "TCP(%u) Timer %llu, state=%d, last =%llu", 
            object_number,
            simulationTime, state, last_received_time);
        if (!retransmitQueue.IsEmpty())
        {
            fprintf(GetLoop()->LogFile, ", Queue ]%llu..%llu]", 
                retransmitQueue.ack_received, retransmitQueue.last_sequence_number_sent);
        }
        fprintf(GetLoop()->LogFile, "\n");
    }

    if (state == connected)
    {
        /* If packets due for retransmission, do it */
        if (simulationTime > rtt + 2 * rtt_dev && !retransmitQueue.IsEmpty())
        {
            TcpMessage * next;

            unsigned long long last_time = simulationTime - rtt - 2 * rtt_dev;

            if (retransmitQueue.ApplyTimer(last_time))
            {
                /* Retransmit the first message in the queue */
                if ((next = retransmitQueue.NextToRetransmit(false)) != NULL)
                {
                    SendCopyOfMessage(next);
                }
            }
        }
        else if (reorderQueue.IsAckNeeded(last_received_time))
        {
            SendControlMessage(ack);
        }
        else 
        /* If nothing in queues, check whether to hang the connection */
        if (retransmitQueue.IsEmpty() &&
            reorderQueue.IsEmpty() &&
            simulationTime >= last_transmit_time + tcp_idle_timeout &&
            simulationTime >= last_received_time + tcp_idle_timeout)
        {
            /* Reset because waited too long */
            state = rst_sent;
            SendControlMessage(rst);
        }

        if (!retransmitQueue.IsEmpty())
        {
            SetTimer(rtt + 2 * rtt_dev);
        }
    }
    else if (state == syn_sent)
    {
        if (simulationTime >= last_transmit_time + rtt + 2 * rtt_dev)
        {
            /* Resend the SYN packet */
            SendControlMessage(syn);
        }
    }
    else if (state == rst_sent)
    {
        /* Resend the RST packet */
        if (simulationTime >= last_transmit_time + rtt + 2 * rtt_dev)
        {
            /* Resend the SYN packet */
            SendControlMessage(rst);
        }
    }
    else if (state == idle)
    {
        reorderQueue.Clean();

        if (!retransmitQueue.IsEmpty())
        {
            retransmitQueue.ResetBeforeSyn();
            state = syn_sent;
            /* Send a SYN packet */
            SendControlMessage(syn);
        }
    }

    /* Todo: when do we need a different timer? */
    if (nb_timers_outstanding <= 0 && state != idle)
    {   
        unsigned long long delay = tcp_idle_timeout;
        if (state == syn_sent || state == rst_sent)
        {
            delay = rtt + 2 * rtt_dev;
        }
        SetTimer(delay);
    }
}

void TcpSim::SendControlMessage(TcpMessageCode code)
{
    TcpMessage * ctrl = new TcpMessage(NULL);
    unsigned long long current_time = GetLoop()->SimulationTime();
    last_transmit_time = current_time;
    if (ctrl != NULL)
    {
        ctrl->flags = code;
        ctrl->transmit_time = current_time;
        ctrl->ack_time = last_remote_transmit_time;
        ctrl->time_since_last_received = current_time - last_received_time;
        if (code == ack)
        {
            reorderQueue.FillAckData(ctrl, ctrl->transmit_time);
        }

        GetPath()->Input(ctrl);
        if (state != idle || code == syn)
        {
            SetTimer(rtt + 2 * rtt_dev);
        }

        if (GetLoop()->LogFile != NULL && code == syn )
        {
            fprintf(GetLoop()->LogFile,
                "SYN(%d) rtt: %llu, rtt_dev: %llu, T: %llu\n",
                object_number, rtt, rtt_dev, current_time);
        }
    }
}

void TcpSim::SendCopyOfMessage(TcpMessage * tm)
{
    TcpMessage * temp = tm->Copy();
    unsigned long long current_time = GetLoop()->SimulationTime();
    tm->transmit_time = current_time;
    last_transmit_time = current_time;

    if (temp != NULL)
    {
        reorderQueue.FillAckData(temp, current_time);
        temp->flags = ack;
        temp->transmit_time = current_time;
        temp->ack_time = last_remote_transmit_time;
        temp->time_since_last_received = current_time - last_received_time;
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
    ack_time(0),
    time_since_last_received(0),
    ISimMessage()
{
    memset(ack_range_first, 0, sizeof(ack_range_first));
    memset(ack_range_last, 0, sizeof(ack_range_last));
    length = 60;
    if (payload != NULL)
    {
        payload->Reference();
        length += payload->length;
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
        tm->ack_time = ack_time;
    }
    return tm;
}

void TcpMessage::Log(FILE * LogFile, bool dropped)
{
    if(LogFile != NULL)
    {
        fprintf(LogFile,
            "%sTCP f:%d (%s), s:%llu, tt:%llu, at:%llu, tsl:%llu",
            (dropped)?"Dropped ":"",
            flags, CodeToText(), sequence, transmit_time, ack_time, time_since_last_received);
        if (flags == ack)
        {
            fprintf(LogFile, ", a=%llu (", ack_number);
            for (unsigned int i = 0; i < nb_nack; i++)
            {
                if (i != 0)
                {
                    fprintf(LogFile, ", ");
                }
                fprintf(LogFile, "%llu-%llu", ack_range_first[i], ack_range_last[i]);
            }
            fprintf(LogFile, ") ");
        }

        if (payload == NULL)
        {
            fprintf(LogFile, "\n");
        }
        else
        {
            payload->Log(LogFile, false);
        }
    }
}

const char * TcpMessage::CodeToText()
{
    char * x = "unknown";

    switch (flags)
    {
    case syn:
        x = "syn";
        break;
    case syn_ack:
        x = "syn_ack";
        break;
    case ack:
        x = "ack";
        break;
    case rst:
        x = "rst";
        break;
    default:
        break;
    }
    return x;
}

TcpSimReorderQueue::TcpSimReorderQueue()
    :
    reorderQueue(NULL),
    last_sequence_received(0),
    last_sequence_processed(0),
    last_ack_sent(0),
    time_last_ack_sent(0),
    dup_received(false)
{
}

TcpSimReorderQueue::~TcpSimReorderQueue()
{
    Clean();
}

bool TcpSimReorderQueue::Insert(TcpMessage * tm)
{
    bool ret = (tm->payload != NULL && tm->sequence > last_sequence_processed);

    if (ret)
    {
        TcpMessage * next = reorderQueue;
        TcpMessage ** p_previous = &reorderQueue;

        while (next != NULL)
        {
            if (next->sequence == tm->sequence)
            {
                /* already present*/
                ret = false;
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

    if (!ret && tm->payload != NULL)
    {
        dup_received = true;
    }

    return ret;
}

ISimMessage * TcpSimReorderQueue::DequeueInOrder()
{
    ISimMessage * ret = NULL;

    /* If receive queue is ordered, pass the packets to the application */
    if (reorderQueue != NULL && reorderQueue->sequence == last_sequence_processed + 1)
    {
        TcpMessage * current = reorderQueue;
        reorderQueue = reorderQueue->next_in_reorder;
        current->next_in_reorder = NULL;

        ret = current->payload;
        current->payload = NULL;
        last_sequence_processed = current->sequence;

        if (current->Dereference())
        {
            delete current;
        }
    }

    return ret;
}

bool TcpSimReorderQueue::IsAckNeeded(unsigned long long last_received_time)
{
    return (last_ack_sent < last_sequence_processed || dup_received ||
        (reorderQueue != NULL && time_last_ack_sent < last_received_time));
}

void TcpSimReorderQueue::FillAckData(TcpMessage * tm, unsigned long long current_time)
{
    int nack_number = 0;
    unsigned long long end_of_range;
    TcpMessage * next = reorderQueue;

    tm->ack_number = last_sequence_processed;
    last_ack_sent = last_sequence_processed;
    time_last_ack_sent = current_time;
    dup_received = false;

    while (next != NULL && nack_number < 16)
    {
        tm->ack_range_first[nack_number] = next->sequence;
        end_of_range = next->sequence;
        while ((next = next->next_in_reorder) != NULL)
        {
            if (next->sequence == end_of_range + 1)
            {
                end_of_range++;
            }
            else
            {
                break;
            }
        }
        tm->ack_range_last[nack_number] = end_of_range;
        nack_number++;
    }
    tm->nb_nack = nack_number;
}

void TcpSimReorderQueue::Clean()
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
}

TcpSimRetransmitQueue::TcpSimRetransmitQueue()
    : 
    retransmitQueue(NULL),
    retransmitQueueLast(NULL),
    last_sequence_number_sent(0),
    last_transmit_time_acked(0),
    furthest_ack_range(0),
    ack_received(0)
{
}

TcpSimRetransmitQueue::~TcpSimRetransmitQueue()
{
}

void TcpSimRetransmitQueue::AddNewMessage(TcpMessage * tm)
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
        retransmitQueueLast->next_in_transmit = tm;
        retransmitQueueLast = tm;
    }
}

/*
 * RACK based retransmission. Packets should be retransmitted if they are
 * not acknowledged and if packets sent after them have been already
 * received.
 */

TcpMessage * TcpSimRetransmitQueue::NextToRetransmit(bool force_retransmit)
{
    TcpMessage * next = retransmitQueue;

    while (next != NULL)
    {
        if (next->transmit_time < last_transmit_time_acked ||
            (next->transmit_time == last_transmit_time_acked &&
            (force_retransmit || next->sequence < furthest_ack_range)))
        {
            break;
        }
        else
        {
            next = next->next_in_transmit;
        }
    }

    return next;
}

/*
 * Upon receiving acks, remove all acknowledged packets from the retransmit queue.
 * Update the ack_received and furthest_ack_range variables.
 */

void TcpSimRetransmitQueue::ApplyAck(TcpMessage * tm)
{
    TcpMessage * next;
    TcpMessage * previous;

    /* Update the last time seen */
    last_transmit_time_acked = std::max(last_transmit_time_acked, tm->ack_time);

    /* If this contains ACK/NACK, dequeue ACKed */
    if (ack_received < tm->ack_number)
    {
        ack_received = tm->ack_number;
        furthest_ack_range = std::max(furthest_ack_range, ack_received);
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

    /* Apply the ack ranges, and remove acked messages from the queue */
    next = retransmitQueue;
    TcpMessage ** p_previous = &retransmitQueue;
    previous = NULL;

    for (unsigned int i = 0; next != NULL && i < tm->nb_nack; i++)
    {
        while (next != NULL && next->sequence < tm->ack_range_first[i])
        {
            previous = next;
            p_previous = &next->next_in_transmit;
            next = next->next_in_transmit;
        }

        while (next != NULL && next->sequence <= tm->ack_range_last[i])
        {
            TcpMessage * current = next;
            *p_previous = next->next_in_transmit;
            next = next->next_in_transmit;

            if (current->Dereference())
            {
                delete current;
            }
        }

        furthest_ack_range = std::max(furthest_ack_range, tm->ack_range_last[i]);
    }

    if (next == NULL)
    {
        retransmitQueueLast = previous;
    }
}

/*
 * Timer logic. If there was nothing received since that date,
 * return true. The next call to the retransmit logic will retransmit
 * the necessary packets.
 */

bool TcpSimRetransmitQueue::ApplyTimer(unsigned long long lastTransmitTime)
{
    bool ret = false;

    if (lastTransmitTime > last_transmit_time_acked)
    {
        ret = true;
        last_transmit_time_acked = lastTransmitTime;
    }
    return ret;
}

void TcpSimRetransmitQueue::ResetBeforeSyn()
{ 
    /* Reset the sequence numbers so the packets are sent in the new connection. */
    TcpMessage * next = retransmitQueue;
    unsigned long long newSeq = 0;
    while (next != NULL)
    {
        newSeq++;
        next->sequence = newSeq;
        next = next->next_in_transmit;
    }
}
