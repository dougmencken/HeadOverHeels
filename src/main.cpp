// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#include <map>

#include "WrappersAllegro.hpp"

#include "Ism.hpp"
#include "GameManager.hpp"
#include "GuiManager.hpp"

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5
#  include <allegro5/allegro.h>
#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4
#  include <allegro.h>
#endif


std::multimap< unsigned int, unsigned int > sizesOfScreen ;

void initAllegro()
{
        allegro::init ();

        // fill list of screen’s sizes

        sizesOfScreen.insert( std::pair< unsigned int, unsigned int >( 640, 480 ) );

        /*  if ( allegro::switchToFullscreenVideo( 800, 600 ) )
                sizesOfScreen.insert( std::pair< unsigned int, unsigned int >( 800, 600 ) ); */

        sizesOfScreen.insert( std::pair< unsigned int, unsigned int >( 800, 600 ) );
        sizesOfScreen.insert( std::pair< unsigned int, unsigned int >( 1024, 576 ) );
        sizesOfScreen.insert( std::pair< unsigned int, unsigned int >( 1024, 600 ) );
        sizesOfScreen.insert( std::pair< unsigned int, unsigned int >( 1024, 768 ) );
        sizesOfScreen.insert( std::pair< unsigned int, unsigned int >( 1280, 720 ) );
        sizesOfScreen.insert( std::pair< unsigned int, unsigned int >( 1280, 1024 ) );

        // switch to chosen size of screen

        bool switched = allegro::switchToWindowedVideo( iso::ScreenWidth(), iso::ScreenHeight() ) ;
        if ( ! switched )
                std::cout << "can’t switch screen to " << iso::ScreenWidth() << " x " << iso::ScreenHeight() << std::endl ;

        allegro::Pict::theScreen().clearToColor( Color::blackColor().toAllegroColor() ) ;

        // initialize handler of keyboard events
        allegro::initKeyboardHandler ();
}


int main( int argc, char** argv )
{
        if ( argc > 0 ) iso::setPathToGame( argv[ 0 ] );

        if ( argc > 1 )
        {
                std::map < std::string /* option */, std::string /* value */ > options;

                for ( int a = 1 ; a < argc ; ++ a )
                {
                        const char* arg = argv[ a ];
                        if ( arg[ 0 ] == '-' && arg[ 1 ] == '-' )
                        {
                                std::string option = std::string( arg + 2 );
                                std::string value = "";

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

        #if DEBUG
                for ( std::map< std::string, std::string >::const_iterator it = options.begin () ; it != options.end () ; ++ it )
                {
                        std::cout << "got option \"" << ( *it ).first << "\" with value \"" << ( *it ).second << "\"" << std::endl ;
                }
        #endif

                if ( options.count( "width" ) > 0 )
                {
                        int width = std::atoi( options[ "width" ].c_str () );
                        if ( width < 640 ) width = 640;
                        iso::setScreenWidth( static_cast< unsigned int >( width ) );
                }

                if ( options.count( "height" ) > 0 )
                {
                        int height = std::atoi( options[ "height" ].c_str () );
                        if ( height < 480 ) height = 480;
                        iso::setScreenHeight( static_cast< unsigned int >( height ) );
                }

                if ( options.count( "head-room" ) > 0 )
                        iso::GameManager::getInstance().setHeadRoom( options[ "head-room" ] );

                if ( options.count( "heels-room" ) > 0 )
                        iso::GameManager::getInstance().setHeelsRoom( options[ "heels-room" ] );
        }

        initAllegro ();

#ifdef __WIN32
        timeBeginPeriod( 1 );
#endif

        gui::GuiManager::getInstance().begin () ;

#ifdef __WIN32
        timeEndPeriod( 1 );
#endif

        iso::GameManager::getInstance().cleanUp () ;

        return EXIT_SUCCESS ;
}
#if defined( USE_ALLEGRO4 ) && USE_ALLEGRO4
END_OF_MAIN()
#endif
