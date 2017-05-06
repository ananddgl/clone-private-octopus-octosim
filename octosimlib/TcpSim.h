/*
* Copyright (c) 2017, Private Octopus, Inc.
* All rights reserved.
*
* Permission to use, copy, modify, and distribute this software for any
* purpose with or without fee is hereby granted, provided that the above
* copyright notice and this permission notice appear in all copies.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Private Octopus, Inc. BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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

    virtual void Log(FILE* LogFile, bool dropped) override;

    const char * CodeToText();

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
    unsigned long long time_since_last_received;

    ISimMessage * payload;
};

class TcpSimReorderQueue
{
public:
    TcpSimReorderQueue();
    ~TcpSimReorderQueue();

    bool Insert(TcpMessage * tm);
    ISimMessage * DequeueInOrder();
    bool IsAckNeeded(unsigned long long last_received_time);
    void FillAckData(TcpMessage * tm, unsigned long long current_time);
    bool IsEmpty() { return reorderQueue == NULL; }
    void Clean();

    TcpMessage * reorderQueue;
    unsigned long long last_ack_sent;
    unsigned long long time_last_ack_sent;
    unsigned long long last_sequence_received;
    unsigned long long last_sequence_processed;
    bool dup_received;
};

class TcpSimRetransmitQueue
{
public:
    TcpSimRetransmitQueue();
    ~TcpSimRetransmitQueue();

    void AddNewMessage(TcpMessage * tm);
    TcpMessage * NextToRetransmit(bool force_retransmit);
    void ApplyAck(TcpMessage * tm);
    bool ApplyTimer(unsigned long long lastTransmitTime);
    bool IsEmpty() {
        return (retransmitQueue == NULL); 
    }
    void ResetBeforeSyn();

    TcpMessage * retransmitQueue;
    TcpMessage * retransmitQueueLast;
    unsigned long long last_transmit_time_acked;
    unsigned long long furthest_ack_range;
    unsigned long long last_sequence_number_sent;
    unsigned long long ack_received;
};

class TcpSim : public ITransport
{
public:
    TcpSim(SimulationLoop* loop, bool quic_mode = false, bool zero_rtt = false);
    ~TcpSim();

    // Inherited via ITransport
    virtual void ApplicationInput(ISimMessage * message) override;
    virtual void Input(ISimMessage * message) override;
    virtual void TimerExpired(unsigned long long simulationTime) override;

    bool quic_mode;
    bool zero_rtt;
    TcpSimRetransmitQueue retransmitQueue;
    TcpSimReorderQueue reorderQueue;
    unsigned long long last_transmit_time;
    unsigned long long last_remote_transmit_time;
    unsigned long long last_received_time;
    unsigned long long tcp_idle_timeout;
    TcpSimState state;

    int nb_packets_deleted;
private:
    void SendControlMessage(TcpMessageCode code);
    void SendCopyOfMessage(TcpMessage * tm);
};

