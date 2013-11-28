#include <filesystem/directory.h>
#include <filesystem.h>

#include <iostream>

using namespace std;

namespace filesystem
{

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

	directory::iterator::iterator( dirent *entry, DIR *dir, directory::options _options ) :
		dir_( dir ),
		entry_( entry ),
		options_( _options )
	{
		if ( dir_ )
		{
			while ( match( options_, *this ) )
			{
				entry_ = readdir( dir_ );
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
		if ( !dir_ ) return directory::iterator( 0, 0, options_ );
		entry_ = readdir( dir_ );
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
		if ( entry_ )
		{
			return entry_->d_name;
		}
		return std::string();
	}

	directory::directory( const string &path, options types ) :
		path_( path ),
		dir_( opendir( path_.c_str() ) ),
		types_( types )
	{
	}

	directory::directory( options types ) :
		path_( cwd() ),
		dir_( opendir( path_.c_str() ) ),
		types_( types )
	{
	}

	directory::iterator directory::begin()
	{
		if ( !dir_ ) return end();
		rewinddir( dir_ );
		return directory::iterator( readdir( dir_ ), dir_, types_ );
	}

	directory::iterator directory::end()
	{
		return directory::iterator( 0, dir_, types_ );
	}

	ostream &operator << ( ostream &stream, const directory::iterator &i )
	{
		return stream << i.string();
	}

}
