#pragma once
#include "IModels.h"
/*
 * Simulation loop. The "Do loop" message examines the next messages 
 * in the queues
 */

class SimulationLoop
{
public:
    SimulationLoop();
    ~SimulationLoop();

    int SubmitMessage(unsigned long long delta_t, ISimObject * target, ISimMessage * message);

    int RequestTimer(unsigned long long delta_t, ISimObject * requester, void * argument);

    int DoLoop();

private:
    /* Sorted list of events*/


};

