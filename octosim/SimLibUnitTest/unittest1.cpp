#include "stdafx.h"
#include "CppUnitTest.h"
#include "../octosimtest/MessageTreetest.h"

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

	};
}