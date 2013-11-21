#include "filesystem.h"
#include "path.h"

#include <sys/stat.h>
#include <unistd.h>

#include <fstream>

using namespace std;

namespace filesystem
{

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

	void make_directory( const filesystem::path &path )
	{
		for ( auto i : path )
		{
			if ( mkdir( i.c_str(), S_IRWXU ) && errno != EEXIST )
			{
				throw std::logic_error( "error creating directory: " + i.string() );
			}
		}
	}

	filesystem::path cwd()
	{
		char buf[ PATH_MAX ];
		getcwd( buf, sizeof( buf ) );
		return filesystem::path( buf );
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
