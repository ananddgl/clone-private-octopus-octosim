#pragma once
class SimulationLoop;

class SimLoopTest
{
public:
    SimLoopTest();
    ~SimLoopTest();

    bool SimLoopDoTest();

private:
    SimulationLoop * loop;
};

