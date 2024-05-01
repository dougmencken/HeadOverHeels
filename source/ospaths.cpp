
#include "ospaths.hpp"

#include "util.hpp"

#include <algorithm>

#include <unistd.h> // getcwd
#include <sys/stat.h> // mkdir, stat

#if defined ( __CYGWIN__ )
    #include <sys/cygwin.h>
    #define DEBUG_CYGWIN_CONVERT_PATH   0
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
        if ( length >= 0 ) {
                windowsPath = static_cast< char * >( malloc( length ) );
                cygwin_conv_path ( CCP_POSIX_TO_WIN_A | CCP_RELATIVE, path.c_str (), windowsPath, length ) ;
        }

#if defined( DEBUG_CYGWIN_CONVERT_PATH ) && DEBUG_CYGWIN_CONVERT_PATH
        fprintf( stdout, "cygwin converted path \"%s\" to \"%s\"\n", path.c_str (), windowsPath ) ;
#endif

        return std::string( windowsPath );
#else
        return path ;
#endif
}

std::string pathToFile( const std::string & folder, const std::string & file )
{
        std::string path = folder ;

        if ( ! file.empty() ) {
                if ( path.at( path.length() - 1 ) != '/' )
                        path.append( "/" );

                path += file ;
        }

        return convertPath( path ) ;
}

std::string FullPathToGame( "nowhere" ) ;

void setPathToGame ( const char * pathToGame )
{
        FullPathToGame = pathToGame ;
        const std::string whereIsGame = pathToGame ;

        // for cases when the game is in PATH, when running with gdb or double-clicking
        if ( whereIsGame == "headoverheels" || whereIsGame == "/usr/bin/headoverheels" )
        {
                FullPathToGame = std::string( BinDirFromConfigure ).append( "/" ).append( "headoverheels" );
                /* customizing the name of the game's binary
                   will need more exquisite logic */
        }

#if defined ( __APPLE__ ) && defined ( __MACH__ )
        const char* lastdot = std::strrchr ( nameFromPath( pathToGame ).c_str(), '.' );
        if ( lastdot != nilPointer && strlen( lastdot ) == 4 &&
                        ( lastdot[ 1 ] == 'a' && lastdot[ 2 ] == 'p' && lastdot[ 3 ] == 'p' ) )
        {       // the game is within an OS X bundle
                // the binary itself is inside whereIsGame/Contents/MacOS
        }
        else
#endif
        {
                const char pathSeparator = '/' ;
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

#if defined( DEBUG ) && DEBUG
        fprintf( stdout, "whereIsGame is \"%s\"\n", whereIsGame.c_str () );
        fprintf( stdout, "FullPathToGame is \"%s\"\n", FullPathToGame.c_str () );
#endif
}

std::string homePath ()
{
        static std::string HomePath( "" );

        if ( HomePath.empty () ) {
                char* home = getenv( "HOME" );
                if ( home != nilPointer ) {
                        HomePath = std::string( home ) + "/.headoverheels/";
                        makeFolder( HomePath );
                }
                else
                        HomePath = sharePath() ;
        }

        return HomePath ;
}

std::string sharePath ()
{
        static std::string SharePath( "" );

        if ( SharePath.empty () )
        {
                SharePath = std::string( DataDirFromConfigure ).append( "/headoverheels/" ) ;

        #if defined ( __APPLE__ ) && defined ( __MACH__ )
                std::string fullpath = FullPathToGame ;
                bool inBundle = false;

                /* when the binary is inside an application bundle
                   fullpath is something like /Applications/Games/Head over Heels.app
                   and nameFromPath gives Head over Heels.app
                */
                const char* lastdot = std::strrchr ( nameFromPath( fullpath ).c_str () , '.' );
                if ( lastdot != NULL && strlen( lastdot ) == 4 )
                        if ( lastdot[ 1 ] == 'a' && lastdot[ 2 ] == 'p' && lastdot[ 3 ] == 'p' )
                                inBundle = true;

                if ( inBundle )
                        SharePath = fullpath + "/Contents/Resources/" ;
        #endif

                fprintf ( stdout, "SharePath is \"%s\"\n", SharePath.c_str () );
        }

        return SharePath ;
}

bool folderExists( const std::string & path )
{
        struct stat info ;

        if ( stat( convertPath( path ).c_str (), &info ) < 0 )
                return false ;
        else if ( ( info.st_mode & S_IFDIR ) != 0 )
                return true ;

        return false ;
}

bool makeFolder ( const std::string & path )
{
        bool made = false ;

        if ( ! folderExists( path ) ) {
                mkdir( convertPath( path ).c_str(), /* rwxr-xr-x */ S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH );
                made = folderExists( path );
        }

        return made ;
}

}
