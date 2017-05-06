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
    return NextCfdValue<unsigned long long>(GetLoop()->InputRnd(),
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
    nb_cfd_points(0),
    cfd_proba(NULL),
    cfd_value(NULL),
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
    return NextCfdValue<unsigned int>(GetLoop()->InputRnd(),
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
