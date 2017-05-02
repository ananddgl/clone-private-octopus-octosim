#include "SimulationRandom.h"
#include "SimulationLoop.h"
#include "LossyLink.h"



LossyLink::LossyLink(SimulationLoop * loop, double lossRate, unsigned long long delay)
    :
    lossRate(lossRate),
    delay(delay),
    messages_sent(0),
    messages_dropped(0),
    IPath(loop)
{
}

LossyLink::~LossyLink()
{
}

void LossyLink::Input(ISimMessage * message)
{
    double d = GetLoop()->Rnd()->GetZeroToOne();

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
        GetLoop()->SubmitMessage(delay, GetTransport(), message);
    }

    
}
