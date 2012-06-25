// ADassert.hpp:  Include File
//
// Copyright 2007 Ian Gordon
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(__ADASSERT_HPP__)
#define __ADASSERT_HPP__

#if !defined(__POSIX__) && !defined(WIN32)
#error Must #define either WIN32 or __POSIX__
#endif
#if defined(__POSIX__) && defined(WIN32)
#error Cannot #define both WIN32 and __POSIX__
#endif

#include <cassert>

#include "ADlogfunc.hpp"

//TODO Make sure ad::Assert is valid

namespace ad
{


/////////////////////////////////////////////////////////////////////////////

#if defined (__POSIX__)

	void AssertFailed(const char* szFilename, const int line);
#define Assert(boolToAssert)  do { if (!(boolToAssert)) ad::AssertFailed(__FILE__, __LINE__); } while(0)
//NOTE while(0) is appended to handle dangling ';' at end of statement "Assert(false);"

#endif // defined (__POSIX__)

/////////////////////////////////////////////////////////////////////////////
	
#if defined(WIN32)

//TODO Handle FILENAME and LINE NUMBER

#pragma warning(push) 
#pragma warning(disable: 4389)

#define Assert(boolSkipAssert) 	{ if (!(boolSkipAssert) ) { _asm{ int 3 }; Log(ad::eERROR, "ASSERT Failed"); } }

#pragma warning(pop)

#endif // defined(WIN32)
}

#endif // !defined(__ADASSERT_HPP__)
