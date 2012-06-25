// ADdatabase.hpp:  Include File
//
// Copyright 2006  3 Against 2 LLC
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(__ADDATABASE_HPP__)
#define __ADDATABASE_HPP__

#if !defined(__POSIX__) && !defined(WIN32)
#error Must #define either WIN32 or __POSIX__
#endif
#if defined(__POSIX__) && defined(WIN32)
#error Cannot #define both WIN32 and __POSIX__
#endif

#include "ADretvals.hpp"
#include <string>

namespace ad
{

class CDatabaseResult
{
public:
	virtual ~CDatabaseResult() {}
	
	virtual unsigned int GetRowCount(void) const = 0;
	virtual unsigned int GetColumnCount(void) const = 0;
	
	virtual ad::ERetVal SelectRow(const unsigned int iRow) = 0;
	virtual ad::ERetVal GetColumn(const unsigned int iColumn, std::string& ssColumnValue) = 0;
};

class CDatabase
{
public:
	virtual ~CDatabase() {}
	
	virtual ad::ERetVal Connect(const std::string& ssHostname, const std::string& ssUsername, const std::string& ssPassword, const std::string& ssDatabase) = 0;
	virtual void Disconnect(void) = 0;

	virtual ad::ERetVal Execute(const std::string& ssCommand, CDatabaseResult* results) = 0;
};
	

} //namespace ad

#endif // !defined(__ADDATABASE_HPP__)
