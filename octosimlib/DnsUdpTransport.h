#pragma once
#include "IModels.h"
class DnsMessage;

class DnsUdpTransport : public ITransport
{
public:
    DnsUdpTransport(SimulationLoop* loop);
    ~DnsUdpTransport();

    // Inherited via ITransport
    virtual void ApplicationInput(ISimMessage * message) override;
    virtual void Input(ISimMessage * message) override;
    virtual void TimerExpired(unsigned long long simulationTime) override;

    DnsMessage * retransmitQueue;
    unsigned long long nextTimer;
    unsigned long long rtt;
    unsigned long long rtt_dev;
private:
    void ResetTimer(unsigned long long delay);
};

