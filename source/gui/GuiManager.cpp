
#include "GuiManager.hpp"

#include "LanguageStrings.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"
#include "Color.hpp"
#include "Menu.hpp"
#include "PictureWidget.hpp"
#include "Label.hpp"
#include "CreateLanguageMenu.hpp"
#include "CreateMainMenu.hpp"

#include "MayNotBePossible.hpp"

#include "ospaths.hpp"
#include "sleep.hpp"

#include <algorithm>


namespace gui
{

GuiManager * GuiManager::instance = nilPointer ;

GuiManager::GuiManager( ) :
        activeScreen( nilPointer ),
        chosenLanguage( "" ),
        languageStrings( nilPointer ),
        active( true ),
        inFullScreen( false )
{
        std::string nameOfWindow = "Head over Heels" ;

#ifdef PACKAGE_VERSION
        std::string version = "v" + std::string( PACKAGE_VERSION );

        if ( version == "v1.30" )
                version = "30th anniversary";
        else if ( version == "v1.31" )
                version = "31st anniversary";
        else if ( version == "v1.32" )
                version = "0x20 32nd anniversary";

        if ( version.find( "v1.4" ) != std::string::npos )
                nameOfWindow = "Foot and Mouth" ;

        nameOfWindow += " " + version ;
#endif

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5
        nameOfWindow += " (allegro5)" ;
#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4
        nameOfWindow += " (allegro4)" ;
#endif

        allegro::setTitleOfAllegroWindow ( nameOfWindow );

        autouniqueptr< allegro::Pict > icon( allegro::Pict::fromPNGFile( ospaths::pathToFile( ospaths::sharePath(), "head48.icon.png" ) ) );
        if ( icon->isNotNil() )
                allegro::setAllegroIcon ( *icon );

        // initialize the sound manager
        SoundManager::getInstance().readSounds( "sounds.xml" );
}

GuiManager::~GuiManager( )
{
        freeScreens () ;

        delete this->languageStrings ;
}

GuiManager& GuiManager::getInstance ()
{
        if ( instance == nilPointer )
                instance = new GuiManager () ;

        return *instance ;
}

void GuiManager::begin ()
{
        // if the language isn’t set, show the menu of languages, or the main menu otherwise
        Action * firstMenu = this->chosenLanguage.empty() ? static_cast< Action * >( new CreateLanguageMenu() )
                                                          : static_cast< Action * >( new CreateMainMenu() ) ;
        if ( firstMenu != nilPointer )
                firstMenu->doIt ();
        else
                throw MayNotBePossible( "can't create the first menu" );

        // draw the user interface and handle keys
        while ( this->active )
        {
                activeScreen->draw ();

                if ( allegro::areKeypushesWaiting() )
                        activeScreen->handleKey( allegro::nextKey() );

                somn::milliSleep( 30 );
        }

        delete firstMenu ;
}

void GuiManager::changeScreen( Screen & newScreen, bool dive )
{
        const std::string & newScreenAction = newScreen.getNameOfAction () ;

        std::map< std::string, ScreenPtr >::const_iterator iscreen = this->screens.find( newScreenAction );
        if ( iscreen != this->screens.end () ) {
                if ( this->activeScreen != nilPointer ) {
                        if ( // it’s not a slide with planets
                             this->activeScreen->getNameOfAction().find( "ShowSlideWithPlanets" ) == std::string::npos )
                                Screen::barWipeHorizontally( * this->activeScreen, newScreen, dive );
                } else
                        // this is the first slide ever shown
                        Screen::randomPixelFadeIn( Color::blackColor(), newScreen );

                setActiveScreen( iscreen->second );
                redraw() ;
        }
        else {
                fprintf( stderr, "there’s no screen for action \" %s \", please create it before use\n", newScreenAction.c_str () );
                throw MayNotBePossible( "the screen for action \" " + newScreenAction + " \" is absent" ) ;
        }
}

ScreenPtr GuiManager::findOrCreateScreenForAction ( Action & action )
{
        const std::string & nameOfAction = action.getNameOfAction() ;

        if ( this->screens.find( nameOfAction ) == this->screens.end () ) {
                std::cout << "making new screen for action \" " << nameOfAction << " \"" << std::endl ;

                this->screens[ nameOfAction ] = ScreenPtr( new Screen( action ) );

                if ( this->screens[ nameOfAction ] == nilPointer )
                        throw MayNotBePossible( "can't make a screen for action \" " + nameOfAction + " \"" ) ;
        }

        return this->screens[ nameOfAction ];
}

void GuiManager::freeScreens ()
{
        this->screens.clear() ;
        setActiveScreen( ScreenPtr () );
}

void GuiManager::refreshScreens ()
{
        for (  std::map< std::string, ScreenPtr >::iterator i = this->screens.begin (); i != this->screens.end (); ++ i ) {
                if ( i->second != nilPointer )
                        i->second->refreshPicturesOfHeadAndHeels () ;
        }

        Screen::refreshBackground () ;
}

void GuiManager::redraw()
{
        if ( this->active && ( this->activeScreen != nilPointer ) )
                this->activeScreen->draw ();
}

void GuiManager::toggleFullScreenVideo ()
{
        bool switched = false;

        if ( this->inFullScreen )
                switched = allegro::switchToWindowedVideo();
        else
                switched = allegro::switchToFullscreenVideo();

        if ( switched ) {
                this->inFullScreen = ! this->inFullScreen ;

                fprintf( stdout, "video is now %s\n", ( this->inFullScreen ? "in full screen" : "in window" ) );
                somn::milliSleep( 80 );
                redraw ();
        }
        else {
                SoundManager::getInstance().stopEverySound ();
                SoundManager::getInstance().play( "menus", "mistake", /* loop */ false );

                if ( this->inFullScreen )
                        allegro::switchToFullscreenVideo();
                else
                        allegro::switchToWindowedVideo();
        }
}

/* private */
void GuiManager::useLanguage( const std::string & language )
{
        if ( this->languageStrings != nilPointer ) {
                delete this->languageStrings ;
                this->languageStrings = nilPointer ;
        }

        fprintf( stdout, "using language \"%s\"\n", language.c_str () );

        this->languageStrings = new LanguageStrings( language + ".xml", "en_US.xml" );
}

}
