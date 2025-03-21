
#include "PresentTheMainMenu.hpp"

#include "GuiManager.hpp"
#include "SoundManager.hpp"

#include "NoSuchSlideException.hpp"


/* private static */
gui::TheMainMenuSlide * gui::PresentTheMainMenu::main_menu_slide = nilPointer ;


void gui::PresentTheMainMenu::act ()
{
        SoundManager::getInstance().playOgg( "music/MainTheme.ogg", /* loop */ true );

        const std::string & nameOfThisAction = getNameOfAction ();

        try {
                GuiManager::getInstance().changeToSlideFor( nameOfThisAction, false );
        }
        catch ( NoSuchSlideException const& e ) {
                // don’t delete main_menu_slide again (this is already done in GuiManager::freeSlides)
                /* if ( gui::PresentTheMainMenu::main_menu_slide != nilPointer ) delete gui::PresentTheMainMenu::main_menu_slide ; */

                std::cout << nameOfThisAction << " is making new slide for the main menu" << std::endl ;
                gui::PresentTheMainMenu::main_menu_slide = new TheMainMenuSlide() ;
                GuiManager::getInstance().setSlideForAction( gui::PresentTheMainMenu::main_menu_slide, nameOfThisAction );
                GuiManager::getInstance().changeToSlideFor( nameOfThisAction, false );
        }
}
