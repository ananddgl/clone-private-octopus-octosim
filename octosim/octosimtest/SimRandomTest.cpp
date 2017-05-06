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

#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include "../octosimlib/SimulationRandom.h"
#include "SimRandomTest.h"



SimRandomTest::SimRandomTest()
{
}


SimRandomTest::~SimRandomTest()
{
}

bool SimRandomTest::SimRandomDoTest()
{
    bool ret = true;

    if (rnd != NULL)
    {
        rnd = new SimulationRandom();

        ret = (rnd != NULL);
    }

    if (ret)
    {
        ret = ZeroToOne();
    }

    for (int i = 4; ret && i >= 1; i--)
    {
        if (!CountTheOneTest(i))
        {
            ret = false;
        }
    }

    if (rnd != NULL)
    {
        delete rnd;
    }

    return ret;
}

/*
 * The count-the-1's test on a stream of bytes: 
 * Consider the file under test as a stream of bytes (four per 32-bit integer). 
 * Each byte can contain from none to eight 1s, with probabilities 1, 8, 28, 56, 
 * 70, 56, 28, 8, 1 over 256. Now let the stream of bytes provide a string of 
 * overlapping 5-letter words, each "letter" taking values A, B, C, D, E. The 
 * letters are determined by the number of 1s in a byte 0, 1, or 2 yield A, 3 
 * yields B, 4 yields C, 5 yields D and 6, 7 or 8 yield E. Thus we have a monkey 
 * at a typewriter hitting five keys with various probabilities (37, 56, 70, 56, 
 * 37 over 256). There are 5^5 (3125) possible 5-letter words, and from a string of 
 * 256,000 (overlapping) 5-letter words, counts are made on the frequencies 
 * for each word. The quadratic form in the weak inverse of the covariance matrix 
 * of the cell counts provides a chisquare test Q5–Q4, the difference of the naive 
 * Pearson sums of (OBS − EXP)2/EXP on counts for 5- and 4-letter cell counts.
 */

static const unsigned int nb_blanks_hex[16] =
{
    0, 1,
    1, 2,
    1, 2, 2, 3,
    1, 2, 2, 3, 2, 3, 3, 4
};

static const unsigned int letter_by_blank[9] =
{
    0, 0, 0, 1, 2, 3, 4, 4, 4
};

static const double blank_probability[9] = {
    1.0 / 256.0, 8.0 / 256.0, 28.0 / 256.0, 56.0 / 256.0, 70.0 / 256.0, 56.0 / 256.0, 28.0 / 256.0, 8.0 / 256.0, 1 / 256.0 };

static const double letter_probability[5] = {
    blank_probability[0] + blank_probability[1] + blank_probability[2],
    blank_probability[3], blank_probability[4], blank_probability[5],
    blank_probability[6] + blank_probability[7] + blank_probability[8]
};

bool SimRandomTest::CountTheOneTest(int used_octets_per_longlong)
{
    bool ret = true;

    /* Allocate string of 256000 chars */
    unsigned char * str256000 = new unsigned char[256000];
    unsigned int sum256[256];
    unsigned int sumblanks[9];
    unsigned int sumletters[5];
    unsigned int * sumwords = new unsigned int[3125];
    double * word_probability = new double[3125];

    if (str256000 == NULL || sumwords == NULL || word_probability == NULL)
    {
        ret = false;
    }
    else
    {
        memset(sum256, 0, sizeof(sum256));
        memset(sumblanks, 0, sizeof(sumblanks));
        memset(sumletters, 0, sizeof(sumletters));
        memset(sumwords, 0, 3125 * sizeof(unsigned int));

        for (int i = 0; i < 3125; i++)
        {
            double x = 1.0;
            int v = i;

            for (int j = 0; j < 5; j++)
            {
                int letter = v % 5;
                x *= letter_probability[letter];
                v /= 5;
            }
            word_probability[i] = x;
        }

        /* fill the characters based on the random draw */
        /* Compute the bits frequency */
        /* Compute the letters frequency */
        /* Document the letters in the table */
        for (int i = 0; i < 256000;)
        {
            unsigned long x = rnd->GetRandom();

            for (int j = 0; j < used_octets_per_longlong && i < 256000; j++, i++)
            {
                unsigned char u = (unsigned char)(x & 0xFF);
                x >>= 8;
                sum256[u] += 1;
                int nbb = nb_blanks_hex[u & 15] + nb_blanks_hex[(u >> 4) & 15];
                sumblanks[nbb] += 1;
                int letter = letter_by_blank[nbb];
                sumletters[letter] += 1;
                str256000[i] = letter;
            }
        }
        /* Now count the words */
        for (int i = 4; i < 256000; i++)
        {
            int w = str256000[i];

            for (int j = 1; j < 5; j++)
            {
                w *= 5;
                w += str256000[i - j];
            }
            sumwords[w] += 1;
        }

        /* Perform statistical tests on the totals */
        int pass = 4;

        if (!FrequencyTest(sum256, 256, 256000, NULL))
        {
            pass -= 1;
        }

        if (!FrequencyTest(sumblanks, 9, 256000, blank_probability))
        {
            pass -= 1;
        }
        
        if (!FrequencyTest(sumletters, 5, 256000, letter_probability))
        {
            pass -= 1;
        }
        
        if (!FrequencyTest(sumwords, 3125, 256000-4, word_probability))
        {
            pass -= 1;
        }

        if (pass < 3)
        {
            ret = false;
        }
    }

    if (str256000 != NULL)
    {
        delete[] str256000;
    }
    if (sumwords != NULL)
    {
        delete[] sumwords;
    }
    if (word_probability != NULL)
    {
        delete[] word_probability;
    }
    return ret;
}

double ks_alpha[5] = { 0.01, 0.05, 0.1, 0.15, 0.2 };
double ks_large[5] = { 1.63, 1.36, 1.22, 1.14, 1.07 };

bool SimRandomTest::FrequencyTest(unsigned int * observed, int nb_buckets, int nb_samples, 
    const double * bucket_probability)
{
    double cfd_observed = 0;
    double cfd_theory = 0;
    double cfd_delta_max = 0;

    /* Compute theoretical and practical CFD, and difference */
    for (int i = 0; i < nb_buckets; i++)
    {
        double bucket_p = observed[i];
        bucket_p /= nb_samples;
        double theory_p = 1.0;
        if (bucket_probability == NULL)
        {
            theory_p /= nb_buckets;
        }
        else
        {
            theory_p = bucket_probability[i];
        }
        cfd_observed += bucket_p;
        cfd_theory += theory_p;
        double cfd_delta = abs(cfd_observed - cfd_theory);
        cfd_delta_max = std::max(cfd_delta_max, cfd_delta);
    }
    /*
     * Compute Kalpha per Kolmogorov table
     */
    double dnalpha = ks_large[0];
    dnalpha /= sqrt((double)nb_samples);

    return dnalpha >= cfd_delta_max;
}

bool SimRandomTest::ZeroToOne()
{
    bool ret = true;
    FILE * F = NULL;

    ret = (fopen_s(&F, "zerotoone.csv", "w") == 0);

    for (int i = 0; ret && i < 1024; i++)
    {
        double d = rnd->GetZeroToOne();
        fprintf(F, """%f"",\n", d);
    }

    if (F != NULL)
    {
        fclose(F);
    }
    return ret;
}

