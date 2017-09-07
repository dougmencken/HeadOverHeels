
#include "SelectLanguage.hpp"
#include "CreateMainMenu.hpp"
#include "GuiManager.hpp"

using gui::SelectLanguage ;


SelectLanguage::SelectLanguage( BITMAP* picture, const std::string& language )
: Action(),
  where( picture ),
  language( language )
{

}

void SelectLanguage::doIt ()
{
        GuiManager::getInstance()->setLanguage( language );

        CreateMainMenu * mainMenu = new CreateMainMenu( this->where );
        mainMenu->doIt ();
}
