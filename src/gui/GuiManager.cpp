
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
using isomot::GameManager;
using isomot::InputManager;
using isomot::SoundManager;


GuiManager* GuiManager::instance = nilPointer ;

GuiManager::GuiManager( ) :
        activeScreen( nilPointer ),
        language( "" ),
        languageManager( nilPointer ),
        active( true ),
        atFullScreen( false ),
        preferencesRead( false )
{
        initAllegro();

        std::string nameOfWindow = "Head over Heels";

#ifdef PACKAGE_VERSION
        std::string version = "v" + std::string( PACKAGE_VERSION );

        if ( version == "v1.30" )
                version = "30th anniversary";
        else if ( version == "v1.31" )
                version = "31st anniversary";
        else if ( version == "v1.32" )
                version = "32nd anniversary";

        nameOfWindow = nameOfWindow + " " + version;
#endif

        allegro::setTitleOfAllegroWindow ( nameOfWindow );

        // create fonts to use in game

        std::string nameOfFontFile = isomot::sharePath() + "font.png" ;
        std::cout << "reading from file \"" << nameOfFontFile << "\" to create fonts used in game" << std::endl ;

        smartptr< allegro::Pict > pictureOfFont( allegro::loadPNG( isomot::pathToFile( nameOfFontFile.c_str () ) ) );
        if ( pictureOfFont->isNotNil() )
        {
                addFont( new Font( "white.plain", *pictureOfFont.get(), Color::whiteColor() ) );
                addFont( new Font( "white.big", *pictureOfFont.get(), Color::whiteColor(), true ) );

                addFont( new Font( "orange.plain", *pictureOfFont.get(), Color::orangeColor() ) );
                addFont( new Font( "cyan.plain", *pictureOfFont.get(), Color::cyanColor() ) );
                addFont( new Font( "yellow.plain", *pictureOfFont.get(), Color::yellowColor() ) );

                addFont( new Font( "orange.big", *pictureOfFont.get(), Color::orangeColor(), true ) );
                addFont( new Font( "cyan.big", *pictureOfFont.get(), Color::cyanColor(), true ) );
                addFont( new Font( "yellow.big", *pictureOfFont.get(), Color::yellowColor(), true ) );

                addFont( new Font( "green.plain", *pictureOfFont.get(), Color::greenColor(), false ) );
                addFont( new Font( "green.big", *pictureOfFont.get(), Color::greenColor(), true ) );
                addFont( new Font( "magenta.plain", *pictureOfFont.get(), Color::magentaColor(), false ) );
                addFont( new Font( "magenta.big", *pictureOfFont.get(), Color::magentaColor(), true ) );
        }
        else
        {
                std::cerr << "oops, can’t get letters of fonts from file \"" << nameOfFontFile << "\"" << std::endl ;
        }

        // create image to draw interface
        this->picture = new Picture( isomot::ScreenWidth(), isomot::ScreenHeight() );

        // initialize sound manager
        SoundManager::getInstance()->readSounds( "sounds.xml" );
}

GuiManager::~GuiManager( )
{
        freeScreens () ;

        std::for_each( this->fonts.begin (), this->fonts.end (), isomot::DeleteIt() );
        this->fonts.clear();

        delete this->languageManager;
}

GuiManager* GuiManager::getInstance ()
{
        if ( instance == nilPointer )
        {
                instance = new GuiManager();
                instance->readPreferences ();

                Screen::refreshBackground ();
        }

        return instance;
}

void GuiManager::readPreferences ()
{
        if ( ! preferencesRead )
        {
                preferencesRead = GameManager::readPreferences( isomot::homePath() + "preferences.xml" ) ;

                if ( ! preferencesRead && instance != nilPointer )
                {
                        instance->setLanguage( "en_US" );
                }
        }
}

void GuiManager::begin ()
{
        // show list of languages
        smartptr< CreateLanguageMenu > languageMenu( new CreateLanguageMenu( this->picture ) );
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
                Screen::barWipeHorizontally( this->activeScreen, newScreen, dive );
                this->activeScreen = newScreen;
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
        return isomot::sharePath() + GameManager::getInstance()->getChosenGraphicSet() + pathSeparator ;
}

void GuiManager::initAllegro()
{
        allegro::init ();

        // 8 bits for each of three colors with 8 bits for alpha channel
        allegro::setDefaultColorDepth( 32 );

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

        bool switched = false;

        if ( this->atFullScreen )
                switched = allegro::switchToFullscreenVideo( isomot::ScreenWidth(), isomot::ScreenHeight() ) ;
        else
                switched = allegro::switchToWindowedVideo( isomot::ScreenWidth(), isomot::ScreenHeight() ) ;

        if ( ! switched ) toggleFullScreenVideo ();

        // initialize handler of keyboard events
        allegro::initKeyboardHandler ();
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
                if ( this->atFullScreen ) allegro::switchToFullscreenVideo();
                else    allegro::switchToWindowedVideo();
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
        std::string pathToTextInGameData = isomot::sharePath() + "text" + pathSeparator ;
        this->languageManager = new LanguageManager( pathToTextInGameData + language + ".xml", pathToTextInGameData + "en_US.xml" );
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
