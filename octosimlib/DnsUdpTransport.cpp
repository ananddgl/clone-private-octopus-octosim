#include "DnsUdpTransport.h"

/*
 * Simulate the "DNS transport with exponential backoff"
 * used over UDP.
 *
 * Upon sending a message, keep a copy.
 * 
 *
 * TO DO: add support for the fallback to TCP.
 */



DnsUdpTransport::DnsUdpTransport(SimulationLoop* loop)
{
}

DnsUdpTransport::~DnsUdpTransport()
{
}
