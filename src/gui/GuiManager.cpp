
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
        this->allegroSetup();

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

        set_window_title ( nameOfWindow.c_str () );

        // create fonts to use in game

        std::string nameOfFontFile = isomot::sharePath() + "font.png" ;
        std::cout << "reading from file \"" << nameOfFontFile << "\" to create fonts used in game" << std::endl ;

        allegro::Pict * pictureOfFont = allegro::loadPNG( isomot::pathToFile( nameOfFontFile.c_str () ) );
        if ( pictureOfFont != nilPointer )
        {
                addFont( new Font( "white.regular", pictureOfFont, Color::whiteColor() ) );
                addFont( new Font( "white.big", pictureOfFont, Color::whiteColor(), true ) );

                addFont( new Font( "orange.regular", pictureOfFont, Color::orangeColor() ) );
                addFont( new Font( "cyan.regular", pictureOfFont, Color::cyanColor() ) );
                addFont( new Font( "yellow.regular", pictureOfFont, Color::yellowColor() ) );

                addFont( new Font( "orange.big", pictureOfFont, Color::orangeColor(), true ) );
                addFont( new Font( "cyan.big", pictureOfFont, Color::cyanColor(), true ) );
                addFont( new Font( "yellow.big", pictureOfFont, Color::yellowColor(), true ) );

                addFont( new Font( "green.regular", pictureOfFont, Color::greenColor(), false ) );
                addFont( new Font( "green.big", pictureOfFont, Color::greenColor(), true ) );
                addFont( new Font( "magenta.regular", pictureOfFont, Color::magentaColor(), false ) );
                addFont( new Font( "magenta.big", pictureOfFont, Color::magentaColor(), true ) );

                allegro::binPicture( pictureOfFont );
        }
        else
        {
                std::cerr << "oops, can’t get letters of fonts from file \"" << nameOfFontFile << "\"" << std::endl ;
        }

        // create image to draw interface
        this->picture = allegro::createPicture( isomot::ScreenWidth(), isomot::ScreenHeight() );

        // initialize sound manager
        SoundManager::getInstance()->readListOfSounds( "sounds.xml" );
}

GuiManager::~GuiManager( )
{
        freeScreens () ;

        std::for_each( this->fonts.begin (), this->fonts.end (), isomot::DeleteObject() );
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
                this->activeScreen->draw( this->picture );

                if ( keypressed() )
                {
                        this->activeScreen->handleKey( readkey() );
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
                activeScreen->draw( this->picture );
        }
}

std::string GuiManager::getPathToPicturesOfGui ()
{
        return isomot::sharePath() + GameManager::getInstance()->getChosenGraphicSet() + pathSeparator ;
}

void GuiManager::allegroSetup()
{
        allegro::init ();

        // 8 bits for each of three colors with 8 bits for alpha channel
        set_color_depth( 32 );

        // fill list of screen’s sizes

        sizesOfScreen.insert( std::pair< unsigned int, unsigned int >( 640, 480 ) );

        /*  if ( allegro::setGraphicsMode( GFX_AUTODETECT_FULLSCREEN, 800, 600 ) )
                sizesOfScreen.insert( std::pair< unsigned int, unsigned int >( 800, 600 ) ); */

        sizesOfScreen.insert( std::pair< unsigned int, unsigned int >( 800, 600 ) );
        sizesOfScreen.insert( std::pair< unsigned int, unsigned int >( 1024, 576 ) );
        sizesOfScreen.insert( std::pair< unsigned int, unsigned int >( 1024, 600 ) );
        sizesOfScreen.insert( std::pair< unsigned int, unsigned int >( 1024, 768 ) );
        sizesOfScreen.insert( std::pair< unsigned int, unsigned int >( 1280, 720 ) );
        sizesOfScreen.insert( std::pair< unsigned int, unsigned int >( 1280, 1024 ) );

        // switch to chosen size of screen

        int magicCard = this->atFullScreen ? GFX_AUTODETECT_FULLSCREEN : GFX_AUTODETECT_WINDOWED ;

        if ( ! allegro::setGraphicsMode( magicCard, isomot::ScreenWidth(), isomot::ScreenHeight() ) )
                toggleFullScreenVideo ();

#ifdef __WIN32
        // when application loses focus, the game will continue in background
        // because there are threads that will continue even when the main thread pauses
        set_display_switch_mode( SWITCH_BACKGROUND );
#endif

        // install handler of keyboard events
        install_keyboard ();
}

bool GuiManager::isAtFullScreen ()
{
        return this->atFullScreen;
}

void GuiManager::toggleFullScreenVideo ()
{
        int magicCardToggled = this->atFullScreen ? GFX_AUTODETECT_WINDOWED : GFX_AUTODETECT_FULLSCREEN ;

        if ( allegro::setGraphicsMode( magicCardToggled, isomot::ScreenWidth(), isomot::ScreenHeight() ) )
        {
                this->atFullScreen = ! this->atFullScreen ;

                fprintf( stdout, "video is now %s\n", ( this->atFullScreen ? "at full screen" : "in window" ) );
                milliSleep( 80 );
                redraw ();
        }
        else
        {
                int magicCard = this->atFullScreen ? GFX_AUTODETECT_FULLSCREEN : GFX_AUTODETECT_WINDOWED ;
                allegro::setGraphicsMode( magicCard, isomot::ScreenWidth(), isomot::ScreenHeight() ) ;
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
        for (  std::list< Font * >::iterator i = fonts.begin (); i != fonts.end (); ++i )
        {
                if ( ( *i )->getName() == color + "." + family )
                {
                        return *i ;
                }
        }

        return nilPointer ;
}
