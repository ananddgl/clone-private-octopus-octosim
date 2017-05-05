#include <stdlib.h>
#include "SimulationLoop.h"
#include "DnsMessage.h"
#include "DnsRecursive.h"
#include "DnsAuthoritative.h"

DnsAuthoritative::DnsAuthoritative(SimulationLoop * loop,
    IDelayDistribution * authoritative_model,
    ILengthDistribution * length_model,
    DnsRecursive * recursive)
    :
    authoritative_model(authoritative_model),
    length_model(length_model),
    recursive(recursive),
    ISimObject(loop)
{
}

DnsAuthoritative::~DnsAuthoritative()
{
}

void DnsAuthoritative::RecursiveInput(ISimMessage * message)
{
    /* Compute the delay */
    unsigned long long current_time = GetLoop()->SimulationTime();
    unsigned long long retrievedValue = 0;
    unsigned long long delay = 0;
    if (length_model)
    {
        message->length = length_model->NextLength();
    }
    DnsMessage * dm = dynamic_cast<DnsMessage*>(message);

    if (dm != NULL)
    {
        /* Check whether this is in cache */
        if (dm != NULL && simulatedCache.Retrieve(dm->qtarget_id, &retrievedValue))
        {
            /* Target is already in cache */
            if (retrievedValue < current_time)
            {
                delay = 0;
            }
            else
            {
                delay = retrievedValue - current_time;
            }
        }
        else
        {
            delay = authoritative_model->NextDelay();
            (void)simulatedCache.Insert(dm->qtarget_id, current_time + delay);
        }
        dm->recursive_time = current_time;
        dm->authoritative_delay = delay;
        /* Log if useful */
        if (dm != NULL && GetLoop()->LogFile != NULL)
        {
            fprintf(GetLoop()->LogFile, "Authoritative(%d) - %s to %llu, delay = %llu, length = %u\n",
                object_number,
                dm->CodeToText(),
                dm->qtarget_id,
                delay,
                message->length);
        }
    }
    /* Delayed input */
    GetLoop()->SubmitMessage(delay, this, message);
}

void DnsAuthoritative::Input(ISimMessage * message)
{
    /* Submit the input directly to the
     * of the recursive resolver. */
    recursive->AuthoritativeInput(message);
}
