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
#include "../octosimlib/IModels.h"
#include "../octosimlib/SimulationLoop.h"
#include "../octosimlib/RandomByCFD.h"
#include "../octosimlib/DnsRecursive.h"
#include "../octosimlib/DnsStub.h"
#include "../octosimlib/TcpSim.h"
#include "../octosimlib/DnsUdpTransport.h"
#include "../octosimlib/LossyLink.h"
#include "ReferenceCfd.h"
#include "DnsRealisticTest.h"



DnsRealisticTest::DnsRealisticTest()
{
}


DnsRealisticTest::~DnsRealisticTest()
{
}

const unsigned int nb_variants = 4;

bool DnsRealisticTest::DnsRealisticDoTest()
{
    bool ret = true;

    for (unsigned int i = 0; ret && i < nb_variants; i++)
    {
        ret = DnsRealisticOneTest(i, 1000, 15000, 0.01);
    }

    return ret;
}

char * csv_variant[] = {
    "dnsUdpRealistic.csv",
    "dnsTcpRealistic.csv",
    "dnsQuicRealistic.csv",
    "dnsQuic0RttRealistic.csv"
};

char * traces_variant[] = {
    "dnsUdpRealisticTraces.txt",
    "dnsTcpRealisticTraces.txt",
    "dnsQuicRealisticTraces.txt",
    "dnsQuic0RttRealisticTraces.txt"
};

bool DnsRealisticTest::DnsRealisticOneTest(unsigned int variant,
    unsigned int nb_packets, unsigned int delay, double lossRate)
{
    bool ret = variant < nb_variants;
    FILE * CsvLog = NULL;
    FILE * TraceLog = NULL;

    if (ret)
    {
        errno_t err = fopen_s(&TraceLog, traces_variant[variant], "w");

        ret = (err == 0);
    }

    if (ret)
    {
        errno_t err = fopen_s(&CsvLog, csv_variant[variant], "w");

        ret = (err == 0);
    }

    SimulationLoop * loop = new SimulationLoop(TraceLog);
    RandomDelayByCFD * arrival_process = new RandomDelayByCFD(loop);
    RandomDelayByCFD * authoritative_process = new RandomDelayByCFD(loop);
    RandomLengthByCFD * source_length = new RandomLengthByCFD(loop);
    RandomLengthByCFD * response_length = new RandomLengthByCFD(loop);

    DnsStub * stub = new DnsStub(loop, CsvLog, nb_packets, arrival_process, source_length);
    DnsRecursive * recursive = new DnsRecursive(loop, authoritative_process, response_length);
    ITransport * transport1 = NULL;
    ITransport * transport2 = NULL;

    switch (variant)
    {
    case 0:
        transport1 = new DnsUdpTransport(loop);
        transport2 = new DnsUdpTransport(loop);
        break;
    case 1:
        transport1 = new TcpSim(loop, false);
        transport2 = new TcpSim(loop, false);
        break;
    case 2:
        transport1 = new TcpSim(loop, true);
        transport2 = new TcpSim(loop, true);
        break;
    case 3:
        transport1 = new TcpSim(loop, true, true);
        transport2 = new TcpSim(loop, true, true);
        break;
    default:
        ret = false;
        break;
    }

    LossyLink * path1 = new LossyLink(loop, lossRate, delay);
    LossyLink * path2 = new LossyLink(loop, lossRate, delay);


    if (loop == NULL || arrival_process == NULL || authoritative_process == NULL ||
        source_length == NULL || response_length == NULL ||
        stub == NULL || recursive == NULL ||
        transport1 == NULL || transport2 == NULL || path1 == NULL || path2 == NULL)
    {
        ret = false;
    }
    else
    {
        /* Creating a two way network. */
        stub->SetTransport(transport1);
        transport1->SetApplication(stub);
        transport1->SetPath(path1);
        path1->SetTransport(transport2);
        recursive->SetTransport(transport2);
        transport2->SetApplication(recursive);
        transport2->SetPath(path2);
        path2->SetTransport(transport1);

        ret = loop->Init();

        if (ret)
        {
            ret = arrival_process->Init(ReferenceCfd::NbPoints(),
                ReferenceCfd::Proba(), ReferenceCfd::Arrival()) &&
                authoritative_process->Init(ReferenceCfd::NbPoints(),
                    ReferenceCfd::Proba(), ReferenceCfd::Authoritative()) &&
                source_length->Init(ReferenceCfd::NbPoints(),
                    ReferenceCfd::Proba(), ReferenceCfd::QueryLength()) &&
                response_length->Init(ReferenceCfd::NbPoints(),
                    ReferenceCfd::Proba(), ReferenceCfd::ResponseLength());
        }

        if (ret)
        {
            loop->RequestTimer(0, stub);

            while (loop->DoLoop());

            if (stub->nb_packets_sent != stub->nb_packets_to_send ||
                stub->nb_transactions_complete != stub->nb_packets_sent)
            {
                ret = false;
            }
        }
    }

    if (path1 != NULL)
        delete path1;
    if (path2 != NULL)
        delete path2;
    if (transport1 != NULL)
        delete transport1;
    if (transport2 != NULL)
        delete transport2;
    if (recursive != NULL)
        delete recursive;
    if (stub != NULL)
        delete stub;
    if (arrival_process != NULL)
        delete arrival_process;
    if (authoritative_process != NULL)
        delete authoritative_process;
    if (source_length != NULL)
        delete source_length;
    if (response_length != NULL)
        delete response_length;
    if (loop != NULL)
        delete loop;

    if (CsvLog != NULL)
    {
        fclose(CsvLog);
    }

    if (TraceLog != NULL)
    {
        fclose(TraceLog);
    }

    return ret;
}
