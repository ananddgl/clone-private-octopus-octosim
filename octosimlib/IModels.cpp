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
#include "SimulationLoop.h"
#include "IModels.h"

ISimMessage::ISimMessage(unsigned int length)
    :
    length(length),
    referenceCount(1)
{
}

ISimMessage::~ISimMessage()
{
}

void ISimMessage::Reference()
{
    referenceCount++;
}

bool ISimMessage::Dereference()
{
    bool ret = referenceCount <= 1;

    if (ret)
    {
        referenceCount = 0;
    }
    else
    {
        referenceCount--;
    }

    return ret;
}

void ISimMessage::Log(FILE * LogFile, bool dropped)
{
    if (LogFile)
    {
        fprintf(LogFile, "%sMessage\n", (dropped) ? "Dropped " : "");
    }
}

ISimObject::ISimObject(SimulationLoop * loop)
    :
    object_number(loop->GetNextObjectNumber()),
    loop(loop)
{
}

ISimObject::~ISimObject()
{
}

IApplication::IApplication(SimulationLoop * loop)
    :
    transport(NULL),
    ISimObject(loop)
{
}

IApplication::~IApplication()
{
}

ITransport::ITransport(SimulationLoop * loop)
    :
    application(NULL),
    path(NULL), 
    nb_timers_outstanding(0),
    next_timer(0),
    rtt(1000000ull),
    rtt_dev(0ull),
    ISimObject(loop)
{
}

ITransport::~ITransport()
{
}

void ITransport::ResetTimer(unsigned long long delay)
{
    /* Note that we do not bother deleting the old timers. */
    if (nb_timers_outstanding == 0 || (GetLoop()->SimulationTime() + delay) < next_timer)
    {
        SetTimer(delay);
    }
}

void ITransport::SetTimer(unsigned long long delay)
{
    next_timer = GetLoop()->SimulationTime() + delay;
    nb_timers_outstanding++;
    GetLoop()->RequestTimer(delay, this);
}

void ITransport::RttUpdate(unsigned long long transmitTime, unsigned long long arrivalTime)
{
    if (arrivalTime >= transmitTime)
    {
        unsigned long long rtt_measured = arrivalTime - transmitTime;

        if (rtt_dev == 0)
        {
            rtt = rtt_measured;
            rtt_dev = rtt / 2;
        }
        else
        {
            unsigned long long delta_rtt = (rtt_measured > rtt) ?
                rtt_measured - rtt : rtt - rtt_measured;
            rtt = (15 * rtt + rtt_measured) / 16;
            rtt_dev = (7 * rtt_dev + delta_rtt) / 8;
        }
    }

    if (GetLoop()->LogFile != NULL)
    {
        fprintf(GetLoop()->LogFile, 
            "RttUpdate(%d) rtt: %llu, rtt_dev: %llu, T: %llu, A: %llu\n",
            object_number, rtt, rtt_dev, transmitTime, arrivalTime);
    }
}

IPath::IPath(SimulationLoop * loop)
    :
    transport(NULL),
    ISimObject(loop)
{
}

IPath::~IPath()
{
}

void IPath::TimerExpired(unsigned long long simulationTime) {}

IDelayDistribution::IDelayDistribution(SimulationLoop * loop)
    :
    ISimObject(loop)
{
}

IDelayDistribution::~IDelayDistribution()
{
}

/* Default input for delay distributions is to just ignore and delete the input */
void IDelayDistribution::Input(ISimMessage * message)
{
    if (message->Dereference())
    {
        delete message;
    }
}

ILengthDistribution::ILengthDistribution(SimulationLoop * loop)
    :
    ISimObject(loop)
{
}

ILengthDistribution::~ILengthDistribution()
{
}

void ILengthDistribution::Input(ISimMessage * message)
{
    if (message->Dereference())
    {
        delete message;
    }
}
