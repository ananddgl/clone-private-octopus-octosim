#include "../octosimlib/WriteOnceHash.h"
#include "../octosimlib/SimulationRandom.h"
#include "WriteOnceTest.h"



WriteOnceTest::WriteOnceTest()
{
}


WriteOnceTest::~WriteOnceTest()
{
}

bool WriteOnceTest::WriteOnceDoTest()
{
    bool ret = true;

    ret = DoOneTest(true, 10);

    if (ret)
    {
        ret = DoOneTest(true, 100);
    }

    if (ret)
    {
        ret = DoOneTest(true, 1000);
    }

    if (ret)
    {
        ret = DoOneTest(false, 1000);
    }

    return ret;
}

bool WriteOnceTest::DoOneTest(bool linearValues, unsigned int nbTests)
{
    bool ret = true;
    WriteOnceHash testHash;
    SimulationRandom rnd;
    unsigned long long retrievedValue;
    unsigned long long testNumbers[1000];
    unsigned long long testValues[1000];
    unsigned long long noSuchNumbers[100];
    

    if (nbTests > 1000)
    {
        return false;
    }

    for (unsigned int i = 0; ret && i < nbTests; i++)
    {
        testNumbers[i] = (linearValues) ? i + 1 : rnd.GetRandom64();
        testValues[i] = (linearValues) ? i + 1 : rnd.GetRandom64();
        if (!testHash.Insert(testNumbers[i], testValues[i]))
            ret = false;
    }

    for (unsigned int i = 0; ret && i < nbTests; i++)
    {
        if (!testHash.Retrieve(testNumbers[i], &retrievedValue))
            ret = false;
        else if (retrievedValue != testValues[i])
            ret = false;
    }

    for (unsigned int i = 0; ret && i < nbTests; i++)
    {
        if (testHash.Insert(testNumbers[i], 123456000ull + i))
            ret = false;
    }

    for (unsigned int i = 0; ret && i < 100; i++)
    {
        noSuchNumbers[i] = (linearValues)? i+1001:rnd.GetRandom64();
        if (testHash.Retrieve(noSuchNumbers[i], &retrievedValue))
            ret = false;
    }

    if (ret && testHash.GetCount() != nbTests)
    {
        ret = false;
    }

    if (ret && testHash.GetSize() > 8*nbTests && testHash.GetSize() > 128)
    {
        ret = false;
    }


    return ret;
}
