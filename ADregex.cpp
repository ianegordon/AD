// ADregex.cpp:  Source File
//
// Copyright 2007 Ian Gordon
//
/////////////////////////////////////////////////////////////////////////////

#include "ADregex.hpp"
#include "ADassert.hpp"

namespace ad
{
	
CRegex::CRegex()
{
	
}

ERetVal CRegex::Init(std::string ssPattern, ECase /*eCase*/)
{
	try
	{
		m_regex = ssPattern;

		return SOK;
	}
	catch (boost::bad_expression)
	{
		return EFAIL;		
	}
}

bool CRegex::Match(const std::string& ssTest) const
{
	if (m_regex.empty() )
	{
		Assert(!"Init Regex Properly First");
		return false;
	}
	return boost::regex_match(ssTest, m_regex);
}

ERetVal CRegex::Search(void)
{
	return ENOTIMPL;
}

ERetVal CRegex::Replace(void)
{
	return ENOTIMPL;
}

}
