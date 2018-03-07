
#include "ExitApplication.hpp"
#include "GuiManager.hpp"
#include "ConfigurationManager.hpp"
#include "Screen.hpp"
#include "Color.hpp"

using gui::ExitApplication;


ExitApplication::ExitApplication( )
        : Action( 0 )
{

}

void ExitApplication::bye ()
{

        GuiManager::getInstance()->getConfigurationManager()->write();
        GuiManager::getInstance()->suspend();

        Screen::randomPixelFadeOut( GuiManager::getInstance()->getActiveScreen(), Color::blackColor() );
}
