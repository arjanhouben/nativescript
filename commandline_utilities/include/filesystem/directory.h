#pragma once

#include <dirent.h>
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
				no_dots = no_dot | no_dot_dot
			};

			class iterator
			{
				public:

					static bool match( options o, const iterator &f );

					explicit iterator( struct dirent *entry = 0, DIR *dir = 0, options _options = all );

					iterator operator*();

					iterator operator++();

					bool operator!=( iterator const& rhs ) const;

					bool operator==( iterator const& rhs ) const;

					bool operator==( const char *rhs ) const;

					std::string string() const;

				private:

					DIR *dir_;
					dirent *entry_;
					options options_;
			};

			directory( const std::string &path = filesystem::cwd().string(), options types = no_dots );

			directory( options types );

			iterator begin();

			iterator end();

		private:

			path path_;
			DIR *dir_;
			options types_;
	};

	std::ostream& operator << ( std::ostream &stream, const directory::iterator &i );

}
