#pragma once

#include <ios>
#include <string>
#include <vector>
#include <filesystem.h>

class Config
{
	public:

		Config();

		static Config readConfigFile( const filesystem::path &path );

		static void writeDefault(std::ostream &stream );

		filesystem::path baseDirectory() const;
		filesystem::path buildDirectory() const;
		filesystem::path sourceDirectory() const;

		std::string compilerCommand() const;
		std::string cxxFlags() const;
		std::string includeDirectories() const;
		std::string linkDirectories() const;
		std::string libraries() const;

		std::string header() const;
		std::string footer() const;

	private:

		bool validate();

		std::string baseDirectory_,
			buildDirectory_,
			sourceDirectory_,
			header_,
			footer_,
			compilerCommand_,
			cxxFlags_,
			includeDirectories_,
			linkDirectories_,
			libraries_;
};
