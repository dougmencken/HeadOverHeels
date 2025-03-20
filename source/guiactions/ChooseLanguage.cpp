
#include "ChooseLanguage.hpp"

#include "PresentTheMainMenu.hpp"
#include "GuiManager.hpp"


void gui::ChooseLanguage::act ()
{
        GuiManager & uiManager = GuiManager::getInstance() ;

        if ( uiManager.countSlides() > 1 ) // not the very first slide
                uiManager.freeSlides() ;

        uiManager.setLanguage( language );

        ( new PresentTheMainMenu() )->doIt ();
}
