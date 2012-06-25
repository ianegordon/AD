// ADsmtp.cpp:  SMTP Source File
//
// Copyright 2007  Ian Gordon
//
/////////////////////////////////////////////////////////////////////////////

#include "ADsmtp.hpp"

#include <iostream>
#include <sstream>
#include <vector>

#include "ADassert.hpp"

using namespace std;
using namespace ad;

static const size_t MAX_LINE_LENGTH(512); // RFC1869 4.1.2 - Includes ending CR/LF
static const size_t MAX_COMMAND_LENGTH(1024); // RFC 1869 

CSMTPContext::CSMTPContext()
: m_eState(SMTP_UNCONNECTED), 
  m_bool8bitmime(false),
  m_boolAuthorizationRequired(false),
  m_boolESMTP(false),
  m_boolPipelining(false),
  m_boolStartTLS(false),
  m_cbMaxSize(0)
{

}

CSMTPContext::~CSMTPContext()
{

}

ad::ERetVal CSMTPContext::Connect(const std::string& ssServer, const uint16_t nPort)
{
	if (SMTP_UNCONNECTED != m_eState)
		return EINIT_ALREADY;

	ERetVal eRetVal;

	m_socket.Socket();
	eRetVal = m_socket.Connect(ssServer, nPort);
	if (Failed(eRetVal) )
	{
		return eRetVal;
	}

	//Server Greeting
	{
		std::vector<uint8_t> vbBuffer(MAX_LINE_LENGTH);

		size_t cbRead;
		eRetVal = m_socket.Read(&vbBuffer[0], MAX_LINE_LENGTH, cbRead);
		if (Failed(eRetVal) )
		{
			Assert(!"Failed(m_socket.Read(&vbBuffer[0], MAX_LINE_LENGTH, cbRead) )");

			return eRetVal;
		}
		else
		{
			if (cbRead) //KM
			{
				vbBuffer[cbRead] = NULL;  //KM  Overflow
				cout << &vbBuffer[0] << endl;  //KM
			}

			//Verify 220 Return Code
			if (3 <= vbBuffer.size() && 0 == memcmp("220", &vbBuffer[0], 3) )
			{
				m_eState = SMTP_AUTHORIZATION;
				return SOK;
			}
		}
	}

	this->Disconnect();
	return EFAIL;
}

void CSMTPContext::Disconnect()
{
	if (m_socket.IsValid() )
	{
		stringstream sstmCommand;
		string ssCommand;
		size_t cbWritten;

		sstmCommand << "QUIT\r\n";
		ssCommand = sstmCommand.str();
		cout << "SERVER < " << ssCommand << endl;
		m_socket.Write(ssCommand.c_str(), ssCommand.length(), cbWritten);
	}

	m_socket.Close();
	m_eState = SMTP_UNCONNECTED;
}

