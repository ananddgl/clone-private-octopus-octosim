#include <stdlib.h>
#include "DnsMessage.h"


DnsMessage::DnsMessage(
    unsigned long long creation_time,
    unsigned long long q_id, 
    unsigned long long qt_id)
    :
    creation_time(creation_time),
    query_id(q_id),
    qtarget_id(qt_id),
    messageCode(query),
    next_in_queue(NULL),
    transmit_time(0),
    current_udp_timer(0),
    udp_repeat_counter(0),
    ISimMessage()
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
        messageCode = DnsMessageCode::response;
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
