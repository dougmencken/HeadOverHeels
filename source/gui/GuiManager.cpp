
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
        activeSlide( nilPointer ),
        chosenLanguage( "" ),
        languageStrings( nilPointer ),
        looping( true ),
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
        freeSlides () ;

        delete this->languageStrings ;
}

GuiManager& GuiManager::getInstance ()
{
        if ( instance == nilPointer )
                instance = new GuiManager () ;

        return *instance ;
}

void GuiManager::firstMenu ()
{
        // if the language isn’t set, show the menu of languages, or the main menu otherwise
        Action * firstMenu = this->chosenLanguage.empty() ? static_cast< Action * >( new CreateLanguageMenu() )
                                                          : static_cast< Action * >( new CreateMainMenu() ) ;
        if ( firstMenu != nilPointer )
                firstMenu->doIt ();
        else
                throw MayNotBePossible( "can't create the first menu" );

        loop() ;

        delete firstMenu ;
}

void GuiManager::loop ()
{
        while ( this->looping )
        {
                // draw an active slide
                this->activeSlide->draw ();

                // handle keys
                if ( allegro::areKeypushesWaiting() )
                        this->activeSlide->handleKey( allegro::nextKey() );

                somn::milliSleep( 30 );
        }
}

void GuiManager::changeSlide( const std::string & newAction, bool dive )
{
        std::map< std::string, ScreenPtr >::const_iterator inewslide = this->slides.find( newAction );
        if ( inewslide != this->slides.end () && inewslide->second != nilPointer ) {
                if ( this->activeSlide != nilPointer ) {
                        if ( ! this->activeSlide->isTransitionOff() )
                                Screen::barWipeHorizontally( * this->activeSlide, * inewslide->second, dive );
                } else
                        // this is the first slide ever shown
                        Screen::randomPixelFadeIn( Color::blackColor(), * inewslide->second );

                setActiveSlide( inewslide->second );
                redraw() ;
        }
        else {
                std::cerr << "there’s no slide for action \"" << newAction << "\", please create it before use" << std::endl ;
                throw MayNotBePossible( "the slide for action \" " + newAction + " \" is absent" ) ;
        }
}

ScreenPtr GuiManager::findOrCreateSlideForAction ( const std::string & nameOfAction )
{
        if ( this->slides.find( nameOfAction ) == this->slides.end () ) {
                std::cout << "making a new slide for action \" " << nameOfAction << " \"" << std::endl ;

                this->slides[ nameOfAction ] = ScreenPtr( new Screen() );

                if ( this->slides[ nameOfAction ] == nilPointer )
                        throw MayNotBePossible( "can't make a slide for action \" " + nameOfAction + " \"" ) ;
        }

        return this->slides[ nameOfAction ];
}

void GuiManager::freeSlides ()
{
        this->slides.clear() ;
        setActiveSlide( /* null */ ScreenPtr() );
}

void GuiManager::refreshSlides ()
{
        for (  std::map< std::string, ScreenPtr >::iterator it = this->slides.begin (); it != this->slides.end (); ++ it ) {
                if ( it->second != nilPointer )
                        it->second->refreshPicturesOfHeadAndHeels () ;
        }

        Screen::refreshBackground () ;
}

void GuiManager::redraw() const
{
        if ( this->looping && this->activeSlide != nilPointer )
                this->activeSlide->draw ();
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
