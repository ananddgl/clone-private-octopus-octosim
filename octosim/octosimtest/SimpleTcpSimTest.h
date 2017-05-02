#pragma once
class SimpleTcpSimTest
{
public:
    SimpleTcpSimTest();
    ~SimpleTcpSimTest();

    bool SimpleTcpSimDoTest();
    bool DoOneTest(int nbPackets, int delta_t, int delay, double lossRate, FILE * F);
};

