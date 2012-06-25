// ADsocket.cpp:  Source File
//
// Copyright 2007 Ian Gordon
//
/////////////////////////////////////////////////////////////////////////////

#include "ADsocket.hpp"

#if defined(__POSIX__)
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
//KM #include <sys/filio.h> //Not available in Linux 
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#elif defined (WIN32)
#include <ws2tcpip.h>
#include <errno.h>
#endif

#include <cstring>
#include <iostream> //DB
#include <sstream>
#include "ADassert.hpp"


using namespace ad;
using namespace std;


const int CTCPSocket::FAMILY(AF_INET);			// Address Family
const int CTCPSocket::SOCKET_TYPE(SOCK_STREAM);	// Socket Type
const int CTCPSocket::PROTOCOL(IPPROTO_TCP);	// Protocol



CTCPSocket::CTCPSocket()
: m_socket(INVALID_SOCKET), 
  m_boolWriteable(false), 
  m_boolReadable(false), 
  m_boolSSL(false)
{
#if defined (OPENSSL_SOCKETS)
	m_psslctx = NULL;
	m_pssl = NULL;
#endif // defined (OPENSSL_SOCKETS)
	memset(m_szFriendlyName, 0, 16); //MNUM
}

CTCPSocket::~CTCPSocket()
{
	if (INVALID_SOCKET != m_socket)
		Close();
}



ERetVal CTCPSocket::Connect(const std::string& ssAddress, const uint16_t port)
{
	if (INVALID_SOCKET == m_socket)
		return EINIT_FIRST;
	if (ssAddress.empty() )
		return EPARAMETER;

	stringstream sstmServicePort;
	sstmServicePort << port;

	addrinfo addrinfoHints;
	addrinfo *aaddrinfoServer(NULL);

	memset(&addrinfoHints, 0, sizeof(addrinfoHints));
	addrinfoHints.ai_family = FAMILY;
	addrinfoHints.ai_socktype = SOCKET_TYPE;
	addrinfoHints.ai_protocol = PROTOCOL;

	int nRetVal;
	nRetVal = getaddrinfo(ssAddress.c_str(), sstmServicePort.str().c_str(), &addrinfoHints, &aaddrinfoServer);
	if (0 != nRetVal || NULL == aaddrinfoServer)
	{
		return EDATA;
	}

//FIXME	nRetVal = connect(m_socket, aaddrinfoServer[0].ai_addr, aaddrinfoServer[0].ai_addrlen);
	nRetVal = connect(m_socket, aaddrinfoServer[0].ai_addr, static_cast<int>(aaddrinfoServer[0].ai_addrlen) ); //WIN32 cast

	freeaddrinfo(aaddrinfoServer);
	aaddrinfoServer = NULL;

	if (0 == nRetVal)
	{
		m_boolWriteable = true;
		m_boolReadable = true;

		return SOK;
	}
	else
	{
//UNUSED		int nError = GetLastError();
		return EFAIL;
	}
}


/* PRELIM
ERetVal CTCPSocket::Bind(const in_addr& addrName, const uint16_t port)
{
	if (INVALID_SOCKET == m_socket)
		return EINIT_FIRST;
	
	sockaddr_in sockaddrLocal;
	memset(&sockaddrLocal, 0, sizeof(sockaddrLocal) );
	//NOTE Do no hton these values on WIN32
	sockaddrLocal.sin_family = AF_INET;
	sockaddrLocal.sin_addr = addrName;
	sockaddrLocal.sin_port = port; 
	
	int nRetVal;
	nRetVal = bind(m_socket, reinterpret_cast<const sockaddr*>(&sockaddrLocal), sizeof(sockaddrLocal) );
	
	if (0 == nRetVal)
		return SOK;
	else
	{
#if defined(TCPSOCKET_DEBUGLOG)
		cerr << "ERR: Unable to bind() - " << GetLastError() << " : " << strerror(GetLastError() ) << endl;
#endif // defined(TCPSOCKET_DEBUGLOG)
		
		return EFAIL;
	}
}
*/

