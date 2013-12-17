#pragma once

#include <string>
#include <vector>

class arguments
{
    public:

        typedef std::string value_type;
        typedef std::vector< value_type > data_type;
        typedef data_type::iterator iterator;
        typedef data_type::const_iterator const_iterator;

        arguments();

        template < class T >
        arguments( T start, T end ) :
            arguments_( start, end ) { }

        const std::string operator[]( size_t idx ) const;

        size_t size() const;

        iterator begin();

        const_iterator begin() const;

        iterator end();

        const_iterator end() const;

        arguments skip( int offset ) const;

    private:

        std::vector< std::string > arguments_;
};

std::ostream& operator << ( std::ostream &stream, const arguments &args );
