//UNDUPE

#if defined(__POSIX__)
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#elif defined (WIN32)
#include <cstdio>
#else
#compiletimeerror
#endif

#include <cstdlib>
#include <cstring>
#include <vector>
#include <iostream> //DB

#include "ADlog.hpp"
#include "ADretvals.hpp"
#include "ADspawn.hpp"

using namespace ad;
using namespace std;

ad::BasePID ad::GetProcessID(void)
{
#if defined(__POSIX__)
	return getpid();
#elif defined(WIN32)
	return _getpid();
#endif
}


ad::ERetVal ad::SpawnVP(char* const argv[], ad::BasePID& pidChild)
{
	if (!(argv && argv[0]))
		return SNOWORK;

#if defined(__POSIX__)

	pidChild = fork();
	if (-1 == pidChild)
	{
		if (EAGAIN == errno)
		{
			return ESYSTEMLIMIT;
		}
		else if (ENOMEM == errno)
		{
			return EOUTOFSPACE;
		}
		else
			return EFAIL;
	}
	else if (0 == pidChild)
	{
		// In child process 
		
		//NOTE exec won't return if it succeeds
		execvp(argv[0], &argv[0]);

		log << Error() << "Failed to execvp " << argv[0] << endl; //DB

		exit(-1);
	}

	return SOK;


#elif defined (WIN32)

	_flushall();  // Win32

	pidChild = _spawnvp(_P_NOWAIT, argv[0], &argv[0]);
	if (-1 == pidChild)
		return EFAIL;
	else
		return SOK;
#endif

}

ad::ERetVal ad::Spawn(const std::string& ssCommandLine, ad::BasePID& pidChild)
{
	//TODO Modernize / Remove Pointer Madness

	if (ssCommandLine.empty() )
		return SNOWORK;

	char* pszCommandLine = new char[ssCommandLine.length() + 1];
	memcpy(pszCommandLine, ssCommandLine.c_str(), ssCommandLine.length() );
	pszCommandLine[ssCommandLine.length() ] = 0;

	char* pchToken = pszCommandLine;

	vector<char*> vpszCommandLine;

	while (0 != *pchToken)
	{
		while (' ' == *pchToken || '\t' == *pchToken || '\n' == *pchToken)
		{
			*pchToken = 0;
			++pchToken;
		}

		vpszCommandLine.push_back(pchToken);

		while (' ' != *pchToken && '\t' != *pchToken && '\n' != *pchToken && 0 != *pchToken)
			++pchToken;
	}

	vpszCommandLine.push_back(NULL);

	ERetVal eRetVal = SpawnVP(&vpszCommandLine[0], pidChild);

	delete [] pszCommandLine;

	return eRetVal;
}
