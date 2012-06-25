// ADinifile.hpp:  Include File
//
// Copyright 2007  Ian Gordon
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(__ADINIFILE_HPP__)
#define __ADINIFILE_HPP__

#if !defined(__POSIX__) && !defined(WIN32)
#error Must #define either WIN32 or __POSIX__
#endif
#if defined(__POSIX__) && defined(WIN32)
#error Cannot #define both WIN32 and __POSIX__
#endif


#include <string>
#include <fstream>
#include <list>

#include "ADretvals.hpp"

namespace ad
{

class CINIFile
{
public:
	CINIFile(const std::string& ssFilename);
	~CINIFile(void);

	ERetVal GetBool(std::string ssSectionName, std::string ssKeyName, bool& boolValue) const;
	ERetVal GetInt(std::string ssSectionName, std::string ssKeyName, int& nValue) const;
	ERetVal GetInt(std::string ssSectionName, std::string ssKeyName, unsigned int& nValue) const;
//	ERetVal GetInt(std::string ssSectionName, std::string ssKeyName, size_t& nValue) const;
	ERetVal GetFloat(std::string ssSectionName, std::string ssKeyName, double& fValue) const;
	ERetVal GetString(std::string ssSectionName, std::string ssKeyName, std::string& ssValue) const;

	bool HasOption(std::string ssSectionName, std::string ssKeyName) const;

	// From Python
	//
	//parser = ConfigParser.SafeConfigParser()
	//lRead = parser.read(ssBigBrotherINIFilename)
	//if parser.has_option("BigBrother", "RepositoryListFilename"):
	//	ssRepositoryList = parser.get("BigBrother", "RepositoryListFilename")


private:
	bool m_boolInit;

	std::string m_ssFilename;
	std::ifstream m_stm;

	ERetVal ParseFile(void);

	struct Entry
	{
		std::string ssSection;
		std::string ssKey;
		std::string ssValue;
	};

	typedef std::list<Entry*> EntryList;
	EntryList m_listpentry;


	//Cannot copy / assign CINIFile
	CINIFile(const CINIFile&);
	const CINIFile& operator=(const CINIFile&);
};

} //namespace ad

#endif // !defined(__ADINIFILE_HPP__)
