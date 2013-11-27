#include <config.h>
#include <filesystem.h>
#include <generic.h>

#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <fstream>

using namespace std;
using namespace filesystem;

typedef map< string, string > Values;

path defaultBaseDir()
{
	const char *homeDir = getenv( "HOME" );
	if ( !homeDir )
	{
		throw logic_error( "no 'HOME' directory found in environment!" );
	}
	return path( homeDir ) / ".nativescript/";
}

Values parseConfig( std::istream &stream )
{
	Values values;

	int lineNr = 0;
	string tmp;
	while ( getline( stream, tmp ) )
	{
		++lineNr;

		if ( tmp.empty() ) continue;

		stringstream line( tmp );

		char singleChar;
		string key;
		if ( ( line >> key >> singleChar ) && singleChar == '=' )
		{
			values[ key ] = tmp.substr( line.tellg() );
		}
		else if ( !key.empty() && key[ 0 ] != '#' )
		{
			stringstream tmpstream;
			tmpstream << "error on config line: " << lineNr << ", line did not follow 'key = value' format";
			throw logic_error( tmpstream.str() );
		}
	}

	return values;
}

class SaveValue
{
	public:

		SaveValue( string *destination = 0 ) :
			destination_( destination ) { }

		bool set( const string &value )
		{
			if ( destination_ && !value.empty() )
			{
				*destination_ = value;
			}
			return destination_;
		}

	private:

		string *destination_;
};

Config::Config() :
	baseDirectory_( defaultBaseDir().string() ),
	buildDirectory_( "build/" ),
	sourceDirectory_( "source/" ),
	header_( "header.cpp" ),
	footer_( "footer.cpp" ),
	compilerCommand_( "clang++" ),
	cxxFlags_( "-g -x c++ -O3 -Wall -Wfatal-errors -std=c++11 -stdlib=libc++" ),
	includeDirectories_( "-I" + baseDirectory_ + "/include" ),
	linkDirectories_(),
	libraries_()
{
}

Config Config::readConfigFile( const path &configPath )
{
	Config config;

	ifstream configFile( configPath.c_str() );
	if ( !configFile )
	{
		const path sample( configPath + ".sample" );
		if ( !exists( sample ) )
		{
			make_directory( sample.dirname() );
			ofstream defaultFile( sample.c_str() );
			writeDefault( defaultFile );
		}
	}
	else
	{
		try
		{
			Values values( parseConfig( configFile ) );

			map< string, SaveValue > saveValues;
			saveValues[ "base_directory" ] = &config.baseDirectory_;
			saveValues[ "build_directory" ] = &config.buildDirectory_;
			saveValues[ "source_directory" ] = &config.sourceDirectory_;
			saveValues[ "compiler" ] = &config.compilerCommand_;
			saveValues[ "cxx_flags" ] = &config.cxxFlags_;
			saveValues[ "include_directories" ] = &config.includeDirectories_;
			saveValues[ "link_directories" ] = &config.linkDirectories_;
			saveValues[ "libraries" ] = &config.libraries_;

			for ( auto value : values )
			{
				if ( !saveValues[ value.first ].set( value.second ) )
				{
					throw logic_error( "\"" + value.first + "\" is not a known configuration setting" );
				}
			}
		}
		catch ( const exception &err )
		{
			throw logic_error( "error while trying to parse \"" + configPath.string() + "\"\n\"" + err.what() );
		}
	}

	return config;
}

void Config::writeDefault( ostream &stream )
{
	Config config;
	stream << "# these are the default values, you can change them to your preference" << endl
		   << "# if this file is missing, it will be auto-generated" << endl
		   << "# this file needs to be renamed from 'config.sample' to 'config' for it to be effective" << endl << endl
		   << "base_directory = " << config.baseDirectory() << endl << endl
		   << "build_directory = " << config.buildDirectory_ << endl << endl
		   << "source_directory = " << config.sourceDirectory_ << endl << endl
		   << "compiler = " << config.compilerCommand_ << endl << endl
		   << "cxx_flags = " << config.cxxFlags_ << endl << endl
		   << "include_directories = " << config.includeDirectories_ << endl << endl
		   << "link_directories = " << config.linkDirectories_ << endl << endl
		   << "libraries = " << config.libraries_ << endl;
}

path Config::baseDirectory() const
{
	return baseDirectory_;
}

path Config::buildDirectory() const
{
	return absolute( buildDirectory_, baseDirectory() );
}

path Config::sourceDirectory() const
{
	return absolute( sourceDirectory_, baseDirectory() );
}

string Config::compilerCommand() const
{
	return compilerCommand_;
}

string Config::cxxFlags() const
{
	return cxxFlags_;
}

string Config::includeDirectories() const
{
	return includeDirectories_;
}

string Config::linkDirectories() const
{
	return linkDirectories_;
}

string Config::libraries() const
{
	return libraries_;
}

string Config::header() const
{
	return header_;
}

string Config::footer() const
{
	return footer_;
}
