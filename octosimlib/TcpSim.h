#pragma once
#include "IModels.h"

enum TcpMessageCode
{
    syn = 1,
    syn_ack = 2,
    ack = 4,
    data = 8,
    rst = 16
};

enum TcpSimState
{
    idle,
    syn_sent,
    connected,
    rst_sent
};

class TcpMessage : public ISimMessage
{
public:
    TcpMessage(ISimMessage * payload);
    ~TcpMessage();

    TcpMessage * Copy();

    TcpMessage * next_in_transmit;
    TcpMessage * next_in_reorder;

    unsigned int flags;
    unsigned long long sequence;
    unsigned long long ack_number;
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
    TcpMessage * retransmitQueueLast;
    TcpMessage * reorderQueue;
    unsigned long long last_sequence_number_sent;
    unsigned long long ack_received;
    unsigned long long last_sequence_received;
    unsigned long long last_sequence_processed;
    unsigned long long last_transmit_or_receive;
    unsigned long long tcp_idle_timeout;
    unsigned long long rtt;
    unsigned long long rtt_dev;
    TcpSimState state;

    int nb_packets_deleted;
private:
    void SendControlMessage(TcpMessageCode code);
    void FillAckData(TcpMessage * tm);
    void SendCopyOfMessage(TcpMessage * tm);
};

