#include "stdafx.h"
#include "CppUnitTest.h"
#include "../octosimtest/MessageTreetest.h"
#include "../octosimtest/SimRandomTest.h"
#include "../octosimtest/SimLoopTest.h"
#include "../octosimtest/WriteOnceTest.h"
#include "../octosimtest/DnsUdpSimTest.h"
#include "../octosimtest/TcpReorderTest.h"
#include "../octosimtest/TcpRetransmitTest.h"
#include "../octosimtest/DnsTcpSimTest.h"
#include "../octosimtest/SimpleTcpSimTest.h"
#include "../octosimtest/CfdRandomTest.h"

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

        TEST_METHOD(TestWriteOnce)
        {
            // TODO: Your test code here
            WriteOnceTest wot;

            bool ret = wot.WriteOnceDoTest();

            Assert::IsTrue(ret, L"Write once test should succeed"); (ret == true);
        }

        TEST_METHOD(TestDnsUdpSim)
        {
            // TODO: Your test code here
            DnsUdpSimTest dust;

            bool ret = dust.DnsUdpSimDoTest();

            Assert::IsTrue(ret, L"Dns Udp Sim test should succeed"); (ret == true);
        }

        TEST_METHOD(TestTcpReorder)
        {
            // TODO: Your test code here
            TcpReorderTest trt;

            bool ret = trt.TcpReorderDoTest();

            Assert::IsTrue(ret, L"Tcp reorder test should succeed"); (ret == true);
        }

        TEST_METHOD(TestTcpRetransmit)
        {
            // TODO: Your test code here
            TcpRetransmitTest trt;

            bool ret = trt.TcpRetransmitDoTest();

            Assert::IsTrue(ret, L"Tcp Retransmit test should succeed"); (ret == true);
        }

        TEST_METHOD(TestSimpleTcpSim)
        {
            // TODO: Your test code here
            SimpleTcpSimTest stst;

            bool ret = stst.SimpleTcpSimDoTest();

            Assert::IsTrue(ret, L"Simple Tcp Sim test should succeed"); (ret == true);
        }

        TEST_METHOD(TestDnsTcpSim)
        {
            // TODO: Your test code here
            DnsTcpSimTest dtst;

            bool ret = dtst.DnsTcpSimDoTest();

            Assert::IsTrue(ret, L"Dns Tcp Sim test should succeed"); (ret == true);
        }

        TEST_METHOD(TestSimpleQuicSim)
        {
            // TODO: Your test code here
            SimpleTcpSimTest stst(true);

            bool ret = stst.SimpleTcpSimDoTest();

            Assert::IsTrue(ret, L"Simple Quic Sim test should succeed"); (ret == true);
        }


        TEST_METHOD(TestDnsQuicSim)
        {
            // TODO: Your test code here
            DnsTcpSimTest dtst(true);

            bool ret = dtst.DnsTcpSimDoTest();

            Assert::IsTrue(ret, L"Dns Quic Sim test should succeed"); (ret == true);
        }

        TEST_METHOD(TestCfdRandom)
        {
            // TODO: Your test code here
            CfdRandomTest crt;

            bool ret = crt.CfdRandomDoTest();

            Assert::IsTrue(ret, L"Cfd Random test should succeed"); (ret == true);
        }
	};
}