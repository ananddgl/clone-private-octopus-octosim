#include <stdlib.h>
#include <algorithm>
#include "MessageTree.h"
#include "SimulationRandom.h"
#include "SimulationLoop.h"



SimulationLoop::SimulationLoop()
    :
    eventTree(NULL),
    simulationTime(0),
    eventCounter(0),
    rnd(NULL)
{
}


SimulationLoop::~SimulationLoop()
{
    if (eventTree != NULL)
        delete eventTree;

    if (rnd != NULL)
        delete rnd;
}

bool SimulationLoop::Init()
{
    bool ret = true;

    if (eventTree == NULL)
    {
        eventTree = new MessageTree();

        ret = eventTree != NULL;
    }

    if (ret && rnd == NULL)
    {
        rnd = new SimulationRandom();
    }

    return ret;
}

bool SimulationLoop::SubmitMessage(unsigned long long delta_t, ISimObject * target, ISimMessage * message)
{
    /* TODO: add lock! */
    bool ret = true;
    MessageTreeNode * msn = new MessageTreeNode(target, message, simulationTime + delta_t,
        eventCounter++);
    MessageTreeNode * added = eventTree->Insert(msn);

    if (added == NULL)
    {
        added->message = NULL;
        delete added;
        ret = false;
    }

    return ret;
}

bool SimulationLoop::RequestTimer(unsigned long long delta_t, ISimObject * requester)
{
    /* TODO: add lock! */
    return SubmitMessage(delta_t, requester, NULL);
}

bool SimulationLoop::DoLoop()
{
    bool ret = true;
    MessageTreeNode * first = eventTree->First();

    if (first == NULL)
    {
        ret = false;
    }
    else
    {
        MessageTreeNode * deleted = eventTree->Remove(first);

        if (deleted == first)
        {
            simulationTime = std::max(simulationTime, first->scheduled_time);
            if (first->message != NULL)
            {
                first->target->Input(first->message);
            }
            else
            {
                first->target->TimerExpired(simulationTime);
            }
            first->message = NULL;
            delete first;
        }
        else
        {
            ret = false;
        }
    }

    return ret;
}

