
#include "ExitApplication.hpp"
#include "GuiManager.hpp"
#include "GameManager.hpp"
#include "Screen.hpp"
#include "Color.hpp"

using gui::ExitApplication;


ExitApplication::ExitApplication( )
        : Action( nilPointer )
{
}

void ExitApplication::bye ()
{
        isomot::GameManager::writePreferences( isomot::homePath() + "preferences.xml" );
        GuiManager::getInstance()->suspend();

        Screen::randomPixelFadeOut( GuiManager::getInstance()->getActiveScreen(), Color::blackColor() );
}
