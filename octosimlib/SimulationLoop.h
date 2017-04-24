#pragma once
#include "IModels.h"
/*
 * Simulation loop. The "Do loop" message examines the next messages 
 * in the queues
 */

class MessageTree;
class SimulationRandom;

class SimulationLoop
{
public:
    SimulationLoop();
    ~SimulationLoop();

    bool Init();

    bool SubmitMessage(unsigned long long delta_t, ISimObject * target, ISimMessage * message);

    bool RequestTimer(unsigned long long delta_t, ISimObject * requester);

    bool DoLoop();

    const unsigned long long SimulationTime() {
        return (const unsigned long long) simulationTime;
    }

    SimulationRandom * Rnd() { return rnd; }

private:
    /* Sorted list of events*/
    MessageTree * eventTree;
    unsigned long long simulationTime;
    unsigned long long eventCounter;
    /* Random number generator */
    SimulationRandom * rnd;
};

