#pragma once
class TcpSimReorderQueue;
class TestMessage;
class TcpMessage;

class TcpReorderTest
{
public:
    TcpReorderTest();
    ~TcpReorderTest();

    bool TcpReorderDoTest();

    bool CheckDelivery(
        int range_last,
        TcpSimReorderQueue * tsrq,
        TestMessage ** testMessage,
        bool *testMessageReceived,
        bool * testMessageDelivered);

    bool CheckAcks(
        int range_last,
        TcpSimReorderQueue * tsrq,
        TcpMessage * tm,
        bool *testMessageReceived);
};

