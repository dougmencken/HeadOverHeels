
#include "ChooseLanguage.hpp"

#include "CreateMainMenu.hpp"
#include "GuiManager.hpp"


namespace gui
{

ChooseLanguage::ChooseLanguage( const std::string & laLangueChoisie )
        : Action( )
        , language( laLangueChoisie )
{

}

void ChooseLanguage::act ()
{
        GuiManager::getInstance().setLanguage( language );

        CreateMainMenu * mainMenu = new CreateMainMenu();
        mainMenu->doIt ();
}

}
