
#include "Ism.hpp"
#include <unistd.h>

#if defined ( __CYGWIN__ )
    #include <sys/cygwin.h>
#endif


namespace isomot
{

void sleep( unsigned long miliseconds )
{
#if defined ( __WIN32 )
        Sleep( miliseconds );
#else
        std::modulus< unsigned long > modulus;
        unsigned long remainder = modulus( miliseconds, 1000 );
        timespec pause;
        pause.tv_sec = miliseconds / 1000;
        pause.tv_nsec = remainder * 1000000;
        nanosleep( &pause, 0 );
#endif
}

const char * pathToFile( const std::string& in )
{
#if defined ( __CYGWIN__ )
        ssize_t length ;
        char * windowsPath = 0;

        /* ssize_t cygwin_conv_path( cygwin_conv_path_t what, const void * from, void * to, size_t size ) */
        /* https://cygwin.com/cygwin-api/func-cygwin-conv-path.html */

        length = cygwin_conv_path ( CCP_POSIX_TO_WIN_A | CCP_RELATIVE, in.c_str (), 0, 0 ) ;
        if ( length >= 0 )
        {
                windowsPath = static_cast< char * >( malloc( length ) );
                cygwin_conv_path ( CCP_POSIX_TO_WIN_A | CCP_RELATIVE, in.c_str (), windowsPath, length ) ;
        }

        /* fprintf( stdout, "cygwin converted path \"%s\" to \"%s\"\n", in.c_str (), windowsPath ) ; */

        return windowsPath ;
#else
        return in.c_str ();
#endif
}

std::string PathToGame( "no way" ) ;
std::string FullPathToGame( "" ) ;

std::string pathToGame ()
{
        return PathToGame ;
}

void setPathToGame ( const char * pathToGame )
{
        PathToGame = pathToGame ;
        FullPathToGame = pathToGame ;

#if defined ( __CYGWIN__ )
        if ( PathToGame == "headoverheels" || PathToGame == "/usr/bin/headoverheels" )
        {  // in case of double-clicking or running with gdb
                FullPathToGame = "/bin/headoverheels" ;
        }
#endif

        char* lastdot = strrchr ( get_filename( PathToGame.c_str () ) , '.' );
        if ( lastdot != 0 && strlen( lastdot ) == 4 &&
                        ( lastdot[ 1 ] == 'a' && lastdot[ 2 ] == 'p' && lastdot[ 3 ] == 'p' ) )
        {  // game is in OS X bundle
                FullPathToGame = PathToGame + pathSeparator + "Contents" + pathSeparator + "MacOS" + pathSeparator ;
        }
        else
        {
                if ( FullPathToGame[ 0 ] != pathSeparator[ 0 ] )
                {  // pathToGame is not full path
                        char folderOfGame[ 1024 ];
                        getcwd( folderOfGame, 1024 ) ;

                        size_t len = strlen( folderOfGame );
                        if ( folderOfGame[ len - 1 ] == pathSeparator[ 0 ] )
                                folderOfGame[ len - 1 ] = 0;

                        FullPathToGame = std::string( folderOfGame ) + pathSeparator + PathToGame ;
                }
        }

        fprintf( stdout, "PathToGame is \"%s\"\n", PathToGame.c_str () );
        fprintf( stdout, "FullPathToGame is \"%s\"\n", FullPathToGame.c_str () );
}

std::string HomePath ;

std::string homePath ()
{
        if ( HomePath.empty () )
        {
        #if defined ( __WIN32 )
                HomePath = sharePath();
        #else /* #elif defined ( __gnu_linux__ ) */
                char* home = getenv( "HOME" );
                assert( home != 0 );
                HomePath = std::string( home ) + "/.headoverheels/";
                if ( ! file_exists( HomePath.c_str(), FA_DIREC, 0 ) )
                {
                        mkdir( HomePath.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH );
                        mkdir( ( HomePath + "savegame/" ).c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH );
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
#if defined ( __CYGWIN__ ) || defined ( __WIN32 )
                const char* cpath = FullPathToGame.c_str ();
#else
                const char* cpath = pathToGame().c_str ();
#endif
                char* filename = get_filename( cpath );

        #if defined ( __APPLE__ ) && defined ( __MACH__ )
                /* when binary is inside application bundle
                   get_executable_name gives something like
                   /Applications/Games/Head over Heels.app
                   and get_filename in its turn gives
                   Head over Heels.app */
                bool inBundle = false;
                char* lastdot = strrchr ( filename , '.' );
                if ( lastdot != NULL && strlen( lastdot ) == 4 )
                        if ( lastdot[ 1 ] == 'a' && lastdot[ 2 ] == 'p' && lastdot[ 3 ] == 'p' )
                                inBundle = true;
        #endif

                std::string container = std::string( cpath, strlen( cpath ) - strlen( filename ) - 1 );
                char* containername = get_filename( container.c_str () );

        #if defined ( __APPLE__ ) && defined ( __MACH__ )
                if ( inBundle ) {
                        SharePath = std::string( cpath );
                        SharePath += "/Contents/Resources/";
                } else {
                        /* not in bundle? okay so go the linux way */
                        SharePath = std::string( cpath, strlen( cpath ) - 1 - strlen( containername ) - 1 - strlen( filename ) );
                        SharePath += "/share/headoverheels/";
                }
        #else
                SharePath = std::string( cpath, strlen( cpath ) - strlen( filename ) - ( 1 + strlen( containername ) ) );
                SharePath += "share" + pathSeparator + "headoverheels" + pathSeparator;
        #endif

                fprintf ( stdout, "SharePath is \"%s\"\n", SharePath.c_str () );
        }

        return SharePath;
}

}
