#include <algorithm>
#include "SimulationRandom.h"
#include "SimulationLoop.h"
#include "LossyLink.h"



LossyLink::LossyLink(SimulationLoop * loop, double lossRate, 
    unsigned long long delay, double data_rate_in_gps)
    :
    lossRate(lossRate),
    delay(delay),
    messages_sent(0),
    messages_dropped(0),
    data_rate_in_gps(data_rate_in_gps),
    queue_time(0),
    IPath(loop)
{
    microseconds_per_byte = (data_rate_in_gps <= 0) ? 0 : 
        0.008 / data_rate_in_gps;
}

LossyLink::~LossyLink()
{
}

void LossyLink::Input(ISimMessage * message)
{
    double d = GetLoop()->Rnd()->GetZeroToOne();
    unsigned long long current_time = GetLoop()->SimulationTime();
    unsigned long long queue_delay = (current_time > queue_time) ? 0 : queue_time - current_time;
    unsigned long long transmit_time = 
        std::max(1ull, (unsigned long long)(microseconds_per_byte*((double)message->length) + 0.5));

    queue_time = current_time + queue_delay + transmit_time;

    if (GetLoop()->LogFile != NULL)
    {
        fprintf(GetLoop()->LogFile, "Link(%u) ", object_number);
        message->Log(GetLoop()->LogFile, d < lossRate);
    }

    if (d < lossRate)
    {
        messages_dropped++;
        if (message->Dereference())
        {
            delete message;
        }
    }
    else
    {
        messages_sent++;
        GetLoop()->SubmitMessage(delay + queue_delay + transmit_time, GetTransport(), message);
    }
}
