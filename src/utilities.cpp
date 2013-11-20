#include "utilities.h"

#include <sys/stat.h>
#include <unistd.h>

#include <string>
#include <fstream>

using namespace std;

bool is_executable(const std::string &filename )
{
	return ( access( filename.c_str(), X_OK ) == -1 );
}

double modification_date( const std::string &filename )
{
	struct stat info = { 0 };
	stat( filename.c_str(), &info );
	return double( info.st_mtimespec.tv_sec ) + double( info.st_mtimespec.tv_nsec ) / 1e6;
}

bool exists( const std::string &path )
{
	return std::ifstream( path.c_str() ) ? true : false;
}


void create_directory( const string &path )
{
	string::const_iterator begin = path.begin();
	string::const_iterator start = begin;

	while ( start != path.end() )
	{
		switch( *start )
		{
			case '\\':
				// escape
				if ( !*++start ) return;
				++start;
				break;
			case '/':
				if ( start != begin )
				{
					const string tmp( begin, start );
					if ( mkdir( tmp.c_str(), S_IRWXU ) && errno != EEXIST )
					{
						throw std::logic_error( "error creating directory: " + tmp );
					}
				}
				++start;
				break;
			default:
				++start;
				break;
		}
	}

	if ( start != begin )
	{
		const string tmp( begin, start );
		if ( mkdir( tmp.c_str(), S_IRWXU ) && errno != EEXIST  )
		{
			throw std::logic_error( "error creating directory: " + tmp );
		}
	}
}

string appendSlash( string str )
{
	if ( str.empty() ||  str.back() == '/' ) return str;
	str.push_back( '/' );
	return str;
}

string defaultBaseDir()
{
	const char *homeDir = getenv( "HOME" );
	if ( homeDir )
	{
		return appendSlash( homeDir ) ;
	}
	return string( '/', 1 );
}

bool is_absolute( const string &path )
{
	if ( path.empty() ) return false;
	if ( path[ 0 ] == '/' ) return true;
	return false;
}

string absolute( const string &path, const string &prefix )
{
	return is_absolute( path ) ? path : prefix + path;
}

string cwd()
{
	char buf[ 1024 ];
	getcwd( buf, 1024 );
	return appendSlash( buf );
}


string fullpath( string str )
{
	if ( !is_absolute( str ) ) str = cwd() + str;
	string::size_type p = str.find_last_of( '/' );
	if ( p != string::npos ) ++p;
	return str.substr( 0, p );
}


string normalize( const string &path )
{
	StringList parts( split( path, "/", true ) );
	StringList::iterator i = parts.begin();
	while ( i != parts.end() )
	{
		if ( *i == "./" || ( i != parts.begin() && *i == "/" ) )
		{
			i = parts.erase( i );
		}
		else if ( *i == "../" )
		{
			i = parts.erase( i - 1, i + 1 );
		}
		else
		{
			++i;
		}
	}
	return join( parts );
}
