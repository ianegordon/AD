// ADinifile.cpp:  CINIFile Source File
//
// Copyright 2007 Ian Gordon
//
/////////////////////////////////////////////////////////////////////////////

#include "ADinifile.hpp"

#include <algorithm>  // For std::mix() & std::max()
#include <iostream>
#include <sstream>
#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/numeric/conversion/converter_policies.hpp>

#include "ADstlfunc.hpp"



using namespace std;
using namespace ad;


CINIFile::CINIFile(const std::string& ssFilename)
try
: m_boolInit(true), m_ssFilename(ssFilename), m_stm(ssFilename.c_str() )
{
	ERetVal eRetVal = ParseFile();

	if (Failed(eRetVal) )
		throw "Exc: Unable to Parse .ini file";
}
catch (...)
{
	//NOTE: In C++, if construction of any base or member subobject fails, the whole objectmust fail.
	//  C++ Reference: If the handler does not exit by throwing an exception (either rethrowing the original exception, or throwing something new)
	//    and control reaches the end of the catch block of a constructor or destructor, then the original exception is automatically rethrown as 
	//    if the handler's last statement had been "throw;".
}


CINIFile::~CINIFile()
{
	for_each(m_listpentry.begin(), m_listpentry.end(), ad::DeleteObject() );
}


ERetVal CINIFile::ParseFile(void)
{
	if (!m_stm.is_open() )
		return EIO_NOTFOUND;
	if (!m_stm.good() )
		return EFAIL;

	int cLinesRead(0);

	std::string ssCurrentSection;
	std::string ssLine;

//KM	cout << m_stm.rdbuf() << endl;
	
	//while (getline(m_stm, ssLine, '\n') )
//	while (!std::getline(m_stm, ssLine).eof() )  //??? Handle \r
	while (std::getline(m_stm, ssLine) )  //??? Handle \r
	{
		++cLinesRead;

		boost::trim(ssLine);

		if (ssLine.empty() )
			continue;

		static const std::string ssCommentChars("#;");
		static const char chOpenSection('[');
		static const char chCloseSection(']');
		static const char chKeyValueSeperator('=');

		if (string::npos != ssCommentChars.find(ssLine[0] ) )
		{
			// Ignore - Line is a comment
		}
		else if (chOpenSection == ssLine[0] )
		{
			// Section Header
			size_t ichCloseSection = ssLine.find_first_of(chCloseSection);
			if (string::npos == ichCloseSection)
			{
				//ERROR
				//FIXME Log Properly
				std::cerr << "ERR: Parsing INIFile " << m_ssFilename << " : " << cLinesRead << " has an unmatched " << chOpenSection << endl;

			}
			else
			{
				ssCurrentSection = ssLine.substr(1, ichCloseSection - 1);
				boost::trim(ssCurrentSection);
				if (ssCurrentSection.empty() )
				{
					//ERROR
					//FIXME Log Properly
					std::cerr << "ERR: Parsing INIFile " << m_ssFilename << " : " << cLinesRead << " has an empty section header" << std::endl;
				}

				boost::to_lower(ssCurrentSection);
			}

		}
		else
		{
			// Key / Value Pair
			size_t ichEquals = ssLine.find(chKeyValueSeperator);
			if (string::npos == ichEquals)
			{
				//ERROR
				//FIXME Log Properly
				std::cerr << "ERR: Parsing INIFile " << m_ssFilename << " : " << cLinesRead << " does not contain a key / value pair" << std::endl;
			}
			else if (ssCurrentSection.empty() )
			{
				//ERROR
				//FIXME Log Properly
				std::cerr << "ERR: Parsing INIFile " << m_ssFilename << " : " << cLinesRead << " is not within a [SectionHeader]" << std::endl;
			}
			else
			{
				//TODO Verify that there are no unhandled exceptions here

				string ssKey = ssLine.substr(0, ichEquals);
				boost::trim(ssKey);
				boost::to_lower(ssKey);

				//FIXME This will not handle string with comment chars
				// StringName = "Embe##ed" ; Note that the ## should be part of StringName 
				size_t ichEndOfValue = ssLine.find_first_of(ssCommentChars);
				if (string::npos == ichEndOfValue)
					ichEndOfValue = ssLine.length() - 1;
				string ssValue = ssLine.substr(ichEquals + 1, ichEndOfValue - ichEquals);
				boost::trim(ssValue);

				if (ssKey.empty() || ssValue.empty() )
				{
					//ERROR
					//FIXME Log Properly
					std::cerr << "ERR: Parsing INIFile " << m_ssFilename << " : " << cLinesRead << " is not a valid key / value pair" << std::endl;
				}
				else
				{
					try
					{
						Entry* pentry = new Entry;
						pentry->ssSection = ssCurrentSection;
						pentry->ssKey = ssKey;
						pentry->ssValue = ssValue;

						m_listpentry.push_back(pentry);

						pentry = NULL;
					}
					catch (...)
					{
						return EFAIL;
					}
				}
			}
		}
	}

	return SOK;
}

