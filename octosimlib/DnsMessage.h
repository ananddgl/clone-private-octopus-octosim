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

