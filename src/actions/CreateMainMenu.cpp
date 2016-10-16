
#include "CreateMainMenu.hpp"
#include "GuiManager.hpp"
#include "LanguageManager.hpp"
#include "SoundManager.hpp"
#include "Font.hpp"
#include "Screen.hpp"
#include "Menu.hpp"

#include "Icon.hpp"
#include "Label.hpp"
#include "CreatePlanetsScreen.hpp"
#include "CreateKeyboardMenu.hpp"
#include "CreateAudioMenu.hpp"
#include "CreateListOfSavedGames.hpp"
#include "ShowAuthors.hpp"
#include "ExitApplication.hpp"

using gui::CreateMainMenu;
using isomot::SoundManager;


/* static */
void CreateMainMenu::placeHeadAndHeels( Screen* screen, bool iconsToo, bool copyrightsToo )
{
        Label* label = 0;
        LanguageText* langString = 0;
        LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();

        langString = languageManager->findLanguageString( "jon" );
        label = new Label( langString->getX(), langString->getY(), langString->getText(), "regular", "multicolor" );
        screen->addWidget( label );

        langString = languageManager->findLanguageString( "ritman" );
        label = new Label( langString->getX(), langString->getY(), langString->getText(), "regular", "multicolor" );
        screen->addWidget( label );

        langString = languageManager->findLanguageString( "bernie" );
        label = new Label( langString->getX(), langString->getY(), langString->getText(), "regular", "multicolor" );
        screen->addWidget( label );

        langString = languageManager->findLanguageString( "drummond" );
        label = new Label( langString->getX(), langString->getY(), langString->getText(), "regular", "multicolor" );
        screen->addWidget( label );

        label = new Label( 200, 24, "HEAD" );
        label->changeFont( "big", "yellow" );
        screen->addWidget( label );

        label = new Label( 280, 45, "OVER", "regular", "multicolor" );
        screen->addWidget( label );

        label = new Label( 360, 24, "HEELS" );
        label->changeFont( "big", "yellow" );
        screen->addWidget( label );

        if ( iconsToo )
        {
                // Icono: Head
                screen->addWidget( new Icon( 206, 84, GuiManager::getInstance()->findImage( "head" ) ) );
                // Icono: Heels
                screen->addWidget( new Icon( 378, 84, GuiManager::getInstance()->findImage( "heels" ) ) );
        }

        if ( copyrightsToo )
        {
                // Etiqueta fija: (C) 1987 OCEAN SOFTWARE LTD
                langString = languageManager->findLanguageString( "ocean" );
                label = new Label( langString->getX(), langString->getY(), langString->getText() );
                label->changeFont( "regular", "cyan" );
                screen->addWidget( label );

                // Etiqueta fija: (C) 2008 JORGE RODRÍGUEZ SANTOS
                langString = languageManager->findLanguageString( "jorge" );
                label = new Label( langString->getX(), langString->getY(), langString->getText() );
                label->changeFont( "regular", "orange" );
                screen->addWidget( label );
        }
}


CreateMainMenu::CreateMainMenu( BITMAP* picture )
: Action(),
  where( picture )
{

}

void CreateMainMenu::doIt ()
{
        if ( ! SoundManager::getInstance()->isPlayingOgg( "music/MainTheme.ogg" ) )
        {
                SoundManager::getInstance()->stopOgg();
                SoundManager::getInstance()->playOgg( "music/MainTheme.ogg", /* loop */ true );
        }

        Screen* screen = new Screen( 0, 0, this->where );
        screen->setBackground( GuiManager::getInstance()->findImage( "background" ) );

        CreateMainMenu::placeHeadAndHeels( screen, true, true );

        Label* label = 0;
        LanguageText* langString = 0;
        LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();

        // Las opciones del menú

        // Nuevo juego
        langString = languageManager->findLanguageString( "new-game" );
        Menu* menu = new Menu( langString->getX(), langString->getY() );
        label = new Label( langString->getText() );
        label->setAction( new CreatePlanetsScreen( this->where, false ) );
        menu->addActiveOption( label );

        // Cargar juego
        label = new Label( languageManager->findLanguageString( "load-game" )->getText() );
        label->setAction( new CreateListOfSavedGames( this->where, true ) );
        menu->addOption( label );

        // Teclado
        label = new Label( languageManager->findLanguageString( "keyboard" )->getText() );
        label->setAction( new CreateKeyboardMenu( this->where ) );
        menu->addOption( label );

        // Sonido
        label = new Label( languageManager->findLanguageString( "audio" )->getText() );
        label->setAction( new CreateAudioMenu( this->where ) );
        menu->addOption( label );

        // Autores
        label = new Label( languageManager->findLanguageString( "credits" )->getText() );
        label->setAction( new ShowAuthors( this->where ) );
        menu->addOption( label );

        // Salir al escritorio
        label = new Label( languageManager->findLanguageString( "exit" )->getText() );
        label->setAction( new ExitApplication() );
        menu->addOption( label );

        screen->addWidget( menu );

        // Crea la cadena de responsabilidad
        screen->setSucessor( menu );

        // Cambia la pantalla mostrada en la interfaz
        GuiManager::getInstance()->changeScreen( screen );
}
