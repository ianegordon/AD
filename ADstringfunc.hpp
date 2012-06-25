// StringFunc.h: definition of String Functions.
//
// Copyright 2007  Ian Gordon
//
//////////////////////////////////////////////////////////////////////

//REFACTOR - transform(ssIn.begin(), ssIn.end(), inserter(ssOut, ssOut.begin()), to_lower);
//NOTE : Clearing a stringstream for reuse
//	sstm.str("");
//NOTE : Assigning a string to a buffer w/ '\0' chars
//	ss.assign("abc\0def\0ghi\0",12);
//TODO TrimWhitespace should return the trimmed String

#if !defined(__ADSTRINGFUNC_HPP__)
#define __ADSTRINGFUNC_HPP__

#if !defined(__POSIX__) && !defined(WIN32)
#error Must #define either WIN32 or __POSIX__
#endif
#if defined(__POSIX__) && defined(WIN32)
#error Cannot #define both WIN32 and __POSIX__
#endif

#if defined(WIN32)
#include "pstdint.h"
#elif defined(__POSIX__)
#include <stdint.h>
#endif

#include <string>
#include <vector>
#include "ADretvals.hpp"

namespace ad {

// Lowercasify
const std::string& ToLower(const std::string& ssString, std::string& ssStringOut);

// Trim leading and ending Whitespace
ad::ERetVal TrimWhitespace(std::string& ssString);

//TODO
ad::ERetVal ConvertUTF8ToUCS2LE(const std::vector<uint8_t>& vbInput, std::vector<uint8_t>& vbOutput);
ad::ERetVal ConvertUTF8ToUCS2BE(const std::vector<uint8_t>& vbInput, std::vector<uint8_t>& vbOutput);
ad::ERetVal ConvertUCS2LEToUTF8(const std::vector<uint8_t>& vbInput, std::vector<uint8_t>& vbOutput);
ad::ERetVal ConvertUCS2BEToUTF8(const std::vector<uint8_t>& vbInput, std::vector<uint8_t>& vbOutput);
} //namespace bc

#endif // !defined(__ADSTRINGFUNC_HPP__)
