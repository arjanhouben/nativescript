#include "filesystem.h"

#include <sys/stat.h>
#if _WIN32
#else
#include <unistd.h>
#endif

#include <fstream>
#include <stdexcept>

using namespace std;
using namespace filesystem;

namespace filesystem
{

	bool is_executable( const path &filename )
	{
#if _WIN32
		filename;
		return true;
#else
		return ( access( filename.c_str(), X_OK ) == -1 );
#endif
	}

	double modification_date( const path &filename )
	{
		struct stat info = { 0 };
		stat( filename.c_str(), &info );
#if __APPLE__
		return double( info.st_mtimespec.tv_sec ) + double( info.st_mtimespec.tv_nsec ) / 1e6;
#else
		return info.st_mtime;;
#endif
	}

	bool exists( const path &path )
	{
		return ifstream( path.c_str() ) ? true : false;
	}

	void make_directory( const path &path )
	{
		for ( auto i : path )
		{
#if _WIN32
			if( 0 )
#else
			if ( mkdir( i.c_str(), S_IRWXU ) && errno != EEXIST )
#endif
			{
				throw logic_error( "error creating directory: " + i );
			}
		}
	}

	path cwd()
	{
#if _WIN32
		return path();
#else
		char buf[ PATH_MAX ];
		getcwd( buf, sizeof( buf ) );
		return path( buf );
#endif
	}

	bool is_absolute( const path &p )
	{
		if ( p.empty() ) return false;
		return p.string()[ 0 ] == '/';
	}

	path absolute( const path &p, const path &prefix )
	{
		if ( is_absolute( p ) ) return p;
		return prefix / p;
	}
}
