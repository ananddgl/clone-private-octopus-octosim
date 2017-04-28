#pragma once
#include "IModels.h"

enum TcpMessageCode
{
    syn = 1,
    ack = 2,
    rst = 4
};

class TcpMessage : public ISimMessage
{
    TcpMessage(ISimMessage * payload);
    ~TcpMessage();

    TcpMessage * next;

    unsigned int flags;
    unsigned long long sequence;
    unsigned long long ack;
    unsigned int nb_nack;
    unsigned long long nack[16];
    unsigned long long transmit_time;

    ISimMessage * payload;
};

class TcpSim : public ITransport
{
public:
    TcpSim(SimulationLoop* loop);
    ~TcpSim();

    // Inherited via ITransport
    virtual void ApplicationInput(ISimMessage * message) override;
    virtual void Input(ISimMessage * message) override;
    virtual void TimerExpired(unsigned long long simulationTime) override;

    TcpMessage * retransmitQueue;
    unsigned long long nextTimer;
    unsigned long long rtt;
    unsigned long long rtt_dev;
    int nb_timers_outstanding;
    int nb_timers_stop;
    int nb_packets_deleted;
private:
    void ResetTimer(unsigned long long delay);
};

