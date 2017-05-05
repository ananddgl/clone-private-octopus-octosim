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
        unsigned long long qtarget_id,
        unsigned int l = 100);
    ~DnsMessage();

    DnsMessage * CreateResponse();
    const char * CodeToText();
    virtual void Log(FILE* LogFile, bool dropped) override;

    unsigned long long creation_time;
    unsigned long long query_id;
    unsigned long long qtarget_id;
    DnsMessage * next_in_queue;
    unsigned long long transmit_time;
    unsigned long long ack_time; /* in responses, the time at which the last message was received */
    unsigned long long recursive_time; /* time at which the recursive processed the response */
    unsigned long long authoritative_delay; /* service delay by the authoritative */
    unsigned long long current_udp_timer;
    int udp_repeat_counter;
    unsigned int query_length;
    DnsMessageCode messageCode;
};

