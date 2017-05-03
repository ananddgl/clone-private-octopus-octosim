#include <stdlib.h>
#include <stdio.h>
#include "../octosimlib/SimulationRandom.h"
#include "../octosimlib/RandomByCFD.h"
#include "CfdRandomTest.h"



CfdRandomTest::CfdRandomTest()
{
}


CfdRandomTest::~CfdRandomTest()
{
}

double test_cfd_p[] = {
    0.025, 0.026, 0.027, 0.028, 0.029, 0.03, 0.032, 0.034, 0.036, 0.038, 0.04,
    0.042, 0.044, 0.046, 0.048, 0.05, 0.06, 0.07, 0.08, 0.09, 0.10, 0.11,
    0.12, 0.13, 0.14, 0.15, 0.16, 0.17, 0.18, 0.19, 0.20, 0.25, 0.30,
    0.35, 0.40, 0.45, 0.50, 0.55, 0.60, 0.65, 0.70, 0.75, 0.80, 0.85,
    0.90, 0.91, 0.92, 0.93, 0.94, 0.95, 0.96, 0.97, 0.98, 0.99
};

unsigned int nb_cfd_p = sizeof(test_cfd_p) / sizeof(double);

unsigned long long test_cfd_v[] = {
    1ull,
    1ull,
    1ull,
    1ull,
    1ull,
    1ull,
    2ull,
    5ull,
    13ull,
    25ull,
    28ull,
    32ull,
    39ull,
    51ull,
    61ull,
    65ull,
    142ull,
    216ull,
    435ull,
    506ull,
    601ull,
    685ull,
    912ull,
    996ull,
    1127ull,
    1274ull,
    1300ull,
    1314ull,
    1330ull,
    1354ull,
    1366ull,
    1576ull,
    1778ull,
    2088ull,
    2728ull,
    4169ull,
    7782ull,
    12837ull,
    20098ull,
    27543ull,
    33724ull,
    40348ull,
    48734ull,
    80779ull,
    218107ull,
    287281ull,
    359721ull,
    590182ull,
    769649ull,
    938782ull,
    1476172ull,
    2329637ull,
    6463161ull,
    13305491ull,
};

unsigned int nb_cfd_v = sizeof(test_cfd_v) / sizeof(unsigned long long);

bool CfdRandomTest::CfdRandomDoTest()
{
    SimulationRandom rnd;
    RandomByCFD cfdr;
    bool ret = (nb_cfd_v == nb_cfd_p);
    unsigned long long test_values[1024];
    FILE * F = NULL;

    if (ret && (fopen_s(&F, "cftRnd.csv", "w") != 0))
    {
        ret = false;
    }

    if (ret)
    {
        ret = cfdr.Init(&rnd, nb_cfd_p, test_cfd_p, test_cfd_v);
    }

    for (int i = 0; ret && i < 1024; i++)
    {
        test_values[i] = cfdr.GetCfdRandom();
        fprintf(F, """%llu"",\n", test_values[i]);
    }

    if (F != NULL)
    {
        fclose(F);
    }

    return ret;
}
