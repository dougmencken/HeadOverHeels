
#include "TheMainMenu.hpp"

#include "Label.hpp"

#include "ShowSlideWithPlanets.hpp"
#include "PresentTheListOfSavedGames.hpp"
#include "CreateOptionsMenu.hpp"
#include "ShowAuthors.hpp"
#include "Quit.hpp"


namespace gui
{

TheMainMenu::TheMainMenu( )
        : Menu( )
        , newGame( addOptionByLanguageTextAlias( "new-game" ) )
        , loadGame( addOptionByLanguageTextAlias( "load-old-game" ) )
        , optionsMenu( addOptionByLanguageTextAlias( "options-menu" ) )
        , showCredits( addOptionByLanguageTextAlias( "show-credits" ) )
        , quitGame( addOptionByLanguageTextAlias( "exit-game" ) )
{
        newGame->setAction( new ShowSlideWithPlanets() );
        loadGame->setAction( new PresentTheListOfSavedGames( true ) );
        optionsMenu->setAction( new CreateOptionsMenu() );
        showCredits->setAction( new ShowAuthors() );
        quitGame->setAction( new Quit() );
}

}
