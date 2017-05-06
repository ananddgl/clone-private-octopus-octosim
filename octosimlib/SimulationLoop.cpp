/*
* Copyright (c) 2017, Private Octopus, Inc.
* All rights reserved.
*
* Permission to use, copy, modify, and distribute this software for any
* purpose with or without fee is hereby granted, provided that the above
* copyright notice and this permission notice appear in all copies.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Private Octopus, Inc. BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdlib.h>
#include <algorithm>
#include "MessageTree.h"
#include "SimulationRandom.h"
#include "SimulationLoop.h"

SimulationLoop::SimulationLoop(FILE * LogFile)
    :
    next_object(0),
    eventTree(NULL),
    simulationTime(0),
    eventCounter(0),
    LogFile(LogFile),
    rnd(NULL),
    input_rnd(NULL)
{
}


SimulationLoop::~SimulationLoop()
{
    if (eventTree != NULL)
        delete eventTree;

    if (rnd != NULL)
        delete rnd;

    if (input_rnd != NULL)
        delete input_rnd;
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

    if (ret && input_rnd == NULL)
    {
        input_rnd = new SimulationRandom();
    }
    return ret;
}

bool SimulationLoop::SubmitMessage(unsigned long long delta_t, ISimObject * target, ISimMessage * message)
{
    /* TODO: add lock! */
    bool ret = true;
    MessageTreeNode * mtn = new MessageTreeNode(target, message, simulationTime + delta_t,
        eventCounter++);
    MessageTreeNode * added = eventTree->Insert(mtn);

    if (added == NULL)
    {
        mtn->message = NULL;
        delete mtn;
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

