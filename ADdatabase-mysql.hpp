// ADskeleton.hpp:  FIXME Include File
//
// Copyright 2006  3 Against 2 LLC
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(__ADDATABASE_MYSQL_HPP__)
#define __ADDATABASE_MYSQL_HPP__

#if !defined(__POSIX__) && !defined(WIN32)
#error Must #define either WIN32 or __POSIX__
#endif
#if defined(__POSIX__) && defined(WIN32)
#error Cannot #define both WIN32 and __POSIX__
#endif

#include "ADdatabase.hpp"
#include "ADretvals.hpp"

//NOTE Due to MySQL needing socket info, we must include winsock2.h BEFORE mysql.h on WIN32
#if defined(WIN32)
#include <winsock2.h>
#include "pstdint.h"
#endif // defined(WIN32)
#include <mysql.h>
#include <string>
#include <vector>


namespace ad
{

class CMYSQLDatabase : public CDatabase
{
public:
	CMYSQLDatabase();
	~CMYSQLDatabase();

	ad::ERetVal Connect(const std::string& ssHostname, const std::string& ssUsername, const std::string& ssPassword, const std::string& ssDatabase);
	void Disconnect(void);

	ad::ERetVal Execute(const std::string& ssCommand);
	void ClearResults(void);

	unsigned int GetRowCount(void);
	unsigned int GetColumnCount(void);

	ad::ERetVal SelectRow(const unsigned int iRow);
	ad::ERetVal GetColumn(const unsigned int iColumn, std::string& ssColumnValue);

	static void EscapeBinaryData(const std::vector<uint8_t>& vbData, std::string& ssEscaped);

private:
	MYSQL* m_pmysql;
	MYSQL_RES* m_presult;
	MYSQL_ROW m_row;
	
	bool m_boolConnected;
	
	int m_iRow;
};

} //namespace ad

#endif // !defined(__ADDATABASE_MYSQL_HPP__)
