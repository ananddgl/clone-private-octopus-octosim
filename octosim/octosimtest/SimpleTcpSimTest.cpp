#include <stdlib.h>
#include "../octosimlib/IModels.h"
#include "../octosimlib/SimulationLoop.h"
#include "../octosimlib/LossyLink.h"
#include "../octosimlib/TcpSim.h"
#include "TestModels.h"
#include "SimpleTcpSimTest.h"

SimpleTcpSimTest::SimpleTcpSimTest()
{
}

SimpleTcpSimTest::~SimpleTcpSimTest()
{
}

bool SimpleTcpSimTest::SimpleTcpSimDoTest()
{
    bool ret = DoOneTest(1, 2000, 7500, 0);

    if (ret)
    {
        ret = DoOneTest(10, 2000, 7500, 0);
    }

    if (ret)
    {
        ret = DoOneTest(10, 2000, 7500, 0.2);
    }

    if (ret)
    {
        ret = DoOneTest(400, 2000, 7500, 0.01);
    }

    return ret;
}

bool SimpleTcpSimTest::DoOneTest(int nbPackets, int delta_t, int delay, double lossRate)
{
    bool ret = true;

    SimulationLoop * loop = new SimulationLoop();
    TestSource * source = new TestSource(loop);
    TestSink * sink = new TestSink(loop);
    TcpSim * transport1 = new TcpSim(loop);
    TcpSim * transport2 = new TcpSim(loop);
    LossyLink * path1 = new LossyLink(loop, lossRate, delay);
    LossyLink * path2 = new LossyLink(loop, lossRate, delay);


    if (loop == NULL || source == NULL || sink == NULL ||
        transport1 == NULL || transport2 == NULL || path1 == NULL || path2 == NULL)
    {
        ret = false;
    }
    else
    {
        /* Creating a two way network. */
        source->SetTransport(transport1);
        transport1->SetApplication(source);
        transport1->SetPath(path1);
        path1->SetTransport(transport2);
        sink->SetTransport(transport2);
        transport2->SetApplication(sink);
        transport2->SetPath(path2);
        path2->SetTransport(transport1);


        ret = loop->Init();

        loop->RequestTimer(0, source);

        while (loop->DoLoop());

        if (source->nb_packets_sent != source->nb_packets_to_send ||
            sink->nb_packets_received != source->nb_packets_sent)
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
    if (sink != NULL)
        delete sink;
    if (source != NULL)
        delete source;
    if (loop != NULL)
        delete loop;

    return ret;
}