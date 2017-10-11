
#include "Ism.hpp"

namespace isomot
{

void sleep( unsigned long miliseconds )
{
#ifdef __WIN32
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

void copyTextFile( const std::string& from, const std::string& to )
{
        std::ifstream inputStream( from.c_str () );
        std::ofstream outputStream( to.c_str () );
        std::string temp;

        while( getline( inputStream, temp ) )
        {
                outputStream << temp << std::endl;
        }
}

std::string wayToString( const Direction& way )
{
        switch ( way )
        {
                case South:             return "south" ;
                case West:              return "west" ;
                case North:             return "north" ;
                case East:              return "east" ;

                case Northeast:         return "northeast" ;
                case Southeast:         return "southeast" ;
                case Southwest:         return "southwest" ;
                case Northwest:         return "northwest" ;
                case Eastnorth:         return "eastnorth" ;
                case Eastsouth:         return "eastsouth" ;
                case Westnorth:         return "westnorth" ;
                case Westsouth:         return "westsouth" ;

                case Up:                return "up" ;
                case Down:              return "down" ;

                case ByTeleport:        return "via teleport" ;
                case ByTeleportToo:     return "via second teleport" ;

                case NoEntry:           return "no entrance" ;
                case NoExit:            return "no exit" ;

                case Restart:           return "rebuild room" ;

                case JustWait:          return "just wait" ;
        }

        return "unknown way" ;
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

                        /// copyTextFile( sharePath() + "configuration.xsd", HomePath + "configuration.xsd" );
                        /// copyTextFile( sharePath() + "savegame/savegame.xsd", HomePath + "savegame/savegame.xsd" );
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
                char cpath[ 1024 ];
                get_executable_name( cpath , 1024 );
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
                SharePath += "share/headoverheels/";
        #endif

                fprintf ( stdout, "SharePath is \"%s\"\n", SharePath.c_str () );
        }

        return SharePath;
}

}
