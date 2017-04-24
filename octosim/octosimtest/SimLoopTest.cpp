#include <stdlib.h>
#include "../octosimlib/IModels.h"
#include "../octosimlib/SimulationLoop.h"
#include "TestModels.h"
#include "SimLoopTest.h"



SimLoopTest::SimLoopTest()
{
}


SimLoopTest::~SimLoopTest()
{
}

bool SimLoopTest::SimLoopDoTest()
{
    bool ret = true;

    SimulationLoop * loop = new SimulationLoop();
    TestSource * source = new TestSource(loop);
    TestSink * sink = new TestSink(loop);
    TestPassthroughTransport * transport1 = new TestPassthroughTransport(loop);
    TestPassthroughTransport * transport2 = new TestPassthroughTransport(loop);
    TestLink * path = new TestLink(loop);

    if (loop == NULL || source == NULL || sink == NULL ||
        transport1 == NULL || transport2 == NULL || path == NULL)
    {
        ret = false;
    }
    else
    {
        /* Creating a simple one way network. */
        source->SetTransport(transport1);
        transport1->SetPath(path);
        path->SetTransport(transport2);
        transport2->SetApplication(sink);

        ret = loop->Init();

        loop->RequestTimer(0, source);

        while (loop->DoLoop());

        if (source->nb_packets_sent != source->nb_packets_to_send ||
            sink->nb_packets_received != source->nb_packets_sent)
        {
            ret = false;
        }
    }

    if (path != NULL)
        delete path;
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
