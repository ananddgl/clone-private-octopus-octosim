#pragma once
#include "SimulationRandom.h"
#include "IModels.h"

template <class DataType>
bool InitCfdValues(unsigned int nb_cfd_points, const double * cfd_proba, const DataType * cfd_value,
    unsigned int * this_nb_cfd_points, double ** this_cfd_proba, DataType ** this_cfd_value)
{
    bool ret = false;
    *this_cfd_proba = new double[nb_cfd_points];
    *this_cfd_value = new DataType[nb_cfd_points];

    if (nb_cfd_points > 0 &&
        *this_cfd_proba != NULL && *this_cfd_value != NULL)
    {
        double previous_p = 0;
        unsigned long long previous_v = 0;
        ret = true;
        (*this_cfd_proba)[0] = cfd_proba[0];
        (*this_cfd_value)[0] = cfd_value[0];

        for (unsigned int i = 1; ret && i < nb_cfd_points; i++)
        {
            if (cfd_proba[i] >= cfd_proba[i - 1] && cfd_value[i] >= cfd_value[i - 1])
            {
                (*this_cfd_proba)[i] = cfd_proba[i];
                (*this_cfd_value)[i] = cfd_value[i];
            }
            else
            {
                ret = false;
            }
        }

        if (ret)
        {
            *this_nb_cfd_points = nb_cfd_points;
        }
    }

    return ret;
}

template <class DataType>
DataType NextCfdValue(
    SimulationRandom * rnd,
    unsigned int nb_cfd_points, const double * cfd_proba, const DataType * cfd_value)
{
    DataType ret = 0;
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
            double delta_x = (delta_v * (proba - cfd_proba[low])) / delta_lh;

            ret = cfd_value[low] + ((DataType)delta_x);
        }
    }

    return ret;
}


class RandomDelayByCFD : public IDelayDistribution
{
public:
    RandomDelayByCFD(SimulationLoop * loop);
    ~RandomDelayByCFD();

    bool Init(
        unsigned int nb_cfd_points, const double * cfd_proba, const unsigned long long * cfd_value);

    virtual unsigned long long NextDelay() override;

private:
    unsigned int nb_cfd_points;
    double * cfd_proba; 
    unsigned long long * cfd_value;

    void Clear();
};

class RandomLengthByCFD : public ILengthDistribution
{
public:
    RandomLengthByCFD(SimulationLoop * loop);
    ~RandomLengthByCFD();

    bool Init(
        unsigned int nb_cfd_points, const double * cfd_proba, const unsigned int * cfd_value);

    virtual unsigned int NextLength() override;

private:
    unsigned int nb_cfd_points;
    double * cfd_proba;
    unsigned int * cfd_value;

    void Clear();
};
