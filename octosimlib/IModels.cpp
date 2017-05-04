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
    if ((rtt + 2 * rtt_dev) > 1000000 && GetLoop()->LogFile != NULL)
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
