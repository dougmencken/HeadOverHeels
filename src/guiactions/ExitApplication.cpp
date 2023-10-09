
#include "ExitApplication.hpp"

#include "ospaths.hpp"

#include "GuiManager.hpp"
#include "GameManager.hpp"
#include "Screen.hpp"
#include "Color.hpp"


namespace gui
{

void ExitApplication::bye ()
{
        iso::GameManager::writePreferences( ospaths::homePath() + "preferences.xml" );
        GuiManager::getInstance().suspend();

        Screen::randomPixelFadeOut( * GuiManager::getInstance().getActiveScreen(), Color::blackColor() );
}

}
