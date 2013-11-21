#include "config.h"
#include "path.h"
#include "filesystem.h"

#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <fstream>

using namespace std;

typedef map< string, string > Values;

filesystem::path defaultBaseDir()
{
	const char *homeDir = getenv( "HOME" );
	if ( !homeDir )
	{
		throw logic_error( "no 'HOME' directory found in environment!" );
	}
	return filesystem::path( homeDir ) / ".runcpp/";
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
		string key, value;
		if ( ( line >> key >> singleChar >> value ) && singleChar == '=' )
		{
			values[ key ] = value;
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
	cxxFlags_( { "-g", "-x", "c++", "-O3", "-Wall", "-Wfatal-errors", "-std=c++11", "-stdlib=libc++" } ),
	includeDirectories_(),
	linkDirectories_(),
	libraries_()
{
}

Config Config::readConfigFile( const filesystem::path &configPath )
{
	Config config;

	ifstream configFile( configPath.c_str() );
	if ( !configFile )
	{
		if ( !exists( configPath / ".sample" ) )
		{
			ofstream defaultFile( ( configPath / ".sample" ).c_str() );
			writeDefault( defaultFile );
			return config;
		}
	}

	try
	{
		Values values( parseConfig( configFile ) );

		map< string, SaveValue > saveValues;
		saveValues[ "base_directory" ] = &config.baseDirectory_;
		saveValues[ "build_directory" ] = &config.buildDirectory_;
		saveValues[ "source_directory" ] = &config.sourceDirectory_;

		for ( auto value : values )
		{
			if ( !saveValues[ value.first ].set( value.second ) )
			{
				throw logic_error( "\"" + value.first + "\" is not a known configuration setting" );
			}
		}

		config.includeDirectories_.push_back( config.baseDirectory().string() );
	}
	catch ( const exception &err )
	{
		throw logic_error( "error while trying to parse \"" + configPath.string() + "\"\n\"" + err.what() );
	}

	return config;
}

void Config::writeDefault( ostream &stream )
{
	Config config;
	stream << "# these are the default values, you can change them to your preference" << endl
		   << "# if this file is missing, it will be auto-generated" << endl
		   << "base_directory = " << config.baseDirectory() << endl
		   << "build_directory = " << config.buildDirectory_ << endl
		   << "source_directory = " << config.sourceDirectory_ << endl;
}

filesystem::path Config::baseDirectory() const
{
	return baseDirectory_;
}

filesystem::path Config::buildDirectory() const
{
	return absolute( buildDirectory_, baseDirectory() );
}

filesystem::path Config::sourceDirectory() const
{
	return absolute( sourceDirectory_, baseDirectory() );
}

string Config::compilerCommand() const
{
	return compilerCommand_;
}

StringList Config::cxxFlags() const
{
	return cxxFlags_;
}

StringList Config::includeDirectories() const
{
	return includeDirectories_;
}

StringList Config::linkDirectories() const
{
	return linkDirectories_;
}

StringList Config::libraries() const
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
