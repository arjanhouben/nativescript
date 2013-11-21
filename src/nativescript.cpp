#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <unistd.h>

//#include "utilities.h"
#include "filesystem.h"
#include "generic.h"
#include "config.h"
#include "path.h"

using namespace std;
using namespace filesystem;

static const string defaultHeader( "#include <iostream>\n"
					  "#include <fstream>\n"
					  "#include <vector>\n"
					  "#include <map>\n"
					  "#include <cctype>\n"
					  "#include <algorithm>\n"
					  "using namespace std;\n"
					  "int main( int argc, const char *argv_c_str[] ) {\n"
					  "\tstd::map< int, string > argv;\n"
					  "\tfor ( int i = 0; i < argc; ++i ) argv[ i ] = argv_c_str[ i ];\n" );

int main( int argc, char *argv[] )
{
	try
	{
		if ( argc < 2 ) return 0;

		const path script( absolute( path( argv[ 1 ] ), cwd() ) );

		const char *homeDir = getenv( "HOME" );
		if ( !homeDir ) throw logic_error( "environment variable 'HOME' not set!" );

		Config config( Config::readConfigFile( path( homeDir ) / ".runcpp" / "config" ) );

		const path exePath = config.buildDirectory() / script;
		const path sourcePath = config.sourceDirectory() / script;

		if ( !exists( exePath ) || modification_date( sourcePath ) <= modification_date( path( script ) ) )
		{
			const path scriptDir = path( script ).dirname();
			make_directory( config.sourceDirectory() / scriptDir );

			ofstream source( sourcePath.c_str() );

			ifstream scriptsource( script.c_str() );
			string line;
			getline( scriptsource, line );
			if ( line.substr( 0, 2 ) != "#!" )
			{
				throw logic_error( "hashbang not found!" );
			}

			ifstream header( config.header().c_str() );
			if ( header )
			{
				source << header.rdbuf();
			}
			else
			{
				source << defaultHeader;
			}

			while ( getline( scriptsource, line ) )
			{
				source << line << endl;
			}

			source << "}";

			source.close();

			make_directory( config.buildDirectory() / scriptDir );

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

		execv( exePath.c_str(), argv + 1 );
	}
	catch ( const exception &err )
	{
		cerr << err.what() << endl;
	}

	return 1;
}
