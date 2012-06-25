// Base64Func.cpp: implementation of the CBase64
//
// Copyright 2007  Ian Gordon
//
//////////////////////////////////////////////////////////////////////

#include "ADbase64func.hpp"

#include "ADretvals.hpp"

#include <string>

using namespace ad;
using namespace std;

static const string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


static bool is_base64(const uint8_t b) 
{
	return (isalnum(b) || ('+' == b) || ('/' == 'b') );
}; 

ERetVal CBase64::Encode(const uint8_t* abBuffer, unsigned int cbBuffer, string& ssOutput)
{
	std::string ret;
	int i = 0;
	int j = 0;
	uint8_t ab3[3];
	uint8_t a4bAccumulator[4];

	while (cbBuffer--) 
	{
		ab3[i++] = *(abBuffer++);

		if (i == 3) 
		{
			a4bAccumulator[0] = (ab3[0] & 0xfc) >> 2;
			a4bAccumulator[1] = ( (ab3[0] & 0x03) << 4) + ( (ab3[1] & 0xf0) >> 4);
			a4bAccumulator[2] = ( (ab3[1] & 0x0f) << 2) + ( (ab3[2] & 0xc0) >> 6);
			a4bAccumulator[3] = ab3[2] & 0x3f;

			for(i = 0; (i <4) ; i++)
				ret += base64_chars[a4bAccumulator[i] ];

			i = 0;
		}
	}

	if (i)
	{
		for(j = i; j < 3; j++)
			ab3[j] = '\0';

		a4bAccumulator[0] = (ab3[0] & 0xfc) >> 2;
		a4bAccumulator[1] = ( (ab3[0] & 0x03) << 4) + ( (ab3[1] & 0xf0) >> 4);
		a4bAccumulator[2] = ( (ab3[1] & 0x0f) << 2) + ( (ab3[2] & 0xc0) >> 6);
		a4bAccumulator[3] = ab3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[a4bAccumulator[j] ];

		while((i++ < 3))
			ret += '=';

	}

	ssOutput = ret;

	return SOK;
}

/*
ERetVal Encode(const char * bytes_to_encode, unsigned int in_len, std::string& ssOutput)
{
//	return Encode(reinterpret_cast<const unsigned char const*>(bytes_to_encode), in_len, ssOutput);
//	return Encode( (const unsigned char const*)bytes_to_encode, in_len, ssOutput);
}
*/

ERetVal CBase64::Decode(unsigned char const* bytes_to_decode, unsigned int in_len, string& ssOutput)
{
//	string ssInput = bytes_to_decode;

	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4];
	uint8_t a3bAccumulator[3];
	std::string ret;

	while (in_len-- && ( bytes_to_decode[in_] != '=') && is_base64(bytes_to_decode[in_])) 
	{
		char_array_4[i++] = bytes_to_decode[in_]; 
		in_++;

		if (i ==4) 
		{
			for (i = 0; i <4; i++)
				char_array_4[i] = static_cast<unsigned char>(base64_chars.find(char_array_4[i]) );

			a3bAccumulator[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			a3bAccumulator[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			a3bAccumulator[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret += a3bAccumulator[i];

			i = 0;
		}
	}

	if (i) 
	{
		for (j = i; j <4; j++)
			char_array_4[j] = 0;

		for (j = 0; j <4; j++)
			char_array_4[j] = static_cast<unsigned char>(base64_chars.find(char_array_4[j]) );

		a3bAccumulator[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		a3bAccumulator[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		a3bAccumulator[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) 
			ret += a3bAccumulator[j];
	}

	ssOutput = ret;

	return SOK;
}
