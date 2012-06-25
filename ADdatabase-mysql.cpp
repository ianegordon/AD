//UNDUPE

// ADskeleton.cpp:  FIXME Source File
//
// Copyright 2006  3 Against 2 LLC
//
/////////////////////////////////////////////////////////////////////////////

#include "ADdatabase-mysql.hpp"

#include <iostream>
#include <sstream>

#include "ADassert.hpp"
#include "ADlog.hpp"


using namespace std;
using namespace ad;

CMYSQLDatabase::CMYSQLDatabase() 
: m_pmysql(NULL), m_presult(NULL), m_row(NULL), m_boolConnected(false)
{

}

CMYSQLDatabase::~CMYSQLDatabase()
{
	this->ClearResults();
	this->Disconnect();
	
	// This should be sorted by the above
	Assert(NULL == m_pmysql);
	Assert(false == m_boolConnected);
}

ad::ERetVal CMYSQLDatabase::Connect(const std::string& ssHostname, const std::string& ssUsername, const std::string& ssPassword, const std::string& ssDatabase)
{
	this->Disconnect();

	m_pmysql = mysql_init(NULL);
	if (NULL == m_pmysql)
		return EFAIL;

	//NOTE: MySQL documentation says to set options after mysql_init(), but before mysql_real_connect()
	//There is a bug in Mysql 5.0.0 - 5.0.18 where MYSQL_OPT_RECONNECT is unset by mysql_real_connect(),
	//so we set it twice.
	
	//NOTE This is workaround for MySQL 5.0+
#if 0 
	my_bool my_true = true;
	
	int nRetVal;
	nRetVal = mysql_options(m_pmysql, MYSQL_OPT_RECONNECT, &my_true);
	if (0 != nRetVal)
	{
#if defined(__POSIX__)
		Assert(!"Unable to mysql_options(,MYSQL_OPT_RECONNECT,) ");
		
		return EFAIL;
#endif // defined(__POSIX__)
	}
#endif //0 

		
		
		MYSQL* pmysqlRetVal = NULL;
		pmysqlRetVal = mysql_real_connect(m_pmysql, ssHostname.c_str(), ssUsername.c_str(), ssPassword.c_str(), ssDatabase.c_str(), MYSQL_PORT, NULL, 0);
		if (pmysqlRetVal != m_pmysql)
		{
			Assert(!"Unable to connect to Mysql");
			
			return EFAIL;
		}
		
#if 0 //NOTE This is valid on MySQL 5.0+
nRetVal = mysql_options(m_pmysql, MYSQL_OPT_RECONNECT, &my_true);
		if (0 != nRetVal)
		{
#if defined(_DEBUG)
			cerr << "ERR: SetupDatabaseConnection() - Unable to mysql_options(,MYSQL_OPT_RECONNECT,) after connect" << endl;
#endif // defined(_DEBUG)
#if defined(__POSIX__)
			Assert(0);
			
			mysql_close(m_pmysql);
			
			return EFAIL;
#endif // defined(__POSIX__)
}
#endif // 0 //NOTE This is valid on MySQL 5.0+

	m_boolConnected = true;

	return SOK;
}

void CMYSQLDatabase::Disconnect(void)
{
	if (m_boolConnected)
	{
		mysql_close(m_pmysql);
		m_pmysql = NULL;
		
		m_boolConnected = false;
	}
}

ad::ERetVal CMYSQLDatabase::Execute(const std::string& ssCommand)
{
	// Get Rid of previous Query
	this->ClearResults();
	
	if (!m_boolConnected)
	{
		ad::log << Error() << "CMYSQLDatabase::Query() ! Must connect to Database before Query" << endl;
		
		return EFAIL;
	}
	
	int nRetVal;
	nRetVal = mysql_query(m_pmysql, ssCommand.c_str() );
	if (0 != nRetVal)
	{
		cerr << "CMYSQLDatabase::Query() ! Unable to perform mysql query: " << ssCommand << endl; //FIXME Log
		
		Assert(0);
		
		return EFAIL;
	}
	else
	{
		m_presult = mysql_store_result(m_pmysql);
		//NOTE If presult is NULL, check to see if the query does not return any fields
		if (m_presult || 0 == mysql_field_count(m_pmysql) )
		{
			return SOK;
		}
		else
		{
			cerr << "CMYSQLDatabase::Query() ! Unable to store result of query" << endl; //FIXME Log
			
			Assert(0);
			
			return EFAIL;
		}
	}
}

void CMYSQLDatabase::ClearResults(void)
{
	if (m_presult)
	{
		mysql_free_result(m_presult);
		m_presult = NULL;
	}
}

unsigned int CMYSQLDatabase::GetRowCount(void)
{
	if (NULL == m_presult)
		return 0;
	else
		return mysql_num_rows(m_presult);
}

unsigned int CMYSQLDatabase::GetColumnCount(void)
{
	if (NULL == m_presult)
		return 0;
	else
		return mysql_num_fields(m_presult);
}

ad::ERetVal CMYSQLDatabase::SelectRow(const unsigned int iRow)
{
	if (GetRowCount() <= iRow)
	{
		return EFAIL;
	}
	else
	{
		mysql_data_seek(m_presult, iRow);
		
		m_row = mysql_fetch_row(m_presult);
		if (NULL == m_row)
			return EFAIL;
		else
			return SOK;
	}
}

ad::ERetVal CMYSQLDatabase::GetColumn(const unsigned int iColumn, std::string& ssColumnValue)
{
	if (GetColumnCount() <= iColumn)
		return EFAIL;
	else if (NULL == m_row)
		return EFAIL;
	else if (NULL == m_row[iColumn])  // column exists, but has NULL value in table
	{
		ssColumnValue.clear();
		return EINIT_FIRST;
	}
	else
	{
		ssColumnValue = m_row[iColumn];
		return SOK;
	}
}

void CMYSQLDatabase::EscapeBinaryData(const std::vector<uint8_t>& vbData, std::string& ssEscaped)
{
	// See: http://dev.mysql.com/doc/refman/5.0/en/string-syntax.html
	/* From : http://sunsite.mff.cuni.cz/MIRRORS/ftp.mysql.com/doc/en/String_syntax.html
	 If you want to insert binary data into a string column (such as a BLOB), the following characters must be represented by escape sequences:
	 
	 NUL ASCII 0. You should represent this by `\0' (a backslash and an ASCII `0' character). 
	 \ ASCII 92, backslash. Represent this by `\\'. 
	 ' ASCII 39, single quote. Represent this by `\''. 
	 " ASCII 34, double quote. Represent this by `\"'. 
	 */
	ssEscaped = "";
	 
	for (size_t ii(0); vbData.size() != ii; ++ii)
	 {
		 switch(vbData[ii])
		 {
			 case '\0': ssEscaped.append("\\0");
				 break;
			 case '\\': ssEscaped.append("\\\\");
				 break;
			 case '\'': ssEscaped.append("\\'");
				 break;
			 case '\"': ssEscaped.append("\\\"");
				 break;
			 default: ssEscaped.push_back(vbData[ii]);
				 break;
		 }
	 }
	
}
