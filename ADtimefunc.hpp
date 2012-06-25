// ADtimefunc.hpp:  Time Func Include File
//
// Copyright 2007 Ian Gordon
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(__ADTIMEFUNC_HPP__)
#define __ADTIMEFUNC_HPP__

#if !defined(__POSIX__) && !defined(WIN32)
#error Must #define either WIN32 or __POSIX__
#endif
#if defined(__POSIX__) && defined(WIN32)
#error Cannot #define both WIN32 and __POSIX__
#endif

#include <string>

#if defined (__POSIX__)
#include <stdint.h>
#elif defined(WIN32)
#include "pstdint.h"
#endif

namespace ad
{
	//! Return System Time (UTC)
	/*!
	Returns the number of seconds since Midnight January 1, 1970 Coordinated Universal Time (UTC)
	*/
	uint64_t GetUTCSeconds(void);

	void SleepSeconds(uint32_t cSeconds);

	std::string GetCurrentDateTimeSS(void);
} //namespace ad

#endif // !defined(__ADTIMEFUNC_HPP__)