ERetVal CINIFile::GetString(std::string ssSectionName, std::string ssKeyName, std::string& ssValue) const
{
	ssValue = "";

	if (!m_stm.is_open())
		return EIO_NOTFOUND;

	boost::to_lower(ssSectionName);
	boost::to_lower(ssKeyName);

	for(EntryList::const_iterator it(m_listpentry.begin() );
		it != m_listpentry.end();
		++it)
	{
		Entry* pentry = *it;
		if (0 == ssSectionName.compare(pentry->ssSection) && 0 == ssKeyName.compare(pentry->ssKey) )
		{
			ssValue = pentry->ssValue;
			return SOK;
		}
	}

	return EFAIL;
}

ERetVal CINIFile::GetBool(std::string ssSectionName, std::string ssKeyName, bool& boolValue) const
{
	ERetVal eRetVal;
	string ssValue;
	
	eRetVal = GetString(ssSectionName, ssKeyName, ssValue);
	if (Failed(eRetVal) )
		return eRetVal;
	else
	{
		boost::to_lower(ssValue);
		if (0 == ssValue.compare("true") || 0 == ssValue.compare("yes") || 0 == ssValue.compare("on") )
			boolValue = true;
		else if (0 == ssValue.compare("false") || 0 == ssValue.compare("no") || 0 == ssValue.compare("off") )
			boolValue = false;
		else
			eRetVal = EDATA;
		
		return eRetVal;
	}
}


ERetVal CINIFile::GetInt(std::string ssSectionName, std::string ssKeyName, int& nValue) const
{
	ERetVal eRetVal;
	string ssValue;
	
	eRetVal = GetString(ssSectionName, ssKeyName, ssValue);
	if (Failed(eRetVal) )
		return eRetVal;
	else
	{
		try
		{
			nValue = boost::lexical_cast<int>(ssValue);
		}
		catch(boost::bad_lexical_cast&)
		{
			eRetVal = EDATA;
		}
	}
	
	return eRetVal;
}
ERetVal CINIFile::GetInt(std::string ssSectionName, std::string ssKeyName, unsigned int& nValue) const
{
	ERetVal eRetVal;
	int nIntValue;
	
	eRetVal = GetInt(ssSectionName, ssKeyName, nIntValue);
	nValue = static_cast<unsigned int>(nIntValue);
	
	return eRetVal;
}

/*
ERetVal CINIFile::GetInt(std::string ssSectionName, std::string ssKeyName, size_t& nValue) const
{
	ERetVal eRetVal;
	int nIntValue;
	
	eRetVal = GetInt(ssSectionName, ssKeyName, nIntValue);
	nValue = static_cast<size_t>(nIntValue);
	
	return eRetVal;
}
*/

ERetVal CINIFile::GetFloat(std::string ssSectionName, std::string ssKeyName, double& fValue) const
{
	ERetVal eRetVal;
	string ssValue;
	
	eRetVal = GetString(ssSectionName, ssKeyName, ssValue);
	if (Failed(eRetVal) )
		return eRetVal;
	else
	{
		try
		{
			fValue = boost::lexical_cast<double>(ssValue);
		}
		catch(boost::bad_lexical_cast&)
		{
			eRetVal = EDATA;
		}
	}
	
	return eRetVal;
}


bool CINIFile::HasOption(std::string ssSectionName, std::string ssKeyName) const
{
	boost::to_lower(ssSectionName);
	boost::to_lower(ssKeyName);

	for(EntryList::const_iterator it(m_listpentry.begin() );
		it != m_listpentry.end();
		++it)
	{
		Entry* pentry = *it;
		if (0 == ssSectionName.compare(pentry->ssSection) && 0 == ssKeyName.compare(pentry->ssKey) )
		{
			return true;
		}
	}

	return false;
}