ERetVal CTCPSocket::Listen(const uint16_t port, const int cBacklog)
{
	return this->Listen("", port, cBacklog);
}

//NOTE Servers do not usually bind to a specific IP address, they just bind to a specific port.  The kernel chooses the source
// IP address when the socket is connected (Stevens - Unix Network Programming Volume 1 - pg 91 - 93)
//TODO Handle ssAddress
ERetVal CTCPSocket::Listen(const std::string& /*ssAddress*/, const uint16_t port, const int cBacklog)
{
	if (INVALID_SOCKET == m_socket)
		return EINIT_FIRST;

	// Allow re-use of existing socket.  
	// Useful for servers where the previous instance may have left an open socket waiting for timeout
	// Ref - http://www.wlug.org.nz/EADDRINUSE 
	unsigned int nValue = 1;
#if defined(__POSIX__)
	if (-1 == setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &nValue, sizeof(nValue) ) ) 
#elif defined(WIN32)
	if (-1 == setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&nValue), sizeof(nValue) ) ) 
#else
#error
#endif
	{
		Assert(!"setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, 1)");
	}
  
  /*
  KM
	sockaddr_in sockaddrListen;
	memset(&sockaddrListen, 0, sizeof(sockaddrListen) );
	sockaddrListen.sin_family = FAMILY;
	sockaddrListen.sin_addr.s_addr = INADDR_ANY;
	sockaddrListen.sin_port = port;

	addrinfo addrinfoListen;
	memset(&addrinfoListen, 0, sizeof(addrinfoListen) );
	addrinfoListen.ai_family = FAMILY;
	addrinfoListen.ai_socktype = SOCKET_TYPE;
	addrinfoListen.ai_protocol = PROTOCOL;
	addrinfoListen.ai_addr = reinterpret_cast<sockaddr*>(&sockaddrListen);
	addrinfoListen.ai_addrlen = sizeof(sockaddrListen);
*/

	in_addr addrLocal;
	memset(&addrLocal, 0, sizeof(addrLocal) );
	addrLocal.s_addr = htonl(INADDR_ANY);

	sockaddr_in sockaddrLocal;
	memset(&sockaddrLocal, 0, sizeof(sockaddrLocal) );
	sockaddrLocal.sin_family = AF_INET;
	sockaddrLocal.sin_addr = addrLocal;
	sockaddrLocal.sin_port = htons(port);

	int nRetVal;

	nRetVal = bind(m_socket, reinterpret_cast<sockaddr*>(&sockaddrLocal), sizeof(sockaddrLocal) );
	if (0 != nRetVal)
	{
		nRetVal = GetLastError();
		//std::cerr << "E" << nRetVal << endl; //KM
		//Assert(!"CTCPSocket::Listen - Unable to bind()");

		if (EADDRINUSE == nRetVal)
			return EINIT_ALREADY;
		else
			return EFAIL;
	}

	nRetVal = listen(m_socket, cBacklog);
	if (0 != nRetVal)
	{
		if (EADDRINUSE == nRetVal)
			cerr << "ERR: EADDRINUSE == listen()" << endl;
		else
			Assert(!"CTCPSocket::Listen - Unable to listen()");

		return EFAIL;
	}

	return SOK;
}


ERetVal CTCPSocket::Accept(CTCPSocket& socketConnected, sockaddr_in& sockaddrConnected)
{
	if (INVALID_SOCKET == m_socket)
		return EINIT_FIRST;
	if (socketConnected.IsValid() )  // Can't overwrite existing socket, socketConnected must be closed first.
		return EPARAMETER;

	BaseSocket basesocketConnected;
	socklen_t cbStruct = sizeof(sockaddrConnected);

repeat:
	basesocketConnected = accept(m_socket, reinterpret_cast<sockaddr*>(&sockaddrConnected), &cbStruct);
	if (0 > basesocketConnected)
	{
		if (EINTR == GetLastError() )
			goto repeat;
	}

	if (INVALID_SOCKET != basesocketConnected)
	{
		socketConnected.m_addrOrigin = sockaddrConnected;
		
		return socketConnected.Socket(basesocketConnected);
	}
	else
	{
		int nError = GetLastError();
		if (EWOULDBLOCK == nError)
			return ENET_WOULDBLOCK;
		else
			return EFAIL;
	}
}


