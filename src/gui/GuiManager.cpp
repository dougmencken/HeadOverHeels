
#include "GuiManager.hpp"
#include "Ism.hpp"
#include "LanguageManager.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"
#include "GameManager.hpp"
#include "Color.hpp"
#include "Screen.hpp"
#include "Menu.hpp"
#include "PictureWidget.hpp"
#include "Label.hpp"
#include "CreateLanguageMenu.hpp"

#include <algorithm>

#ifdef DEBUG
/* #  define DEBUG_GUI       1 */
#endif

using gui::GuiManager;
using gui::Screen;
using gui::Font;
using gui::LanguageManager;
using iso::GameManager;
using iso::InputManager;
using iso::SoundManager;


GuiManager * GuiManager::instance = nilPointer ;

GuiManager::GuiManager( ) :
        activeScreen( nilPointer ),
        language( "" ),
        languageManager( nilPointer ),
        active( true ),
        atFullScreen( false ),
        preferencesRead( false )
{
        std::string nameOfWindow = "Head over Heels";

#ifdef PACKAGE_VERSION
        std::string version = "v" + std::string( PACKAGE_VERSION );

        if ( version == "v1.30" )
                version = "30th anniversary";
        else if ( version == "v1.31" )
                version = "31st anniversary";
        else if ( version == "v1.32" )
                version = "32nd anniversary";

        nameOfWindow += " " + version ;
#endif

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5
        nameOfWindow += " (allegro5)" ;
#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4
        nameOfWindow += " (allegro4)" ;
#endif

        allegro::setTitleOfAllegroWindow ( nameOfWindow );

        // create fonts to use in game

        addFont( new Font( "white.plain", Color::whiteColor() ) );
        addFont( new Font( "white.big", Color::whiteColor(), true ) );

        addFont( new Font( "orange.plain", Color::orangeColor() ) );
        addFont( new Font( "cyan.plain", Color::cyanColor() ) );
        addFont( new Font( "yellow.plain", Color::yellowColor() ) );

        addFont( new Font( "orange.big", Color::orangeColor(), true ) );
        addFont( new Font( "cyan.big", Color::cyanColor(), true ) );
        addFont( new Font( "yellow.big", Color::yellowColor(), true ) );

        addFont( new Font( "green.plain", Color::greenColor(), false ) );
        addFont( new Font( "green.big", Color::greenColor(), true ) );
        addFont( new Font( "magenta.plain", Color::magentaColor(), false ) );
        addFont( new Font( "magenta.big", Color::magentaColor(), true ) );

        // create image to draw interface
        this->picture = new Picture( iso::ScreenWidth(), iso::ScreenHeight() );

        // initialize sound manager
        SoundManager::getInstance().readSounds( "sounds.xml" );
}

GuiManager::~GuiManager( )
{
        freeScreens () ;

        std::for_each( this->fonts.begin (), this->fonts.end (), iso::DeleteIt() );
        this->fonts.clear();

        delete this->languageManager;
}

GuiManager& GuiManager::getInstance ()
{
        if ( instance == nilPointer )
                instance = new GuiManager () ;

        return *instance ;
}

void GuiManager::readPreferences ()
{
        if ( ! preferencesRead )
        {
                preferencesRead = GameManager::readPreferences( iso::homePath() + "preferences.xml" ) ;

                if ( ! preferencesRead )
                {
                        GuiManager::getInstance().setLanguage( "en_US" );
                        preferencesRead = true ;
                }
        }
}

void GuiManager::begin ()
{
        readPreferences () ;

        // show list of languages
        autouniqueptr< CreateLanguageMenu > languageMenu( new CreateLanguageMenu( this->picture ) );
        languageMenu->doIt ();

        // draw user interface and handle keys
        while ( this->active )
        {
                activeScreen->draw( picture->getAllegroPict() );

                if ( allegro::areKeypushesWaiting() )
                {
                        activeScreen->handleKey( allegro::nextKey() );
                }

                milliSleep( 30 );
        }
}

