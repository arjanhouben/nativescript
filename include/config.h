#pragma once

#include <ios>
#include <string>
#include <vector>
#include "path.h"

typedef std::vector< std::string > StringList;

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
		StringList cxxFlags() const;
		StringList includeDirectories() const;
		StringList linkDirectories() const;
		StringList libraries() const;

		std::string header() const;
		std::string footer() const;

	private:

		bool validate();

		std::string baseDirectory_,
			buildDirectory_,
			sourceDirectory_,
			header_,
			footer_,
			compilerCommand_;

		StringList cxxFlags_,
			includeDirectories_,
			linkDirectories_,
			libraries_;
};
