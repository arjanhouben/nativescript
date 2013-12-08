#include <arguments.h>

arguments::arguments( int argc, char *argv[] ) :
	arguments_( argv, argv + argc ) { }

const std::string arguments::operator[]( size_t idx ) const
{
	if ( idx < arguments_.size() ) return arguments_[ idx ];
	return std::string();
}

size_t arguments::size() const
{
	return arguments_.size();
}
		
arguments::iterator arguments::begin()
{
	return arguments_.begin();
}

arguments::const_iterator arguments::begin() const
{
	return arguments_.begin();
}

arguments::iterator arguments::end()
{
	return arguments_.end();
}

arguments::const_iterator arguments::end() const
{
	return arguments_.end();
}