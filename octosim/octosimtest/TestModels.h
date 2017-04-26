#pragma once

#include "../octosimlib/IModels.h"

class TestMessage : public ISimMessage
{
public:
    TestMessage(int n);
    ~TestMessage() {}

    int message_number;
};

class TestSource : public IApplication
{
public:
    int nb_packets_to_send;
    int nb_packets_sent;
    unsigned int message_delay;

    TestSource(SimulationLoop * loop);

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
    int delay;

    TestSimpleDelay(int delay, SimulationLoop * loop);
    ~TestSimpleDelay();

    // Inherited via IDelayDistribution
    virtual void Input(ISimMessage * message) override;

    virtual unsigned long long NextDelay() override;

};