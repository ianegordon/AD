/*
 *  adlog.cpp
 *  IOStreamLogger
 *
 *  Created by Ian Gordon on 3/14/08.
 *  Copyright 2008 Ian Gordon. All rights reserved.
 *
 */

#include "ADlog.hpp"

#if defined(WIN32)
#include <windows.h>
#endif // defined(WIN32)
#if defined(__POSIX__)
#include <stdarg.h>
#include <syslog.h>
#endif // defined(__POSIX__)


//using namespace ad;

//Old school
//static ad::basic_debugbuf<char> debugstream;
//std::ostream ad::log(&debugstream);

static ad::basic_debugbuf<char> debugstreamil;
ad::dostream ad::log(&debugstreamil);


const char* ad::LoggingLevelToSZ(ad::ELogLevel eLogLevel)
{
	switch (eLogLevel)
	{
		case ad::eLOG_ERROR: 
			return "ERROR ";
			break;
		case ad::eLOG_WARNING: 
			return "WARNING ";
			break;
		case ad::eLOG_INFO: 
			return "INFO ";
			break;
		case ad::eLOG_DEBUG: 
			return "DEBUG ";
			break;
		default:
			return "??? ";
			break;
	}
}

int ad::LoggingLevelToSyslogValue(ad::ELogLevel eLogLevel)
{
#if defined(__POSIX__)
	switch (eLogLevel)
	{
		case ad::eLOG_ERROR: 
			return LOG_ERR;
			break;
		case ad::eLOG_WARNING: 
			return LOG_WARNING;
			break;
		case ad::eLOG_INFO: 
			return LOG_INFO;
			break;
		case ad::eLOG_DEBUG: 
			return LOG_DEBUG;
			break;
		default:
			return LOG_DEBUG;
			break;
	}
#elif defined(WIN32)
	return 0;
#endif
}

// Having these functions in the source file instead of the header caused GCC4.2 to scream
#if defined(__POSIX__)
/*
template<>
void ad::basic_debugbuf<char>::output_debug_string(const char *text)
{
	if (text && *text)
	{
		std::cout << LoggingLevelToSZ(m_eLoggingLevel) << text << std::flush;
		syslog( LoggingLevelToSyslogValue(m_eLoggingLevel), text);
	}
}
 */
#elif defined(WIN32)
/*
template<>
void ad::basic_debugbuf<char>::output_debug_string(const char *text)
{
	std::cout << LoggingLevelToSZ(m_eLoggingLevel) << text << std::flush;
	
	::OutputDebugStringA(text);
}
*/
#endif


/* TODO widestring support?
template<>
void ad::basic_debugbuf<wchar_t>::output_debug_string(const wchar_t *text)
{
//    ::OutputDebugStringW(text);
//	ad::dout << text << std::flush;
	std::cout << text << std::flush;

	syslog(LOG_DEBUG, text);
}
*/

