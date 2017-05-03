#pragma once
#include <stdio.h>
#include "WriteOnceHash.h"
#include "IModels.h"

class DnsStub : public IApplication
{
public:
    DnsStub(SimulationLoop * loop, FILE* FStats, 
        int nb_packets_to_send, 
        IDelayDistribution * source_process,
        ILengthDistribution * source_length_process = NULL);
    ~DnsStub();

    // Inherited via IApplication
    virtual void Input(ISimMessage * message) override;
    virtual void TimerExpired(unsigned long long simulationTime) override;

    FILE* FStats;
    int nb_packets_to_send;
    IDelayDistribution * source_process;
    ILengthDistribution * source_length_process;
    int nb_packets_sent;
    int nb_transactions_complete;
    int nb_duplicate_transactions;
    unsigned long long next_target_id;

    WriteOnceHash completedCache;

};

