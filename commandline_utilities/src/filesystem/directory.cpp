#include <filesystem/directory.h>
#include <filesystem.h>

#ifndef _WIN32
#include <dirent.h>
#endif

#include <iostream>

using namespace std;

namespace filesystem
{
    static const directory_entry_t empty_entry = { 0 };

    directory_entry_t next_directory_entry( directory_t &dir )
    {
        directory_entry_t entry = empty_entry;
        if ( dir )
        {
#if _WIN32
            if ( !FindNextFile( dir, &entry ) ) entry = empty_entry;
#else
            entry = readdir( dir );
#endif
        }
        return entry;
    }

    pair< directory_t, directory_entry_t > first_directory_entry( const path &p )
    {
        pair< directory_t, directory_entry_t > result;
#if _WIN32
        result.first = FindFirstFile( native( p ).c_str(), &result.second );
#else
        result.first = opendir( p.c_str() );
        result.second = next_directory_entry( result.first );
#endif
        return result;
    }

    bool directory::iterator::match( directory::options o, const iterator &f )
    {
        const std::string &str( f.string() );

        if ( str.empty() ) return false;

        switch ( o )
        {
            case no_dot:
                return str == ".";
            case no_dot_dot:
                return str == "..";
            case no_start_dot:
                return str[ 0 ] == '.';
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
        handleOptions();
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
        if ( !dir_ ) return directory::iterator( 0, 0, options_ );
        entry_ = next_directory_entry( dir_ );
        handleOptions();
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

    void directory::iterator::handleOptions()
    {
        if ( dir_ )
        {
            while ( match( options_, *this ) )
            {
                entry_ = next_directory_entry( dir_ );
            }
        }
    }

    ostream &operator << ( ostream &stream, const directory::iterator &i )
    {
        return stream << i.string();
    }

}
