// ADskeleton.hpp:  Include File
//
// Copyright 2007  Ian Gordon
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(__ADREGEX_HPP__)
#define __ADREGEX_HPP__

#if !defined(__POSIX__) && !defined(WIN32)
#error Must #define either WIN32 or __POSIX__
#endif
#if defined(__POSIX__) && defined(WIN32)
#error Cannot #define both WIN32 and __POSIX__
#endif

#include <string>
#include <vector>
#include <boost/regex.hpp>

#include "ADretvals.hpp"

namespace ad
{

class CRegex
{
public:
	CRegex();
	
	//! Case Sensitive or Case Insensitive matching
	enum ECase
	{
		CASE_SENSITIVE,
		CASE_INSENSITIVE
	};

	//! Takes Regex Pattern and (optionally) whether to do a case sensitive check
	/*!
	\param ssPattern The Regex pattern to check. Ex: "TODO"
	\param eCase Case insensitive or case sensitive matching (Default: Sensitive)
	\return SOK or EFAIL
	*/
	ERetVal Init(std::string ssPattern, ECase eCase = CASE_SENSITIVE);

	bool Match(const std::string& ssTest) const;
	ERetVal Search(void);
	ERetVal Replace(void);

private:
	std::vector<int> m_vnOffsets;  //!< Vector of matches

	boost::regex m_regex; 
};

} //namespace ad

#endif // !defined(__ADREGEX_HPP__)
