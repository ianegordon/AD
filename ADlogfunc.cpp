// ADlogfunc.cpp:  Source File
//
// Copyright 2007  Ian Gordon
//
/////////////////////////////////////////////////////////////////////////////

#include "ADlogfunc.hpp"

#include <iostream>

static const char* LogPriorityToSZ(const ad::ELogPriority ePriority)
{
	if (ad::eERROR == ePriority)
		return "ERROR";
	else if (ad::eWARNING == ePriority)
		return "Warning";
	else if (ad::eINFO == ePriority)
		return "Info";
	else if (ad::eDEBUG == ePriority)
		return "Debug";
	else
		return "Log";
	
}


#if defined(WIN32)

void ad::Log(const ad::ELogPriority ePriority, const std::string& ssLogEntry)
{
	if (0 == ssLogEntry.size() )
		return;

	std::cout << LogPriorityToSZ(ePriority) << std::endl;
}

#elif defined (__POSIX__)

#include <syslog.h>
#include <stdarg.h>

void ad::Log(const ad::ELogPriority ePriority, const std::string& ssLogEntry)
{
	if (0 == ssLogEntry.size() )
		return;

	int nPriority;
	if (eERROR == ePriority)
		nPriority = LOG_ERR;
	else if (eWARNING == ePriority)
		nPriority = LOG_WARNING;
	else if (eINFO == ePriority)
		nPriority = LOG_INFO;
	else if (eDEBUG == ePriority)
		nPriority = LOG_DEBUG;

	syslog(nPriority, ssLogEntry.c_str() );
	
	std::cout << LogPriorityToSZ(ePriority) << " : " << ssLogEntry.c_str() << std::endl;
}

#endif


