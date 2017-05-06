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
