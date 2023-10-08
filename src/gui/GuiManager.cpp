
#include "GuiManager.hpp"
#include "Ism.hpp"
#include "LanguageManager.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"
#include "GameManager.hpp"
#include "Color.hpp"
#include "Menu.hpp"
#include "PictureWidget.hpp"
#include "Label.hpp"
#include "CreateLanguageMenu.hpp"

#include "WrappersAllegro.hpp"

#include <algorithm>

#if defined( DEBUG ) && DEBUG
# define DEBUG_GUI_SCREENS      0
#endif


namespace gui
{

GuiManager * GuiManager::instance = nilPointer ;

GuiManager::GuiManager( ) :
        activeScreen( nilPointer ),
        language( "" ),
        languageManager( nilPointer ),
        active( true ),
        atFullScreen( false )
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

        // initialize sound manager
        iso::SoundManager::getInstance().readSounds( "sounds.xml" );
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

void GuiManager::begin ()
{
        // show the list of languages
        autouniqueptr< CreateLanguageMenu > languageMenu( new CreateLanguageMenu() );
        if ( languageMenu != nilPointer )
                languageMenu->doIt ();
        else
                std::cerr << "can't create the language menu, the first screen of user interface" << std::endl ;

        // draw the user interface and handle keys
        while ( this->active )
        {
                activeScreen->draw ();

                if ( allegro::areKeypushesWaiting() )
                {
                        activeScreen->handleKey( allegro::nextKey() );
                }

                milliSleep( 30 );
        }
}

void GuiManager::changeScreen( const Screen& newScreen, bool dive )
{
# if  defined( DEBUG_GUI_SCREENS )  &&  DEBUG_GUI_SCREENS

        if ( listOfScreens.size () > 0 )
        {
                fprintf ( stdout, " + +  s c r e e n z \n" ) ;
                for (  std::map< std::string, ScreenPtr >::const_iterator i = listOfScreens.begin (); i != listOfScreens.end (); ++i )
                {
                        if ( i->second != nilPointer )
                        {
                                Action * action = i->second->getActionOfScreen () ;
                                std::cout << "   screen @ " << i->second.tostring() ;
                                std::cout << " for action @ " << util::pointer2string( action ) ;
                                std::cout << " \" " << action->getNameOfAction() << " \"" << std::endl ;
                        }
                }
                fprintf ( stdout, " - -  s c r e e n z \n" ) ;
        }

        if ( activeScreen != nilPointer )
        {
                Action* escape = activeScreen->getEscapeAction () ;
                fprintf( stdout, ". previous screen was for action \" %s \" with escape action \" %s \"\n",
                                 activeScreen->getActionOfScreen ()->getNameOfAction().c_str (),
                                 ( escape != nilPointer ? escape->getNameOfAction().c_str () : "none" ) );
        }

        Action* escapeOfNewScreen = newScreen.getEscapeAction () ;
        fprintf( stdout, ". screen to change to is for action \" %s \" with escape action \" %s \"\n",
                         newScreen.getActionOfScreen ()->getNameOfAction().c_str (),
                         ( escapeOfNewScreen != nilPointer ? escapeOfNewScreen->getNameOfAction().c_str () : "none" )
        ) ;

# endif

        if ( listOfScreens.find( newScreen.getActionOfScreen()->getNameOfAction() ) != listOfScreens.end () )
        {
                if ( activeScreen != nilPointer &&
                        ( activeScreen->getActionOfScreen() == nilPointer ||
                                activeScreen->getActionOfScreen()->getNameOfAction() != "CreatePlanetsScreen" ) )
                {
                        Screen::barWipeHorizontally( *activeScreen, newScreen, dive );
                }

                setActiveScreen( ScreenPtr( & const_cast< Screen& >( newScreen ) ) );
                redraw() ;
        }
        else
        {
                fprintf( stderr, "there’s no screen for action \" %s \", please create it before use\n",
                                 newScreen.getActionOfScreen()->getNameOfAction().c_str () );
        }
}

ScreenPtr GuiManager::findOrCreateScreenForAction ( Action* action )
{
        if ( action == nilPointer )
        {
                std::cerr << "screen for nil action is nil screen" << std::endl ;
                return ScreenPtr () ;
        }

        std::string nameOfAction = action->getNameOfAction() ;

        if ( listOfScreens.find( nameOfAction ) != listOfScreens.end () )
        {
                ScreenPtr theScreen = listOfScreens[ nameOfAction ];
                std::cout << "here’s existing screen for action \" " << nameOfAction << " \"" << std::endl ;
                return theScreen;
        }

        std::cout << "going to create new screen for action \" " << nameOfAction << " \"" << std::endl ;
        ScreenPtr newScreen( new Screen( action ) );
        listOfScreens[ nameOfAction ] = newScreen ;
        return newScreen;
}

void GuiManager::freeScreens ()
{
        listOfScreens.clear() ;
        setActiveScreen( ScreenPtr () );
}

void GuiManager::refreshScreens ()
{
        for (  std::map< std::string, ScreenPtr >::iterator i = listOfScreens.begin (); i != listOfScreens.end (); ++i )
        {
                if ( i->second != nilPointer )
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
                activeScreen->draw ();
        }
}

std::string GuiManager::getPathToThesePictures ()
{
        return iso::sharePath() + iso::GameManager::getInstance().getChosenGraphicsSet() ;
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

Font* GuiManager::getOrCreateFontByFamilyAndColor ( const std::string& family, const std::string& color )
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

        IF_DEBUG( std::cout << "making font with family \"" << familyToLook << "\" and color \"" << colorToLook << "\"" << std::endl )

        Font* newFont = new Font( colorToLook + "." + familyToLook, colorToLook, family == "big" );
        addFont( newFont );

        return newFont ;
}

}
