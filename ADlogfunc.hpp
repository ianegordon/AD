// ADlogfunc.hpp:  Include File
//
// Copyright 2007  Ian Gordon
//
/////////////////////////////////////////////////////////////////////////////

// DEPRECATED - Use AD/ADlog
//
//TODO Rename to SystemLog()



#if !defined(__ADLOGFUNC_HPP__)
#define __ADLOGFUNC_HPP__

#if !defined(__POSIX__) && !defined(WIN32)
#error Must #define either WIN32 or __POSIX__
#endif
#if defined(__POSIX__) && defined(WIN32)
#error Cannot #define both WIN32 and __POSIX__
#endif

/*
log_emerg = LOG_EMERG,    // no point in continuing (syslog's LOG_EMERG)
log_fatal = LOG_ALERT,    // no point in continuing (syslog's LOG_ALERT)
log_internal = LOG_CRIT,  // internal error
log_error = LOG_ERR,      // client error
log_warning = LOG_WARNING,// client warning
log_info = LOG_INFO,      // just for yucks
log_debug=LOG_DEBUG,      // for debugging gory details
*/

#include <iostream>
#include <string>


namespace ad
{

	enum ELogPriority
	{
		eERROR,
		eWARNING,
		eINFO,
		eDEBUG
	};

	//! Log Message to the system log / screen / logfile
	void Log(const ELogPriority ePriority, const std::string& ssLogEntry);

	//??? LogError(), LogWarning(), LogInfo(), LogDebug()
} //namespace ad

#endif // !defined(__ADLOGFUNC_HPP__)
