#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#include <dlfcn.h>
#endif
#include <map>
#include <stdexcept>
#include <functional>
#include <algorithm>

#include <generic.h>
#include <config.h>
#include <filesystem.h>
#include <arguments.h>
#include <defines.h>

using namespace std;
using namespace filesystem;

void handle_script( const arguments & );

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
        action( const T &t ) :
            func_( t ) { }

        void operator()( const arguments &args )
        {
            func_( args );
        }

    private:
        function< void( const arguments & ) > func_;
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
#if _WIN32
		"BOOL WINAPI DllMain( HINSTANCE, DWORD, LPVOID ) { return true; }"
#endif
        "extern \"C\" __declspec(dllexport) int nativescript_main( const arguments &args ) {\n" );

extern "C"
{
    typedef __declspec(dllimport) int (*nativescript_main_t)( arguments&& );
}

Config get_config()
{
    return Config( Config::readConfigFile( path( get_homedir() ) / ".nativescript" / "config" ) );
}

path executable_path( const Config &config, const path &script, build_type build = release )
{
    path exe = config.buildDirectory() / script;
    if ( build == debug )
    {
        exe += "d";
    }
    exe += detected::executable_extension;
    return exe;
}

path debug_script_path( const Config &config, const path &script )
{
    return config.buildDirectory() / script + ".debugger";
}

const path get_executable( const Config &config, const path &script, build_type build = release )
{
    const path exePath = executable_path( config, script, build );
    const path sourcePath = config.sourceDirectory() / script + ".cpp";

    if ( !exists( exePath ) || modification_date( sourcePath ) <= modification_date( path( script ) ) )
    {
        const path scriptDir = path( script ).dirname();
        make_directory( config.sourceDirectory() / scriptDir );

        ofstream destination( native( sourcePath ) );

        if ( !destination )
        {
            throw logic_error( "could not write source file \"" + sourcePath.string() + "\"" );
        }

        ifstream scriptsource( native( script ) );
        string line;
        getline( scriptsource, line );
        if ( line.substr( 0, 2 ) != "#!" )
        {
            throw logic_error( "hashbang not found!" );
        }

        ifstream header( native( config.header() ) );
        if ( header )
        {
            destination << header.rdbuf();
        }
        else
        {
            destination << defaultHeader;
        }

        while ( getline( scriptsource, line ) )
        {
            destination << line << endl;
        }

        destination << "return 0; }";

        destination.close();

        make_directory( config.buildDirectory() / scriptDir );

        const string quote( "\"" ), space( " " );

        stringstream command;
#if _WIN32
        command << quote;
#endif
        command << quote << native( config.compilerCommand() ) << quote << space
                << config.cxxFlags() << space
#if _WIN32
				<< "/D_USRDLL /D_WINDLL "
#endif
                << ( ( build == release ) ? config.releaseFlags() : config.debugFlags() ) << space
                << quote << native( sourcePath ) << quote << space
                << config.includeDirectories() << space
                << detected::include_dir << quote << native( config.baseDirectory() / "include" ) << quote << space
                << config.libraries() << space
                << detected::library_cmd << "commandline_utilities" << detected::library_postfix << space
                << detected::linker_prefix
                << detected::output_cmd << quote << native( exePath ) << quote << space // << "-shared" << space
                << config.linkDirectories() << space
                << detected::library_dir << quote << native( config.baseDirectory() / "lib" ) << quote;
//                << " > " << quote << native( exePath ) << ".log" << quote << " 2>&1";
#if _WIN32
        command << quote;
#endif

        cout << command.str() << endl;
//        if ( system( ( quote + command.str() + quote ) ) )
        if ( system( ( command.str() ) ) )
        {
            ifstream errorFile( native( exePath + ".log" ) );
            throw logic_error( "problem compiling script\n" + string( istream_iterator< char >( errorFile ), istream_iterator< char >() ) );
        }
    }

    return exePath;
}

void handle_script( const arguments &args )
{
    const path script( absolute( path( args[ 1 ] ), cwd() ) );

    Config config( get_config() );

    const path exePath = get_executable( config, script );

#if _WIN32
	HMODULE handle = LoadLibraryA( exePath.c_str() );
#else
    void *handle = dlopen( exePath.c_str(), RTLD_LAZY );
#endif

    if ( !handle )
    {
		throw logic_error( "could not load script: \"" + exePath.string() + "\" " + to_string( GetLastError() ) );
    }

    nativescript_main_t nativescript_main;
	
#if _WIN32
	nativescript_main = reinterpret_cast< nativescript_main_t >( GetProcAddress( handle, "nativescript_main" ) );
#else
    nativescript_main = reinterpret_cast< nativescript_main_t >( dlsym( handle, "nativescript_main" ) );
#endif

    if ( !nativescript_main )
    {
        throw logic_error( "could not call main loop from script: \"" + exePath.string() + "\"" );
    }

    exit( nativescript_main( args.skip( 1 ) ) );
}

void debug_script( const arguments &args )
{
    const path script( absolute( path( args[ 2 ] ), cwd() ) );

    Config config( get_config() );

    const string exe( get_executable( config, script, debug ).string() );

    const string debug_script( debug_script_path( config, script ).string() );

    ofstream stream( debug_script.c_str() );

    stream << "file " << exe << endl
                 << "r ";
    for ( size_t i = 3; i < args.size(); ++i )
    {
        stream << args[ i ] << ' ';
    }
    stream << endl << "q" << endl;

    stream.close();

    // for some reason, lldb won't start in execv
    exit( system( ( config.debugCommand() + " -s " + debug_script ) ) );
//	execv( config.debugCommand(), argv + 1 );
}

void print_usage( const arguments & )
{
    cout << "you're doing it wrong!" << endl;
}

void show_executable( const arguments &args )
{
    const path script( absolute( path( args[ 2 ] ), cwd() ) );
    Config config( get_config() );
    cout << executable_path( config, script ) << endl;
}

void show_debug_executable( const arguments &args )
{
    const path script( absolute( path( args[ 2 ] ), cwd() ) );
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
        arguments args( argv, argv + argc );

        if ( argc < 2 )
        {
            print_usage( args );
        }
        else
        {
            argument_handling[ args[ 1 ] ]( args );
        }
    }
    catch ( const exception &err )
    {
        cerr << err.what() << endl;
        return 1;
    }

    return  0;
}
