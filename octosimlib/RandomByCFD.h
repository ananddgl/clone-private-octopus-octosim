#pragma once
class SimulationRandom;

class RandomByCFD
{
public:
    RandomByCFD();
    ~RandomByCFD();

    bool Init(SimulationRandom * rnd,
        unsigned int nb_cfd_points, double * cfd_proba, unsigned long long * cfd_value);

    unsigned long long GetCfdRandom();

private:
    SimulationRandom * rnd;
    unsigned int nb_cfd_points;
    double * cfd_proba; 
    unsigned long long * cfd_value;

    void Clear();
};

