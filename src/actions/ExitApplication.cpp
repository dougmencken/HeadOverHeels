#include "ExitApplication.hpp"
#include "GuiManager.hpp"
#include "ConfigurationManager.hpp"

using gui::ExitApplication;


ExitApplication::ExitApplication( )
: Action( )
{

}

void ExitApplication::bye ()
{

        GuiManager::getInstance()->getConfigurationManager()->write();
        GuiManager::getInstance()->suspend();

}
