#pragma once
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

