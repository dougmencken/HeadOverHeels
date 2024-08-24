
#include "Quit.hpp"

#include "ospaths.hpp"

#include "GuiManager.hpp"
#include "GamePreferences.hpp"
#include "Slide.hpp"
#include "Color.hpp"


void gui::Quit::bye ()
{
        GamePreferences::writePreferences( ospaths::pathToFile( ospaths::homePath(), "preferences.xml" ) );

        GuiManager::getInstance().exit() ;

        Slide::randomPixelFadeOut( * GuiManager::getInstance().getActiveSlide(), Color::blackColor() );
}