ERetVal CTCPSocket::Close(void)
{
	int nRetVal;

	if (m_boolSSL)
	{
#if defined (OPENSSL_SOCKETS)
		Assert(m_pssl);

		SSL_shutdown(m_pssl);
#endif // defined (OPENSSL_SOCKETS)

		m_boolSSL = false;

		nRetVal = 0;
	}
	else
	{
#if defined(__POSIX__)
		nRetVal = close(m_socket);
#elif defined(WIN32)
		nRetVal = closesocket(m_socket);
#endif
	}

	if (0 == nRetVal)
	{
		m_socket = INVALID_SOCKET;

		m_boolReadable = false;
		m_boolWriteable = false;

		return SOK;
	}
	else
	{
		if (EINTR == GetLastError() )
		{
			return EFAIL;
		}
#if defined (__POSIX__)
		else if (EBADF == GetLastError() )
		{
			m_socket = INVALID_SOCKET;

			return SNOWORK;
		}
#endif // defined (__POSIX__)
		else
		{
//			int nError = GetLastError();
			return EFAIL;
		}
	}
}

void CTCPSocket::Shutdown(EShutdownHow eHow)
{
	if (INVALID_SOCKET == this->GetBaseSocket() )
		return;
	
	int nHow;

#if defined (__POSIX__)
	if (SHUTDOWN_SEND == eHow)
		nHow = SHUT_WR;
	else if (SHUTDOWN_RECEIVE == eHow)
		nHow = SHUT_RD;
	else // if (SHUTDOWN_BOTH == eHow || (SHUTDOWN_SEND & eHow && SHUTDOWN_RECEIVE & eHow) )
		nHow = SHUT_RDWR;
#elif defined(WIN32)
	if (SHUTDOWN_SEND == eHow)
		nHow = SD_SEND;
	else if (SHUTDOWN_RECEIVE == eHow)
		nHow = SD_RECEIVE;
	else // if (SHUTDOWN_BOTH == eHow || (SHUTDOWN_SEND & eHow && SHUTDOWN_RECEIVE & eHow) )
		nHow = SD_BOTH;
#else
#error
#endif

	//TODO Support SSL Sockets

	shutdown(this->GetBaseSocket(), nHow);
}


ERetVal CTCPSocket::Write(const std::vector<uint8_t>& vbSource, size_t& cbWritten)
{
	if (m_boolSSL)
		return this->WriteTLS(&vbSource[0], vbSource.size(), cbWritten);
	else
		return this->Write(&vbSource[0], vbSource.size(), cbWritten);
}

ERetVal CTCPSocket::Write(const char* pabSource, const size_t cbSize, size_t& cbWritten)
{
	if (m_boolSSL)
		return this->WriteTLS(reinterpret_cast<const uint8_t*>(pabSource), cbSize, cbWritten);
	else
		return this->Write(reinterpret_cast<const uint8_t*>(pabSource), cbSize, cbWritten);
}

ERetVal CTCPSocket::Write(const uint8_t* pabSource, const size_t cbSize, size_t& cbWritten)
{
	if (INVALID_SOCKET == m_socket)
		return EINIT_FIRST;
	if (!m_boolWriteable)
		return EPERMISSION;
	if (NULL == pabSource)
		return EPOINTER;
	if (m_boolSSL)
		return WriteTLS(pabSource, cbSize, cbWritten);

	//NOTE cbSize can be 0. 
	//  MSDN: Calling send with a zero len parameter is permissible and will be treated 
	//  by implementations as successful. In such cases, send will return zero as a valid 
	//  value. For message-oriented sockets, a zero-length transport datagram is sent.

	cbWritten = 0;

	int cbSendWritten;

	//NOTE: Retry while (SOCKET_ERROR == cbSendWritten && EINTR == GetLastError() )
	do
	{
		//FIXME Verify cast on POSIX
#if defined (__POSIX__)
		cbSendWritten = send(m_socket, pabSource, cbSize, 0); // 0 = No Flags
#elif defined (WIN32)
		cbSendWritten = send(m_socket, reinterpret_cast<const char*>(pabSource), static_cast<int>(cbSize), 0); // 0 = No Flags
#endif
	}
	while (SOCKET_ERROR == cbSendWritten && EINTR == GetLastError() );

	if (SOCKET_ERROR == cbSendWritten)
	{
		std::cerr << "Socket send Error  E" << GetLastError()  << std::endl;

		cbWritten = 0;
		//KM Assert(0);

		return EFAIL;
	}

	cbWritten = cbSendWritten;

	return SOK;
}

