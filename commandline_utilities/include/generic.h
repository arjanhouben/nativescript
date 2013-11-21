#pragma once

#include <vector>
#include <string>

typedef std::vector< std::string > stringlist;

template < class T >
std::string join( const T &t, const std::string &seperator = std::string(), const std::string &prefix = std::string(), const std::string &postix = std::string() )
{
	std::string result;
	for ( auto s : t )
	{
		if ( !result.empty() ) result.append( seperator );
		result += prefix + s + postix;
	}
	return result;
}

template < class T >
std::string join( T start, T end, const std::string &seperator = std::string(), const std::string &prefix = std::string(), const std::string &postix = std::string() )
{
	std::string result;
	while ( start != end )
	{
		if ( !result.empty() ) result.append( seperator );
		result += prefix + *start + postix;
	}
	return result;
}

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
