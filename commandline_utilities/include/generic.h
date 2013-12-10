#pragma once

#include <vector>
#include <string>
#include <iostream>

typedef std::vector< std::string > stringlist;

#if 0
template < typename InputIterator, typename Seperator = std::string, typename Prefix = std::string, typename Postfix = std::string >
std::string join( InputIterator start, InputIterator end, Seperator seperator = Seperator(), Prefix prefix = Prefix(), Postfix postix = Postfix() )
{
    std::string result;
    while ( start != end )
    {
        if ( !result.empty() ) result += seperator;
        result += prefix + *start + postix;
        ++start;
    }
    return result;
}

template < typename Input, typename Seperator = std::string, typename Prefix = std::string, typename Postfix = std::string >
std::string join( Input input, Seperator seperator = Seperator(), Prefix prefix = Prefix(), Postfix postix = Postfix() )
{
    return join( input.begin(), input.end(), seperator, prefix, postix );
}
#endif

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

template < class T >
OutputHelper&& operator << ( const Output&, const T &t )
{
    std::cout << t;
    return std::move( OutputHelper() );
}

template < class T >
const OutputHelper& operator << ( const OutputHelper &helper, const T &t )
{
    std::cout << t;
    return helper;
}

std::string get_env( const std::string &key );

std::string get_homedir();

void trim( std::string &s );