ERetVal CTCPSocket::WriteTLS(const uint8_t* pabSource, const size_t cbSize, size_t& cbWritten)
{
#if !defined (OPENSSL_SOCKETS)
	return ENOTIMPL;
#else
	if (INVALID_SOCKET == m_socket)
		return EINIT_FIRST;
	if (!m_boolWriteable)
		return EFAIL;
	if (NULL == pabSource)
		return EPOINTER;
	if (!m_boolSSL)
	{
		Assert(!"CTCPSocket::WriteTLS - SSL not active");
		return EFAIL;
	}

	int cbSendWritten(0);
	do
	{
		cbSendWritten = SSL_write(m_pssl, pabSource, cbSize);
	}
	while (SSL_ERROR_WANT_WRITE == cbSendWritten);

	//TODO Handle End of Socket, etc.

	if (0 > cbWritten)
		return EFAIL;
	else
	{
		cbWritten = cbSendWritten;

		return SOK;
	}
#endif // !defined (OPENSSL_SOCKETS)
}

ERetVal CTCPSocket::Read(uint8_t* pabDestination, const size_t cbSize, size_t& cbRead)
{
	if (INVALID_SOCKET == m_socket)
		return EINIT_FIRST;

	if (m_boolSSL)
		return RecvTLS(pabDestination, cbSize, cbRead, RECV_READ);
	else
		return Recv(pabDestination, cbSize, cbRead, RECV_READ);
}

ERetVal CTCPSocket::Read(std::vector<uint8_t>& vbBuffer)
{
	if (INVALID_SOCKET == m_socket)
		return EINIT_FIRST;

	int cbMaxRcvBuf(0);
	socklen_t sizeofcbMaxRcvBuf(sizeof(cbMaxRcvBuf) );

	int nRetVal;
	nRetVal = getsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&cbMaxRcvBuf), &sizeofcbMaxRcvBuf);
	if (0 != nRetVal)
	{
		Assert(0);
		return EFAIL;
	}

	vbBuffer.resize(cbMaxRcvBuf);

	ERetVal eRetVal;
	size_t cbRead;
	eRetVal = Read(&vbBuffer[0], vbBuffer.size(), cbRead);

	if (Succeeded(eRetVal) )
		vbBuffer.resize(cbRead);
	else
		vbBuffer.resize(0);

	return eRetVal;
}

ERetVal CTCPSocket::Peek(uint8_t* pabDestination, const size_t cbSize, size_t& cbRead)
{
	if (INVALID_SOCKET == m_socket)
		return EINIT_FIRST;

	if (m_boolSSL)
		return RecvTLS(pabDestination, cbSize, cbRead, RECV_PEEK);
	else
		return Recv(pabDestination, cbSize, cbRead, RECV_PEEK);
}

