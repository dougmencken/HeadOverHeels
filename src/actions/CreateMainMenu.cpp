
#include "CreateMainMenu.hpp"
#include "GuiManager.hpp"
#include "LanguageManager.hpp"
#include "SoundManager.hpp"
#include "Screen.hpp"
#include "Menu.hpp"

#include "Icon.hpp"
#include "Label.hpp"
#include "CreatePlanetsScreen.hpp"
#include "CreateKeyboardMenu.hpp"
#include "CreateAudioMenu.hpp"
#include "CreateVideoMenu.hpp"
#include "CreateListOfSavedGames.hpp"
#include "ShowAuthors.hpp"
#include "ExitApplication.hpp"

using gui::CreateMainMenu;
using isomot::SoundManager;


/* static */
void CreateMainMenu::placeHeadAndHeels( Screen* screen, bool iconsToo, bool copyrightsToo )
{
        Label* label = 0;

        label = new Label( 64, 22, "Jon", "regular", "multicolor" );
        screen->addWidget( label );

        label = new Label( 40, 52, "Ritman", "regular", "multicolor" );
        screen->addWidget( label );

        label = new Label( 500, 22, "Bernie", "regular", "multicolor" );
        screen->addWidget( label );

        label = new Label( 483, 52, "Drummond", "regular", "multicolor" );
        screen->addWidget( label );

        label = new Label( 200, 24, "HEAD" );
        label->changeFontAndColor( "big", "yellow" );
        screen->addWidget( label );

        label = new Label( 280, 45, "OVER", "regular", "multicolor" );
        screen->addWidget( label );

        label = new Label( 360, 24, "HEELS" );
        label->changeFontAndColor( "big", "yellow" );
        screen->addWidget( label );

        if ( iconsToo )
        {
                screen->addWidget( new Icon( 206, 84, GuiManager::getInstance()->findImage( "head" ) ) );
                screen->addWidget( new Icon( 378, 84, GuiManager::getInstance()->findImage( "heels" ) ) );
        }

        if ( copyrightsToo )
        {
                const int whereX = 56;
                const int whereY = 440;
                const int stepY = 28;

                // (c) 1987 Ocean Software Ltd.
                std::string copyrightString ( "{ 1987 Ocean Software Ltd." );
                label = new Label( whereX, whereY, copyrightString );
                label->changeFontAndColor( "regular", "cyan" );
                screen->addWidget( label );

                // (c) 2009 Jorge Rodríguez Santos
                copyrightString = "{ 2009 Jorge Rodríguez Santos" ;
                label = new Label( whereX, whereY - stepY, copyrightString );
                label->changeFontAndColor( "regular", "orange" );
                screen->addWidget( label );

                // (c) 2017 Douglas Mencken
                copyrightString = "{ 2017 Douglas Mencken" ;
                label = new Label( whereX, whereY - stepY - stepY, copyrightString );
                label->changeFontAndColor( "regular", "yellow" );
                screen->addWidget( label );
        }
}


CreateMainMenu::CreateMainMenu( BITMAP* picture ) :
        Action(),
        where( picture )
{

}

void CreateMainMenu::doIt ()
{
        if ( ! SoundManager::getInstance()->isPlayingOgg( "music/MainTheme.ogg" ) )
        {
                ///SoundManager::getInstance()->stopAnyOgg();
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

        label = new Label( languageManager->findLanguageString( "audio" )->getText() );
        label->setAction( new CreateAudioMenu( this->where ) );
        menu->addOption( label );

        label = new Label( languageManager->findLanguageString( "video" )->getText() );
        label->setAction( new CreateVideoMenu( this->where ) );
        menu->addOption( label );

        label = new Label( languageManager->findLanguageString( "credits" )->getText() );
        label->setAction( new ShowAuthors( this->where ) );
        menu->addOption( label );

        label = new Label( languageManager->findLanguageString( "exit" )->getText() );
        label->setAction( new ExitApplication() );
        menu->addOption( label );

        screen->addWidget( menu );
        screen->setNext( menu );

        GuiManager::getInstance()->changeScreen( screen );
}
