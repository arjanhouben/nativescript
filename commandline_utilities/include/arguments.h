#pragma once

#include <string>
#include <vector>

class arguments
{
	public:

		typedef std::vector< std::string > value_type;
		typedef value_type::iterator iterator;
		typedef value_type::const_iterator const_iterator;

		arguments( int argc, char *argv[] );

		const std::string operator[]( size_t idx ) const;

		size_t size() const;
		
		iterator begin();

		const_iterator begin() const;
		
		iterator end();

		const_iterator end() const;

	private:

		std::vector< std::string > arguments_;
};