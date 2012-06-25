#include "ADrng.hpp"

/* A C-program for MT19937: Integer version (1999/10/28)          */
/*  RNGGenerate() generates one pseudorandom unsigned integer (32bit) */
/* which is uniformly distributed among 0 to 2^32-1  for each     */
/* call. RNGSeed(seed) sets initial values to the working area   */
/* of 624 words. Before RNGGenerate(), RNGSeed(seed) must be         */
/* called once. (seed is any 32-bit integer.)                     */
/*   Coded by Takuji Nishimura, considering the suggestions by    */
/* Topher Cooper and Marc Rieffel in July-Aug. 1997.              */

/* This library is free software under the Artistic license:       */
/* see the file COPYING distributed together with this code.       */
/* For the verification of the code, its output sequence file      */
/* mt19937int.out is attached (2001/4/2)                           */

/* Copyright (C) 1997, 1999 Makoto Matsumoto and Takuji Nishimura. */
/* Any feedback is very welcome. For any question, comments,       */
/* see http://www.math.keio.ac.jp/matumoto/emt.html or email       */
/* matumoto@math.keio.ac.jp                                        */

/* REFERENCE                                                       */
/* M. Matsumoto and T. Nishimura,                                  */
/* "Mersenne Twister: A 623-Dimensionally Equidistributed Uniform  */
/* Pseudo-Random Number Generator",                                */
/* ACM Transactions on Modeling and Computer Simulation,           */
/* Vol. 8, No. 1, January 1998, pp 3--30.                          */

using namespace ad;

CRNG::CRNG(void)
{
	this->Seed(0xfade1976); /* a default initial seed is used   */
}

CRNG::~CRNG(void)
{
}

/* Initializing the array with a seed */
void CRNG::Seed(const uint32_t& nSeed)
{
	// re-seed generator
	mt[0]= nSeed;
	for (mti = 1; MERS_N != mti; ++mti) 
	{
		mt[mti] = (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30) ) + mti);
	}

	//NOTE Reset m_nSeed to the input value
	m_nSeed = nSeed;
	m_cGenerated = 0;
}

uint32_t CRNG::Generate(void) 
{
	// generate 32 random-ish bits
	unsigned int y;

	if (mti >= MERS_N) 
	{
		// generate MERS_N words at one time
		const unsigned int LOWER_MASK = (1LU << MERS_R) - 1; // lower MERS_R bits
		//IEG	New style removes warning
		//const unsigned int UPPER_MASK = -1L  << MERS_R;      // upper (32 - MERS_R) bits
		const unsigned int UPPER_MASK = 0xffffffff  << MERS_R;      // upper (32 - MERS_R) bits

		int kk;
		for (kk=0; kk < MERS_N-MERS_M; kk++) 
		{    
			y = (mt[kk] & UPPER_MASK) | (mt[kk+1] & LOWER_MASK);
			mt[kk] = mt[kk+MERS_M] ^ (y >> 1) ^ MAG01[y & 1];
		}

		for (; kk < MERS_N-1; kk++) 
		{    
			y = (mt[kk] & UPPER_MASK) | (mt[kk+1] & LOWER_MASK);
			mt[kk] = mt[kk+(MERS_M-MERS_N)] ^ (y >> 1) ^ MAG01[y & 1];
		}      

		y = (mt[MERS_N-1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
		mt[MERS_N-1] = mt[MERS_M-1] ^ (y >> 1) ^ MAG01[y & 1];
		mti = 0;
	}

	y = mt[mti++];

	// Tempering (May be omitted):
	y ^=  y >> MERS_U;
	y ^= (y << MERS_S) & MERS_B;
	y ^= (y << MERS_T) & MERS_C;
	y ^=  y >> MERS_L;

	++m_cGenerated;

	return y;
}
