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

#include <stdlib.h>
#include "SimulationLoop.h"
#include "DnsMessage.h"
#include "DnsRecursive.h"

DnsRecursive::DnsRecursive(SimulationLoop * loop, 
    IDelayDistribution * authoritative_model,
    ILengthDistribution * length_model)
    :
    authoritative(loop, authoritative_model, length_model, this),
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