ERetVal CTCPSocket::Recv(uint8_t* pabDestination, const size_t cbSize, size_t& cbRecv, ERecvFlag eRecvFlag)
{
	if (!m_boolReadable)
		return EPERMISSION;
	if (m_boolSSL)
	{
		Assert(0);
		return EFAIL;
	}
	
	Assert(INVALID_SOCKET != m_socket);  //KM m_boolReadable should catch closed sockets

	int nFlags;
	if (RECV_PEEK == eRecvFlag)
		nFlags = MSG_PEEK;
	else // if (RECV_READ)
		nFlags = 0;

	//NOTE: Retry while (SOCKET_ERROR == cbSendWritten && EINTR == GetLastError() )
	do
	{
			//FIXME Verify cast on POSIX
#if defined (__POSIX__)
		cbRecv = recv(m_socket, pabDestination, cbSize, nFlags);
#elif defined (WIN32)
		cbRecv = recv(m_socket, reinterpret_cast<char*>(pabDestination), static_cast<int>(cbSize), nFlags);
#endif
	}
	while (SOCKET_ERROR == cbRecv && EINTR == GetLastError() );


	ERetVal eRetVal;

	if (SOCKET_ERROR == cbRecv)
	{
		if (EWOULDBLOCK == GetLastError() )
		{
			cbRecv = 0;

			eRetVal = ENET_WOULDBLOCK;
		}
		else if (ECONNABORTED == GetLastError() )
		{
			cbRecv = 0;

			m_boolReadable = false;
			m_boolWriteable = false;

			eRetVal = ENET_CONNABORTED;
		}
		else if (ECONNRESET == GetLastError() )
		{
			cbRecv = 0;

			m_boolReadable = false;
			m_boolWriteable = false;

			eRetVal = ENET_CONNABORTED;
		}
		else
		{
			//TODO
			cbRecv = 0;
			Assert(0);

			eRetVal = EFAIL;
		}
	}
	else if (0 == cbRecv) // Handle Finished Sockets
	{
		m_boolReadable = false;

		eRetVal = SNOWORK;
	}
	else
	{
		eRetVal = SOK;
	}

	return eRetVal;
}

//TODO Fixme
ERetVal CTCPSocket::RecvTLS(uint8_t* pabDestination, const size_t cbSize, size_t& cbRecv, ERecvFlag eRecvFlag)
{
#if !defined (OPENSSL_SOCKETS)
	cbRecv = 0;

	return ENOTIMPL;
	//TODO return EFAIL
#else
	if (!m_boolReadable)
		return EFAIL;
	if (!m_boolSSL)
	{
		Assert(0);
		return EFAIL;
	}

	do
	{
		if (RECV_READ == eRecvFlag)
			cbRecv = SSL_read(m_pssl, pabDestination, cbSize);
		else
			cbRecv = SSL_peek(m_pssl, pabDestination, cbSize);
	}
	while (SSL_ERROR_WANT_READ == cbRecv);

	//TODO Handle End of Socket, etc.


	if (0 > cbRecv)
		return EFAIL;
	else
		return SOK;
#endif
}

BaseSocket CTCPSocket::GetBaseSocket(void) const
{
	return m_socket;
}


int CTCPSocket::GetLastError(void) const
{
#if defined(__POSIX__)
	return errno;
#elif defined(WIN32)
	return WSAGetLastError();
#endif
}


ERetVal CTCPSocket::Socket(const ESocketBlocking eBlocking)
{
	if (INVALID_SOCKET != m_socket)
		return EINIT_ALREADY;

	m_socket = socket(FAMILY, SOCKET_TYPE, PROTOCOL);

	if (INVALID_SOCKET == m_socket)
	{
#if defined(WIN32)
		if (WSANOTINITIALISED == GetLastError() )
		{
			// A successful WSAStartup call must occur before using this function.
			Assert(false);

			return EINIT_FIRST;
		}
#endif

		return EMEM_ALLOC;
	}
	else
	{
		ERetVal eRetVal;


		if (BLOCKING == eBlocking)
			eRetVal = SetBlocking(true);
		else  // NONBLOCKING == eBlocking
			eRetVal = SetBlocking(false);

		return eRetVal;
	}
}


ERetVal CTCPSocket::Socket(BaseSocket& basesocket)
{
	if (INVALID_SOCKET != m_socket)
		return EINIT_ALREADY;

	//TODO Verify basesocket is a socket
	m_socket = basesocket;

	//??? Verify both channels are open
	m_boolReadable = true;
	m_boolWriteable = true;

	return SOK;
}



#if defined (__POSIX__)

