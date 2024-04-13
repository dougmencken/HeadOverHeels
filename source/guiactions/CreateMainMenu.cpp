
#include "CreateMainMenu.hpp"

#include "GuiManager.hpp"
#include "GameManager.hpp"
#include "LanguageStrings.hpp"
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


CreateMainMenu::CreateMainMenu( ) : Action( )
{

}

void CreateMainMenu::act ()
{
        SoundManager::getInstance().playOgg( "music/MainTheme.ogg", /* loop */ true );

        Screen & screen = * GuiManager::getInstance().findOrCreateScreenForAction( *this );

        if ( screen.countWidgets() == 0 )
        {
                screen.placeHeadAndHeels( /* icons */ true, /* copyrights */ true );

                LanguageStrings* languageStrings = GuiManager::getInstance().getLanguageStrings() ;

                Label* newGame = new Label( languageStrings->getTranslatedStringByAlias( "new-game" )->getText() );
                Label* loadGame = new Label( languageStrings->getTranslatedStringByAlias( "load-game" )->getText() );
                Label* defineKeys = new Label( languageStrings->getTranslatedStringByAlias( "keys-menu" )->getText() );
                Label* adjustAudio = new Label( languageStrings->getTranslatedStringByAlias( "audio-menu" )->getText() );
                Label* adjustVideo = new Label( languageStrings->getTranslatedStringByAlias( "video-menu" )->getText() );
                Label* showCredits = new Label( languageStrings->getTranslatedStringByAlias( "show-credits" )->getText() );
                Label* quitGame = new Label( languageStrings->getTranslatedStringByAlias( "exit-game" )->getText() );

                newGame->setAction( new CreatePlanetsScreen( false ) );
                loadGame->setAction( new CreateListOfSavedGames( true ) );
                defineKeys->setAction( new CreateKeyboardMenu() );
                adjustAudio->setAction( new CreateAudioMenu() );
                adjustVideo->setAction( new CreateVideoMenu() );
                showCredits->setAction( new ShowAuthors() );
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

                screen.addWidget( menu );
                screen.setNextKeyHandler( menu );
        }

        GuiManager::getInstance().changeScreen( screen, false );
}
