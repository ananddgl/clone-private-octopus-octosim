#include <stdlib.h>
#include "SimulationLoop.h"
#include "DnsMessage.h"
#include "DnsRecursive.h"

DnsRecursive::DnsRecursive(SimulationLoop * loop, 
    IDelayDistribution * authoritative_model)
    :
    authoritative(loop, authoritative_model, this),
    IApplication(loop)
{
}

DnsRecursive::~DnsRecursive()
{
}

void DnsRecursive::Input(ISimMessage * message)
{
    DnsMessage * dm = dynamic_cast<DnsMessage*>(message);

    /* Check whether this is a response or a query.
     * Only process queries
     */
    if (dm != NULL && dm->messageCode == DnsMessageCode::query)
    {
        /* Create a response from the query */ 
        DnsMessage * r = dm->CreateResponse();
        if (r != NULL)
        {
            /* If the query is in the cache, respond directly. */
            if (simulatedCache.Retrieve(r->qtarget_id))
            {
                GetTransport()->ApplicationInput(r);
            }
            else
            {
                /* If not in cache, send to authoritative. */
                authoritative.RecursiveInput(r);
            }
        }
    }

    /* End of the line for this message */
    
    if (message->Dereference())
    {
        delete message;
    }
}

void DnsRecursive::AuthoritativeInput(ISimMessage * message)
{
    DnsMessage * dm = dynamic_cast<DnsMessage*>(message);
    /* Add the results to the cache. */
    if (dm != NULL) 
    {
        (void)simulatedCache.Insert(dm->qtarget_id);
        /* These are new results, send the response. */
        GetTransport()->ApplicationInput(dm);
    }
    else
    {
        /* Not expected */
        if (message->Dereference())
        {
            delete message;
        }
    }
}
