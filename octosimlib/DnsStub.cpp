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
    /* If this is a response, write a summary line */
    if (dm != NULL && dm->messageCode != DnsMessageCode::query )
    {
        if (completedCache.Retrieve(dm->qtarget_id))
        {
            nb_duplicate_transactions++;
        }
        else
        {
            completedCache.Insert(dm->qtarget_id);
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
