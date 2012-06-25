// UNDUPE

//
// returns ERetVals
//
// argv[0] is file to execute, including relative path
// argv[1]-[n] are additional arguments to Spawn
// argv[n+1] should be null
//
// eg. 
//
// char* AppArgs[] = {"./helloworld", "argA", "arg b", 0};
// char* AppArgs[] = {"xbot", "Crafty", 0};
//
// eRetVal = Spawn(AppArgs, pid);
//
// the system path will be searched for the file if not found 
// in current directory and a path is not specified.
//

#if !defined(__ADSPAWN_HPP__)
#define __ADSPAWN_HPP__

#if !defined(__POSIX__) && !defined(WIN32)
#error Must #define either WIN32 or __POSIX__
#endif
#if defined(__POSIX__) && defined(WIN32)
#error Cannot #define both WIN32 and __POSIX__
#endif

#include "ADretvals.hpp"
#include <string>

#if defined(__POSIX__)
#include <unistd.h>
#elif defined (WIN32)
#include <process.h>
#endif

namespace ad
{
#if defined(__POSIX__)
typedef pid_t BasePID;
#elif defined (WIN32)
typedef intptr_t BasePID;
#endif

ad::BasePID GetProcessID(void);
ad::ERetVal Spawn(const std::string& ssCommandLine, ad::BasePID& pidChild);
ad::ERetVal SpawnVP(char* const argv[], ad::BasePID& pidChild);

} //namespace ad

#endif // !defined(__ADSPAWN_HPP__)
