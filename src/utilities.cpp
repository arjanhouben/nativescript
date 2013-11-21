#include "utilities.h"

#include <sys/stat.h>
#include <unistd.h>

#include <string>
#include <fstream>

#if 0

using namespace std;

bool is_executable( const filesystem::path &filename )
{
	return ( access( filename.c_str(), X_OK ) == -1 );
}

double modification_date( const filesystem::path &filename )
{
	struct stat info = { 0 };
	stat( filename.c_str(), &info );
	return double( info.st_mtimespec.tv_sec ) + double( info.st_mtimespec.tv_nsec ) / 1e6;
}

bool exists( const filesystem::path &path )
{
	return std::ifstream( path.c_str() ) ? true : false;
}

void create_directory( const filesystem::path &path )
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

path appendSlash( path str )
{
	if ( str.empty() || str.back() == '/' ) return str;
	str.push_back( '/' );
	return str;
}

bool is_absolute( const filesystem::path &path )
{
	if ( path.empty() ) return false;
	if ( path[ 0 ] == '/' ) return true;
	return false;
}

path absolute( const filesystem::path &path, const filesystem::path &prefix )
{
	return is_absolute( path ) ? path : prefix + path;
}

path cwd()
{
	char buf[ 1024 ];
	getcwd( buf, 1024 );
	return appendSlash( buf );
}

path fullpath( path str )
{
	if ( !is_absolute( str ) ) str = cwd() + str;
	string::size_type p = str.find_last_of( '/' );
	if ( p != string::npos ) ++p;
	return str.substr( 0, p );
}

path normalize( const filesystem::path &path )
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
#endif
