// ADtimefunc.cpp:  Time Func Source File
//
// Copyright 2007 Ian Gordon
//
/////////////////////////////////////////////////////////////////////////////

#include "ADtimefunc.hpp"
#include <boost/date_time/gregorian/gregorian.hpp> //include all types plus i/o
#include <boost/date_time/posix_time/posix_time.hpp>
#include <string>
#include <sstream>


#if defined (WIN32)
#define NOMINMAX  //Stop definition of min / max macros // http://www.devx.com/tips/Tip/14540
#include <windows.h>


//TODO verify
uint64_t ad::GetUTCSeconds(void)
{
	FILETIME filetimeCurrent; // 64-bit value representing the number of 100-nanosecond intervals since January 1, 1601 (UTC).
	GetSystemTimeAsFileTime(&filetimeCurrent);

	uint64_t nTime(0);
	memcpy(&nTime, &filetimeCurrent, sizeof(nTime) );
	nTime = nTime / 10000000; // Number of 100-nanosecond intervals in a second
	nTime = nTime + 11644473600; // 369 Years to reach January 1, 1970  (369 * 356 * 24 * 60 * 60)
	//TODO Check for leap seconds in the above value then make a note

	return nTime;
}


#elif defined(__POSIX__)

#include <sys/time.h>
#include <unistd.h>


//TODO verify
uint64_t ad::GetUTCSeconds(void)
{
	time_t timeCurrent;
	timeCurrent = time(NULL);

	return static_cast<uint64_t>(timeCurrent);
}

#endif



void ad::SleepSeconds(uint32_t cSeconds)
{
#if defined (WIN32)
	Sleep(cSeconds * 1000);
#elif defined (__POSIX__)
	sleep(cSeconds);
#endif // __POSIX
}

std::string ad::GetCurrentDateTimeSS(void)
{
//	boost::gregorian::date d(boost::gregorian::day_clock::local_day());
//	boost::posix_time::ptime t(boost::posix_time::second_clock::local_time());
	boost::posix_time::ptime t(boost::posix_time::second_clock::universal_time());

	return to_simple_string(t);
}

