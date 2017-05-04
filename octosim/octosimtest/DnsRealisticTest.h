#pragma once
class DnsRealisticTest
{
public:
    DnsRealisticTest();
    ~DnsRealisticTest();

    bool DnsRealisticDoTest();

    bool DnsRealisticOneTest(unsigned int variant, unsigned int nb_packets,
        unsigned int delay, double lossRate);
};

