// ADskeleton.hpp:  Include File
//
// Copyright 2006  3 Against 2 LLC
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(__ADDATABASE_POSTGRESQL_HPP__)
#define __ADDATABASE_POSTGRESQL_HPP__

#if !defined(__POSIX__) && !defined(WIN32)
#error Must #define either WIN32 or __POSIX__
#endif
#if defined(__POSIX__) && defined(WIN32)
#error Cannot #define both WIN32 and __POSIX__
#endif

#if defined(WIN32)
#include "pstdint.h"
#endif // defined(WIN32)

#include <string>
#include <vector>

#include <postgresql84/libpq-fe.h>


#include "ADdatabase.hpp"
#include "ADretvals.hpp"

namespace ad
{

class CPostgreSQLResult;
	
class CPostgreSQLDatabase //: public CDatabase
{
public:
	CPostgreSQLDatabase();
	virtual ~CPostgreSQLDatabase();

	ad::ERetVal Connect(const std::string& ssHostname, const std::string& ssUsername, const std::string& ssPassword, const std::string& ssDatabase);
	void Disconnect(void);

	ad::ERetVal Execute(const std::string& ssCommand, CPostgreSQLResult* pResults = NULL);

	static void EscapeBinaryData(const std::vector<uint8_t>& vbData, std::string& ssEscaped);

private:
	PGconn* m_pconn;
};

	
class CPostgreSQLResult //: public CDatabaseResult
{
public:
	CPostgreSQLResult();
	~CPostgreSQLResult();
	
	unsigned int GetRowCount(void) const;
	unsigned int GetColumnCount(void) const;
	
	ad::ERetVal SelectRow(const unsigned int iRow);
	ad::ERetVal GetColumn(const unsigned int iColumn, std::string& ssColumnValue);
	
	static ad::ERetVal ColumnToBool(const std::string& ssColumnValue, bool& boolValue);
	
private:
	void Reset(void);
	
	PGconn* m_pconn;
	PGresult* m_presult;
	
	int m_iRow;
	
	friend class CPostgreSQLDatabase;
};

	
	
} //namespace ad

#endif // !defined(__ADDATABASE_POSTGRESQL_HPP__)
