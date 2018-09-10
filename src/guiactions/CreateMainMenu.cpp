
#include "CreateMainMenu.hpp"
#include "GuiManager.hpp"
#include "LanguageManager.hpp"
#include "SoundManager.hpp"
#include "LanguageText.hpp"
#include "Screen.hpp"
#include "Menu.hpp"

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


CreateMainMenu::CreateMainMenu( allegro::Pict* picture ) :
        Action( picture )
{

}

void CreateMainMenu::doAction ()
{
        if ( ! SoundManager::getInstance()->isPlayingOgg( "music/MainTheme.ogg" ) )
        {
                ///SoundManager::getInstance()->stopAnyOgg();
                SoundManager::getInstance()->playOgg( "music/MainTheme.ogg", /* loop */ true );
        }

        Screen* screen = GuiManager::getInstance()->findOrCreateScreenForAction( this );
        if ( screen->countWidgets() == 0 )
        {
                screen->placeHeadAndHeels( /* icons */ true, /* copyrights */ true );

                LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();

                // Las opciones del menú

                Label* newGame = new Label( languageManager->findLanguageStringForAlias( "new-game" )->getText() );
                Label* loadGame = new Label( languageManager->findLanguageStringForAlias( "load-game" )->getText() );
                Label* defineKeys = new Label( languageManager->findLanguageStringForAlias( "keyboard-menu" )->getText() );
                Label* adjustAudio = new Label( languageManager->findLanguageStringForAlias( "audio-menu" )->getText() );
                Label* adjustVideo = new Label( languageManager->findLanguageStringForAlias( "video-menu" )->getText() );
                Label* showCredits = new Label( languageManager->findLanguageStringForAlias( "show-credits" )->getText() );
                Label* quitGame = new Label( languageManager->findLanguageStringForAlias( "exit-game" )->getText() );

                newGame->setAction( new CreatePlanetsScreen( getWhereToDraw(), false ) );
                loadGame->setAction( new CreateListOfSavedGames( getWhereToDraw(), true ) );
                defineKeys->setAction( new CreateKeyboardMenu( getWhereToDraw() ) );
                adjustAudio->setAction( new CreateAudioMenu( getWhereToDraw() ) );
                adjustVideo->setAction( new CreateVideoMenu( getWhereToDraw() ) );
                showCredits->setAction( new ShowAuthors( getWhereToDraw() ) );
                quitGame->setAction( new ExitApplication() );

                Menu * menu = new Menu( );
                menu->setVerticalOffset( 12 );

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

        GuiManager::getInstance()->changeScreen( screen, false );
}
