#include "DnsMessage.h"
#include "SimulationRandom.h"
#include "SimulationLoop.h"
#include "DnsStub.h"



DnsStub::DnsStub(SimulationLoop * loop, FILE* FStats,
    int nb_packets_to_send, IDelayDistribution * source_process)
    :
    FStats(FStats),
    nb_packets_to_send(nb_packets_to_send),
    source_process(source_process),
    nb_packets_sent(0),
    nb_transactions_complete(0),
    IApplication(loop)
{
}


DnsStub::~DnsStub()
{
    if (source_process)
    {
        delete source_process;
    }
}

/*
 * Input from the network.
 * This means a query was served. Time to put out statistics.
 */
void DnsStub::Input(ISimMessage * message)
{
    DnsMessage * dm = dynamic_cast<DnsMessage*>(message);
    /* If this is a response, write a summary line */
    if (dm != NULL && dm->messageCode != DnsMessageCode::query)
    {
        if (dm->messageCode == DnsMessageCode::response)
        {
            nb_transactions_complete++;
        }
        if (FStats != NULL)
        {
            unsigned long long arrival = GetLoop()->SimulationTime();
            fprintf(FStats,
                """%llu"",""%llu"",""%llu"",""%s"",""%llu"",""%llu"",""%d""\n",
                arrival,
                dm->creation_time,
                arrival - dm->creation_time,
                dm->CodeToText(),
                dm->query_id,
                dm->qtarget_id,
                dm->udp_repeat_counter
            );
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
    DnsMessage * dm = new DnsMessage(
        GetLoop()->SimulationTime(),
        GetLoop()->Rnd()->GetRandom64(),
        GetLoop()->Rnd()->GetRandom64());

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
        }
    }
}
