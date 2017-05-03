#include <stdlib.h>
#include <string.h>
#include "SimulationLoop.h"
#include "SimulationRandom.h"
#include "RandomByCFD.h"



RandomDelayByCFD::RandomDelayByCFD(SimulationLoop * loop)
    :
    nb_cfd_points(0),
    cfd_proba(NULL),
    cfd_value(NULL),
    IDelayDistribution(loop)
{
}


RandomDelayByCFD::~RandomDelayByCFD()
{
    Clear();
}

bool RandomDelayByCFD::Init( 
    unsigned int nb_cfd_points, const double * cfd_proba, const unsigned long long * cfd_value)
{
    Clear();

    return InitCfdValues<unsigned long long>(nb_cfd_points, cfd_proba, cfd_value,
        &this->nb_cfd_points, &this->cfd_proba, &this->cfd_value);
}

unsigned long long RandomDelayByCFD::NextDelay()
{
    return NextCfdValue<unsigned long long>(GetLoop()->Rnd(),
        nb_cfd_points, cfd_proba, cfd_value);
}

void RandomDelayByCFD::Clear()
{
    if (cfd_proba != NULL)
    {
        delete cfd_proba;
        cfd_proba = NULL;
    }

    if (cfd_value != NULL)
    {
        delete cfd_value;
        cfd_value = NULL;
    }

    nb_cfd_points = 0;
}

RandomLengthByCFD::RandomLengthByCFD(SimulationLoop * loop)
    :
    ILengthDistribution(loop)
{
}

RandomLengthByCFD::~RandomLengthByCFD()
{
    Clear();
}

bool RandomLengthByCFD::Init(unsigned int nb_cfd_points, const double * cfd_proba, const unsigned int * cfd_value)
{
    Clear();

    return InitCfdValues<unsigned int>(nb_cfd_points, cfd_proba, cfd_value,
        &this->nb_cfd_points, &this->cfd_proba, &this->cfd_value);
}

unsigned int RandomLengthByCFD::NextLength()
{
    return NextCfdValue<unsigned int>(GetLoop()->Rnd(),
        nb_cfd_points, cfd_proba, cfd_value);
}

void RandomLengthByCFD::Clear()
{
    if (cfd_proba != NULL)
    {
        delete cfd_proba;
        cfd_proba = NULL;
    }

    if (cfd_value != NULL)
    {
        delete cfd_value;
        cfd_value = NULL;
    }

    nb_cfd_points = 0;
}
