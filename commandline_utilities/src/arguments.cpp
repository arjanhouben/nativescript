#include <arguments.h>
#include <cstdlib>
#include <ostream>

arguments::arguments() :
    arguments_() { }

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

arguments arguments::skip( int offset ) const
{
    if ( abs( offset ) >= size() ) return arguments();

    if ( offset > 0 )
    {
        return arguments( begin() + offset, end() );
    }

    return arguments( begin(), end() + offset );
}


std::ostream &operator << ( std::ostream &stream, const arguments &args )
{
    std::ostream_iterator< std::string > dest( stream, " " );
    std::copy( args.begin(), args.end(), dest );
    return stream;
}
