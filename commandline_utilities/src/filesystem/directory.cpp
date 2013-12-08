#include <filesystem/directory.h>
#include <filesystem.h>

#ifndef _WIN32
#include <dirent.h>
#endif

#include <iostream>

using namespace std;

namespace filesystem
{
    directory_entry_t next_directory_entry( directory_t &dir )
    {
#if _WIN32
        directory_entry_t entry;
        FindNextFile( dir, &entry );
        return entry;
#else
        return readdir( dir );
#endif
    }

    pair< directory_t, directory_entry_t > first_directory_entry( const path &p )
    {
        pair< directory_t, directory_entry_t > result;
#if _WIN32
        result.first = FindFirstFile( p.c_str(), &result.second );
#else
        result.first = opendir( p.c_str() );
        result.second = next_directory_entry( result.first );
#endif
        return result;
    }

    bool directory::iterator::match( directory::options o, const iterator &f )
    {
        if ( f.empty() ) return false;

        switch ( o )
        {
            case no_dot:
                return f == ".";
            case no_dot_dot:
                return f == "..";
            case no_start_dot:
                return f[ 0 ] == '.';
            case no_dots:
                return match( no_dot, f ) || match( no_dot_dot, f ) || match( no_start_dot, f );
            case all:
            default:
                return false;
        }
    }

    directory::iterator::iterator( directory_entry_t entry, directory_t dir, directory::options _options ) :
        dir_( dir ),
        entry_( entry ),
        options_( _options )
    {
        if ( dir_ )
        {
            while ( match( options_, *this ) )
            {
                entry_ = next_directory_entry( dir_ );
            }
        }
    }

    const directory::iterator &directory::iterator::operator*() const
    {
        return *this;
    }

    directory::iterator& directory::iterator::operator*()
    {
        return *this;
    }

    directory::iterator directory::iterator::operator++()
    {
        if ( !dir_ ) return directory::iterator( directory_entry_t(), directory_t(), options_ );
        entry_ = next_directory_entry( dir_ );
        return *this;
    }

    directory::iterator directory::iterator::operator++(int)
    {
        directory::iterator result( *this );
        operator ++();
        return result;
    }

    bool directory::iterator::operator!=( const directory::iterator &rhs ) const
    {
        return string() != rhs.string();
    }

    bool directory::iterator::operator==( const directory::iterator &rhs ) const
    {
        return string() == rhs.string();
    }

    bool directory::iterator::operator==( const char *rhs ) const
    {
        return string() == rhs;
    }

    bool directory::iterator::empty() const
    {
        return string().empty();
    }

    char directory::iterator::operator[]( size_t index ) const
    {
        return string()[ index ];
    }

    string directory::iterator::string() const
    {
#if _WIN32
        return entry_.cFileName;
#else
        if ( entry_ )
        {
            return entry_->d_name;
        }
        return std::string();
#endif
    }

    directory::directory( const string &path, options types ) :
        path_( path ),
        types_( types )
    {
    }

    directory::directory( options types ) :
        path_( cwd() ),
        types_( types )
    {
    }

    directory::iterator directory::begin()
    {
        pair< directory_t, directory_entry_t > start = first_directory_entry( path_ );

        return directory::iterator( start.second, start.first, types_ );
    }

    directory::iterator directory::end()
    {
        return directory::iterator( directory_entry_t(), directory_t(), types_ );
    }

    ostream &operator << ( ostream &stream, const directory::iterator &i )
    {
        return stream << i.string();
    }

}
