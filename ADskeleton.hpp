// ADskeleton.hpp:  FIXME Include File
//
// Copyright 2009 Ian Gordon
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(__ADSKELETON_HPP__)
#define __ADSKELETON_HPP__

#if 0 //KM Before use is new classes (And #endif below)

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

	uint32_t DummyFunc(void);

} //namespace ad

#endif // 0  //KM Before use is new classes

#endif // !defined(__ADSKELETON_HPP__)
