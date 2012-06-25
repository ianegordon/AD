// ADassert.cpp:  Source File
//
// Copyright 2007 Ian Gordon
//
/////////////////////////////////////////////////////////////////////////////

#include "ADassert.hpp"
#include "ADlog.hpp"

#if defined (__POSIX__)
#include <cassert>
#elif defined(MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4389)
#endif // defined (__POSIX__)

namespace ad
{

	void AssertFailed(const char* szFilename, const int nLine)
	{
		log << Error() << "Assert Failed :: " << szFilename << " : " << nLine << std::endl;
		
#if defined(DEBUG)
		assert(0);
#endif // defined(DEBUG)
	}
} // namespace ad

#if defined(MSC_VER)
#pragma warning(pop)
#endif
