#include <filesystem/directory.h>
#include <filesystem.h>

#include <iostream>

using namespace std;

namespace filesystem
{

	bool directory::iterator::match( directory::options o, const iterator &f )
	{
		switch ( o )
		{
			case no_dot:
				return f == ".";
			case no_dot_dot:
				return f == "..";
			case no_dots:
				return match( no_dot, f ) || match( no_dot_dot, f );
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
		while ( match( options_, *this ) )
		{
			entry_ = readdir( dir_ );
		}
	}

	directory::iterator directory::iterator::operator*()
	{
		return *this;
	}

	directory::iterator directory::iterator::operator++()
	{
		entry_ = readdir( dir_ );
		return *this;
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
		return std::string() == rhs;
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
		rewinddir( dir_ );
		return directory::iterator( readdir( dir_ ), dir_, types_ );
	}

	directory::iterator directory::end()
	{
		return directory::iterator( 0, dir_, types_ );
	}

	ostream &operator << ( ostream &stream, const directory::iterator &i )
	{
		return stream << "i.string()";
	}

}
