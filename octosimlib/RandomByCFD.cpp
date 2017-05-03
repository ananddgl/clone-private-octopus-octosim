#include <stdlib.h>
#include <string.h>
#include "SimulationRandom.h"
#include "RandomByCFD.h"



RandomByCFD::RandomByCFD()
    :
    nb_cfd_points(0),
    cfd_proba(NULL),
    cfd_value(NULL)
{
}


RandomByCFD::~RandomByCFD()
{
    Clear();
}

bool RandomByCFD::Init(SimulationRandom * rnd, 
    unsigned int nb_cfd_points, double * cfd_proba, unsigned long long * cfd_value)
{
    bool ret = false;
    Clear();
    this->rnd = rnd;
    this->cfd_proba = new double[nb_cfd_points];
    this->cfd_value = new unsigned long long[nb_cfd_points];

    if (rnd != NULL && nb_cfd_points > 0 &&
        this->cfd_proba != NULL && this->cfd_value != NULL)
    {
        double previous_p = 0;
        unsigned long long previous_v = 0;
        ret = true;
        this->cfd_proba[0] = cfd_proba[0];
        this->cfd_value[0] = cfd_value[0];

        for (unsigned int i = 1; ret && i < nb_cfd_points; i++)
        {
            if (cfd_proba[i] >= cfd_proba[i - 1] && cfd_value[i] >= cfd_value[i - 1])
            {
                this->cfd_proba[i] = cfd_proba[i];
                this->cfd_value[i] = cfd_value[i];
            }
            else
            {
                ret = false;
            }
        }

        if (ret)
        {
            this->nb_cfd_points = nb_cfd_points;
        }
    }

    return ret;
}

unsigned long long RandomByCFD::GetCfdRandom()
{
    unsigned long long ret = 0;
    double proba = rnd->GetZeroToOne();

    if (nb_cfd_points > 0)
    {
        int low = 0;
        int high = nb_cfd_points - 1;

        if (proba >= cfd_proba[high])
        {
            low = high;
        }
        else if (proba <= cfd_proba[low])
        {
            high = low;
        }
        else
        {
            while (low + 1 < high)
            {
                int next = (low + high) / 2;

                if (proba == cfd_proba[next])
                {
                    high = low = next;
                }
                else if (proba < cfd_proba[next])
                {
                    high = next;
                }
                else
                {
                    low = next;
                }
            }
        }

        if (low == high)
        {
            ret = cfd_value[low];
        }
        else
        {
            double delta_v = (double)(cfd_value[high] - cfd_value[low]);
            double delta_lh = cfd_proba[high] - cfd_proba[low];
            double delta_x = (delta_v * (proba - cfd_proba[low]))/ delta_lh;

            ret = cfd_value[low] + ((unsigned long long) delta_x);
        }
    }

    return ret;
}

void RandomByCFD::Clear()
{
    rnd = NULL;

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
