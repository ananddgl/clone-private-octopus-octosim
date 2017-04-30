#pragma once
class TcpSimRetransmitQueue;

class TcpRetransmitTest
{
public:
    TcpRetransmitTest();
    ~TcpRetransmitTest();

    bool TcpRetransmitDoTest();
    bool CheckRetransmit(TcpSimRetransmitQueue * tsrq, 
        int nb_in_batch, unsigned long long * retransmit_batch,
        unsigned long long current_time);
    bool AckAndCheck(
        TcpSimRetransmitQueue * tsrq, 
        unsigned long long ack_time, unsigned long long rtt,
        TcpMessage * ack, int nb_numbers, unsigned long long * ack_and_sack);
};

