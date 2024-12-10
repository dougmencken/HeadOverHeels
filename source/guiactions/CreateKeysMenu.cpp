
#include "CreateKeysMenu.hpp"

#include "GuiManager.hpp"
#include "InputManager.hpp"
#include "RedefineKey.hpp"
#include "Font.hpp"
#include "Label.hpp"
#include "SlideWithHeadAndHeels.hpp"
#include "CreateOptionsMenu.hpp"


void gui::CreateKeysMenu::act ()
{
        SlideWithHeadAndHeels & gameKeysSlide = GuiManager::getInstance().findOrCreateSlideWithHeadAndHeelsForAction( getNameOfAction() );

        if ( gameKeysSlide.isNewAndEmpty() )
        {
                gameKeysSlide.setEscapeAction( new CreateOptionsMenu() );

                gameKeysSlide.placeHeadAndHeels( /* icons */ false, /* copyrights */ false );

                this->menuOfKeys.deleteAllOptions() ;

                // add menu option for each key
                std::vector< std::string > userActions ;
                InputManager::getInstance().getAllActions( userActions );
                for ( unsigned int i = 0 ; i < userActions.size () ; ++ i )
                {
                        const std::string & theAction = userActions[ i ];
                        std::string xmlAction = ( theAction == "take&jump" ) ? "takeandjump" : theAction ;

                        Label* label = menuOfKeys.addOptionByLanguageTextAlias( xmlAction );

                        std::string theKey = InputManager::getInstance().getUserKeyFor( theAction );
                        menuOfKeys.setValueOf( label->getText(), CreateKeysMenu::allegroKeyToMenuKey( theKey ) );

                        if ( theKey == "none" ) label->getFontToChange().setColor( "cyan" );

                        label->setAction( new RedefineKey( menuOfKeys, theAction ) );
                }

                gameKeysSlide.addWidget( & this->menuOfKeys );
        }

        // select the first menu option
        menuOfKeys.resetActiveOption() ;

        gameKeysSlide.setKeyHandler( & this->menuOfKeys );

        GuiManager::getInstance().changeSlide( getNameOfAction(), true );
}
