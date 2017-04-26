
#include "SelectLanguage.hpp"
#include "CreateMainMenu.hpp"
#include "GuiManager.hpp"
#include "ConfigurationManager.hpp"

using gui::SelectLanguage;


SelectLanguage::SelectLanguage( BITMAP* picture, const std::string& language )
: Action(),
  where( picture ),
  language( language )
{

}

void SelectLanguage::doIt ()
{
        ConfigurationManager* configuration = GuiManager::getInstance()->getConfigurationManager();
        configuration->setLanguage( language );
        configuration->write();

        GuiManager::getInstance()->assignLanguage( language );

        CreateMainMenu * mainMenu = new CreateMainMenu( this->where );
        mainMenu->doIt ();
}
