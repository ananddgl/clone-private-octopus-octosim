#pragma once
class DnsUdpSimTest
{
public:
    DnsUdpSimTest();
    ~DnsUdpSimTest();

    bool DnsUdpSimDoTest();
private:
    bool DoOneTest(int nbPackets, int delay, double lossRate, FILE * Fstats = NULL, bool doCsvLog = false);
};

