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
