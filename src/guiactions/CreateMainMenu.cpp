
#include "CreateMainMenu.hpp"
#include "GuiManager.hpp"
#include "LanguageManager.hpp"
#include "SoundManager.hpp"
#include "LanguageText.hpp"
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

        Screen* screen = GuiManager::getInstance()->findOrCreateScreenForAction( this, this->where );
        if ( screen->countWidgets() == 0 )
        {
                screen->placeHeadAndHeels( /* icons */ true, /* copyrights */ true );

                LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();

                // Las opciones del menú

                LanguageText* langString = languageManager->findLanguageString( "new-game" );

                Label* newGame = new Label( langString->getText() );
                Label* loadGame = new Label( languageManager->findLanguageString( "load-game" )->getText() );
                Label* defineKeys = new Label( languageManager->findLanguageString( "keyboard-menu" )->getText() );
                Label* adjustAudio = new Label( languageManager->findLanguageString( "audio-menu" )->getText() );
                Label* adjustVideo = new Label( languageManager->findLanguageString( "video-menu" )->getText() );
                Label* showCredits = new Label( languageManager->findLanguageString( "show-credits" )->getText() );
                Label* quitGame = new Label( languageManager->findLanguageString( "exit-game" )->getText() );

                newGame->setAction( new CreatePlanetsScreen( this->where, false ) );
                loadGame->setAction( new CreateListOfSavedGames( this->where, true ) );
                defineKeys->setAction( new CreateKeyboardMenu( this->where ) );
                adjustAudio->setAction( new CreateAudioMenu( this->where ) );
                adjustVideo->setAction( new CreateVideoMenu( this->where ) );
                showCredits->setAction( new ShowAuthors( this->where ) );
                quitGame->setAction( new ExitApplication() );

                Menu * menu = new Menu( langString->getX(), langString->getY() );

                menu->addOption( newGame );
                menu->addOption( loadGame );
                menu->addOption( defineKeys );
                menu->addOption( adjustAudio );
                menu->addOption( adjustVideo );
                menu->addOption( showCredits );
                menu->addOption( quitGame );

                screen->addWidget( menu );
                screen->setKeyHandler( menu );
        }

        GuiManager::getInstance()->changeScreen( screen );
}
