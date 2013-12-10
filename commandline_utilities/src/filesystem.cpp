#include "filesystem.h"

#include <sys/stat.h>
#if _WIN32
#else
#include <unistd.h>
#endif

#include <fstream>
#include <stdexcept>
#include <cassert>

#include <iostream>

using namespace std;
using namespace filesystem;

namespace filesystem
{
	
    bool is_executable( const path &filename )
    {
#if _WIN32
		return ( GetFileAttributes( native( filename ).c_str() ) & FILE_EXECUTE ) != 0;
#else
        return ( access( filename.c_str(), X_OK ) == -1 );
#endif
    }

    bool is_directory( const path &filename )
    {
#if _WIN32
		return ( GetFileAttributes( native( filename ).c_str() ) & FILE_ATTRIBUTE_DIRECTORY ) != 0;
#else
		struct stat info;
		if ( stat( filename.c_str(), &info ) ) return 0;
		return info.st_mode & S_IFDIR;
#endif
    }

    double modification_date( const path &filename )
    {
#if _WIN32
        HANDLE file = CreateFile( native( filename ).c_str(), GENERIC_READ,
            FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
            0,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            0 );
        if ( !file ) return 0;
        FILETIME modified = { 0 };
        GetFileTime( file, 0, 0, &modified );
        CloseHandle( file );
		return static_cast< double >( *reinterpret_cast< PULONG64 >( &modified ) );
#else
        struct stat info = { 0 };
        stat( filename.c_str(), &info );
#if __APPLE__
        return double( info.st_mtimespec.tv_sec ) + double( info.st_mtimespec.tv_nsec ) / 1e6;
#else
        return info.st_mtime;;
#endif
#endif
    }

    bool exists( const path &path )
    {
#if _WIN32
        return GetFileAttributes( native( path ).c_str() ) != INVALID_FILE_ATTRIBUTES;
#else
        struct stat attributes;
        return !stat( path.c_str(), &attributes );
#endif
    }

    void make_directory( const path &path )
    {
        if ( exists( path ) ) return;

        for ( auto i : path )
        {
#if _WIN32
            if ( CreateDirectory( native( i ).c_str(), 0 ) && GetLastError() != ERROR_ALREADY_EXISTS )
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
        string result( GetCurrentDirectory( 0, 0 ), 0 );
        GetCurrentDirectory( result.size(), &result[ 0 ] );
        while ( result.back() == 0 ) result.pop_back();
        return path( result );
#else
        char buf[ PATH_MAX ];
        getcwd( buf, sizeof( buf ) );
        return path( buf );
#endif
    }

    bool is_absolute( const path &p )
    {
        if ( p.empty() ) return false;
#if _WIN32
        const string str( p.string() );
        string::size_type pos = str.find_first_not_of( "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" );
        if ( pos == 0 || pos == string::npos ) return false;
        if ( str[ pos ] != ':' ) return false;
        return true;
#else
        return p.string()[ 0 ] == '/';
#endif
    }

    path absolute( const path &p, const path &prefix )
    {
        if ( is_absolute( p ) ) return p;
        return prefix / p;
    }
}
