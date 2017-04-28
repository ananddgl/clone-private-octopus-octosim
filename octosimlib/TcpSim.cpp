#include <stdlib.h>
#include <algorithm>
#include "SimulationLoop.h"
#include "TcpSim.h"



TcpSim::TcpSim(SimulationLoop* loop)
    :
    retransmitQueue(NULL),
    nextTimer(1000000000ull),
    rtt(1000000ull),
    rtt_dev(0ull),
    nb_timers_outstanding(0),
    nb_timers_stop(0),
    nb_packets_deleted(0),
    ITransport(loop)
{
}


TcpSim::~TcpSim()
{
}

void TcpSim::ApplicationInput(ISimMessage * message)
{
    /* Create a TCP Message based on the application */
    /* Assign a number, etc. */
    /* Add at the end of the queue */
    /* If the connection is not active, send a SYN */
    /* Else, fill the packet header and send it */
}

void TcpSim::Input(ISimMessage * message)
{
    /* If this is a SYN, respond with SYNACK. Transmit pending packets. */
    /* If this is a SYNACK, transmit pending packets. */
    /* If this contains ACK/NACK, dequeue ACKed, transmit NACK if needed */
    /* If this is data, insert in receive queue */
    /* If receive queue is ordered, pass the packets to the application */
    /* If ACK state changed, send an ACK */
}

void TcpSim::TimerExpired(unsigned long long simulationTime)
{
    /* If packets due for retransmission, do it */
    /* If nothing in queues, check whether to hang the connection */
}

TcpMessage::TcpMessage(ISimMessage * payload)
    :
    payload(payload),
    flags(0),
    sequence(0),
    ack(0),
    nb_nack(0),
    transmit_time(0),
    ISimMessage()
{
    memset(nack, 0, sizeof(nack));
    if (payload != NULL)
    {
        payload->Reference();
    }
}

TcpMessage::~TcpMessage()
{
    if (payload != NULL && payload->Dereference())
    {
        delete payload;
    }
}
