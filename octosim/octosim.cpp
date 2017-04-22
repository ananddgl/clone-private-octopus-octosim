// octosim.cpp : testing a simple transport/transaction model.
//
// The model starts with an arrival process.
// Once a transaction arrives, it is passed to the client input, where a 
// process keeps track of ongoing transactions. The client process is represented
// by a C++ object of class "transport" and will simulate the transport
// algorithm. We are interested in three transports:
// * Basic UDP, where the ack is the completion of the end-to-end transaction.
// * TCP/TLS, with its own ack process plus head of queue blocking.
// * QUIC, with its ack process and the parallel streams.
// For TCP and QUIC, we assume RACK. For UDP, we assume exponential back off.
// Transport can subscribe to a timer service, which will call them back.
// We will not simulate flow control -- we assume that there is plenty of
// bandwidth for the DNS.
// The transport creates a packet, that embeds transport logic and app logic.
// The packet is passed to a link, which keeps it for some time (queue and delay)
// and may include some losses. If a packet is lost, the corresponding object 
// is removed. Otherwise, it is delivered to the receiving transport.
// At the received transport, the application object is passed to the
// receiving app, which produces a response after a programmed delay (need a delay model).
//
// That means 5 objects:
// * Source App (client)
// * Source Transport
// * Link
// * Dest Transport
// * Dest App (server)
//
// The communication is carried via an event queue. Each event has a simulated time.
//
// The simulation stops when the source app does not have any more traffic -- based on
// time or number of transactions.
//
// The traces are written to a CSV file by the Source App.
//

#include "stdafx.h"


int main()
{
    /* Testing a simple transport model. */
    return 0;
}

