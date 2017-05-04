#include <stdlib.h>
#include <algorithm>
#include "DnsMessage.h"
#include "SimulationLoop.h"
#include "DnsUdpTransport.h"

/*
 * Simulate the "DNS transport with exponential backoff"
 * used over UDP.
 *
 * Upon sending a message, keep a copy.
 */

DnsUdpTransport::DnsUdpTransport(SimulationLoop* loop)
    :
    retransmitQueue(NULL),
    nb_packets_deleted(0),
    last_received_time(0),
    ITransport(loop)
{
}

DnsUdpTransport::~DnsUdpTransport()
{
    /* To do: clear the retransmit queue */
    while (retransmitQueue != NULL)
    {
        DnsMessage * dm = retransmitQueue;
        retransmitQueue = dm->next_in_queue;
        dm->next_in_queue = NULL;
        if (dm->Dereference())
        {
            delete dm;
        }
    }
}

/*
 * On application input:
 * - If the message is a query, add it to the queue
 */
void DnsUdpTransport::ApplicationInput(ISimMessage * message)
{
    DnsMessage * dm = dynamic_cast<DnsMessage*>(message);

    if (dm != NULL)
    {
        if (dm->messageCode == DnsMessageCode::query)
        {
            dm->Reference();
            dm->next_in_queue = retransmitQueue;
            retransmitQueue = dm;
            dm->current_udp_timer = rtt + 2*rtt_dev;
            ResetTimer(dm->current_udp_timer);
            dm->ack_time = last_received_time;
        }
        dm->transmit_time = GetLoop()->SimulationTime();

        GetPath()->Input(dm);
    }
    else
    {
        if (message->Dereference())
        {
            delete message;
        }
    }
}

void DnsUdpTransport::Input(ISimMessage * message)
{
    DnsMessage * dm = dynamic_cast<DnsMessage*>(message);

    if (dm != NULL)
    {
        last_received_time = std::max(last_received_time, dm->transmit_time);

        if (dm->messageCode != DnsMessageCode::query)
        {
            /* Find the message that this response acknowledges */

            DnsMessage * next = retransmitQueue;
            DnsMessage ** previous = &retransmitQueue;

            while (next != NULL)
            {
                if (next->query_id == dm->query_id)
                {
                    *previous = next->next_in_queue;
                    next->next_in_queue = NULL;
                    RttUpdate(dm->ack_time, GetLoop()->SimulationTime());
                    if (next->Dereference())
                    {
                        delete next;
                    }
                    break;
                }
                else
                {
                    previous = &next->next_in_queue;
                    next = next->next_in_queue;
                }
            }
        }
        /* Wheter query or response, pass to the application */
        GetApplication()->Input(dm);
    }
    else
    {
        if (message->Dereference())
        {
            delete message;
        }
    }
}

void DnsUdpTransport::TimerExpired(unsigned long long simulationTime)
{
    if (nb_timers_outstanding > 0)
        nb_timers_outstanding--;
    else
        nb_timers_outstanding = 0;


    if (GetLoop()->LogFile != NULL)
    {
        fprintf(GetLoop()->LogFile, "UDP(%u) Timer %llu, last time = %llu",
            object_number,
            simulationTime,
            last_received_time);
        if (!(retransmitQueue == NULL))
        {
            fprintf(GetLoop()->LogFile, ", Queue [%llu..",
                retransmitQueue->query_id);
        }
        fprintf(GetLoop()->LogFile, "\n");
    }

    if (simulationTime >= next_timer || nb_timers_outstanding <= 0)
    {
        /* Find all the pending messages larger than simulation time */
        DnsMessage * dm = retransmitQueue;
        DnsMessage ** previous = &retransmitQueue;
        unsigned long long min_timer = 1000000;

        while (dm != NULL)
        {
            DnsMessage * deleted = NULL;

            if (dm->transmit_time + dm->current_udp_timer <= simulationTime)
            {
                /* Timer has expired -- this means the message was dropped */
                if (dm->udp_repeat_counter < 4)
                {
                    /* retransmit */
                    dm->transmit_time = simulationTime;
                    dm->udp_repeat_counter++;
                    dm->current_udp_timer *= 3;
                    min_timer = std::min(min_timer, dm->current_udp_timer);
                    dm->Reference();
                    GetPath()->Input(dm);
                }
                else
                {
                    /* too many repeats, give up */
                    deleted = dm;
                }
            }
            else
            {
                min_timer = std::min(min_timer, dm->transmit_time + dm->current_udp_timer - simulationTime);
            }

            if (!deleted)
            {
                previous = &dm->next_in_queue;
            }

            dm = dm->next_in_queue;

            if (deleted)
            {
                nb_packets_deleted++;
                deleted->next_in_queue = NULL;
                *previous = dm;
                if (deleted->Dereference())
                {
                    delete deleted;
                }
                else
                {
                    deleted->messageCode = DnsMessageCode::error_failed;
                    GetApplication()->Input(deleted);
                }
            }
        }

        if (retransmitQueue != NULL)
        {
            SetTimer(min_timer);
        }
    }
}