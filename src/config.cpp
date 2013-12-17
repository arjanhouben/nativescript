#include <config.h>
#include <filesystem.h>
#include <generic.h>
#include <defines.h>

#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <fstream>
#include <stdexcept>

using namespace std;
using namespace filesystem;

typedef map< string, string > Values;

path defaultBaseDir()
{
    return path( get_homedir() ) / ".nativescript/";
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
            values[ key ] = tmp.substr( static_cast< size_t >( line.tellg() ) );
            trim( values[ key ] );
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
            return destination_ != 0;
        }

        std::string str() const
        {
            return destination_ ? *destination_ : std::string();
        }

    private:

        string *destination_;
};

ostream& operator << ( ostream &stream, const SaveValue &value )
{
    return stream << value.str();
}

Config::Config() :
    baseDirectory_( defaultBaseDir().string() ),
    buildDirectory_( "build/" ),
    sourceDirectory_( "source/" ),
    header_( "header.cpp" ),
    footer_( "footer.cpp" ),
    compilerCommand_( detected::compiler ),
    debugCommand_( "lldb" ),
    cxxFlags_( detected::cxx_flags ),
    releaseFlags_( detected::cxx_flags_release ),
    debugFlags_( detected::cxx_flags_debug ),
    includeDirectories_(),
    linkDirectories_(),
    libraries_()
{
}

map< string, SaveValue > Config::getConfigValues()
{
    map<string, SaveValue> saveValues;
    saveValues[ "base_directory" ] = &baseDirectory_;
    saveValues[ "build_directory" ] = &buildDirectory_;
    saveValues[ "source_directory" ] = &sourceDirectory_;
    saveValues[ "compiler" ] = &compilerCommand_;
    saveValues[ "debugger" ] = &debugCommand_;
    saveValues[ "cxx_flags" ] = &cxxFlags_;
    saveValues[ "release_flags" ] = &releaseFlags_;
    saveValues[ "debug_flags" ] = &debugFlags_;
    saveValues[ "include_directories" ] = &includeDirectories_;
    saveValues[ "link_directories" ] = &linkDirectories_;
    saveValues[ "libraries" ] = &libraries_;
    return saveValues;
}

Config Config::readConfigFile( const path &configPath )
{
    Config config;

    ifstream configFile( configPath.string() );
    if ( !configFile )
    {
        const path sample( configPath + ".sample" );
        if ( !exists( sample ) )
        {
            make_directory( sample.dirname() );
            ofstream defaultFile( sample.string() );
            writeDefault( defaultFile );
        }
    }
    else
    {
        try
        {
            Values values( parseConfig( configFile ) );

            map< string, SaveValue > saveValues( config.getConfigValues() );

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
           << "# this file needs to be renamed from 'config.sample' to 'config' for it to be effective" << endl << endl;
    for ( auto i : config.getConfigValues() )
    {
        stream << i.first << " = " << i.second << endl << endl;
    }
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

string Config::debugCommand() const
{
    return debugCommand_;
}

string Config::cxxFlags() const
{
    return cxxFlags_;
}

string Config::releaseFlags() const
{
    return releaseFlags_;
}

string Config::debugFlags() const
{
    return debugFlags_;
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
