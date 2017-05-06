/*
* Copyright (c) 2017, Private Octopus, Inc.
* All rights reserved.
*
* Permission to use, copy, modify, and distribute this software for any
* purpose with or without fee is hereby granted, provided that the above
* copyright notice and this permission notice appear in all copies.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Private Octopus, Inc. BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "DnsMessage.h"
#include "SimulationRandom.h"
#include "SimulationLoop.h"
#include "DnsStub.h"



DnsStub::DnsStub(SimulationLoop * loop, FILE* FStats,
    int nb_packets_to_send, IDelayDistribution * source_process,
    ILengthDistribution * source_length_process)
    :
    FStats(FStats),
    nb_packets_to_send(nb_packets_to_send),
    source_process(source_process),
    source_length_process(source_length_process),
    nb_packets_sent(0),
    nb_transactions_complete(0),
    nb_duplicate_transactions(0),
    next_target_id(0),
    IApplication(loop)
{
}


DnsStub::~DnsStub()
{
}

/*
 * Input from the network.
 * This means a query was served. Time to put out statistics.
 */
void DnsStub::Input(ISimMessage * message)
{
    DnsMessage * dm = dynamic_cast<DnsMessage*>(message);
    unsigned long long retrieved_value;

    /* If this is a response, write a summary line */
    if (dm != NULL && dm->messageCode != DnsMessageCode::query )
    {
        if (completedCache.Retrieve(dm->qtarget_id, &retrieved_value))
        {
            nb_duplicate_transactions++;
        }
        else
        {
            completedCache.Insert(dm->qtarget_id, dm->qtarget_id);
            if (dm->messageCode == DnsMessageCode::response)
            {
                nb_transactions_complete++;
            }
            if (FStats != NULL)
            {
                unsigned long long arrival = GetLoop()->SimulationTime();
                fprintf(FStats,
                    """%llu"",""%llu"",""%llu"",""%llu"",""%llu"",""%llu"",""%s"",""%llu"",""%llu"",""%u"",""%u"",""%d""\n",
                    arrival,
                    dm->creation_time,
                    arrival - dm->creation_time,
                    dm->recursive_time - dm->creation_time,
                    dm->authoritative_delay,
                    arrival - dm->creation_time - dm->authoritative_delay,
                    dm->CodeToText(),
                    dm->query_id,
                    dm->qtarget_id,
                    dm->query_length,
                    dm->length,
                    dm->udp_repeat_counter
                );
            }
        }
    }
    if (message->Dereference())
    {
        delete message;
    }
}

/*
 * Timer expired.
 * This means it is time to send the next query.
 */
void DnsStub::TimerExpired(unsigned long long simulationTime)
{
    /* Create a transaction */
    next_target_id++;

    DnsMessage * dm = new DnsMessage(
        GetLoop()->SimulationTime(),
        GetLoop()->Rnd()->GetRandom64(),
        next_target_id); // GetLoop()->Rnd()->GetRandom64());

    if (dm != NULL)
    {
        /* Submit to the selected transport */
        GetTransport()->ApplicationInput(dm);

        /* increment the counters */
        nb_packets_sent++;

        if (nb_packets_sent < nb_packets_to_send)
        {
            /* Get a new timer for the new packet. */
            GetLoop()->RequestTimer(source_process->NextDelay(), this);
            /* Set the length according to the distribution */
            if (source_length_process != NULL)
            {
                dm->length = source_length_process->NextLength();
            }
        }
    }
}
