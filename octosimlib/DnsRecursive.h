#pragma once
//#include "WriteOnceHash.h" 
#include "IModels.h"
#include "DnsAuthoritative.h"
#include "WriteOnceHash.h"

class DnsRecursive : public IApplication
{
public:
    DnsRecursive(SimulationLoop * loop, IDelayDistribution * authoritative_model);
    ~DnsRecursive();

    DnsAuthoritative authoritative;

    // Inherited via IApplication
    virtual void Input(ISimMessage * message) override;

    void AuthoritativeInput(ISimMessage * message);
};

