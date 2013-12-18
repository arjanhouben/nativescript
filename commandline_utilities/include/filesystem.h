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
     * @brief is_directory checks if @filename is a directory
     * @param filename path to file
     * @return true if the filename is a directory
     */
    bool is_directory( const path &filename );

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

    /**
     * @brief is_absolute
     * @param p
     * @return true if @p is absolute
     */
    bool is_absolute(const filesystem::path &p );

    /**
     * @brief absolute
     * @param p
     * @param prefix
     * @return absolute path to @p, if @p is relative, @prefix is used to make it absolute
     */
    path absolute( const path &p, const path &prefix );

    /**
     * @brief cwd
     * @return current working directory
     */
    path cwd();

    /**
     * @brief rename
     * @param from
     * @param to
     * @return true on success
     */
    void rename( const path &from, const path &to );

    /**
     * @brief remove
     * @param file
     * @return true on success
     */
    void remove( const path &file );

    /**
     * @brief copies @source into @dest, if source is a directory, the directory and its contents are recreated at @dest
     * @param source
     * @param dest
     * @return true if source was successfully copied into dest
     */
    void copy( const path &source, const path &dest );
}
