#pragma once
#include <stdio.h>
#include "IModels.h"

class LossyLink : public IPath
{
public:
    LossyLink(SimulationLoop * loop, double lossRate, unsigned long long delay);
    ~LossyLink();

    // Inherited via IPath
    virtual void Input(ISimMessage * message) override;

    double lossRate;
    unsigned long long delay;
    unsigned long messages_sent;
    unsigned long messages_dropped;
};

