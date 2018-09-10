
#include "Ism.hpp"

#include <algorithm>


#if defined ( __CYGWIN__ )
    #include <sys/cygwin.h>
#endif

#ifndef PATH_MAX
    #define PATH_MAX 4096
#endif


void milliSleep( unsigned long milliseconds )
{
#if defined ( __WIN32 )
        Sleep( milliseconds );
#else
        nanoSleep( milliseconds * 1000000 );
#endif
}

#ifndef __WIN32

void microSleep( unsigned long microseconds )
{
        nanoSleep( microseconds * 1000 );
}

void nanoSleep ( unsigned long nanoseconds )
{
        std::modulus< unsigned long > modulus;
        unsigned long remainder = modulus( nanoseconds, 1000000000 );
        timespec pause;
        pause.tv_sec = nanoseconds / 1000000000;
        pause.tv_nsec = remainder;
        nanosleep( &pause, NULL );
}

#endif


namespace isomot
{

unsigned int screenWidth = 640 ;

unsigned int screenHeight = 480 ;

unsigned int ScreenWidth() {  return screenWidth ;  }

unsigned int ScreenHeight() {  return screenHeight ;  }

void setScreenWidth( unsigned int width )
{
        if ( width < 640 ) width = 640;
        screenWidth = width;
}

void setScreenHeight( unsigned int height )
{
        if ( height < 480 ) height = 480;
        screenHeight = height;
}

std::string makeRandomString( const size_t length )
{
        static const char alphanum[] =  "0123456789"
                                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                        "abcdefghijklmnopqrstuvwxyz" ;
        static const size_t howManyChars = sizeof( alphanum );

        std::string result( length, ' ' );

        for ( size_t i = 0 ; i < length; ++ i )
        {
                result[ i ] = alphanum[ rand() % ( howManyChars - 1 ) ];
        }

        return result;

}

std::string toStringWithOrdinalSuffix( unsigned int number )
{
        unsigned int mod10 = number % 10;
        unsigned int mod100 = number % 100;

        std::ostringstream result ;
        result << number ;

        if ( mod10 == 1 && mod100 != 11 )
        {
                result << "st";
        }
        else if ( mod10 == 2 && mod100 != 12 )
        {
                result << "nd";
        }
        else if ( mod10 == 3 && mod100 != 13 )
        {
                result << "rd";
        }
        else
        {
                result << "th";
        }

        return result.str() ;
}

std::string nameFromPath( std::string const& path )
{
        return std::string (
                std::find_if( path.rbegin(), path.rend(), IsPathSeparator() ).base(),
                path.end() );
}

const char * pathToFile( const std::string& in )
{
#if defined ( __CYGWIN__ )
        ssize_t length ;
        char * windowsPath = nilPointer;

        /* ssize_t cygwin_conv_path( cygwin_conv_path_t what, const void * from, void * to, size_t size ) */
        /* https://cygwin.com/cygwin-api/func-cygwin-conv-path.html */

        length = cygwin_conv_path ( CCP_POSIX_TO_WIN_A | CCP_RELATIVE, in.c_str (), nilPointer, 0 ) ;
        if ( length >= 0 )
        {
                windowsPath = static_cast< char * >( malloc( length ) );
                cygwin_conv_path ( CCP_POSIX_TO_WIN_A | CCP_RELATIVE, in.c_str (), windowsPath, length ) ;
        }

#if defined( DEBUG ) && DEBUG
        /// fprintf( stdout, "cygwin converted path \"%s\" to \"%s\"\n", in.c_str (), windowsPath ) ;
#endif

        return windowsPath ;
#else
        return in.c_str ();
#endif
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
                FullPathToGame = "/bin/headoverheels" ;
#else
                FullPathToGame = "/usr/bin/headoverheels" ;  /* hard-coded */
                                                             /* for other paths like /usr/local
                                                                and~or custom names of game
                                                                it needs more sophiscated logic */
#endif
        }

#if defined ( __APPLE__ ) && defined ( __MACH__ )
        char* lastdot = std::strrchr ( nameFromPath( pathToGame ).c_str(), '.' );
        if ( lastdot != nilPointer && strlen( lastdot ) == 4 &&
                        ( lastdot[ 1 ] == 'a' && lastdot[ 2 ] == 'p' && lastdot[ 3 ] == 'p' ) )
        {       // game is in OS X bundle
                // binary itself lives inside whereIsGame/Contents/MacOS
        }
        else
#endif
        {
                if ( FullPathToGame[ 0 ] != pathSeparator[ 0 ] )
                {  // it’s not full path
                        char folderOfGame[ PATH_MAX ];
                        getcwd( folderOfGame, PATH_MAX ) ;

                        size_t len = strlen( folderOfGame );
                        if ( folderOfGame[ len - 1 ] == pathSeparator[ 0 ] )
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
        #else /// #elif defined ( __gnu_linux__ )
                char* home = getenv( "HOME" );
                if ( home != nilPointer )
                {
                        HomePath = std::string( home ) + "/.headoverheels/";
                        if ( ! isomot::folderExists( HomePath ) )
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
                char* lastdot = std::strrchr ( filename.c_str() , '.' );
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
                SharePath += "share" + pathSeparator + "headoverheels" + pathSeparator;
        #endif

                fprintf ( stdout, "SharePath is \"%s\"\n", SharePath.c_str () );
        }

        return SharePath;
}

}
