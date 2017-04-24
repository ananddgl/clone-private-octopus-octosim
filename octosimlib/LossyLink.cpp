#include "SimulationRandom.h"
#include "SimulationLoop.h"
#include "LossyLink.h"



LossyLink::LossyLink(SimulationLoop * loop)
    :
    lossRate(0),
    delay(2000),
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

    if (d < lossRate)
    {
        messages_dropped++;
        delete message;
    }
    else
    {
        messages_sent++;
        GetLoop()->SubmitMessage(delay, GetTransport(), message);
    }
}
