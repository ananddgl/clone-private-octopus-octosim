#pragma once
class SimulationRandom;

class SimRandomTest
{
public:
    SimRandomTest();
    ~SimRandomTest();

    bool SimRandomDoTest();

private:
    SimulationRandom * rnd;

    bool CountTheOneTest(int used_octets_per_longlong);
    bool FrequencyTest(unsigned int * observed, int nb_buckets, int nb_samples, 
        const double * bucket_probability);
};

