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
    unsigned long long last_received_time;
    int nb_packets_deleted;
};

