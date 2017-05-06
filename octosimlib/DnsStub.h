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
#include <stdio.h>
#include "WriteOnceHash.h"
#include "IModels.h"

class DnsStub : public IApplication
{
public:
    DnsStub(SimulationLoop * loop, FILE* FStats, 
        int nb_packets_to_send, 
        IDelayDistribution * source_process,
        ILengthDistribution * source_length_process = NULL);
    ~DnsStub();

    // Inherited via IApplication
    virtual void Input(ISimMessage * message) override;
    virtual void TimerExpired(unsigned long long simulationTime) override;

    FILE* FStats;
    int nb_packets_to_send;
    IDelayDistribution * source_process;
    ILengthDistribution * source_length_process;
    int nb_packets_sent;
    int nb_transactions_complete;
    int nb_duplicate_transactions;
    unsigned long long next_target_id;

    WriteOnceHash completedCache;

};

