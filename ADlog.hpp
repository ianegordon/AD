/*
 *  adlog.h
 *  IOStreamLogger
 *
 *  Created by Ian Gordon on 3/14/08.
 *
 */

// Sven Axelsson - Using an output stream for debugging
// http://www.codeproject.com/debug/debugout.asp#xx179752xx
// IEG - Updated to handle strings > 1023 characters length

// ad::log is used as a standard stream (think std::cout or std::cerr).  
// Under Posix systems it also sends the value to syslog()
// Under Windows systems it also sends the value to OutputDebugString()

//NOTE : Under osx you may have to adjust the syslog threshold
// Add the following to /etc/syslog.conf :
// # Log anything of INFO or higher - IEG
// *.info							/var/log/messages

// Enumeration :: Levels
// ---- LOG_EMERG     A panic condition.  This is normally broadcast to all users.
// ---- LOG_ALERT     A condition that should be corrected immediately, such as a corrupted system database.
// ---- LOG_CRIT      Critical conditions, e.g., hard device errors.
// eERROR = LOG_ERR       Errors.
// eWARNING = LOG_WARNING   Warning messages.
// ---- LOG_NOTICE    Conditions that are not error conditions, but should possi-bly possibly bly be handled specially.
// eINFO = LOG_INFO      Informational messages.
// eDEBUG = LOG_DEBUG     Messages that contain information normally of use only when debugging a program.

// Usage:
// ad::log << ad::Error() << "Index Out of Bounds" << std::endl;
//
// std::endl and std::flush work as expected.

//
//////////////////////////////////////////////////////////////////////

#if !defined(__ADLOG_HPP__)
#define __ADLOG_HPP__

#if !defined(__POSIX__) && !defined(WIN32)
#error Must #define either WIN32 or __POSIX__
#endif
#if defined(__POSIX__) && defined(WIN32)
#error Cannot #define both WIN32 and __POSIX__
#endif

#include <iostream>
#include <sstream>

#if defined(__POSIX__)
#include <stdarg.h>
#include <syslog.h>
#elif defined(WIN32)
#include <winsock2.h>
#include <windows.h>
#endif // defined(__POSIX__)

namespace ad {

enum ELogLevel
{
	eLOG_ERROR,
	eLOG_WARNING,
	eLOG_INFO,
	eLOG_DEBUG
};
	
const char* LoggingLevelToSZ(ad::ELogLevel eLogLevel);
int LoggingLevelToSyslogValue(ad::ELogLevel eLogLevel);

	
template <class CharT, class TraitsT = std::char_traits<CharT> >
class basic_debugbuf : public std::basic_stringbuf<CharT, TraitsT>
{
public:

	basic_debugbuf()
	{
		SetLoggingLevel(eLOG_INFO);
	}
	
	virtual ~basic_debugbuf()
	{
		if (0 != std::basic_stringbuf<CharT, TraitsT>::pbase() )
			sync();
	}

	void SetLoggingLevel(const ELogLevel eLogLevel)
	{ 
		switch (eLogLevel)
		{
			case eLOG_ERROR: 
			case eLOG_WARNING: 
			case eLOG_INFO: 
			case eLOG_DEBUG: 
				m_eLoggingLevel = eLogLevel;
				break;
			default:
				m_eLoggingLevel = eLOG_DEBUG;
				break;
		}
	}
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

protected:
	enum { _MAXSIZE = 1023};

	ELogLevel m_eLoggingLevel;

	virtual std::streamsize xsputn(const CharT *_S, std::streamsize _N)  
	{    
		std::streamsize _Ns = 0;    
		while (0 < _N) // loop as long as we wrote the whole string    
		{      
			std::streamsize _M = std::basic_stringbuf<CharT, TraitsT>::pptr() - std::basic_stringbuf<CharT, TraitsT>::pbase();
			
			if (_M >= _MAXSIZE) // synchronize if there are already 1023 chars
				sync();
			
			_M = _MAXSIZE - _M; // maximal amount of chars we can write this time      
			
			if (_N < _M)        
				_M = _N;      

			_M = std::basic_stringbuf<CharT, TraitsT>::xsputn(_S, _M);      
			
			_S += _M; 
			_Ns += _M; 
			_N -= _M; 
		}
		
		return _Ns;  
	}

#if defined(__POSIX__)
	void output_debug_string(const char *text)
	{
		if (text && *text)
		{
			std::cout << LoggingLevelToSZ(m_eLoggingLevel) << text << std::flush;
			syslog( LoggingLevelToSyslogValue(m_eLoggingLevel), "%s", text);
		}
	}
#elif defined(WIN32)
	void output_debug_string(const char *text)
	{
		std::cout << LoggingLevelToSZ(m_eLoggingLevel) << text << std::flush;
	
		::OutputDebugStringA(text);
	}
#endif

	int sync()
	{
		std::basic_stringbuf<CharT, TraitsT>::sputc('\0');

		output_debug_string(std::basic_stringbuf<CharT, TraitsT>::str().c_str() );
		
		str(std::basic_string<CharT>());    // Clear the string buffer
		
		return 0;
	}

//	void output_debug_string(const CharT *text);
};

//class basic_dostream : public std::basic_ostream<CharT, TraitsT>
//template<class CharT, class TraitsT = std::char_traits<CharT> >
class basic_dostream : public std::ostream
{
public:

	basic_dostream(basic_debugbuf<char>* sb) : std::ostream(sb)
	{
		m_psb = sb;
	}

protected:
	basic_debugbuf<char>* m_psb;
	
	friend struct Error;
	friend struct Warning;
	friend struct Info;
	friend struct Debug;
};

typedef basic_dostream dostream;

extern dostream log;


struct Error
{
	dostream &operator()(dostream &stream) const
	{
		if (stream.m_psb)
			stream.m_psb->SetLoggingLevel(eLOG_ERROR);
		
		return stream;
	}
};

inline dostream& operator<<(dostream& os, const Error& f)
{
	return f(os);
}

struct Warning
{
	dostream &operator()(dostream &stream) const
	{
		if (stream.m_psb)
			stream.m_psb->SetLoggingLevel(eLOG_WARNING);
		
		return stream;
	}
};

inline dostream& operator<<(dostream& os, const Warning& f)
{
	return f(os);
}

struct Info
{
	dostream &operator()(dostream &stream) const
	{
		if (stream.m_psb)
			stream.m_psb->SetLoggingLevel(eLOG_INFO);
		
		return stream;
	}
};

inline dostream& operator<<(dostream& os, const Info& f)
{
	return f(os);
}

struct Debug
{
	dostream &operator()(dostream &stream) const
	{
		if (stream.m_psb)
			stream.m_psb->SetLoggingLevel(eLOG_DEBUG);
		
		return stream;
	}
};

inline dostream& operator<<(dostream& os, const Debug& f)
{
	return f(os);
}






} //namespace ad


#endif // !defined(__ADLOG_HPP__)
