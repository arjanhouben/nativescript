#pragma once

#include <string>
#include <vector>

typedef std::vector< std::string > stringlist;

namespace filesystem
{
	class path
	{
		public:

			path( const std::string &path = std::string() );

			const char* c_str() const;

			path begin() const;

			path end() const;

			bool operator != ( const path& ) const;

			bool operator != ( const char *str ) const;

			bool operator != ( char str ) const;

			bool operator == ( const path& ) const;

			bool operator == ( const char *str ) const;

			bool operator == ( char str ) const;

			path& operator /= ( const path& );

			path operator / ( const path& ) const;

			path operator / ( const char *str ) const;

			path& operator++();

			path operator*() const;

			bool empty() const;

			path back() const;

			path dirname() const;

			std::string string() const;

		private:

			path( const stringlist &paths, size_t pos );

			std::string path_;
			size_t position_;
	};

	typedef std::vector< path > pathlist;

	std::ostream& operator << ( std::ostream &stream, const filesystem::path &path );
}