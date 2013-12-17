#pragma once

#include <vector>
#include <string>
#include <iostream>

class arguments;

typedef std::vector< std::string > stringlist;

template < class T >
stringlist split( const T &t, const std::string &separator = " ", bool keepSeparator = false )
{
    stringlist result;
    std::string::size_type s = 0;
    std::string::size_type e = t.find( separator, s );
    while ( e != std::string::npos )
    {
        result.push_back( t.substr( s, e - s + separator.size() ) );
        s = e + separator.size();
        e = t.find( separator, s );
    }
    if ( s != t.size() ) result.push_back( t.substr( s ) );
    return result;
}

struct OutputHelper
{
    ~OutputHelper()
    {
        std::cout << std::endl;
    }
};

struct Output
{
    Output() { }
} static const out;

template < typename T >
OutputHelper&& operator << ( const Output&, const T &t )
{
    std::cout << t;
    return std::move( OutputHelper() );
}

template < typename T >
const OutputHelper& operator << ( const OutputHelper &helper, const T &t )
{
    std::cout << t;
    return helper;
}

std::string get_env( const std::string &key );

std::string get_homedir();

void trim( std::string &s );

double to_number( const std::string &str );

int system( const std::string &path );

int system( const std::string &path, const arguments &args );
