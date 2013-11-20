#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <unistd.h>

#include "utilities.h"
#include "config.h"

using namespace std;

int main( int argc, char *argv[] )
{
	if ( argc < 2 ) return 0;

	try
	{
		const char *script = argv[ 1 ];

		const string scriptDir = fullpath( script );

		const char *homeDir = getenv( "HOME" );
		if ( !homeDir ) throw logic_error( "environment variable 'HOME' not set!" );

		const string configPath = string( homeDir ) + "/.runcpp/.config";
		ifstream configFile( configPath.c_str() );
		Config config( configFile );

		const string exePath = normalize( config.buildDirectory() + script );
		const string sourcePath = normalize( config.sourceDirectory() + script );

		if ( !exists( exePath ) || modification_date( sourcePath ) <= modification_date( script ) )
		{
			create_directory( normalize( config.sourceDirectory() + scriptDir ) );

			ofstream source( sourcePath.c_str() );
#if 0
			source << "#include <iostream>" << endl
				   << "#include <fstream>" << endl
				   << "#include <vector>" << endl
				   << "#include <map>" << endl
				   << "#include <cctype>" << endl
				   << "#include <algorithm>" << endl
				   << "using namespace std;" << endl
				   << "#include \"/Users/arjan/C++/runcpp/include/utils.h\"" << endl
				   << "int main( int argc, const char *argv[] ) {" << endl
				   << "std::map< int, var > args;" << endl
				   << "for ( int i = 0; i < argc; ++i ) args[ i ] = argv[ i ];" << endl;
#endif

			ifstream scriptsource( script );
			string line;
			getline( scriptsource, line );
			if ( line.substr( 0, 2 ) != "#!" )
			{
				throw logic_error( "hashbang not found!" );
			}

			ifstream header( config.header().c_str() );
			source << header.rdbuf();

			while ( getline( scriptsource, line ) )
			{
				source << line << endl;
			}

			source << "}";

			source.close();

			create_directory( normalize( config.buildDirectory() + scriptDir ) );

			stringstream command;
			command << config.compilerCommand() << ' '
					<< join( config.cxxFlags(), " " ) << ' '
					<< join( config.includeDirectories(), " ", "-I" ) << ' '
					<< join( config.linkDirectories(), " ", "-L" ) << ' '
					<< join( config.libraries() ) << ' '
					<< sourcePath
					<< " -o " << exePath;

			cout << command.str() << endl;
			system( command.str().c_str() );
		}

//		string debugger;

//		if ( getenv( "DEBUG" ) )
//		{
//			debugger = "lldb ";
//		}

		execv( config.compilerCommand().c_str(), argv + 1 );
	}
	catch ( const exception &err )
	{
		cerr << err.what() << endl;
	}

	return 1;
}
