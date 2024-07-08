
#include "ChooseLanguage.hpp"

#include "CreateMainMenu.hpp"
#include "GuiManager.hpp"


void gui::ChooseLanguage::act ()
{
        if ( GuiManager::getInstance().countScreens() > 1 ) // not the very first screen
                GuiManager::getInstance().freeScreens() ;

        GuiManager::getInstance().setLanguage( language );

        CreateMainMenu * mainMenu = new CreateMainMenu();
        mainMenu->doIt ();
}
