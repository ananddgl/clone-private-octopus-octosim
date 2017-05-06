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

#include "../octosimlib/IModels.h"

class TestMessage : public ISimMessage
{
public:
    TestMessage(int n, unsigned int l = 100);
    ~TestMessage() {}

    virtual void Log(FILE* LogFile, bool dropped) override;

    int message_number;
};

class TestSource : public IApplication
{
public:
    int nb_packets_to_send;
    int nb_packets_sent;
    unsigned int message_delay;

    TestSource(SimulationLoop * loop, int nb_packets_to_send = 10);

    ~TestSource();

    // Inherited via IApplication
    virtual void Input(ISimMessage * message) override;
    virtual void TimerExpired(unsigned long long simulationTime) override;
};

class TestSink : public IApplication
{
public:
    int nb_packets_received;

    TestSink(SimulationLoop * loop);
    ~TestSink();

    // Inherited via IApplication
    virtual void Input(ISimMessage * message) override;
};

class TestPassthroughTransport : public ITransport
{
public:
    TestPassthroughTransport(SimulationLoop * loop);
    ~TestPassthroughTransport();

    // Inherited via ITransport

    virtual void ApplicationInput(ISimMessage * message) override;

    virtual void Input(ISimMessage * message) override;
};

class TestLink : public IPath
{
public:
    int delay;

    TestLink(SimulationLoop * loop);
    ~TestLink();

    // Inherited via IPath
    virtual void Input(ISimMessage * message) override;

};

class TestSimpleDelay : public IDelayDistribution
{
public:
    unsigned int delay;

    TestSimpleDelay(unsigned int delay, SimulationLoop * loop);
    ~TestSimpleDelay();

    // Inherited via IDelayDistribution
    virtual void Input(ISimMessage * message) override;

    virtual unsigned long long NextDelay() override;

};

class TestSimpleLength : public ILengthDistribution
{
public:
    unsigned int length;

    TestSimpleLength(unsigned int length, SimulationLoop * loop);
    ~TestSimpleLength();

    virtual unsigned int NextLength() override;

};