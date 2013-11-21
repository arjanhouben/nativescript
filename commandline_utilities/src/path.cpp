#include "path.h"
#include "generic.h"

#include <string>

using namespace std;

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

filesystem::path::path( const std::string &path ) :
	path_( cleanup( path ) ),
	position_( path_.size() )
{
}

const char *filesystem::path::c_str() const
{
	return string().c_str();
}

filesystem::path filesystem::path::begin() const
{
	path p( *this );
	p.position_ = p.path_.find( '/', 1 );
	if ( p.position_ == string::npos ) p.position_ = path_.size();
	return p;
}

filesystem::path filesystem::path::end() const
{
	path p( *this );
	p.position_ = string::npos;
	return p;
}

bool filesystem::path::operator != ( const filesystem::path &path ) const
{
	return !( *this == path );
}

bool filesystem::path::operator !=( const char *str ) const
{
	return !( *this == str );
}

bool filesystem::path::operator != ( char c ) const
{
	return !( *this == c );
}

bool filesystem::path::operator == ( const filesystem::path &path ) const
{
	return position_ == path.position_ && path_ == path.path_;
}

bool filesystem::path::operator ==( const char *str ) const
{
	return path_ == str;
}

bool filesystem::path::operator == ( char c ) const
{
	return path_.size() == 1 && path_[ 0 ] == c;
}

filesystem::path& filesystem::path::operator /= ( const filesystem::path &p )
{
	path_ = cleanup( path_ + '/' +  p.string() );
	position_ = path_.size();
	return *this;
}

filesystem::path filesystem::path::operator / ( const filesystem::path &p ) const
{
	path result( *this );
	return result /= p;
}

filesystem::path filesystem::path::operator / ( const char *str ) const
{
	return *this / path( str );
}

filesystem::path& filesystem::path::operator++()
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

filesystem::path filesystem::path::operator*() const
{
	return string();
}

bool filesystem::path::empty() const
{
	return path_.empty();
}

filesystem::path filesystem::path::dirname() const
{
	string::size_type p = path_.rfind( '/' );
	if ( p == string::npos ) return path( "." );
	return path_.substr( 0, p );
}

std::string filesystem::path::string() const
{
	return path_.substr( 0, position_ );
}


std::ostream &filesystem::operator << ( std::ostream &stream, const filesystem::path &path )
{
	return stream << path.string();
}
