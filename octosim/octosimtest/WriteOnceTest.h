#pragma once
class WriteOnceTest
{
public:
    WriteOnceTest();
    ~WriteOnceTest();

    bool WriteOnceDoTest();
    bool DoOneTest(bool linearValues, unsigned int nbTests);
};

