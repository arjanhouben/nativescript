#pragma once

#if 0
#include <string>
#include <vector>

typedef std::vector< std::string > StringList;

/**
 * @brief is_executable checks if @filename is marked as executable
 * @param filename path to file
 * @return true if the filename is marked executable
 */
bool is_executable( const std::string &filename );

/**
 * @brief modification_date returns the modification date of @filename
 * @param filename path to file
 * @return modification date of @old_filename
 */
double modification_date( const std::string &filename );

/**
 * @brief create_directory tries to create the directory, throws if unsuccesful
 * @param path directory to be created
 */
void create_directory( const std::string &path );

/**
 * @brief exists check if file exists
 * @param path path to file
 * @return true if @path exists
 */
bool exists( const std::string &path );

std::string appendSlash( std::string str );

std::string defaultBaseDir();

bool is_absolute( const std::string &path );

std::string absolute( const std::string &path, const std::string &prefix );

std::string cwd();

std::string fullpath( std::string str );

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
StringList split( const T &t, const std::string &separator = " ", bool keepSeparator = false )
{
	StringList result;
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

std::string normalize( const std::string &path );
#endif