ERetVal CTCPSocket::SetBlocking(const bool boolBlocking)
{
	int nValue;

	nValue = fcntl(m_socket, F_GETFL, 0);
	if (0 > nValue)
	{
		Assert(0);

		return EFAIL;
	}
	else
	{
		if (boolBlocking)
			nValue = nValue & ~O_NONBLOCK;
		else
			nValue = nValue | O_NONBLOCK;

		nValue = fcntl(m_socket, F_SETFL, nValue);
		if (0 > nValue)
		{
			Assert(0);

			return EFAIL;
		}
		else
			return SOK;
	}
}

#elif defined (WIN32)

ERetVal CTCPSocket::SetBlocking(const bool boolBlocking)
{
	int nRetVal;

	int nValue = !boolBlocking;
	nRetVal = ioctlsocket(m_socket, FIONBIO, reinterpret_cast<u_long*>(&nValue) );

	if (0 != nRetVal)
	{
		Assert(0);

		return EFAIL;
	}
	else
	{
		return SOK;
	}
}

#endif

bool CTCPSocket::IsValid(void) const
{
	if (INVALID_SOCKET == m_socket)
		return false;
	else
		return true;
}


size_t CTCPSocket::GetIncomingByteCount(void) const
{
	if (INVALID_SOCKET == m_socket)
		return 0;

	int nRetVal;

#if defined(__POSIX__)
	int cbBytes;
	nRetVal = ioctl(m_socket, FIONREAD, reinterpret_cast<char*>(&cbBytes) );
#elif defined(WIN32)
	u_long cbBytes;
	nRetVal = ioctlsocket(m_socket, FIONREAD, &cbBytes);
#endif

	if (SOCKET_ERROR == nRetVal)
	{
		cbBytes = 0;
	}

	return cbBytes;
}



ERetVal CTCPSocket::OSNetworkStartup(void)
{
#if defined (WIN32)
	WORD wVersionRequested(MAKEWORD(2, 2) );
	WSADATA wsadata;
	int nWSRetVal;
	nWSRetVal = WSAStartup(wVersionRequested, &wsadata);
	if (0 != nWSRetVal)
	{
		Assert(0);

		WSACleanup();

		return EFAIL;
	}
#endif

#if defined (OPENSSL_SOCKETS)
	SSL_load_error_strings();
	SSL_library_init();  //TODO Retvals
#endif // defined (OPENSSL_SOCKETS)

	return SOK;
}

void CTCPSocket::SetCloseOnExec(bool boolCloseOnExec)
{
#if defined(TCPSOCKET_DEBUGLOG)
	cerr << "CIPSocket::SetCloseOnExec(bool boolCloseOnExec)" << endl;
#endif
	
#if defined (__POSIX__)
	int nValue;
	
	nValue = fcntl(m_socket, F_GETFD);
	if (-1 == nValue)
	{
		std::cout << "CIPSocket::SetCloseOnExec - Unable to get Close-On-Exec" << std::endl;
		
		Assert(0);
	}
	else
	{
		if (boolCloseOnExec)
			nValue = nValue | FD_CLOEXEC;
		else
			nValue = nValue & ~FD_CLOEXEC;
		
		int nRetVal;
		nRetVal = fcntl(m_socket, F_SETFD, nValue);
		if (-1 == nRetVal)
		{
#if defined(TCPSOCKET_DEBUGLOG)
			std::cerr << "CIPSocket::SetCloseOnExec - Unable to set Close-On-Exec" << std::endl;
#endif // defined(TCPSOCKET_DEBUGLOG)
			
			Assert(0);
		}
	}
	
	
#elif defined (WIN32)
	//Not Implemented
#endif
	
	return;
}

bool CTCPSocket::IsCloseOnExec(void) const
{
	bool boolIsCloseOnExec;
	
#if defined(TCPSOCKET_DEBUGLOG)
	cerr << "CIPSocket::IsCloseOnExec(void) const" << endl;
#endif
	
#if defined (__POSIX__)
	int nValue;
	
	nValue = fcntl(m_socket, F_GETFD, 0);
	
	boolIsCloseOnExec = (nValue & FD_CLOEXEC);
	
#elif defined (WIN32)
	boolIsCloseOnExec = true;
#endif
	
	return boolIsCloseOnExec;
}


