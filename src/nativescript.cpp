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
					  "\tstd::map< int, var > args;\n"
					  "\tfor ( int i = 0; i < argc; ++i ) argv[ i ] = argv_c_str[ i ];\n" );

int main( int argc, char *argv[] )
{
	if ( argc < 2 ) return 0;

	try
	{
		const char *script = argv[ 1 ];

		const path scriptDir = path( script ).dirname();

		const char *homeDir = getenv( "HOME" );
		if ( !homeDir ) throw logic_error( "environment variable 'HOME' not set!" );

		const path configPath = path( homeDir ) / ".runcpp" / "config";
		ifstream configFile( configPath.c_str() );
		if ( !configFile )
		{
			ofstream defaultFile( configPath.c_str() );
			Config::writeDefault( defaultFile );
			defaultFile.close();
			configFile.open( configPath.c_str() );
		}

		Config config( Config::readConfigFile( configPath ) );

		const path exePath = config.buildDirectory() / script;
		const path sourcePath = config.sourceDirectory() / script;

		if ( !exists( exePath ) || modification_date( sourcePath ) <= modification_date( path( script ) ) )
		{
			create_directory( config.sourceDirectory() / scriptDir );

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

			create_directory( config.buildDirectory() / scriptDir );

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
