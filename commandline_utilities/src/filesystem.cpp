#include "filesystem.h"

#include <sys/stat.h>
#if _WIN32
#else
#include <unistd.h>
#endif

#include <fstream>
#include <stdexcept>
#include <cassert>
#include <array>
#include <iostream>

using namespace std;
using namespace filesystem;

namespace filesystem
{

    bool is_executable( const path &filename )
    {
#if _WIN32
        return ( GetFileAttributes( filename.c_str() ) & FILE_EXECUTE ) != 0;
#else
        return ( access( filename.c_str(), X_OK ) == -1 );
#endif
    }

    bool is_directory( const path &filename )
    {
#if _WIN32
        return ( GetFileAttributes( filename.c_str() ) & FILE_ATTRIBUTE_DIRECTORY ) != 0;
#else
        struct stat info;
        if ( stat( filename.c_str(), &info ) ) return 0;
        return info.st_mode & S_IFDIR;
#endif
    }

    double modification_date( const path &filename )
    {
#if _WIN32
        HANDLE file = CreateFile( filename.c_str(), GENERIC_READ,
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
        return GetFileAttributes( path.c_str() ) != INVALID_FILE_ATTRIBUTES;
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
            if ( CreateDirectory( i.c_str(), 0 ) && GetLastError() != ERROR_ALREADY_EXISTS )
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

    void rename( const path &from, const path &to )
    {
        if ( std::rename( from.c_str(), to.c_str() ) != 0 )
        {
            throw logic_error( "could not rename \"" + from.string() + "\" to \"" + to.string() + "\"" );
        }
    }

    void remove( const path &file )
    {
        if ( std::remove( file.c_str() ) != 0 )
        {
            throw logic_error( "could not remove \"" + file.string() + "\"" );
        }
    }

    template < typename Buffer >
    void copy( const path &source, const path &dest, Buffer &buffer )
    {
        if ( is_directory( source ) )
        {
            make_directory( dest );

            for ( auto f : directory( source ) )
            {
                copy( source / f, dest / f.basename(), buffer );
            }
        }
        else
        {
            ifstream src( source.string(), ios::binary );
            if ( !src )
            {
                throw logic_error( "could not read \"" + source.string() + "\" when trying to copy it to \"" + dest.string() + "\"" );
            }
            ofstream dst( dest.string(), ios::binary );
            if ( !dst )
            {
                throw logic_error( "could not write \"" + dest.string() + "\" when trying to copy it from \"" + source.string() + "\"" );
            }

            const auto buffer_size = buffer.size() * sizeof( typename Buffer::value_type );

            while ( src )
            {
                src.read( reinterpret_cast< char* >( buffer.data() ), buffer_size );
                dst.write( reinterpret_cast< const char* >( buffer.data() ), src.gcount() );
            }
        }
    }

    void copy( const path &source, const path &dest )
    {
        array< size_t, 1024 > buffer;
        copy( source, dest, buffer );
    }

}
