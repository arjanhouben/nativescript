#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <map>
#include <stdexcept>

#include <generic.h>
#include <config.h>
#include <filesystem.h>

using namespace std;
using namespace filesystem;

void handle_script( int, char *[] );

enum build_type
{
	release,
	debug
};

class action
{
	public:

		action() :
			func_( handle_script ) { }

		template < typename T >
		action( T &&t ) :
			func_( t ) { }

		void operator()( int argc, char *argv[] )
		{
			func_( argc, argv );
		}

	private:
		function< void( int, char *[] ) > func_;
};

map< string, action > initialize_argument_handling();

static map< string, action > argument_handling( initialize_argument_handling() );

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

Config get_config()
{
	const char *homeDir = getenv( "HOME" );
	if ( !homeDir ) throw logic_error( "environment variable 'HOME' not set!" );

	return Config( Config::readConfigFile( path( homeDir ) / ".nativescript" / "config" ) );
}

const path executable_path( const Config &config, const path &script, build_type build = release )
{
	path exe = config.buildDirectory() / script;
	if ( build == debug )
	{
		exe += "d";
	}
	return exe;
}

const path get_executable( const Config &config, const path &script, build_type build = release )
{
	const path exePath = executable_path( config, script, build );
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
				<< ( ( build == release ) ? config.releaseFlags() : config.debugFlags() ) << ' '
				<< config.includeDirectories() << ' '
				<< config.linkDirectories() << ' '
				<< config.libraries() << ' '
				<< "-I" << config.baseDirectory() / "include" << ' '
				<< "-L" << config.baseDirectory() / "lib" << ' '
				<< sourcePath << ' '
				<< "-lcommandline_utilities "
				<< "-o " << exePath;

		cout << command.str() << endl;
		if ( system( command.str().c_str() ) )
		{
			throw logic_error( "problem compiling script" );
		}
	}

	return exePath;
}

void handle_script( int argc, char *argv[] )
{
	const path script( absolute( path( argv[ 1 ] ), cwd() ) );

	Config config( get_config() );

	const path exePath = get_executable( config, script );

	execv( exePath.c_str(), argv + 1 );
}

void debug_script( int argc, char *argv[] )
{
	const path script( absolute( path( argv[ 2 ] ), cwd() ) );

	Config config( get_config() );

	const string exe( get_executable( config, script, debug ).string() );
//	argv[ 1 ] = const_cast< char* >( exe.c_str() );

	// for some reason, lldb won't start in execv
	exit( system( ( config.debugCommand() + " " + exe ).c_str() ) );
//	execv( config.debugCommand().c_str(), argv + 1 );
}

void print_usage( int argc, char *argv[] )
{
	cout << "you're doing it wrong!" << endl;
}

void show_executable( int argc, char *argv[] )
{
	const path script( absolute( path( argv[ 2 ] ), cwd() ) );
	Config config( get_config() );
	cout << executable_path( config, script ) << endl;
}

void show_debug_executable( int argc, char *argv[] )
{
	const path script( absolute( path( argv[ 2 ] ), cwd() ) );
	Config config( get_config() );
	cout << executable_path( config, script, debug ) << endl;
}

map< string, action > initialize_argument_handling()
{
	map< string, action > argument_handling;
	argument_handling[ "--debug" ] = debug_script;
	argument_handling[ "--usage" ] =
			argument_handling[ "--help" ] = print_usage;
	argument_handling[ "--exe" ] = show_executable;
	argument_handling[ "--debug_exe" ] = show_debug_executable;
	return argument_handling;
}

int main( int argc, char *argv[] )
{
	try
	{
		if ( argc < 2 )
		{
			print_usage( argc, argv );
		}
		else
		{
			argument_handling[ argv[ 1 ] ]( argc, argv );
		}
	}
	catch ( const exception &err )
	{
		cerr << err.what() << endl;
		return 1;
	}

	return  0;
}
