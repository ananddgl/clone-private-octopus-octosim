#pragma once
#include <stdio.h>

class DnsTcpSimTest
{
public:
    DnsTcpSimTest(bool quic_mode = false);
    ~DnsTcpSimTest();

    bool DnsTcpSimDoTest();
private:
    bool DoOneTest(int nbPackets, int delay, double lossRate, FILE* F = NULL, bool doCsvLog = false);

    bool quic_mode;
};

