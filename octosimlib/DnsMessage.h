#pragma once
#include "IModels.h"

enum DnsMessageCode {
    query,
    response,
    error_too_long,
    error_failed
};

class DnsMessage : public ISimMessage
{
public:
    DnsMessage(
        unsigned long long creation_time,
        unsigned long long query_id,
        unsigned long long qtarget_id);
    ~DnsMessage();

    DnsMessage * CreateResponse();
    const char * CodeToText();

    unsigned long long creation_time;
    unsigned long long query_id;
    unsigned long long qtarget_id;
    DnsMessage * next_in_queue;
    unsigned long long transmit_time;
    unsigned long long current_udp_timer;
    int udp_repeat_counter;
    DnsMessageCode messageCode;

};

