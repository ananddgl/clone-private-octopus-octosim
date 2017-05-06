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
    SimulationRandom * InputRnd() { return input_rnd; }

    FILE* LogFile;

private:
    /* Counter of created objects */
    unsigned int next_object;
    /* Sorted list of events*/
    MessageTree * eventTree;
    unsigned long long simulationTime;
    unsigned long long eventCounter;
    /* Random number generator */
    SimulationRandom * rnd; /* General purpose */
    SimulationRandom * input_rnd; /* Input generation */
};

