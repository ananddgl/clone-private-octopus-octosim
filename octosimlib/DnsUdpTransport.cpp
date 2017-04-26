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
            dm->current_udp_timer = rtt;
            dm->transmit_time = GetLoop()->SimulationTime();
            ResetTimer(dm->current_udp_timer);
        }

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
                    if (next->Dereference())
                    {
                        delete next;
                    }
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
    if (simulationTime >= nextTimer)
    {
        /* Find all the pending messages larger than simulation time */
        DnsMessage * dm = retransmitQueue;
        DnsMessage ** previous = &retransmitQueue;
        unsigned long long min_timer = 10000000;

        while (dm != NULL)
        {
            DnsMessage * deleted = NULL;

            if (dm->transmit_time + dm->current_udp_timer <= simulationTime)
            {
                /* Timer has expired -- this means the message was deleted */
                if (dm->udp_repeat_counter < 4)
                {
                    /* retransmit */
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

            if (!deleted)
            {
                previous = &dm->next_in_queue;
            }

            dm = dm->next_in_queue;

            if (deleted)
            {
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

        ResetTimer(min_timer);
    }
}

void DnsUdpTransport::ResetTimer(unsigned long long delay)
{
    /* Note that we do not bother deleting the old timers. */
    if ((GetLoop()->SimulationTime() + delay) < nextTimer)
    {
        nextTimer = GetLoop()->SimulationTime() + delay;
        GetLoop()->RequestTimer(delay, this);
    }
}

