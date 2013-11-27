#pragma once

#include <filesystem/path.h>
#include <filesystem/directory.h>

namespace filesystem
{
	/**
	 * @brief is_executable checks if @filename is marked as executable
	 * @param filename path to file
	 * @return true if the filename is marked executable
	 */
	bool is_executable( const path &filename );

	/**
	 * @brief modification_date returns the modification date of @filename
	 * @param filename path to file
	 * @return modification date of @old_filename
	 */
	double modification_date( const path &filename );

	/**
	 * @brief create_directory tries to create the directory, throws if unsuccesful
	 * @param path directory to be created
	 */
	void make_directory( const path &path );

	/**
	 * @brief exists check if file exists
	 * @param path path to file
	 * @return true if @path exists
	 */
	bool exists( const path &path );

	bool is_absolute(const filesystem::path &p );

	path absolute( const path &p, const path &prefix );

	path cwd();
}
