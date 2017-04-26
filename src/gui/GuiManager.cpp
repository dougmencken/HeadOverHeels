
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
#include "Icon.hpp"
#include "Label.hpp"
#include "actions/CreateLanguageMenu.hpp"

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
        languageManager( 0 ),
        active( true )
{
        // Inicialización de Allegro
        this->allegroSetup();

        // Título de la ventana
        std::string nameOfWindow = "Head over Heels";
#ifdef PACKAGE_VERSION
        nameOfWindow = nameOfWindow + " v" + PACKAGE_VERSION;
#endif
        set_window_title ( nameOfWindow.c_str () );

        loadImages ();

        std::string pathToFont = isomot::sharePath();

        // Every font used in the game is created here

        gui::FontManager::getInstance()->createFont( "regular-white", pathToFont + "font.png", makecol( 255, 255, 255 ) );
        gui::FontManager::getInstance()->createFont( "big-white", pathToFont + "font.png", makecol( 255, 255, 255 ), true );

        gui::FontManager::getInstance()->createFont( "regular-orange", pathToFont + "font.png", makecol( 239, 129, 0 ) );
        gui::FontManager::getInstance()->createFont( "regular-cyan", pathToFont + "font.png", makecol( 0, 228, 231 ) );
        gui::FontManager::getInstance()->createFont( "regular-yellow", pathToFont + "font.png", makecol( 255, 255, 50 ) );

        gui::FontManager::getInstance()->createFont( "big-orange", pathToFont + "font.png", makecol( 239, 129, 0 ), true );
        gui::FontManager::getInstance()->createFont( "big-cyan", pathToFont + "font.png", makecol( 0, 228, 231 ), true );
        gui::FontManager::getInstance()->createFont( "big-yellow", pathToFont + "font.png", makecol( 255, 255, 50 ), true );

        // Se crea la imagen donde se dibujará la interfaz gráfica
        this->picture = create_bitmap_ex( 32, 640, 480 );

        // Se crea el gestor de configuración y se cargan los datos del disco
        configurationManager = new ConfigurationManager( isomot::homePath() + "configuration.xml" );
        configurationManager->read();

        std::string language = configurationManager->getLanguage() ;
        if ( language.compare( "en_UK" ) == 0 )
                language = "en_US"; // for compatibility

        assignLanguage( language );

        // Se comunica al gestor de entradas qué teclas
        InputManager* inputManager = InputManager::getInstance();
        for ( size_t i = 0; i < InputManager::numberOfKeys; i++ )
        {
                inputManager->changeUserKey( InputManager::namesOfKeys[ i ], configurationManager->getKey( InputManager::namesOfKeys[ i ] ) );
        }

        // Se inicializa el gestor de sonido cargando los sonidos
        SoundManager::getInstance()->readListOfSounds( "sounds.xml" );
}

GuiManager::~GuiManager( )
{
        freeImages () ;

        freeScreens () ;

        delete this->languageManager;
        delete this->configurationManager;
}

GuiManager* GuiManager::getInstance ()
{
        if ( instance == 0 )
        {
                instance = new GuiManager();
        }

        return instance;
}

