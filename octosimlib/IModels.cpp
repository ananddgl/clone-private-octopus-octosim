#include <stdlib.h>
#include "IModels.h"

ISimMessage::ISimMessage()
    :
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

ISimObject::ISimObject(SimulationLoop * loop)
    :
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
    ISimObject(loop)
{
}

ITransport::~ITransport()
{
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

IDelayDistribution::IDelayDistribution(SimulationLoop * loop)
    :
    ISimObject(loop)
{
}

IDelayDistribution::~IDelayDistribution()
{
}

