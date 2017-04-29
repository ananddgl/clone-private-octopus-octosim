#pragma once
class DnsTcpSimTest
{
public:
    DnsTcpSimTest();
    ~DnsTcpSimTest();

    bool DnsTcpSimDoTest();
private:
    bool DoOneTest(int nbPackets, int delay, double lossRate);
};

