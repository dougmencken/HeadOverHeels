
#include "GuiManager.hpp"
#include "Ism.hpp"
#include "ConfigurationManager.hpp"
#include "LanguageManager.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"
#include "GameManager.hpp"
#include "Color.hpp"
#include "Screen.hpp"
#include "Menu.hpp"
#include "Picture.hpp"
#include "Label.hpp"
#include "CreateLanguageMenu.hpp"

#ifdef DEBUG
/* #  define DEBUG_GUI       1 */
#endif

using gui::GuiManager;
using gui::Screen;
using gui::Font;
using gui::LanguageManager;
using gui::ConfigurationManager;
using isomot::InputManager;
using isomot::SoundManager;
using isomot::ScreenWidth;
using isomot::ScreenHeight;


GuiManager* GuiManager::instance = 0;

GuiManager::GuiManager( ) :
        screen( 0 ),
        language( "" ),
        languageManager( 0 ),
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

        nameOfWindow = nameOfWindow + " " + version;
#endif

        set_window_title ( nameOfWindow.c_str () );

        std::string pathToFont = isomot::sharePath();

        // create fonts to use in game

        addFont( new Font( "white.regular", pathToFont + "font.png", Color::whiteColor() ) );
        addFont( new Font( "white.big", pathToFont + "font.png", Color::whiteColor(), true ) );

        addFont( new Font( "orange.regular", pathToFont + "font.png", Color::orangeColor() ) );
        addFont( new Font( "cyan.regular", pathToFont + "font.png", Color::cyanColor() ) );
        addFont( new Font( "yellow.regular", pathToFont + "font.png", Color::yellowColor() ) );

        addFont( new Font( "orange.big", pathToFont + "font.png", Color::orangeColor(), true ) );
        addFont( new Font( "cyan.big", pathToFont + "font.png", Color::cyanColor(), true ) );
        addFont( new Font( "yellow.big", pathToFont + "font.png", Color::yellowColor(), true ) );

        addFont( new Font( "green.regular", pathToFont + "font.png", Color::greenColor(), false ) );
        addFont( new Font( "green.big", pathToFont + "font.png", Color::greenColor(), true ) );
        addFont( new Font( "magenta.regular", pathToFont + "font.png", Color::magentaColor(), false ) );
        addFont( new Font( "magenta.big", pathToFont + "font.png", Color::magentaColor(), true ) );

        // create image to draw interface
        this->picture = create_bitmap_ex( 32, ScreenWidth, ScreenHeight );

        configurationManager = new ConfigurationManager( isomot::homePath() + "preferences.xml" );

        // initialize sound manager
        SoundManager::getInstance()->readListOfSounds( "sounds.xml" );
}

GuiManager::~GuiManager( )
{
        freeScreens () ;

        std::for_each( this->fonts.begin (), this->fonts.end (), isomot::DeleteObject() );
        this->fonts.clear();

        delete this->languageManager;
        delete this->configurationManager;
}

GuiManager* GuiManager::getInstance ()
{
        if ( instance == 0 )
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
                preferencesRead = configurationManager->read() ;

                if ( ! preferencesRead && instance != 0 )
                {
                        instance->setLanguage( "en_US" );
                }
        }
}

void GuiManager::begin ()
{
        // show list of languages
        std::auto_ptr< CreateLanguageMenu > languageMenu( new CreateLanguageMenu( this->picture ) );
        languageMenu->doIt ();

        // draw user interface and handle keys
        while ( this->active )
        {
                this->screen->draw( this->picture );

                if ( keypressed() )
                {
                        this->screen->handleKey( readkey() );
                }

                sleep( 30 );
        }
}

void GuiManager::changeScreen( Screen* newScreen, bool dive )
{
        if ( newScreen == 0 ) return ;

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
                                 ( escape != 0 ? escape->getNameOfAction().c_str () : "none" ) );
        }

        if ( newScreen )
        {
                Action* actionOfNewScreen = newScreen->getActionOfScreen () ;
                Action* escapeOfNewScreen = newScreen->getEscapeAction () ;
                fprintf( stdout, ". new screen is for action \" %s \" with escape action \" %s \"\n",
                                 actionOfNewScreen->getNameOfAction().c_str (),
                                 ( escapeOfNewScreen != 0 ? escapeOfNewScreen->getNameOfAction().c_str () : "none" )
                ) ;
        }

# endif

        if ( listOfScreens.find( newScreen->getActionOfScreen()->getNameOfAction() ) != listOfScreens.end () )
        {
                Screen::barWipeHorizontally( this->screen, newScreen, dive );
                this->screen = newScreen;
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
        if ( action == 0 )
        {
                std::cerr << "screen for nil action is nil screen" << std::endl ;
                return 0 ;
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
        if ( ( this->active ) && ( this->screen != 0 ) )
        {
                screen->draw( this->picture );
        }
}

std::string GuiManager::getPathToPicturesOfGui ()
{
        return isomot::sharePath() + isomot::GameManager::getInstance()->getChosenGraphicSet() + pathSeparator ;
}

void GuiManager::allegroSetup()
{
        allegro_init ();

        // 8 bits for each of three colors with 8 bits for alpha channel
        set_color_depth( 32 );

        int magicCard = this->atFullScreen ? GFX_AUTODETECT_FULLSCREEN : GFX_AUTODETECT_WINDOWED ;
        set_gfx_mode( magicCard, ScreenWidth, ScreenHeight, 0, 0 );

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
        set_gfx_mode( magicCardToggled, ScreenWidth, ScreenHeight, 0, 0 );
        this->atFullScreen = ! this->atFullScreen ;

        fprintf( stdout, "video is now %s\n", ( this->atFullScreen ? "at full screen" : "in window" ) );
        sleep( 80 );
        redraw ();
}

void GuiManager::assignLanguage( const std::string& language )
{
        if ( this->languageManager != 0 )
        {
                delete this->languageManager;
                this->languageManager = 0;
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

        return 0;
}
