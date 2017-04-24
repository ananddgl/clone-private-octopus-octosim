#include <stdlib.h>
#include "IModels.h"

ISimMessage::ISimMessage()
{
}

ISimMessage::~ISimMessage()
{
}

ISimObject::ISimObject(SimulationLoop * loop)
{
    this->loop = loop;
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