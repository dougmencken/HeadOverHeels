
#include "SelectLanguage.hpp"
#include "CreateMainMenu.hpp"
#include "GuiManager.hpp"

using gui::SelectLanguage ;


SelectLanguage::SelectLanguage( Picture* picture, const std::string& language )
        : Action( picture )
        , language( language )
{

}

void SelectLanguage::doAction ()
{
        GuiManager::getInstance()->setLanguage( language );

        CreateMainMenu * mainMenu = new CreateMainMenu( getWhereToDraw() );
        mainMenu->doIt ();
}
