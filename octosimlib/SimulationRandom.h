#pragma once
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
*
* The xorshift128plus algorithm and code are copied from the Wikipedia entry:
* https://en.wikipedia.org/wiki/Xorshift
* The algorithm was derived by Sebastiano Vigna from the original xorshift
* algorithm designed by George Marsaglia.
*
*
* The Blowfish encryption algorithm was designed by Bruce Schneier and is
* documented here: https://www.schneier.com/academic/blowfish/.
*
* The Blowfish code is copied and adapated to C++ from the original
* code by Bruce Schneier, available at:
* https://www.schneier.com/code/bfsh-sch.zip
*/

/*
 * Reproducible Random number generation. 
 * The same seed always generates the same sequence.
 * The sequence is meant to have good properties, i.e. random distribution, etc.
 * Implemented as two step: basic internal generation, followed by
 * encryption. The purpose of encryption is to hide the internal cycles,
 * and thus avoid artefacts. Also, by only retaining the least 
 * significant 32 bits of the result, we ensure that values can be
 * repeated -- while the 64 bit would be a simple sequence.
 *
 * The basic generation is an XOR shift generator.
 * The encryption is a reduced pass implementation of Blowfish,
 * using only 4 passes instead of the nominal 16, for speed.
 */
class SimulationRandom
{
public:
    SimulationRandom();
    ~SimulationRandom();

    void Init(unsigned char * seed, unsigned int seed_length);

    unsigned long GetRandom();

    unsigned long long GetRandom64();

    unsigned long GetRandomUniform(unsigned long range_max);

    double GetZeroToOne();


private:
    unsigned long long state[2];
    unsigned long blowfish_P[18];
    const unsigned long long blowfish_nbrounds = 4;

    unsigned long long xorshift128plus(void);
    unsigned long blowfish_F(unsigned long x);
    unsigned long long blowfish_encipher(unsigned long long x);
};

