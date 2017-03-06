
#include "GuiManager.hpp"
#include "Ism.hpp"
#include "FontManager.hpp"

#include "ConfigurationManager.hpp"
#include "LanguageManager.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"
#include "Font.hpp"
#include "Screen.hpp"
#include "Menu.hpp"
#include "Icon.hpp"
#include "Label.hpp"
#include "actions/CreateMainMenu.hpp"
#include "actions/CreateLanguageMenu.hpp"

using gui::GuiManager;
using gui::LanguageManager;
using gui::ConfigurationManager;
using isomot::InputManager;
using isomot::SoundManager;


GuiManager* GuiManager::instance = 0;

GuiManager::GuiManager( )
: screen( 0 ),
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

        // Ruta absoluta a los datos
        std::string path = isomot::sharePath();

        // Carga y almacena las imágenes

        BITMAP* bitmap = 0;

        bitmap = load_png( ( path + "gfx/gui.png/background.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "background" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/blacktooth.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "blacktooth" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/byblos.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "byblos" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/crown.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "crown" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/donuts.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "donuts" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/egyptus.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "egyptus" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/grey-crown.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "grey-crown" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/grey-donuts.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "grey-donuts" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/grey-handbag.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "grey-handbag" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/grey-head.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "grey-head" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/grey-heels.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "grey-heels" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/grey-horn.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "grey-horn" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/handbag.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "handbag" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/head.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "head" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/heels.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "heels" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/horn.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "horn" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/penitentiary.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "penitentiary" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/safari.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "safari" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/option.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "option" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/selected-option.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "selected-option" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/selected-option-mini.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "selected-option-mini" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/jail-frame.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "jail-frame" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/blacktooth-frame.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "blacktooth-frame" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/market-frame.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "market-frame" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/themoon-frame.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "themoon-frame" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/egyptus-frame.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "egyptus-frame" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/penitentiary-frame.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "penitentiary-frame" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/safari-frame.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "safari-frame" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/byblos-frame.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "byblos-frame" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/shield.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "shield" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/shield.gray.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "shield.gray" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/high-jumps.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "high-jumps" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/high-jumps.gray.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "high-jumps.gray" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/high-speed.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "high-speed" ] = bitmap;

        bitmap = load_png( ( path + "gfx/gui.png/high-speed.gray.png" ).c_str (), 0 );
        assert( bitmap );
        images[ "high-speed.gray" ] = bitmap;

        // Se crean, a través del gesto de fuentes, todos los tipos de letra empleados en el juego
        gui::FontManager::getInstance()->createFont( "regular-white", path + "gfx/font.png", makecol( 255, 255, 255 ) );
        gui::FontManager::getInstance()->createFont( "big-white", path + "gfx/font.png", makecol( 255, 255, 255 ), true );
        gui::FontManager::getInstance()->createFont( "regular-orange", path + "gfx/font.png", makecol( 239, 129, 0 ) );
        gui::FontManager::getInstance()->createFont( "regular-cyan", path + "gfx/font.png", makecol( 0, 228, 231 ) );
        gui::FontManager::getInstance()->createFont( "regular-yellow", path + "gfx/font.png", makecol( 255, 255, 50 ) );
        gui::FontManager::getInstance()->createFont( "big-orange", path + "gfx/font.png", makecol( 239, 129, 0 ), true );
        gui::FontManager::getInstance()->createFont( "big-cyan", path + "gfx/font.png", makecol( 0, 228, 231 ), true );
        gui::FontManager::getInstance()->createFont( "big-yellow", path + "gfx/font.png", makecol( 255, 255, 50 ), true );

        // Se crea la imagen donde se dibujará la interfaz gráfica
        this->picture = create_bitmap_ex( 32, 640, 480 );

        // Se crea el gestor de configuración y se cargan los datos del disco
        this->configurationManager = new ConfigurationManager( isomot::homePath() + "configuration.xml" );
        this->configurationManager->read();

        // No hay lengua definida todavía
        this->languageManager = 0;

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
        for ( std::map< std::string, BITMAP* >::iterator i = this->images.begin (); i != this->images.end (); ++i )
        {
                delete ( *i ).second;
        }

        this->images.clear();

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

void GuiManager::enter ()
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

void GuiManager::changeScreen( Screen* screen )
{
        if ( this->screen != 0 )
        {
                delete this->screen;
        }

        this->screen = screen;
}

void GuiManager::refresh()
{
        if ( this->active )
        {
                this->screen->draw( this->picture );
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

        std::string pathToTextInGameData = isomot::sharePath() + "text/";
        this->languageManager = new LanguageManager( pathToTextInGameData + language + ".xml", pathToTextInGameData + "en_US.xml" );
}

void GuiManager::allegroSetup()
{
        // Inicializa Allegro
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
