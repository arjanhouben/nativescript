#include "generic.h"

using namespace std;

string get_env( const string &key )
{
	string result;
#if _WIN32
	char *value = 0;
	size_t len = 0;
	errno_t err = _dupenv_s( &value, &len, key.c_str() );
	if ( !err )
	{
		if ( len > 1 )
		{
			result.assign( value, value + len - 1 );
		}
		free( value );
	}
#else
	result = getenv( key.c_str() );
#endif
	return result;
}

string get_homedir()
{
	string homeDir = get_env( "HOME" );
#if _WIN32
	if ( homeDir.empty() )
	{
		homeDir = get_env( "HOMEDRIVE" );
		homeDir += get_env( "HOMEPATH" );
	}
#endif
	if ( homeDir.empty() ) throw logic_error( "environment variable 'HOME' not set!" );
	return homeDir;
}

void trim( string &s )
{
    auto start = s.begin();
    while ( start != s.end() && isspace( *start ) )
    {
        start = s.erase( start );
    }

	while ( !s.empty() && isspace( s.back() ) )
	{
		s.pop_back();
	}
}