#include "SimulationLoop.h"



SimulationLoop::SimulationLoop()
{
}


SimulationLoop::~SimulationLoop()
{
}

int SimulationLoop::SubmitMessage(unsigned long long delta_t, ISimObject * target, ISimMessage * message)
{
    return 0;
}

int SimulationLoop::RequestTimer(unsigned long long delta_t, ISimObject * requester, void * argument)
{
    return 0;
}