void CTCPSocket::SetReuseAddress(bool boolReuserAddress)
{
	// Allow re-use of existing socket.  
	// Useful for servers where the previous instance may have left an open socket waiting for timeout
	// Ref - http://www.wlug.org.nz/EADDRINUSE 
	unsigned int nValue;
	if (boolReuserAddress)
		nValue = 1;
	else
		nValue = 0;
	
	this->SetOption(SO_REUSEADDR, &nValue, sizeof(nValue) );
}

ERetVal CTCPSocket::SetOption(int optname, const void* pvValue, int cbValue)
{
#if defined(TCPSOCKET_DEBUGLOG)
	cerr << "CIPSocket::SetOption(int optname, const void* pvValue, int cbValue)" << endl;
#endif
	
	int nRetVal;
	
#if defined (__POSIX__)
	nRetVal = setsockopt(m_socket, SOL_SOCKET, optname, pvValue, cbValue);
#elif defined (WIN32)
	nRetVal = setsockopt(m_socket, SOL_SOCKET, optname, (char*)pvValue, cbValue); //CAST
#endif
	if (0 == nRetVal)
		return SOK;
	else
		return EFAIL;
}


void CTCPSocket::OSNetworkShutdown(void)
{
#if defined (OPENSSL_SOCKETS)
	//TODO Uninit SSL_library_init();
#endif // defined (OPENSSL_SOCKETS)

#if defined (WIN32)
	WSACleanup();
#endif

	return;
}

std::string CTCPSocket::GetHostname(void)
{
	int nRetVal;

	const int cchHostname(256);
	char achHostname[cchHostname];
	nRetVal = gethostname(achHostname, cchHostname);
	if (SOCKET_ERROR == nRetVal)
	{
		Assert(!"Unable to gethostname");
		return "";
	}
	else
	{
		return std::string(achHostname);
	}
}

//FIXME Handle multiple IPs
std::string CTCPSocket::GetIPAddress(void)
{
	//We jump through a couple hoops in order to get the external facing IP address

	int nRetVal;

	// Determine Host
	const int cchHostname(256);
	char achHostname[cchHostname];
	nRetVal = gethostname(achHostname, cchHostname);
	if (SOCKET_ERROR == nRetVal)
	{
		Assert(!"Unable to gethostname");
		return "";
	}
	
	// Determine IP address from host
	hostent* phostent(NULL);
	phostent = gethostbyname(achHostname);
//NOTE	gethostent() returns 127.0.0.1 for the ip address
	if (NULL == phostent)
	{
//FIXME Doesn't work on FreeBSD7
		//Assert(!"Unable to gethostbyname");
		return "";
	}
	else
	{
		if (phostent->h_addr_list[0])
		{
			struct in_addr addr;
			memcpy(&addr, phostent->h_addr_list[0], sizeof(struct in_addr) );

			return inet_ntoa(addr);
		}
	}

	return "";
}

const char* CTCPSocket::GetOriginIPAddress(void)
{
	return inet_ntoa(m_addrOrigin.sin_addr);
}


ERetVal CTCPSocket::StartTLS(void)
{
#if !defined (OPENSSL_SOCKETS)
	return ENOTIMPL;

#else
	m_psslctx = SSL_CTX_new(TLSv1_client_method() );
	if (NULL == m_psslctx)
	{
		Assert(0);

		return EFAIL;
	}

	m_pssl = SSL_new(m_psslctx);
	if (NULL == m_pssl)
	{
		Assert(0);

		return EFAIL;
	}

	int nRetVal;

	nRetVal = SSL_set_fd(m_pssl, GetBaseSocket() );
	if (1 != nRetVal)
	{
		Assert(0);

		return EFAIL;
	}

	nRetVal = SSL_connect(m_pssl);
	//??? Why not nRetVal = SSL_do_handshake(g_pssl);
	if (1 != nRetVal)
	{
		Assert(0);

		return EFAIL;
	}

	m_boolSSL = true;

	return SOK;

#endif
}
