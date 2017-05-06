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
    ack_time(0),
    recursive_time(0),
    authoritative_delay(0),
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
        response->ack_time = transmit_time;
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
        fprintf(LogFile, "%sDNS C:%llu, T:%llu, A:%llu, %s, QId:%llu, UT: %llu, Id:%llx\n",
            (dropped) ? "Dropped " : "", creation_time, transmit_time, ack_time,
            CodeToText(), qtarget_id, current_udp_timer, query_id);
    }
}
