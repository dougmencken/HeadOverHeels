
#include "GuiManager.hpp"
#include "Ism.hpp"
#include "FontManager.hpp"

#include "ConfigurationManager.hpp"
#include "LanguageManager.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"
#include "GameManager.hpp"
#include "Font.hpp"
#include "Screen.hpp"
#include "Menu.hpp"
#include "Picture.hpp"
#include "Label.hpp"
#include "CreateLanguageMenu.hpp"

#define DEBUG_GUI       0

using gui::GuiManager;
using gui::Screen;
using gui::LanguageManager;
using gui::ConfigurationManager;
using isomot::InputManager;
using isomot::SoundManager;


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
        nameOfWindow = nameOfWindow + " v" + PACKAGE_VERSION;
#endif
        set_window_title ( nameOfWindow.c_str () );

        std::string pathToFont = isomot::sharePath();

        // every font used in the game is created here

        gui::FontManager::getInstance()->createFont( "regular-white", pathToFont + "font.png", makecol( 255, 255, 255 ) );
        gui::FontManager::getInstance()->createFont( "big-white", pathToFont + "font.png", makecol( 255, 255, 255 ), true );

        gui::FontManager::getInstance()->createFont( "regular-orange", pathToFont + "font.png", makecol( 239, 129, 0 ) );
        gui::FontManager::getInstance()->createFont( "regular-cyan", pathToFont + "font.png", makecol( 0, 228, 231 ) );
        gui::FontManager::getInstance()->createFont( "regular-yellow", pathToFont + "font.png", makecol( 255, 255, 50 ) );

        gui::FontManager::getInstance()->createFont( "big-orange", pathToFont + "font.png", makecol( 239, 129, 0 ), true );
        gui::FontManager::getInstance()->createFont( "big-cyan", pathToFont + "font.png", makecol( 0, 228, 231 ), true );
        gui::FontManager::getInstance()->createFont( "big-yellow", pathToFont + "font.png", makecol( 255, 255, 50 ), true );

        // create image to draw interface
        this->picture = create_bitmap_ex( 32, 640, 480 );

        configurationManager = new ConfigurationManager( isomot::homePath() + "preferences.xml" );

        // initialize sound manager
        SoundManager::getInstance()->readListOfSounds( "sounds.xml" );
}

GuiManager::~GuiManager( )
{
        freeScreens () ;

        delete this->languageManager;
        delete this->configurationManager;
}

GuiManager* GuiManager::getInstance ()
{
        if ( instance == 0 )
        {
                instance = new GuiManager();
                instance->readPreferences ();
        }

        return instance;
}

void GuiManager::readPreferences ()
{
        if ( ! preferencesRead )
        {
                configurationManager->read() ;
                preferencesRead = true ;
        }
}

void GuiManager::begin ()
{
        // Se presenta la lista de lenguas disponibles
        // Present the list of languages
        std::auto_ptr< CreateLanguageMenu > languageMenu( new CreateLanguageMenu( this->picture ) );
        languageMenu->doIt ();

        // Dibuja la interfaz de usuario y procesa la pulsación de teclas
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

void GuiManager::changeScreen( Screen* newScreen )
{
        assert( newScreen );

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
                this->screen = newScreen;
                redraw() ;
        }
        else
        {
                fprintf( stderr, "there's no screen for action \" %s \", please create it before use\n",
                                 newScreen->getActionOfScreen()->getNameOfAction().c_str () );
        }
}

Screen * GuiManager::findOrCreateScreenForAction ( Action* action, BITMAP* picture )
{
        assert( action );
        assert( picture );

        std::string nameOfAction = action->getNameOfAction() ;

        if ( listOfScreens.find( nameOfAction ) != listOfScreens.end () )
        {
                Screen * theScreen = listOfScreens[ nameOfAction ];
                fprintf( stdout, "here's existing screen for action \" %s \"\n", nameOfAction.c_str () );
                return theScreen;
        }

        fprintf( stdout, "going to create new screen for action \" %s \"\n", nameOfAction.c_str () );
        Screen * newScreen = new Screen( picture, action );
        listOfScreens[ nameOfAction ] = newScreen;
        return newScreen;
}

void GuiManager::freeScreens ()
{
        listOfScreens.clear() ;
        fprintf( stdout, "now list of screens is empty\n" );
}

void GuiManager::refreshScreens ()
{
        for (  std::map< std::string, Screen * >::iterator i = listOfScreens.begin (); i != listOfScreens.end (); ++i )
        {
                if ( i->second )
                {
                        i->second->refreshPictures () ;
                        i->second->refreshBackground () ;
                }
        }
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
        return isomot::sharePath() + isomot::GameManager::getInstance()->getChosenGraphicSet() + "/" + "gui.png" + "/" ;
}

void GuiManager::allegroSetup()
{
        allegro_init ();

        // 8 bits for each of three colors with 8 bits for alpha channel
        set_color_depth( 32 );

        int magicCard = this->atFullScreen ? GFX_AUTODETECT_FULLSCREEN : GFX_AUTODETECT_WINDOWED ;
        set_gfx_mode( magicCard, 640, 480, 0, 0 );

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
        set_gfx_mode( magicCardToggled, 640, 480, 0, 0 );
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
        std::string pathToTextInGameData = isomot::sharePath() + "text/";
        this->languageManager = new LanguageManager( pathToTextInGameData + language + ".xml", pathToTextInGameData + "en_US.xml" );
}
