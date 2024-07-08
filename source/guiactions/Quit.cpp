
#include "Quit.hpp"

#include "ospaths.hpp"

#include "GuiManager.hpp"
#include "GamePreferences.hpp"
#include "Screen.hpp"
#include "Color.hpp"


namespace gui
{

void Quit::bye ()
{
        GamePreferences::writePreferences( ospaths::pathToFile( ospaths::homePath(), "preferences.xml" ) );
        GuiManager::getInstance().suspend() ;

        Screen::randomPixelFadeOut( * GuiManager::getInstance().getActiveScreen(), Color::blackColor() );
}

}
