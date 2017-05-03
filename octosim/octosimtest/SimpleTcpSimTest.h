#pragma once
class SimpleTcpSimTest
{
public:
    SimpleTcpSimTest(bool quic_mode = false);
    ~SimpleTcpSimTest();

    bool SimpleTcpSimDoTest();
    bool DoOneTest(int nbPackets, int delta_t, int delay, double lossRate, FILE * F);

    bool quic_mode;
};

