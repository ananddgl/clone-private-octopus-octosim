#pragma once
#include "IModels.h"
#include "WriteOnceHash.h"

class SimulationLoop;
class IDelayDistribution;
class DnsRecursive;

class DnsAuthoritative : public ISimObject
{
public:
    DnsAuthoritative(SimulationLoop * loop,
        IDelayDistribution * authoritative_model,
        ILengthDistribution * length_model,
        DnsRecursive * recursive);

    ~DnsAuthoritative();

    void RecursiveInput(ISimMessage * message) ;
    virtual void Input(ISimMessage * message) override;

private:
    DnsRecursive * recursive;
    IDelayDistribution * authoritative_model;
    ILengthDistribution * length_model;
    WriteOnceHash simulatedCache;
};
