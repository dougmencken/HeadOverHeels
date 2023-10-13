
#include "SelectLanguage.hpp"
#include "CreateMainMenu.hpp"
#include "GuiManager.hpp"

using gui::SelectLanguage ;


SelectLanguage::SelectLanguage( const std::string& language )
        : Action( )
        , language( language )
{

}

void SelectLanguage::doAction ()
{
        GuiManager::getInstance().setLanguage( language );

        CreateMainMenu * mainMenu = new CreateMainMenu();
        mainMenu->doIt ();
}
