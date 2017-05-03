#include <stdlib.h>
#include "DnsMessage.h"


DnsMessage::DnsMessage(
    unsigned long long creation_time,
    unsigned long long q_id, 
    unsigned long long qt_id,
    unsigned int l)
    :
    creation_time(creation_time),
    query_id(q_id),
    qtarget_id(qt_id),
    messageCode(query),
    next_in_queue(NULL),
    transmit_time(0),
    current_udp_timer(0),
    udp_repeat_counter(0),
    query_length(0),
    ISimMessage(l)
{
}

DnsMessage::~DnsMessage()
{
}

DnsMessage * DnsMessage::CreateResponse()
{
    DnsMessage * response = new DnsMessage(
        creation_time, query_id, qtarget_id);

    if (response != NULL)
    {
        response->messageCode = DnsMessageCode::response;
        response->query_length = length;
    }

    return response;
}

const char * DnsMessage::CodeToText()
{
    char * ret = "Unknown";
    switch (messageCode)
    {
    case query:
        ret = "Query";
        break;
    case response:
        ret = "Response";
        break;
    case error_too_long:
        ret = "Too long";
        break;
    case error_failed:
        ret = "Failed";
        break;
    default:
        break;
    }
    return ret;
}

void DnsMessage::Log(FILE * LogFile, bool dropped)
{
    if (LogFile)
    {
        fprintf(LogFile, "%sDNS C:%llu, T:%llu, A:%llu, %s, Id:%llx, QId:%llx\n",
            (dropped) ? "Dropped " : "", creation_time, transmit_time, ack_time,
            CodeToText(), query_id, qtarget_id);
    }
}
