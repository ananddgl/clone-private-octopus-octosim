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
            /* send to authoritative. This avoids sending messages directly from within a call back */
            authoritative.RecursiveInput(r);
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
