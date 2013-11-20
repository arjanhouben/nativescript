#pragma once

#include <ios>
#include <string>
#include <vector>

typedef std::vector< std::string > StringList;

class Config
{
	public:

		Config( std::istream &stream );

		std::string baseDirectory() const;
		std::string buildDirectory() const;
		std::string sourceDirectory() const;

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
			compilerCommand_,
			header_,
			footer_;

		StringList cxxFlags_,
			includeDirectories_,
			linkDirectories_,
			libraries_;
};
