//UNDUPE - Use boost functions

// StringFunc.cpp: implementation of the string functions.
//
// Copyright 2007  Ian Gordon
//
//////////////////////////////////////////////////////////////////////

#include "ADstringfunc.hpp"
#include "ADretvals.hpp"

#include <locale>
#include <algorithm>
#include <functional>

using namespace std;

namespace ad {

// From c++ FAQ:
const std::string& ToLower(const std::string& ssString, std::string& ssStringOut)
{
	/*
	if (&ssString == &ssStringOut)  //Error
		return ssString;
	else
	*/
	{
//		ssStringOut.resize(ssString.length() );
		ssStringOut = ssString;

		std::locale loc;
		std::transform(ssString.begin(), ssString.end(), ssStringOut.begin(),
			std::bind1st( std::mem_fun( &std::ctype<char>::tolower ),
			&std::use_facet< std::ctype<char> >( loc ) ) );

		return ssString;
	}
}

/*
std::string stringToLower(std::string myString)
{
	static ToLower down(std::locale::classic());
	std::transform(myString.begin(), myString.end(), myString.begin(), down);
	return myString;
}

bool IsInt(const std::string& ss, int& n)
{
	std::istringstream sstm(ss);

	int nValue;
	if (!(sstm >> nValue) )
		return false;
	else
	{
		n = nValue;
		return true;
	}
}
*/

// Trim leading and trailing Whitespace
//??? What is faster erase() of substr()
ad::ERetVal TrimWhitespace(std::string& ssString)
{
    if (ssString.empty() )
        return SNOWORK;

	const std::string ssTrimCharacters = " \t\n\r";

    // trim leading whitespace
    string::size_type ichNotWS(ssString.find_first_not_of(ssTrimCharacters) );
	if (std::string::npos == ichNotWS)
	{
		ssString.erase();
	}
	else
	{
		ssString.erase(0, ichNotWS);

		// trim trailing whitespace
		ichNotWS = ssString.find_last_not_of(" \t\n\r"); 
		ssString.erase(ichNotWS + 1);
	}

    return SOK;

	/* Alternative
	size_t startIndex = str.find_first_not_of(ChrsToTrim);
	if (startIndex == std::string::npos){str.erase(); return;}
	if (TrimDir < 2) str = str.substr(startIndex, str.size()-startIndex);
	if (TrimDir!=1) str = str.substr(0, str.find_last_not_of(ChrsToTrim) + 1);
	*/
}



} // namespace bc
