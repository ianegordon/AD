// ADsocket.hpp:  Socket Include File
//
// Copyright 2007 Ian Gordon
//
/////////////////////////////////////////////////////////////////////////////

// To include OpenSSL suuport OPENSSL_SOCKETS must be defined

// TODO IP6 Support


#if !defined(__ADSOCKET_HPP__)
#define __ADSOCKET_HPP__

#if !defined(__POSIX__) && !defined(WIN32)
#error Must #define either WIN32 or __POSIX__
#endif
#if defined(__POSIX__) && defined(WIN32)
#error Cannot #define both WIN32 and __POSIX__
#endif


#if defined(__POSIX__)
#include <stdint.h>
#include <tcpd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#elif defined(WIN32)
#include <Winsock2.h>
#define ECONNABORTED WSAECONNABORTED 
#define EWOULDBLOCK WSAEWOULDBLOCK  
#define ENOTSOCK WSAENOTSOCK  
#define ECONNRESET WSAECONNRESET
#define EADDRINUSE WSAEADDRINUSE
// #define EINTR WSAEINTR  //??? KM Already in errno.h
#include "pstdint.h"
#endif //defined(WIN32)

#if defined (OPENSSL_SOCKETS)
#include <openssl/ssl.h>
#endif // defined (OPENSSL_SOCKETS)

#include <string>
#include <vector>

#include "ADretvals.hpp"


namespace ad
{

#if defined(__POSIX__)
	typedef int BaseSocket;
	static const int INVALID_SOCKET(-1);
	static const ssize_t SOCKET_ERROR(-1);
#elif defined (WIN32)
	typedef SOCKET BaseSocket;
#endif

	class CTCPSocket
	{
	public:
		CTCPSocket();
		~CTCPSocket();

		enum ESocketBlocking
		{
			BLOCKING,
			NON_BLOCKING
		};

		ERetVal Socket(const ESocketBlocking eBlocking = BLOCKING);
		ERetVal Socket(BaseSocket& basesocket);

		//\ @retval SOK : EDATA - Unable to determine host : EFAIL - Unable to connect to host
		ERetVal Connect(const std::string& ssAddress, const uint16_t port);
		ERetVal Close(void);

		enum EShutdownHow
		{
			SHUTDOWN_SEND = 1,
			SHUTDOWN_RECEIVE = 2,
			SHUTDOWN_BOTH = 3
		};
		void Shutdown(EShutdownHow eHow);

		//TODO ERetVal Bind(const in_addr& addrName, const uint16_t port);

		/// Returns EINIT_ALREADY if the port is already in use
		//FIXME Duplication Below
		ERetVal Listen(const uint16_t port, const int cBacklog = 0);  //KM ???
		ERetVal Listen(const std::string& ssAddress, const uint16_t port, const int cBacklog = 0);
		ERetVal Accept(CTCPSocket& socketConnected, sockaddr_in& sockaddrConnected);

		ERetVal Peek(uint8_t* pabDestination, const size_t cbSize, size_t& cbRead);

		ERetVal Read(uint8_t* pabDestination, const size_t cbSize, size_t& cbRead);
		ERetVal Read(std::vector<uint8_t>& vbBuffer);

		ERetVal Write(const char* pabSource, const size_t cbSize, size_t& cbWritten);
		ERetVal Write(const uint8_t* pabSource, const size_t cbSize, size_t& cbWritten);
		ERetVal Write(const std::vector<uint8_t>& vbSource, size_t& cbWritten);

		BaseSocket GetBaseSocket(void) const;
		int GetLastError(void) const;

		bool IsValid(void) const;

		size_t GetIncomingByteCount(void) const;

		ERetVal StartTLS(void);

		void SetCloseOnExec(bool boolCloseOnExec);
		bool IsCloseOnExec(void) const;
		
		void SetReuseAddress(bool boolReuserAddress);
		
		ERetVal SetOption(int optname, const void* pvValue, int cbValue);
	
		const char* GetSZ(void) const {
			return m_szFriendlyName;
		}
		
		static ERetVal OSNetworkStartup(void);
		static void OSNetworkShutdown(void);

		static std::string GetHostname(void);
		static std::string GetIPAddress(void);
		
		const char* GetOriginIPAddress(void);
		sockaddr_in m_addrOrigin;  // Set in accept() on incoming connections

	private:
		BaseSocket m_socket;
		
		bool m_boolWriteable;
		bool m_boolReadable;

		bool m_boolSSL;
		
		//TODO IP6 support
		char m_szFriendlyName[16]; // Maximum Length : 111.222.111.222\0

		static const int FAMILY;		// Address Family = AF_INET
		static const int SOCKET_TYPE;	// Socket Type = SOCK_STREAM
		static const int PROTOCOL;		// Protocol = IPPROTO_TCP

#if defined (OPENSSL_SOCKETS)
		SSL_CTX* m_psslctx;
		SSL* m_pssl;
#endif // defined (OPENSSL_SOCKETS)

		enum ERecvFlag
		{
			RECV_READ,
			RECV_PEEK
		};
		ERetVal Recv(uint8_t* pabDestination, const size_t cbSize, size_t& cbRecv, ERecvFlag eRecvFlag);
		ERetVal RecvTLS(uint8_t* pabDestination, const size_t cbSize, size_t& cbRecv, ERecvFlag eRecvFlag);

		ERetVal WriteTLS(const uint8_t* pabSource, const size_t cbSize, size_t& cbWritten);

		ERetVal SetBlocking(const bool boolBlocking);
	};

} //namespace ad

#endif // !defined(__ADSOCKET_HPP__)
