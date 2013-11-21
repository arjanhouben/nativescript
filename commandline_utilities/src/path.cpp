#include "path.h"
#include "generic.h"

#include <iostream>

using namespace std;

const string slash( "/" ), dot( "." );

stringlist cleanup( stringlist list )
{
	if ( !list.empty() )
	{
		stringlist::iterator i = list.begin();
		stringlist::iterator last = i++;
		while ( i != list.end() )
		{
			if ( *i == *last || *last == dot )
			{
				i = list.erase( last );
				if ( i == list.end() ) break;
			}
			last = i++;
		}
	}
	return list;
}

stringlist stringToParts( const string &str )
{
	stringlist result;
	string::size_type start = 0;
	string::size_type end = str.find( '/', start );
	while ( end != string::npos )
	{
		if ( start != end )
		{
			result.push_back( str.substr( start, end - start ) );
		}
		result.push_back( string( 1, '/' ) );
		start = end + 1;
		end = str.find( '/', start );
	}
	result.push_back( str.substr( start ) );
	return result;
}

filesystem::path::path( const std::string &path ) :
	position_( 0 ),
	paths_( cleanup( stringToParts( path ) ) ),
	path_( join( paths_ ) )
{
}

filesystem::path::path( const stringlist &paths, size_t pos ) :
	position_( pos ),
	paths_( paths ),
	path_( join( paths_ ) )
{
}

const char *filesystem::path::c_str() const
{
	return path_.c_str();
}

filesystem::path filesystem::path::begin() const
{
	return path( paths_, 0 );
}

filesystem::path filesystem::path::end() const
{
	return path( paths_, paths_.size() );
}

bool filesystem::path::operator != ( const filesystem::path &path ) const
{
	return path_ != path.path_ || position_ != path.position_;
}

bool filesystem::path::operator !=( const char *str ) const
{
	return *this != filesystem::path( str );
}

bool filesystem::path::operator != ( char c ) const
{
	return *this != filesystem::path( std::string( 1, c ) );
}

bool filesystem::path::operator == ( const filesystem::path &path ) const
{
	return path_ == path.path_ && position_ == path.position_;
}

bool filesystem::path::operator ==( const char *str ) const
{
	return *this == path( str );
}

bool filesystem::path::operator ==( char c ) const
{
	return *this == path( std::string( 1, c ) );
}

const std::string& filesystem::path::operator[]( size_t i ) const
{
	return paths_[ i ];
}

filesystem::path& filesystem::path::operator /= ( const filesystem::path &p )
{
	if ( paths_.empty() || paths_.back() != slash )
	{
		paths_.push_back( slash );
	}
	paths_.insert( paths_.end(), p.paths_.begin(), p.paths_.end() );
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

filesystem::path filesystem::path::operator++() const
{
	return path( paths_, std::max( position_ + 1, paths_.size() ) );
}

filesystem::path filesystem::path::operator*() const
{
	return string();
}

bool filesystem::path::empty() const
{
	return paths_.empty();
}

filesystem::path filesystem::path::dirname() const
{
	return *this;
//	if ( paths_.empty() ) return path();
//	return path( paths_.back().back() == '/' ? join( paths_.begin(), paths_.end() ) : join( paths_.begin(), paths_.begin() + paths_.size() - 1 ) );
}

const std::string &filesystem::path::string() const
{
	path_ = join( paths_ );
	return path_;
}


std::ostream &filesystem::operator << ( std::ostream &stream, const filesystem::path &path )
{
	return stream << path.string();
}
