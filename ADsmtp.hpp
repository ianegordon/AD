// ADsmtp.hpp:  SMTP Include File
//
// Copyright 2007  Ian Gordon
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(__ADSMTP_HPP__)
#define __ADSMTP_HPP__

#if !defined(__POSIX__) && !defined(WIN32)
#error Must #define either WIN32 or __POSIX__
#endif
#if defined(__POSIX__) && defined(WIN32)
#error Cannot #define both WIN32 and __POSIX__
#endif

#include <string>

#include "pstdint.h"
#include "ADretvals.hpp"
#include "ADsocket.hpp"


namespace ad
{
	class CSMTPContext
	{
	public:
		CSMTPContext();
		~CSMTPContext();

		ad::ERetVal Connect(const std::string& ssServer, const uint16_t nPort = 25);
		void Disconnect(void);

		//TODO ad::ERetVal Login(const std::string& ssHostname);
		ad::ERetVal EHLO(void);  //TODO Move to Private call from Login()

		void SetSender(const std::string& ssSender);
		void AddRecipient(const std::string& ssRecipient);

		void Send(const std::string& ssMessage);

	private:
		enum EState {
			SMTP_UNCONNECTED,
			SMTP_AUTHORIZATION,
			SMTP_TRANSACTION,
			SMTP_RECIPIENTS,
			SMTP_DATA
		} m_eState;

		ad::CTCPSocket m_socket;

		bool m_bool8bitmime;
		bool m_boolAuthorizationRequired;
		bool m_boolESMTP;
		bool m_boolPipelining;
		bool m_boolStartTLS;
		size_t m_cbMaxSize;

		// ProcessCapabilities(void);
	};

} // namespace ad


#endif // !defined(__ADSMTP_HPP__)