ad::ERetVal CSMTPContext::EHLO(void)
{
	if (SMTP_AUTHORIZATION != m_eState)
		return EWRONG_STATE;

	ERetVal eRetVal;

	stringstream sstmCommand;
	string ssCommand;
	size_t cbWritten;

	string ssHostname("192.168.1.8");
	sstmCommand << "EHLO " << ssHostname << "\r\n";
	ssCommand = sstmCommand.str();
	cout << "SERVER < " << ssCommand << endl;
	m_socket.Write(ssCommand.c_str(), ssCommand.length(), cbWritten);

	// REPLY
	bool boolReadingResponse(true);
	while(boolReadingResponse)
	{
		std::vector<uint8_t> vbBuffer(0x100);
		size_t cbRead;
		eRetVal = m_socket.Read(&vbBuffer[0], vbBuffer.size(), cbRead);
		if (Failed(eRetVal) )
		{
			Assert(0);

			return eRetVal;
		}
		else
		{
			if (cbRead) //KM
			{
				vbBuffer[cbRead] = NULL;  //KM
				cout << &vbBuffer[0] << endl;  //KM
			}

			//Verify Return Code
			if (3 <= vbBuffer.size() && 0 == memcmp("250", &vbBuffer[0], 3) )
			{
				// EHLO Successful
				m_boolESMTP = true;

				string ssResults(reinterpret_cast<char*>(&vbBuffer[0]) );

				if (string::npos != ssResults.find("STARTTLS") )
				{
					m_boolStartTLS = true;
				}

				if (string::npos != ssResults.find("AUTH") )
				{
					m_boolAuthorizationRequired = true;
				}
			}

			//TODO Test that we have received all data
			boolReadingResponse = false;
		}
	}

	if (!m_boolESMTP)
	{
		cout << "Unable to EHLO, try HELO" << endl;

		stringstream sstmCommand;
		string ssCommand;
		size_t cbWritten;

		string ssHostname("192.168.1.8");
		sstmCommand << "HELO " << ssHostname << "\r\n";
		ssCommand = sstmCommand.str();
		cout << "SERVER < " << ssCommand << endl;
		m_socket.Write(ssCommand.c_str(), ssCommand.length(), cbWritten);

		// REPLY
		bool boolReadingResponse(true);
		while(boolReadingResponse)
		{
			std::vector<uint8_t> vbBuffer(0x100);
			size_t cbRead;
			eRetVal = m_socket.Read(&vbBuffer[0], vbBuffer.size(), cbRead);
			if (Failed(eRetVal) )
			{
				Assert(0);

				return eRetVal;
			}
			else
			{
				if (cbRead) //KM
				{
					vbBuffer[cbRead] = NULL;  //KM
					cout << &vbBuffer[0] << endl;  //KM
				}

				//Verify Return Code
				if (3 <= vbBuffer.size() && 0 == memcmp("250", &vbBuffer[0], 3) )
				{
					// EHLO Successful
					m_boolESMTP = true; 
				}

				//TODO Test that we have received all data
				boolReadingResponse = false;
			}
		}
	}

	if (m_boolStartTLS)
	{
		cout << "Starting TLS" << endl;

		stringstream sstmCommand;
		string ssCommand;
		size_t cbWritten;

		sstmCommand << "STARTTLS\r\n";
		ssCommand = sstmCommand.str();
		cout << "SERVER < " << ssCommand << endl;
		m_socket.Write(ssCommand.c_str(), ssCommand.length(), cbWritten);

		// REPLY
		{
			std::vector<uint8_t> vbBuffer(0x100);
			size_t cbRead;
			eRetVal = m_socket.Read(&vbBuffer[0], vbBuffer.size(), cbRead);
			if (Failed(eRetVal) )
			{
				Assert(0);

				return eRetVal;
			}
			else
			{
				if (cbRead) //KM
				{
					vbBuffer[cbRead] = NULL;  //KM
					cout << &vbBuffer[0] << endl;  //KM
				}

				//Verify Return Code
				if (3 <= vbBuffer.size() && 0 == memcmp("220", &vbBuffer[0], 3) )
				{
					eRetVal = m_socket.StartTLS();
					if (Succeeded(eRetVal) )
					{
						stringstream sstmCommand;
						string ssCommand;
						size_t cbWritten;
						string ssHostname("192.168.1.8");
						sstmCommand << "EHLO " << ssHostname << "\r\n";
						ssCommand = sstmCommand.str();
						cout << "SERVER < " << ssCommand << endl;
						m_socket.Write(ssCommand.c_str(), ssCommand.length(), cbWritten);

						// REPLY
						bool boolReadingResponse(true);
						while(boolReadingResponse)
						{
							std::vector<uint8_t> vbBuffer(0x100);
							size_t cbRead;
							eRetVal = m_socket.Read(&vbBuffer[0], vbBuffer.size(), cbRead);
							if (Failed(eRetVal) )
							{
								Assert(0);

								return eRetVal;
							}
							else
							{
								if (cbRead) //KM
								{
									vbBuffer[cbRead] = NULL;  //KM
									cout << &vbBuffer[0] << endl;  //KM
								}

								//Verify Return Code
								if (3 <= vbBuffer.size() && 0 == memcmp("250", &vbBuffer[0], 3) )
								{
									// EHLO Successful
									m_boolESMTP = true;

									string ssResults(reinterpret_cast<char*>(&vbBuffer[0]) );

									if (string::npos != ssResults.find("STARTTLS") )
									{
										m_boolStartTLS = true;
									}

									if (string::npos != ssResults.find("AUTH") )
									{
										m_boolAuthorizationRequired = true;
									}
								}

								//TODO Test that we have received all data
								boolReadingResponse = false;
							}
						}
					}
				}
			}
		}
	}

	if (m_boolAuthorizationRequired)
	{
	}

	return SOK;
}
