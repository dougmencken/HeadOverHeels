
#include "Ism.hpp"
#include "GameManager.hpp"
#include "GuiManager.hpp"

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5
#  include <allegro5/allegro.h>
#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4
#  include <allegro.h>
#endif


int main( int argc, char** argv )
{
        if ( argc > 0 ) isomot::setPathToGame( argv[ 0 ] );

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
                        isomot::setScreenWidth( static_cast< unsigned int >( width ) );
                }

                if ( options.count( "height" ) > 0 )
                {
                        int height = std::atoi( options[ "height" ].c_str () );
                        if ( height < 480 ) height = 480;
                        isomot::setScreenHeight( static_cast< unsigned int >( height ) );
                }

                if ( options.count( "head-room" ) > 0 )
                        isomot::GameManager::getInstance()->setHeadRoom( options[ "head-room" ] );

                if ( options.count( "heels-room" ) > 0 )
                        isomot::GameManager::getInstance()->setHeelsRoom( options[ "heels-room" ] );
        }

#ifdef __WIN32
        timeBeginPeriod( 1 );
#endif

        gui::GuiManager::getInstance()->begin ();

#ifdef __WIN32
        timeEndPeriod( 1 );
#endif

        return EXIT_SUCCESS;
}
#if defined( USE_ALLEGRO4 ) && USE_ALLEGRO4
END_OF_MAIN()
#endif
