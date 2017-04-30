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
    unsigned long long ack_range_first[16];
    unsigned long long ack_range_last[16];
    unsigned long long transmit_time;
    unsigned long long ack_time;

    ISimMessage * payload;
};

class TcpSimReorderQueue
{
public:
    TcpSimReorderQueue();
    ~TcpSimReorderQueue();

    bool Insert(TcpMessage * tm);
    ISimMessage * DequeueInOrder();
    void FillAckData(TcpMessage * tm);
    bool IsEmpty() { return reorderQueue == NULL; }
    void Clean();

    TcpMessage * reorderQueue;
    unsigned long long last_sequence_received;
    unsigned long long last_sequence_processed;
};

class TcpSimRetransmitQueue
{
public:
    TcpSimRetransmitQueue();
    ~TcpSimRetransmitQueue();

    void AddNewMessage(TcpMessage * tm);
    TcpMessage * NextToRetransmit();
    void ApplyAck(TcpMessage * tm);
    bool ApplyTimer(unsigned long long lastTransmitTime);
    bool IsEmpty() {
        return (retransmitQueue == NULL); 
    }
    void ResetBeforeSyn();

    TcpMessage * retransmitQueue;
    TcpMessage * retransmitQueueLast;
    unsigned long long last_retransmit_sequence;
    unsigned long long last_transmit_time_acked;
    unsigned long long furthest_ack_range;
    unsigned long long last_sequence_number_sent;
    unsigned long long ack_received;
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
#if 1
    TcpSimRetransmitQueue retransmitQueue;
#else
    TcpMessage * retransmitQueue;
    TcpMessage * retransmitQueueLast;
    unsigned long long last_sequence_number_sent;
    unsigned long long ack_received;
#endif
    TcpSimReorderQueue reorderQueue;
    unsigned long long last_transmit_or_receive;
    unsigned long long last_received_time;
    unsigned long long tcp_idle_timeout;
    unsigned long long rtt;
    unsigned long long rtt_dev;
    TcpSimState state;

    int nb_packets_deleted;
private:
    void SendControlMessage(TcpMessageCode code);
    void SendCopyOfMessage(TcpMessage * tm);
};
