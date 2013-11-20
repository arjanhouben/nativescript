#include "config.h"
#include "utilities.h"

#include <string>
#include <iostream>
#include <sstream>
#include <map>

using namespace std;

typedef map< string, string > Values;

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

		char equals;
		string key, value;
		if ( line >> key >> equals >> value && equals == '=' )
		{
			values[ key ] = value;
		}
		else
		{
			cerr << "error on config line: " << lineNr << ", line did not follow 'key = value' format" << endl;
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
				*destination_ = appendSlash( value );
			}
			return destination_;
		}

	private:

		string *destination_;
};

Config::Config( std::istream &stream ) :
	baseDirectory_( defaultBaseDir() ),
	buildDirectory_( "build/" ),
	sourceDirectory_( "source/" ),
	compilerCommand_( "clang++" ),
	header_( "header.cpp" ),
	footer_( "footer.cpp" ),
	cxxFlags_( { "-g", "-x", "c++", "-O3", "-Wall", "-Wfatal-errors", "-std=c++11", "-stdlib=libc++" } ),
	includeDirectories_(),
	linkDirectories_(),
	libraries_()
{
	Values values( parseConfig( stream ) );

	map< string, SaveValue > saveValues;
	saveValues[ "base_directory" ] = &baseDirectory_;
	saveValues[ "build_directory" ] = &buildDirectory_;
	saveValues[ "source_directory" ] = &sourceDirectory_;

	for ( auto value : values )
	{
		if ( !saveValues[ value.first ].set( value.second ) )
		{
			cerr << "\"" << value.first << "\" is not a known configuration setting" << endl;
		}
	}
}

string Config::baseDirectory() const
{
	return baseDirectory_;
}

string Config::buildDirectory() const
{
	return absolute( buildDirectory_, baseDirectory() );
}

string Config::sourceDirectory() const
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
