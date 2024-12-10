
#include "PresentTheMainMenu.hpp"

#include "GuiManager.hpp"
#include "SoundManager.hpp"


/* private static */
gui::TheMainMenuSlide * gui::PresentTheMainMenu::main_menu_slide = nilPointer ;


void gui::PresentTheMainMenu::act ()
{
        SoundManager::getInstance().playOgg( "music/MainTheme.ogg", /* loop */ true );

        if ( gui::PresentTheMainMenu::main_menu_slide == nilPointer ) {
                gui::PresentTheMainMenu::main_menu_slide = new TheMainMenuSlide() ;
                GuiManager::getInstance().setSlideForAction( gui::PresentTheMainMenu::main_menu_slide, getNameOfAction() );
        }

        GuiManager::getInstance().changeSlide( getNameOfAction(), false );
}
