// ADdatabase-postgresql.cpp:  Source File
//
// Copyright 2006  3 Against 2 LLC
//
/////////////////////////////////////////////////////////////////////////////

#include "ADdatabase-postgresql.hpp"

#include <iomanip>
#include <sstream>
#include <stdint.h>

#include "ADassert.hpp"

using namespace std;
using namespace ad;


CPostgreSQLResult::CPostgreSQLResult() 
  : m_presult(NULL), m_iRow(UINT_MAX)
{
	
}

CPostgreSQLResult::~CPostgreSQLResult()
{
	this->Reset();
}

void CPostgreSQLResult::Reset(void)
{
	if (m_presult)
	{
		PQclear(m_presult);
		m_presult = NULL;
	}
	
	m_iRow = UINT_MAX;
}

unsigned int CPostgreSQLResult::GetRowCount(void) const
{
	if (NULL == m_presult)
		return 0;
	else
		return PQntuples(m_presult);
}

unsigned int CPostgreSQLResult::GetColumnCount(void) const
{
	if (NULL == m_presult)
		return 0;
	else
		return PQnfields(m_presult);
}
	
ad::ERetVal CPostgreSQLResult::SelectRow(const unsigned int iRow)
{
	if (UINT_MAX == iRow)
	{
		int nDummy(0); ++nDummy;
		return EFAIL;
	}

	if (this->GetRowCount() <= iRow)
		return EFAIL;
	else
	{
		m_iRow = iRow;
		
		return SOK;
	}
}

ad::ERetVal CPostgreSQLResult::GetColumn(const unsigned int iColumn, std::string& ssColumnValue)
{
	if (-1 == m_iRow)
	{
		int nDummy(0); ++nDummy;
		return EFAIL;
	}

	ssColumnValue.clear();

	if (this->GetColumnCount() <= iColumn)
		return EFAIL;
	else
	{
		ssColumnValue = PQgetvalue(m_presult, m_iRow, iColumn);
		return SOK;
	}
}
	
ad::ERetVal CPostgreSQLResult::ColumnToBool(const std::string& ssColumnValue, bool& boolValue)
{
	if (0 == ssColumnValue.compare("f") )
	{
		boolValue = false;
		
		return SOK;
	}
	else if (0 == ssColumnValue.compare("t") )
	{
		boolValue = true;
		
		return SOK;
	}
	else
		return EFAIL;
}







CPostgreSQLDatabase::CPostgreSQLDatabase() 
  : m_pconn(NULL)
{
}

CPostgreSQLDatabase::~CPostgreSQLDatabase()
{
	this->Disconnect();
}

ad::ERetVal CPostgreSQLDatabase::Connect(const std::string& ssHostname, const std::string& ssUsername, const std::string& ssPassword, const std::string& ssDatabase)
{
	stringstream sstm;

	if (!ssHostname.empty() )
	{
		sstm << "host = " << ssHostname << " ";
	}

	if (!ssUsername.empty() )
	{
		sstm << "user = " << ssUsername << " ";
	}

	if (!ssPassword.empty() )
	{
		sstm << "password = " << ssPassword << " ";
	}

	if (!ssDatabase.empty() )
	{
		sstm << "dbname = " << ssDatabase << " ";
	}

	m_pconn = PQconnectdb(sstm.str().c_str() );
	if (CONNECTION_OK == PQstatus(m_pconn) )
		return SOK;
	else
		return EFAIL;
}

void CPostgreSQLDatabase::Disconnect(void)
{
//KM	Assert( (!m_ppgconn && !m_boolConnected) || (m_ppgconn && m_boolConnected) )

	if (m_pconn)
	{
		PQfinish(m_pconn);
		m_pconn = NULL;
	}
}

ad::ERetVal CPostgreSQLDatabase::Execute(const std::string& ssCommand, CPostgreSQLResult* pResults)
{
	if (pResults)
		pResults->Reset();

	PGresult* presult = PQexec(m_pconn, ssCommand.c_str() );
	if (PGRES_COMMAND_OK != PQresultStatus(presult) && PGRES_TUPLES_OK != PQresultStatus(presult) )
	{
		if (presult)
			PQclear(presult);

		return EFAIL;
	}
	else
	{
		if (pResults)
			pResults->m_presult = presult;
		else
			PQclear(presult);

		return SOK;
	}
}

void CPostgreSQLDatabase::EscapeBinaryData(const std::vector<uint8_t>& vbData, std::string& ssEscaped)
{
	ssEscaped = "";
	
	stringstream sstmEscaped;

	for (size_t ii(0); vbData.size() != ii; ++ii)
	{
		uint8_t b = vbData[ii];
		if ( (0 <= b && b <= 31) ||
			b == 39 || b == 92 ||
			(127 <= b && b <= 255) )
		{
			sstmEscaped << "\\\\" << setfill('0') << setw(3) << std::oct << static_cast<int>(b);
		}
		else
		{
			sstmEscaped << b;
		}
	}
	
	ssEscaped = sstmEscaped.str();
	
	return;
}

