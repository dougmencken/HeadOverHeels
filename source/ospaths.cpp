
#include "ospaths.hpp"
#include "util.hpp"

#include "path_prefix.h"

#include <algorithm>

#include <unistd.h> // getcwd

#if defined ( __CYGWIN__ )
    #include <sys/cygwin.h>
#endif

#ifndef PATH_MAX
    #define PATH_MAX 4096
#endif


namespace ospaths
{

std::string nameFromPath( std::string const& path )
{
        return std::string (
                std::find_if( path.rbegin(), path.rend(), IsPathSeparator() ).base(),
                path.end() );
}

static std::string convertPath( const std::string& path )
{
#if defined ( __CYGWIN__ )
        ssize_t length ;
        char * windowsPath = nilPointer;

        /* ssize_t cygwin_conv_path( cygwin_conv_path_t what, const void * from, void * to, size_t size ) */
        /* https://cygwin.com/cygwin-api/func-cygwin-conv-path.html */

        length = cygwin_conv_path ( CCP_POSIX_TO_WIN_A | CCP_RELATIVE, path.c_str (), nilPointer, 0 ) ;
        if ( length >= 0 )
        {
                windowsPath = static_cast< char * >( malloc( length ) );
                cygwin_conv_path ( CCP_POSIX_TO_WIN_A | CCP_RELATIVE, path.c_str (), windowsPath, length ) ;
        }

#if defined( DEBUG ) && DEBUG
        /// fprintf( stdout, "cygwin converted path \"%s\" to \"%s\"\n", path.c_str (), windowsPath ) ;
#endif

        return std::string( windowsPath );
#else
        return path ;
#endif
}

std::string pathToFile( const std::string& folder, const std::string& file )
{
        std::string path = folder ;

        if ( ! file.empty() )
        {
                if ( path.at( path.length() - 1 ) != ospaths::pathSeparator ().at( 0 ) )
                        path.append( ospaths::pathSeparator () );

                path += file ;
        }

        return convertPath( path ) ;
}

std::string FullPathToGame( "no way" ) ;

void setPathToGame ( const char * pathToGame )
{
        FullPathToGame = pathToGame ;
        const std::string whereIsGame = pathToGame ;

        // for cases when game is in PATH, when running with gdb or double-clicking
        if ( whereIsGame == "headoverheels" || whereIsGame == "/usr/bin/headoverheels" )
        {
#if defined ( __CYGWIN__ )
                FullPathToGame = "/bin/headoverheels" ;         /* hard-coded */
#else
                /* more sophiscated logic */ // at least than = "/usr/bin/headoverheels"
                FullPathToGame = std::string( ThatPrefixFromConfigure ) + "/bin/headoverheels" ;
                /* and those who wish a custom name for the game's binary
                   may sophiscate it further more */
#endif
        }

#if defined ( __APPLE__ ) && defined ( __MACH__ )
        const char* lastdot = std::strrchr ( nameFromPath( pathToGame ).c_str(), '.' );
        if ( lastdot != nilPointer && strlen( lastdot ) == 4 &&
                        ( lastdot[ 1 ] == 'a' && lastdot[ 2 ] == 'p' && lastdot[ 3 ] == 'p' ) )
        {       // game is in OS X bundle
                // binary itself lives inside whereIsGame/Contents/MacOS
        }
        else
#endif
        {
                char pathSeparator = ospaths::pathSeparator()[ 0 ] ;
                if ( FullPathToGame[ 0 ] != pathSeparator )
                {  // it’s not full path
                        char folderOfGame[ PATH_MAX ];
                        char* cwd = getcwd( folderOfGame, PATH_MAX ) ;
                        if ( cwd == nilPointer )
                                folderOfGame[ 0 ] = '\0' ;

                        size_t len = strlen( folderOfGame );
                        if ( folderOfGame[ len - 1 ] == pathSeparator )
                                folderOfGame[ len - 1 ] = '\0';

                        FullPathToGame = std::string( folderOfGame ) + pathSeparator + whereIsGame ;
                }
        }

        fprintf( stdout, "whereIsGame is \"%s\"\n", whereIsGame.c_str () );
        fprintf( stdout, "FullPathToGame is \"%s\"\n", FullPathToGame.c_str () );
}

std::string HomePath ;

std::string homePath ()
{
        if ( HomePath.empty () )
        {
        #if defined ( __WIN32 ) || defined ( __CYGWIN__ )
                HomePath = sharePath();
        #else
                char* home = getenv( "HOME" );
                if ( home != nilPointer )
                {
                        HomePath = std::string( home ) + "/.headoverheels/";
                        if ( ! ospaths::folderExists( HomePath ) )
                        {
                                mkdir( HomePath.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH );
                                mkdir( ( HomePath + "savegame/" ).c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH );
                        }
                }
                else
                {
                        HomePath = sharePath();
                }
        #endif
        }

        return HomePath;
}

std::string SharePath ;

std::string sharePath ()
{
        if ( SharePath.empty () )
        {
                std::string cpath = FullPathToGame ;
                std::string filename = nameFromPath( cpath ) ;

        #if defined ( __APPLE__ ) && defined ( __MACH__ )
                /* when binary is inside application bundle
                   get_executable_name gives something like
                   /Applications/Games/Head over Heels.app
                   and nameFromPath in its turn gives
                   Head over Heels.app */
                bool inBundle = false;
                const char* lastdot = std::strrchr ( filename.c_str() , '.' );
                if ( lastdot != NULL && strlen( lastdot ) == 4 )
                        if ( lastdot[ 1 ] == 'a' && lastdot[ 2 ] == 'p' && lastdot[ 3 ] == 'p' )
                                inBundle = true;
        #endif

                std::string container = cpath.substr( 0, cpath.length() - filename.length() - 1 );
                std::string containername = nameFromPath( container ) ;

        #if defined ( __APPLE__ ) && defined ( __MACH__ )
                if ( inBundle ) {
                        SharePath = cpath ;
                        SharePath += "/Contents/Resources/";
                } else {
                        /* not in bundle? okay so go the linux way */
                        SharePath = cpath.substr( 0, cpath.length() - 1 - containername.length() - 1 - filename.length() );
                        SharePath += "/share/headoverheels/";
                }
        #else
                SharePath = cpath.substr( 0, cpath.length() - filename.length() - ( 1 + containername.length() ) );
                SharePath += "share" + ospaths::pathSeparator () + "headoverheels" + ospaths::pathSeparator () ;
        #endif

                fprintf ( stdout, "SharePath is \"%s\"\n", SharePath.c_str () );
        }

        return SharePath;
}

bool folderExists( const std::string & path )
{
        struct stat info ;

        if ( stat( path.c_str (), &info ) < 0 )
                return false ;
        else if ( ( info.st_mode & S_IFDIR ) != 0 )
                return true ;

        return false ;
}

}
