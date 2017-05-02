#pragma once
#include <stdio.h>
/*
 * Simulation loop. The "Do loop" message examines the next messages 
 * in the queues
 */

class MessageTree;
class SimulationRandom;
class ISimObject;
class ISimMessage;

class SimulationLoop
{
public:
    SimulationLoop(FILE* LogFile = NULL);
    ~SimulationLoop();

    bool Init();

    unsigned int GetNextObjectNumber() { return ++next_object; };

    bool SubmitMessage(unsigned long long delta_t, ISimObject * target, ISimMessage * message);

    bool RequestTimer(unsigned long long delta_t, ISimObject * requester);

    bool DoLoop();

    const unsigned long long SimulationTime() {
        return (const unsigned long long) simulationTime;
    }

    SimulationRandom * Rnd() { return rnd; }

    FILE* LogFile;

private:
    /* Counter of created objects */
    unsigned int next_object;
    /* Sorted list of events*/
    MessageTree * eventTree;
    unsigned long long simulationTime;
    unsigned long long eventCounter;
    /* Random number generator */
    SimulationRandom * rnd;
};

