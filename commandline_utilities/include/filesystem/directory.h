#pragma once

#if _WIN32
#include <Windows.h>
typedef HANDLE directory_t;
typedef WIN32_FIND_DATA directory_entry_t;
#else
//#include <dirent.h>
typedef struct DIR* directory_t;
typedef struct dirent* directory_entry_t;
#endif
#include <filesystem/path.h>

namespace filesystem
{
	path cwd();

	class directory
	{
		public:

			enum options
			{
				all = 0,
				no_dot = 1 << 0,
				no_dot_dot = 1 << 1,
				no_start_dot = 1 << 2,
				no_dots = no_dot | no_dot_dot | no_start_dot
			};

			class iterator
			{
				public:

					static bool match( options o, const iterator &f );

					explicit iterator( directory_entry_t entry, directory_t dir, options _options );

					const iterator& operator*() const;

					iterator& operator*();

					iterator operator++();

					iterator operator++( int );

					bool operator!=( iterator const& rhs ) const;

					bool operator==( iterator const& rhs ) const;

					bool operator==( const char *rhs ) const;

					bool empty() const;

					char operator[]( size_t index ) const;

					std::string string() const;

				private:

					directory_t dir_;
					directory_entry_t entry_;
					options options_;
			};

			directory( const std::string &path = filesystem::cwd().string(), options types = no_dots );

			directory( options types );

			iterator begin();

			iterator end();

		private:

			path path_;
//			directory_entry_t dir_;
			options types_;
	};

	std::ostream& operator << ( std::ostream &stream, const directory::iterator &i );

}
