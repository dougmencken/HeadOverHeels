
#include "GuiManager.hpp"

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
#include "version.hpp"

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

        std::string version = "v" + gameVersion() ;

        if ( version.find( "v1.4" ) != std::string::npos )
                nameOfWindow = "Foot and Mouth" ;

        nameOfWindow += " " + version ;

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
                if ( this->activeSlide != nilPointer ) {
                        // draw an active slide
                        this->activeSlide->draw ();

                        // handle keys
                        if ( allegro::areKeypushesWaiting() )
                                this->activeSlide->handleKey( allegro::nextKey() );
                }
                else throw MayNotBePossible( "there’s no active slide" ) ;

                // no te comas la CPU
                // do not eat the CPU
                somn::milliSleep( 25 );
        }
}

void GuiManager::changeSlide( const std::string & newAction, bool dive )
{
        std::map< std::string, Slide * >::const_iterator inewslide = this->slides.find( newAction );
        if ( inewslide != this->slides.end () && inewslide->second != nilPointer ) {
                const Slide & newSlide = * inewslide->second ;
                Slide * oldSlide = getActiveSlide () ;

                // show transition effect between slides
                if ( oldSlide != nilPointer ) {
                        if ( ! oldSlide->isTransitionFromThisSlideOff() && ! newSlide.isTransitionToThisSlideOff() )
                                Slide::barWipeHorizontally( * oldSlide, newSlide, dive );
                } else
                        // this is the first slide ever shown
                        Slide::randomPixelFadeIn( Color::blackColor(), newSlide );

                setActiveSlide( inewslide->second );

                allegro::emptyKeyboardBuffer() ;
                redraw() ;
        }
        else {
                std::cerr << "there’s no slide for action \"" << newAction << "\", please create it before use" << std::endl ;
                throw MayNotBePossible( "the slide for action \" " + newAction + " \" is absent" ) ;
        }
}

/* private */
void GuiManager::setActiveSlide ( Slide * newSlide )
{
        for ( std::map< std::string, Slide * >::iterator it = this->slides.begin(); it != this->slides.end(); ++ it ) {
                if ( it->second == newSlide ) {
                        this->activeSlide = newSlide ;
                        return ;
                }
        }
}

Slide & GuiManager::findOrCreateSlideForAction ( const std::string & nameOfAction )
{
        if ( this->slides.find( nameOfAction ) == this->slides.end () ) {
                std::cout << "making a new slide for action \" " << nameOfAction << " \"" << std::endl ;

                this->slides[ nameOfAction ] = new Slide() ;

                if ( this->slides[ nameOfAction ] == nilPointer )
                        throw MayNotBePossible( "can't make a slide for action \" " + nameOfAction + " \"" ) ;
        }

        return * this->slides[ nameOfAction ];
}

void GuiManager::freeSlides ()
{
        for ( std::map< std::string, Slide * >::iterator it = this->slides.begin(); it != this->slides.end(); ++ it )
                if ( it->second != nilPointer ) {
                        delete it->second ;
                        it->second = nilPointer ;
                }

        this->slides.clear() ;

        std::cout << "all previously created slides were freed" << std::endl ;

        setActiveSlide( nilPointer );
}

void GuiManager::refreshSlides ()
{
        for ( std::map< std::string, Slide * >::iterator it = this->slides.begin (); it != this->slides.end (); ++ it ) {
                if ( it->second != nilPointer )
                        it->second->refreshPicturesOfHeadAndHeels () ;
        }

        Slide::refreshBackground () ;
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

}
