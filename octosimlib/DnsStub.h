#pragma once
#include <stdio.h>
#include "IModels.h"

class DnsStub : public IApplication
{
public:
    DnsStub(SimulationLoop * loop, FILE* FStats, 
        int nb_packets_to_send, IDelayDistribution * source_process);
    ~DnsStub();

    // Inherited via IApplication
    virtual void Input(ISimMessage * message) override;
    virtual void TimerExpired(unsigned long long simulationTime) override;

    FILE* FStats;
    int nb_packets_to_send;
    IDelayDistribution * source_process;
    int nb_packets_sent;
    int nb_transactions_complete;
};

