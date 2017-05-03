#pragma once
#include <stdio.h>
#include "IModels.h"

class LossyLink : public IPath
{
public:
    LossyLink(SimulationLoop * loop, double lossRate, 
        unsigned long long delay, double data_rate_in_gps = 1.0);
    ~LossyLink();

    // Inherited via IPath
    virtual void Input(ISimMessage * message) override;

    double data_rate_in_gps;
    double microseconds_per_byte;
    double lossRate;
    unsigned long long delay;
    unsigned long messages_sent;
    unsigned long messages_dropped;
    unsigned long long queue_time;
};

