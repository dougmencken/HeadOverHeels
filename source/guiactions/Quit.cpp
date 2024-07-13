
#include "Quit.hpp"

#include "ospaths.hpp"

#include "GuiManager.hpp"
#include "GamePreferences.hpp"
#include "Screen.hpp"
#include "Color.hpp"


void gui::Quit::bye ()
{
        GamePreferences::writePreferences( ospaths::pathToFile( ospaths::homePath(), "preferences.xml" ) );

        GuiManager::getInstance().exit() ;

        Screen::randomPixelFadeOut( * GuiManager::getInstance().getActiveSlide(), Color::blackColor() );
}
