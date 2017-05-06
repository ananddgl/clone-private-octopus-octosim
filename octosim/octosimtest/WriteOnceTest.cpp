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