void GuiManager::changeScreen( Screen* newScreen, bool dive )
{
        if ( newScreen == nilPointer ) return ;

# if  defined( DEBUG_GUI )  &&  DEBUG_GUI

        if ( listOfScreens.size () > 0 )
        {
                fprintf ( stderr, " + +  s c r e e n z \n" ) ;
                for (  std::map< std::string, Screen * >::iterator i = listOfScreens.begin (); i != listOfScreens.end (); ++i )
                {
                        if ( i->second )
                        {
                                Action * action = i->second->getActionOfScreen () ;
                                std::cerr << "   screen @ " << i->second ;
                                std::cerr << "  action @ " << action ;
                                std::cerr << " \" " << action->getNameOfAction() << " \"" << std::endl ;
                        }
                }
                fprintf ( stderr, " - -  s c r e e n z \n" ) ;
        }

        if ( this->screen )
        {
                Action* action = screen->getActionOfScreen () ;
                Action* escape = screen->getEscapeAction () ;
                fprintf( stdout, ". previous screen was for action \" %s \" with escape action \" %s \"\n",
                                 action->getNameOfAction().c_str (),
                                 ( escape != nilPointer ? escape->getNameOfAction().c_str () : "none" ) );
        }

        if ( newScreen )
        {
                Action* actionOfNewScreen = newScreen->getActionOfScreen () ;
                Action* escapeOfNewScreen = newScreen->getEscapeAction () ;
                fprintf( stdout, ". new screen is for action \" %s \" with escape action \" %s \"\n",
                                 actionOfNewScreen->getNameOfAction().c_str (),
                                 ( escapeOfNewScreen != nilPointer ? escapeOfNewScreen->getNameOfAction().c_str () : "none" )
                ) ;
        }

# endif

        if ( listOfScreens.find( newScreen->getActionOfScreen()->getNameOfAction() ) != listOfScreens.end () )
        {
                if ( activeScreen == nilPointer || activeScreen->getActionOfScreen() == nilPointer ||
                        activeScreen->getActionOfScreen()->getNameOfAction() != "CreatePlanetsScreen" )
                {
                        Screen::barWipeHorizontally( this->activeScreen, newScreen, dive );
                }
                setActiveScreen( newScreen );
                redraw() ;
        }
        else
        {
                fprintf( stderr, "there’s no screen for action \" %s \", please create it before use\n",
                                 newScreen->getActionOfScreen()->getNameOfAction().c_str () );
        }
}

Screen * GuiManager::findOrCreateScreenForAction ( Action* action )
{
        if ( action == nilPointer )
        {
                std::cerr << "screen for nil action is nil screen" << std::endl ;
                return nilPointer ;
        }

        std::string nameOfAction = action->getNameOfAction() ;

        if ( listOfScreens.find( nameOfAction ) != listOfScreens.end () )
        {
                Screen * theScreen = listOfScreens[ nameOfAction ];
                std::cout << "here’s existing screen for action \" " << nameOfAction << " \"" << std::endl ;
                return theScreen;
        }

        std::cout << "going to create new screen for action \" " << nameOfAction << " \"" << std::endl ;
        Screen * newScreen = new Screen( action );
        listOfScreens[ nameOfAction ] = newScreen;
        return newScreen;
}

void GuiManager::freeScreens ()
{
        listOfScreens.clear() ;
        std::cout << "now list of screens is empty" << std::endl ;
}

void GuiManager::refreshScreens ()
{
        for (  std::map< std::string, Screen * >::iterator i = listOfScreens.begin (); i != listOfScreens.end (); ++i )
        {
                if ( i->second )
                {
                        i->second->refreshPicturesOfHeadAndHeels () ;
                }
        }

        Screen::refreshBackground () ;
}

void GuiManager::redraw()
{
        if ( this->active && ( this->activeScreen != nilPointer ) )
        {
                activeScreen->draw( picture->getAllegroPict() );
        }
}

std::string GuiManager::getPathToPicturesOfGui ()
{
        return iso::sharePath() + GameManager::getInstance().getChosenGraphicSet() ;
}

bool GuiManager::isAtFullScreen ()
{
        return this->atFullScreen;
}

void GuiManager::toggleFullScreenVideo ()
{
        bool switched = false;

        if ( this->atFullScreen )
                switched = allegro::switchToWindowedVideo();
        else
                switched = allegro::switchToFullscreenVideo();

        if ( switched )
        {
                this->atFullScreen = ! this->atFullScreen ;

                fprintf( stdout, "video is now %s\n", ( this->atFullScreen ? "at full screen" : "in window" ) );
                milliSleep( 80 );
                redraw ();
        }
        else
        {
                iso::SoundManager::getInstance().stopEverySound ();
                iso::SoundManager::getInstance().play( "gui", iso::Activity::Mistake, /* loop */ false );

                if ( this->atFullScreen )
                        allegro::switchToFullscreenVideo();
                else
                        allegro::switchToWindowedVideo();
        }
}

void GuiManager::assignLanguage( const std::string& language )
{
        if ( this->languageManager != nilPointer )
        {
                delete this->languageManager;
                this->languageManager = nilPointer;
        }

        fprintf( stdout, "language \"%s\"\n", language.c_str () );
        std::string pathToText = iso::sharePath() + "text" + util::pathSeparator() ;
        this->languageManager = new LanguageManager( pathToText + language + ".xml", pathToText + "en_US.xml" );
}

Font* GuiManager::findFontByFamilyAndColor ( const std::string& family, const std::string& color )
{
        std::string familyToLook = ( ! family.empty() ) ? family : "plain";
        std::string colorToLook = ( ! color.empty() ) ? color : "white";

        for (  std::list< Font * >::const_iterator i = fonts.begin (); i != fonts.end (); ++i )
        {
                if ( ( *i )->getName() == colorToLook + "." + familyToLook )
                {
                        return ( *i ) ;
                }
        }

        std::cout << "there’s no font with family \"" << familyToLook << "\" and color \"" << colorToLook << "\"" << std::endl ;

        if ( fonts.size() == 0 )
                std::cout << "list of fonts is empty" << std::endl ;
        else
        {
                std::cout << "list of fonts" << std::endl ;
                for (  std::list< Font * >::const_iterator i = fonts.begin (); i != fonts.end (); ++i )
                        std::cout << "   " << ( *i )->getName() << std::endl ;
        }

        return nilPointer ;
}
