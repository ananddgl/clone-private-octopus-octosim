#pragma once
#include "IModels.h"

class LossyLink : IPath
{
public:
    LossyLink(SimulationLoop * loop);
    ~LossyLink();

    // Inherited via IPath
    virtual void Input(ISimMessage * message) override;
    virtual void TimerExpired(unsigned long long simulationTime) override;

    double lossRate;
    unsigned long long delay;
    unsigned long messages_sent;
    unsigned long messages_dropped;

};

