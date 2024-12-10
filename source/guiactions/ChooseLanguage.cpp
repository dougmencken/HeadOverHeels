
#include "ChooseLanguage.hpp"

#include "PresentTheMainMenu.hpp"
#include "GuiManager.hpp"


void gui::ChooseLanguage::act ()
{
        if ( GuiManager::getInstance().countSlides() > 1 ) // not the very first slide
                GuiManager::getInstance().freeSlides() ;

        GuiManager::getInstance().setLanguage( language );

        ( new PresentTheMainMenu() )->doIt ();
}
