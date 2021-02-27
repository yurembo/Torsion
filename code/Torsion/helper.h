#pragma once

#include <locale>
#include <iostream>
#include <string>
#include <sstream>
#include <comdef.h> 
#include <vector>

using namespace std;

static wstring widen( const string& str )
{
    wostringstream wstm ;
    const ctype<wchar_t>& ctfacet = use_facet<ctype<wchar_t>>(wstm.getloc()) ;
    for( size_t i=0 ; i<str.size() ; ++i ) 
              wstm << ctfacet.widen( str[i] ) ;
    return wstm.str() ;
}

static string narrow( const wstring& str )
{
    ostringstream stm ;

    // Incorrect code from the link
    // const ctype<char>& ctfacet = use_facet<ctype<char>>(stm.getloc());

    // Correct code.
    const ctype<wchar_t>& ctfacet = use_facet<ctype<wchar_t>>(stm.getloc());

    for( size_t i=0 ; i<str.size() ; ++i ) 
                  stm << ctfacet.narrow( str[i], 0 ) ;
    return stm.str() ;
}

static string convertWStringToString(wstring str)
{
	wstring wide(str); 
	string fileName( wide.begin(), wide.end() );
	return fileName;
}

static string utf16ToUTF8( const wstring &s )
{
    const int size = ::WideCharToMultiByte( CP_UTF8, 0, s.c_str(), -1, NULL, 0, 0, NULL );

    vector<char> buf( size );
    ::WideCharToMultiByte( CP_UTF8, 0, s.c_str(), -1, &buf[0], size, 0, NULL );

    return string( &buf[0] );
}