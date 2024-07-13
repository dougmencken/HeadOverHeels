// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman, Bernie Drummond and Guy Stevens, released by Ocean Software Ltd. in 1987
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#include <map>
#include <vector>
#include <string>

#include "WrappersAllegro.hpp"

#include "GameManager.hpp"
#include "GamePreferences.hpp"
#include "GuiManager.hpp"
#include "GameMap.hpp"

#if defined( __gnu_linux__ ) && defined( USE_ALLEGRO4 ) && USE_ALLEGRO4
#include "SoundManager.hpp"
#endif

#include "ospaths.hpp"

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5
#  include <allegro5/allegro.h>
#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4
#  include <allegro.h>
#endif


class KnownOption
{
private:
        std::string theOption ;
        bool hasValue ;
        std::string theDescription ;

public:
        KnownOption( const std::string & option, bool has, const std::string & about )
                : theOption( option )
                , hasValue( has )
                , theDescription( about )
        {}

        const std::string & getOption () const {  return theOption ;  }

        bool withValue () const {  return hasValue ;  }

        const std::string & getDescription () const {  return theDescription ;  }

} ;

std::vector< KnownOption > knownOptions ;


void initAllegro ()
{
        allegro::init ();

        // switch to the chosen size of screen
        GamePreferences::allegroWindowSizeToScreenSize ();

        // initialize the handler of keyboard events
        allegro::initKeyboardHandler ();
}


int main( int argc, char** argv )
{
        std::cout << "Head over Heels" << std::endl ;
        std::cout << "the free and open source remake (in C++)" << std::endl ;
# ifdef PACKAGE_VERSION
        std::cout << "version " << std::string( PACKAGE_VERSION ) << std::endl;
# endif
        std::cout << std::endl ;

        if ( argc > 0 ) ospaths::setPathToGame( argv[ 0 ] );

        knownOptions.push_back( KnownOption( "help", false, "shows this text which describes all the known options" ) );
# if defined( __gnu_linux__ ) && defined( USE_ALLEGRO4 ) && USE_ALLEGRO4
        // GNU/Linux with Allegro 4 only
        knownOptions.push_back( KnownOption( "linux-audio", true, "which audio interface to use, one of =auto (default), =alsa, =oss, =esd, =arts, =jack, or =none" ) );
# endif
        knownOptions.push_back( KnownOption( "width", true, "the width of the game's screen, default is the value from preferences or =640" ) );
        knownOptions.push_back( KnownOption( "height", true, "the height of the game's screen, default is the value from preferences or =480" ) );
        knownOptions.push_back( KnownOption( "head-room", true, "the room's file name where Head begins the game, =blacktooth1head.xml by default" ) );
        knownOptions.push_back( KnownOption( "heels-room", true, "the room's file name where Heels begins the game, =blacktooth23heels.xml by default" ) );
        knownOptions.push_back( KnownOption( "build-all-rooms", false, "build all the game rooms at once before beginning the game" ) );
        knownOptions.push_back( KnownOption( "begin-game", false, "no menus, just begin the new game" ) );

        bool newGameNoGui = false ;

        if ( argc > 1 )
        {
                std::map < std::string /* option */, std::string /* value */ > options ;

                for ( int a = 1 ; a < argc ; ++ a )
                {
                        const char* arg = argv[ a ];
                        if ( arg[ 0 ] == '-' && arg[ 1 ] == '-' )
                        {
                                std::string option = std::string( arg + 2 );
                                std::string value = "" ;

                                size_t whereIsEqu = option.find( "=" );
                                if ( whereIsEqu != std::string::npos )
                                {
                                        value = option.substr( whereIsEqu + 1 );
                                        if ( value.at( 0 ) == '\"' && value.at( value.size() - 1 ) == '\"' )
                                                value = value.substr( 1, value.size() - 2 );

                                        option = option.substr( 0, whereIsEqu );
                                }

                                options[ option ] = value ;
                        }
                }

                for ( std::map< std::string, std::string >::const_iterator oi = options.begin () ; oi != options.end () ; ++ oi )
                {
                        std::string option = ( *oi ).first ;
                        std::string value = ( *oi ).second ;

                        bool knownOption = false ;
                        for ( std::vector< KnownOption >::const_iterator ko = knownOptions.begin () ;
                                        ko != knownOptions.end () ; ++ ko )
                        {
                                if ( option == ( *ko ).getOption () )
                                {
                                        knownOption = true ;
                                        break ;
                                }
                        }

                        if ( knownOption ) {
                                std::cout << "got option \"" << option << "\"" ;
                                if ( ! value.empty () ) std::cout << " with value \"" << value << "\"" ;
                                std::cout << std::endl ;
                        } else
                                std::cout << "got unknown option \"" << option << "\"" << std::endl ;
                }

                if ( options.count( "help" ) > 0 )
                {
                        std::cout << std::endl << "The options are" << std::endl ;
                        for ( std::vector< KnownOption >::const_iterator ko = knownOptions.begin () ;
                                        ko != knownOptions.end () ; ++ ko )
                        {
                                std::cout << "     " ;
                                std::cout << "--" << ( *ko ).getOption () ;
                                if ( ( *ko ).withValue () ) std::cout << "=(value)" ;
                                std::cout << " — " ;
                                std::cout << ( *ko ).getDescription () ;
                                std::cout << std::endl ;
                        }

                        // and don't run the game, just exit
                        return EXIT_SUCCESS ;
                }

        # if defined( __gnu_linux__ ) && defined( USE_ALLEGRO4 ) && USE_ALLEGRO4
                if ( options.count( "linux-audio" ) > 0 )
                {
                        SoundManager::setAudioInterface( options[ "linux-audio" ] ) ;
                }
        # endif

                if ( options.count( "width" ) > 0 )
                {
                        int width = std::atoi( options[ "width" ].c_str () );
                        GamePreferences::setScreenWidth( static_cast< unsigned int >( width ) );
                        GamePreferences::keepThisWidth( true );
                }

                if ( options.count( "height" ) > 0 )
                {
                        int height = std::atoi( options[ "height" ].c_str () );
                        GamePreferences::setScreenHeight( static_cast< unsigned int >( height ) );
                        GamePreferences::keepThisHeight( true );
                }

                if ( options.count( "head-room" ) > 0 )
                        GameManager::getInstance().setHeadRoom( options[ "head-room" ] );

                if ( options.count( "heels-room" ) > 0 )
                        GameManager::getInstance().setHeelsRoom( options[ "heels-room" ] );

                if ( options.count( "build-all-rooms" ) > 0 )
                        GameMap::setBuildEveryRoomAtOnce( true );

                if ( options.count( "begin-game" ) > 0 )
                        newGameNoGui = true ;
        }

        initAllegro ();

        GamePreferences::readPreferences( ospaths::pathToFile( ospaths::homePath(), "preferences.xml" ) );

        if ( newGameNoGui )
                GameManager::getInstance().begin () ;
        else
                gui::GuiManager::getInstance().firstMenu () ;

        GameManager::getInstance().cleanUp () ;
        gui::GuiManager::getInstance().freeSlides () ;

        std::cout << std::endl << "bye :*" << std::endl ;

        return EXIT_SUCCESS ;
}
#if defined( USE_ALLEGRO4 ) && USE_ALLEGRO4
END_OF_MAIN()
#endif
