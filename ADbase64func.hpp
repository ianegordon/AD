// Base64Func.hpp: definition of CBase64
//
// Copyright 2007  Ian Gordon
//
//////////////////////////////////////////////////////////////////////
// Original Author: Rene Nyffenegger  http://www.adp-gmbh.ch/cpp/common/base64.html
// Clean Up
// FInd original internet source to credit
//TODO Convert to boost::archive::iterators

#if !defined(__ADBASE64_HPP__)
#define __ADBASE64_HPP__

#include "ADretvals.hpp"
//#include "pstdint.h"
#include <string>
#include "pstdint.h"

//TODO Decode string -> vector<byte>

namespace ad
{

class CBase64
{
public:
	static ad::ERetVal Encode(const uint8_t* abBuffer, unsigned int cbBuffer, std::string& ssOutput);  
	static ad::ERetVal Decode(unsigned char const* bytes_to_decode, unsigned int in_len, std::string& ssOutput);  

	//TODO
	//	static ad::ERetVal Encode(const char * bytes_to_encode, unsigned int in_len, std::string& ssOutput);  
	// vector<> based functions
	// size_t GetEncodedSize(cb) const;
	// size_t GetDecodedSize(cb) const;
};

} //namespace bc

#endif // !defined(__ADBASE64_HPP__)
