#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <unistd.h>

#include <generic.h>
#include <config.h>
#include <filesystem.h>

using namespace std;
using namespace filesystem;


static const string defaultHeader(
		"#include <array>\n"
		"#include <bitset>\n"
		"#include <deque>\n"
		"#include <forward_list>\n"
		"#include <list>\n"
		"#include <map>\n"
		"#include <queue>\n"
		"#include <set>\n"
		"#include <stack>\n"
		"#include <unordered_map>\n"
		"#include <unordered_set>\n"
		"#include <vector>\n"
		"#include <algorithm>\n"
		"#include <chrono>\n"
		"#include <functional>\n"
		"#include <iterator>\n"
		"#include <memory>\n"
		"#include <stdexcept>\n"
		"#include <tuple>\n"
		"#include <utility>\n"
		"#include <locale>\n"
		"#include <codecvt>\n"
		"#include <string>\n"
		"#include <regex>\n"
		"#include <fstream>\n"
		"#include <iomanip>\n"
		"#include <ios>\n"
		"#include <iosfwd>\n"
		"#include <iostream>\n"
		"#include <istream>\n"
		"#include <ostream>\n"
		"#include <sstream>\n"
		"#include <streambuf>\n"
		"#include <exception>\n"
		"#include <limits>\n"
		"#include <new>\n"
		"#include <typeinfo>\n"
		"#include <thread>\n"
		"#include <mutex>\n"
		"#include <condition_variable>\n"
		"#include <future>\n"
		"#include <complex>\n"
		"#include <random>\n"
		"#include <valarray>\n"
		"#include <numeric>\n"
		"#include <commandline_utilities>\n"
		"using namespace std;\n"
		"using namespace filesystem;\n"
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
					<< config.cxxFlags() << ' '
					<< config.includeDirectories() << ' '
					<< config.linkDirectories() << ' '
					<< config.libraries() << ' '
					<< "-I" << config.baseDirectory() / "include" << ' '
					<< "-L" << config.baseDirectory() / "lib" << ' '
					<< "-lcommandline_utilities "
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
