#include "stdafx.h"
#include "CppUnitTest.h"
#include "../octosimtest/MessageTreetest.h"
#include "../octosimtest/SimRandomTest.h"
#include "../octosimtest/SimLoopTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SimLibUnitTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMessageTree)
		{
			// TODO: Your test code here
            MessageTreetest mtt;

            bool ret = mtt.MessageTreeDoTest();

            Assert::IsTrue(ret, L"Message tree test should succeed"); (ret == true);
		}

        TEST_METHOD(TestSimRandom)
        {
            // TODO: Your test code here
            SimRandomTest srt;

            bool ret = srt.SimRandomDoTest();

            Assert::IsTrue(ret, L"Sim random test should succeed"); (ret == true);
        }

        TEST_METHOD(TestSimLoop)
        {
            // TODO: Your test code here
            SimLoopTest slt;

            bool ret = slt.SimLoopDoTest();

            Assert::IsTrue(ret, L"Sim loop test should succeed"); (ret == true);
        }
	};
}