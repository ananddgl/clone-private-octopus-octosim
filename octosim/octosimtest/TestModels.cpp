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
#include "../octosimlib/SimulationLoop.h"
#include "TestModels.h"

TestSource::TestSource(SimulationLoop * loop, int nb_packets_to_send)
    :
    nb_packets_to_send (nb_packets_to_send),
    nb_packets_sent(0),
    message_delay(500),
    IApplication(loop)
{
}

TestSource::~TestSource() {}

// Inherited via IApplication

inline void TestSource::Input(ISimMessage * message)
{
    delete message;
}

inline void TestSource::TimerExpired(unsigned long long simulationTime)
{
    if (nb_packets_sent < nb_packets_to_send)
    {
        nb_packets_sent++;
        TestMessage * m = new TestMessage(nb_packets_sent);

        GetTransport()->ApplicationInput(m);
        if (nb_packets_sent < nb_packets_to_send)
        {
            GetLoop()->RequestTimer(message_delay, this);
        }
    }
}


TestSink::TestSink(SimulationLoop * loop)
    :
    nb_packets_received(0),
    IApplication(loop)
{
}

TestSink::~TestSink()
{
}

void TestSink::Input(ISimMessage * message)
{
    nb_packets_received++;
}

TestPassthroughTransport::TestPassthroughTransport(SimulationLoop * loop)
    :
    ITransport(loop)
{}

TestPassthroughTransport::~TestPassthroughTransport()
{
}

void TestPassthroughTransport::ApplicationInput(ISimMessage * message)
{
    GetPath()->Input(message);
}

void TestPassthroughTransport::Input(ISimMessage * message)
{
    GetApplication()->Input(message);
}

TestLink::TestLink(SimulationLoop * loop)
    :
    delay(10000),
    IPath(loop)
{
}

TestLink::~TestLink()
{
}

void TestLink::Input(ISimMessage * message)
{
    GetLoop()->SubmitMessage(delay, GetTransport(), message);
}

TestMessage::TestMessage(int n, unsigned int l)
    :
    message_number(n),
    ISimMessage(l)
{ }

void TestMessage::Log(FILE * LogFile, bool dropped)
{
    if (LogFile)
    {
        fprintf(LogFile, "%sTestMessage %d\n",
            (dropped) ? "Dropped " : "", message_number);
    }
}

TestSimpleDelay::TestSimpleDelay(unsigned int delay, SimulationLoop * loop)
    :
    delay(delay),
    IDelayDistribution(loop)
{
}

TestSimpleDelay::~TestSimpleDelay()
{
}

void TestSimpleDelay::Input(ISimMessage * message)
{
    if (message->Dereference())
    {
        delete message;
    }
}

unsigned long long TestSimpleDelay::NextDelay()
{
    return delay;
}

TestSimpleLength::TestSimpleLength(unsigned int length, SimulationLoop * loop)
    :
    length(length),
    ILengthDistribution(loop)
{
}

TestSimpleLength::~TestSimpleLength()
{
}

unsigned int TestSimpleLength::NextLength()
{
    return length;
}
