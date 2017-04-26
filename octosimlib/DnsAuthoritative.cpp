#include <stdlib.h>
#include "SimulationLoop.h"
#include "DnsRecursive.h"
#include "DnsAuthoritative.h"

DnsAuthoritative::DnsAuthoritative(SimulationLoop * loop,
    IDelayDistribution * authoritative_model,
    DnsRecursive * recursive)
    :
    authoritative_model(authoritative_model),
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
    unsigned long long delay = authoritative_model->NextDelay();
    /* Delayed input */
    GetLoop()->SubmitMessage(delay, this, message);
}

void DnsAuthoritative::Input(ISimMessage * message)
{
    /* Submit the input directly to the
     * of the recursive resolver. */
    recursive->AuthoritativeInput(message);
}
