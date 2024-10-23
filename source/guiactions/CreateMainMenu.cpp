
#include "CreateMainMenu.hpp"

#include "GuiManager.hpp"
#include "SoundManager.hpp"
#include "Label.hpp"
#include "Slide.hpp"

#include "ShowSlideWithPlanets.hpp"
#include "CreateListOfSavedGames.hpp"
#include "CreateOptionsMenu.hpp"
#include "ShowAuthors.hpp"
#include "Quit.hpp"


void gui::CreateMainMenu::act ()
{
        SoundManager::getInstance().playOgg( "music/MainTheme.ogg", /* loop */ true );

        Slide & mainMenuSlide = GuiManager::getInstance().findOrCreateSlideForAction( getNameOfAction() );

        if ( mainMenuSlide.isNewAndEmpty() )
        {
                mainMenuSlide.placeHeadAndHeels( /* icons */ true, /* copyrights */ true );

                mainMenu.deleteAllOptions() ;

                Label* newGame = mainMenu.addOptionByLanguageTextAlias( "new-game" ) ;
                Label* loadGame = mainMenu.addOptionByLanguageTextAlias( "load-old-game" ) ;
                Label* optionsMenu = mainMenu.addOptionByLanguageTextAlias( "options-menu" ) ;
                Label* showCredits = mainMenu.addOptionByLanguageTextAlias( "show-credits" ) ;
                Label* quitGame = mainMenu.addOptionByLanguageTextAlias( "exit-game" ) ;

                newGame->setAction( new ShowSlideWithPlanets( false ) );
                loadGame->setAction( new CreateListOfSavedGames( true ) );
                optionsMenu->setAction( new CreateOptionsMenu() );
                showCredits->setAction( new ShowAuthors() );
                quitGame->setAction( new Quit() );

                mainMenuSlide.addWidget( & mainMenu );
                mainMenuSlide.setKeyHandler( & mainMenu );
        }

        GuiManager::getInstance().changeSlide( getNameOfAction(), false );
}
