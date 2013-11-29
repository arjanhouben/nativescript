#include <filesystem/path.h>
#include "generic.h"

#include <string>
#include <iostream>

using namespace std;

namespace filesystem
{
	const string slash( "/" ), dot( "." );

	string cleanup( string list )
	{
		string::size_type p = list.find( "//" );
		while ( p != string::npos )
		{
			list.erase( p, 1 );
			p = list.find( "//", p );
		}

		p = list.find( "./" );
		while ( p != string::npos )
		{
			if ( !p || list[ p - 1 ] != '.' )
			{
				list.erase( p, 2 );
			}
			else
			{
				++p;
			}
			p = list.find( "./", p );
		}

		p = list.find( "/.." );
		while ( p != string::npos )
		{
			if ( p )
			{
				string::size_type before = list.rfind( '/', p - 1 );
				if ( before == string::npos ) break;
				list.erase( before, p - before + 3 );
			}
			else
			{
				list.erase( 1, 3 );
			}
			p = list.find( "/.." );
		}

		return list;
	}

	path::path( const std::string &path ) :
		path_( cleanup( path ) ),
		position_( path_.size() )
	{
	}

	const char *path::c_str() const
	{
		return string().c_str();
	}

	path path::begin() const
	{
		path p( *this );
		p.position_ = p.path_.find( '/', 1 );
		if ( p.position_ == string::npos ) p.position_ = path_.size();
		return p;
	}

	path path::end() const
	{
		path p( *this );
		p.position_ = string::npos;
		return p;
	}

	bool path::operator != ( const path &path ) const
	{
		return !( *this == path );
	}

	bool path::operator !=( const char *str ) const
	{
		return !( *this == str );
	}

	bool path::operator != ( char c ) const
	{
		return !( *this == c );
	}

	bool path::operator == ( const path &path ) const
	{
		return position_ == path.position_ && path_ == path.path_;
	}

	bool path::operator ==( const char *str ) const
	{
		return string() == str;
	}

	bool path::operator == ( char c ) const
	{
		return path_.size() == 1 && path_[ 0 ] == c;
	}

	path& path::operator /= ( const path &p )
	{
		path_ = cleanup( path_ + '/' +  p.string() );
		position_ = path_.size();
		return *this;
	}

	path path::operator / ( const path &p ) const
	{
		path result( *this );
		return result /= p;
	}

	path path::operator / ( const char *str ) const
	{
		return *this / path( str );
	}

	path& path::operator += ( const std::string &str )
	{
		path_.append( str );
		position_ = path_.size();
		return *this;
	}

	path& path::operator++()
	{
		if ( position_ == path_.size() )
		{
			// jump to end
			position_ = string::npos;
		}
		else
		{
			position_ = path_.find( '/', position_ + 1 );
			if ( position_ == string::npos ) position_ = path_.size();
		}
		return *this;
	}

	std::string path::operator*() const
	{
		return string();
	}

	bool path::empty() const
	{
		return path_.empty();
	}

	path path::dirname() const
	{
		string::size_type p = path_.rfind( '/' );
		if ( p == string::npos ) return path( "." );
		return path_.substr( 0, p );
	}

	std::string path::string() const
	{
		return path_.substr( 0, position_ );
	}

	path operator + ( const path &p, const string &str )
	{
		path result( p );
		return result += str;
	}

	std::ostream& operator << ( std::ostream &stream, const filesystem::path &p )
	{
		return stream << p.string();
	}

}