void GuiManager::loadImages ()
{
        std::string gfxSet = isomot::GameManager::getInstance()->getChosenGraphicSet() ;
        fprintf ( stdout, "get images for user interface from set \"%s\"\n", gfxSet.c_str () );
        std::string pathToGraphics = isomot::sharePath() + gfxSet + "/";

        BITMAP* bitmap = 0;

        bitmap = load_png( ( pathToGraphics + "gui.png/background.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "background" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/blacktooth.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "blacktooth" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/byblos.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "byblos" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/crown.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "crown" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/donuts.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "donuts" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/egyptus.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "egyptus" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/grey-crown.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "grey-crown" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/grey-donuts.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "grey-donuts" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/grey-handbag.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "grey-handbag" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/grey-head.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "grey-head" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/grey-heels.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "grey-heels" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/grey-horn.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "grey-horn" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/handbag.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "handbag" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/head.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "head" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/heels.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "heels" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/horn.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "horn" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/penitentiary.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "penitentiary" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/safari.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "safari" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/option.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "option" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/selected-option.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "selected-option" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/selected-option-mini.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "selected-option-mini" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/jail-frame.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "jail-frame" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/blacktooth-frame.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "blacktooth-frame" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/market-frame.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "market-frame" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/themoon-frame.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "themoon-frame" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/egyptus-frame.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "egyptus-frame" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/penitentiary-frame.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "penitentiary-frame" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/safari-frame.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "safari-frame" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/byblos-frame.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "byblos-frame" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/shield.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "shield" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/shield.gray.png" ).c_str (), 0 );
        if ( bitmap == 0 )
                bitmap = load_png( ( pathToGraphics + "gui.png/shield.png" ).c_str (), 0 );
        images[ "shield.gray" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/high-jumps.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "high-jumps" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/high-jumps.gray.png" ).c_str (), 0 );
        if ( bitmap == 0 )
                bitmap = load_png( ( pathToGraphics + "gui.png/high-jumps.png" ).c_str (), 0 );
        images[ "high-jumps.gray" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/high-speed.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "high-speed" ] = bitmap;

        bitmap = load_png( ( pathToGraphics + "gui.png/high-speed.gray.png" ).c_str (), 0 );
        if ( bitmap == 0 )
                bitmap = load_png( ( pathToGraphics + "gui.png/high-speed.png" ).c_str (), 0 );
        images[ "high-speed.gray" ] = bitmap;
}

void GuiManager::freeImages ()
{
        for ( std::map< std::string, BITMAP* >::iterator i = this->images.begin (); i != this->images.end (); ++i )
        {
                delete ( *i ).second;
        }

        this->images.clear();
}

void GuiManager::reloadImages ()
{
        freeImages ();
        loadImages ();
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
                refresh() ;
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

void GuiManager::refresh()
{
        if ( ( this->active ) && ( this->screen != 0 ) )
        {
                screen->draw( this->picture );
        }
}

BITMAP* GuiManager::findImage( const std::string& name )
{
        std::map< std::string, BITMAP* >::iterator i  = images.find( name );
        return ( i != images.end () ? ( *i ).second : 0 );
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

void GuiManager::allegroSetup()
{
        allegro_init ();

        // 8 bits for each of three colors with 8 bits for alpha channel
        set_color_depth( 32 );

        // try 640 pixels x 480 pixels at full screen
        // if that can’t be done use 640 pixels x 480 pixels in window
        this->atFullScreen = true;
        if ( set_gfx_mode( GFX_AUTODETECT_FULLSCREEN, 640, 480, 0, 0 ) != 0 )
        {
                fprintf( stdout, "can't get 640 pixels x 480 pixels at full screen\n" );
                this->atFullScreen = false;
                set_gfx_mode( GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0 );
        }

#ifdef __WIN32
        // When this application loses focus the game will continue in background
        // because there are threads that will continue even when the main thread pauses
        set_display_switch_mode( SWITCH_BACKGROUND );
#endif

        // Instala el controlador de eventos del teclado
        install_keyboard ();
}

bool GuiManager::isAtFullScreen ()
{
        return this->atFullScreen;
}

void GuiManager::toggleFullScreenVideo ()
{
        int magicCard = GFX_AUTODETECT_FULLSCREEN;
        if ( this->atFullScreen )
                magicCard = GFX_AUTODETECT_WINDOWED;

        set_gfx_mode( magicCard, 640, 480, 0, 0 );
        this->atFullScreen = ! this->atFullScreen ;
        fprintf( stdout, "video is now %s\n", ( this->atFullScreen ? "at full screen" : "in window" ) );
        sleep( 80 );
        refresh ();
}

ConfigurationManager* GuiManager::getConfigurationManager() const
{
        return this->configurationManager;
}

LanguageManager* GuiManager::getLanguageManager() const
{
        return this->languageManager;
}
