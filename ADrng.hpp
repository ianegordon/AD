// ADskeleton.hpp:  FIXME Include File
//
// Copyright 2007 Ian Gordon
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(__ADPRANDOM_HPP__)
#define __ADPRANDOM_HPP__

#if !defined(__POSIX__) && !defined(WIN32)
#error Must #define either WIN32 or __POSIX__
#endif
#if defined(__POSIX__) && defined(WIN32)
#error Cannot #define both WIN32 and __POSIX__
#endif


#if defined(__POSIX__)
#include <stdlib.h>
#include <stdint.h>
#elif defined(WIN32)
#include "pstdint.h"
#else
#error Must #define WIN32 or __POSIX__
#endif //defined(WIN32)


namespace ad
{

// constants for MT19937:
static const int MERS_N(624);
static const int MERS_M(397);
static const int MERS_R(31);
static const int MERS_U(11);
static const int MERS_S(7);
static const int MERS_T(15);
static const int MERS_L(18);
static const uint32_t MERS_A(0x9908B0DF);
static const uint32_t MERS_B(0x9D2C5680);
static const uint32_t MERS_C(0xEFC60000);
static const uint32_t MAG01[] = { 0x0, MERS_A };

class CRNG
{
public:
	CRNG(void);
	~CRNG(void);

	void Seed(const uint32_t& nSeed);
	uint32_t Generate(void);

	uint32_t GetSeed(void) const { return m_nSeed; }
	unsigned int GetNumbersGenerated(void) const { return m_cGenerated; }

protected:
	uint32_t m_nSeed;
	unsigned int m_cGenerated;

	uint32_t mt[MERS_N];             // state vector
	int mti;                             // index into mt
};

//const unsigned int CRNG::mag01[2] = { 0x0, MERS_A }; //STATIC 

} //namespace ad

#endif // !defined(__ADPRANDOM_HPP__)
