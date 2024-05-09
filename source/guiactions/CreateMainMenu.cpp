
#include "CreateMainMenu.hpp"

#include "GuiManager.hpp"
#include "SoundManager.hpp"
#include "LanguageStrings.hpp"
#include "Label.hpp"
#include "Menu.hpp"
#include "Screen.hpp"

#include "CreatePlanetsScreen.hpp"
#include "CreateListOfSavedGames.hpp"
#include "CreateOptionsMenu.hpp"
#include "ShowAuthors.hpp"
#include "ExitApplication.hpp"


namespace gui
{

void CreateMainMenu::act ()
{
        SoundManager::getInstance().playOgg( "music/MainTheme.ogg", /* loop */ true );

        Screen & screen = * GuiManager::getInstance().findOrCreateScreenForAction( *this );

        if ( screen.isNewAndEmpty() )
        {
                screen.placeHeadAndHeels( /* icons */ true, /* copyrights */ true );

                LanguageStrings* languageStrings = GuiManager::getInstance().getLanguageStrings() ;

                Label* newGame = new Label( languageStrings->getTranslatedTextByAlias( "new-game" )->getText() );
                Label* loadGame = new Label( languageStrings->getTranslatedTextByAlias( "load-old-game" )->getText() );
                Label* optionsMenu = new Label( languageStrings->getTranslatedTextByAlias( "options-menu" )->getText() );
                Label* showCredits = new Label( languageStrings->getTranslatedTextByAlias( "show-credits" )->getText() );
                Label* quitGame = new Label( languageStrings->getTranslatedTextByAlias( "exit-game" )->getText() );

                newGame->setAction( new CreatePlanetsScreen( false ) );
                loadGame->setAction( new CreateListOfSavedGames( true ) );
                optionsMenu->setAction( new CreateOptionsMenu() );
                showCredits->setAction( new ShowAuthors() );
                quitGame->setAction( new ExitApplication() );

                Menu * menu = new Menu( );
                menu->setVerticalOffset( 12 );

                menu->addOption( newGame );
                menu->addOption( loadGame );
                menu->addOption( optionsMenu );
                menu->addOption( showCredits );
                menu->addOption( quitGame );

                screen.addWidget( menu );
                screen.setNextKeyHandler( menu );
        }

        GuiManager::getInstance().changeScreen( screen, false );
}

}
