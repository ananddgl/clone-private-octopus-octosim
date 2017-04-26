#include <stdlib.h>
#include "../octosimlib/IModels.h"
#include "../octosimlib/SimulationLoop.h"
#include "../octosimlib/DnsRecursive.h"
#include "../octosimlib/DnsStub.h"
#include "../octosimlib/DnsUdpTransport.h"
#include "../octosimlib/LossyLink.h"
#include "TestModels.h"
#include "DnsUdpSimTest.h"



DnsUdpSimTest::DnsUdpSimTest()
{
}


DnsUdpSimTest::~DnsUdpSimTest()
{
}

bool DnsUdpSimTest::DnsUdpSimDoTest()
{
    bool ret = DoOneTest(1, 7500, 0);

    if (ret)
    {
        ret = DoOneTest(10, 7500, 0);
    }

    if (ret)
    {
        ret = DoOneTest(10, 7500, 0.2);
    }

    if (ret)
    {
        ret = DoOneTest(100, 7500, 0.1);
    }

    return ret;
}

bool DnsUdpSimTest::DoOneTest(int nbPackets, int delay, double lossRate)
{
    bool ret = true;

    SimulationLoop * loop = new SimulationLoop();
    TestSimpleDelay * arrival_process = new TestSimpleDelay(5000, loop);
    TestSimpleDelay * authoritative_process = new TestSimpleDelay(3000, loop);
    DnsStub * stub = new DnsStub(loop, NULL, nbPackets, arrival_process);
    DnsRecursive * recursive = new DnsRecursive(loop, authoritative_process);
    DnsUdpTransport * transport1 = new DnsUdpTransport(loop);
    DnsUdpTransport * transport2 = new DnsUdpTransport(loop);
    LossyLink * path1 = new LossyLink(loop, lossRate, delay);
    LossyLink * path2 = new LossyLink(loop, lossRate, delay);


    if (loop == NULL || arrival_process == NULL || authoritative_process == NULL ||
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

        loop->RequestTimer(0, stub);

        while (loop->DoLoop());

        if (stub->nb_packets_sent != stub->nb_packets_to_send ||
            stub->nb_transactions_complete != stub->nb_packets_sent)
        {
            ret = false;
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
    if (loop != NULL)
        delete loop;

    return ret;
}
