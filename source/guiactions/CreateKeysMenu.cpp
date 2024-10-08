
#include "CreateKeysMenu.hpp"

#include "GuiManager.hpp"
#include "InputManager.hpp"
#include "RedefineKey.hpp"
#include "Font.hpp"
#include "Label.hpp"
#include "Slide.hpp"
#include "MenuWithValues.hpp"
#include "CreateOptionsMenu.hpp"
#include "LanguageStrings.hpp"


void gui::CreateKeysMenu::act ()
{
        Slide & gameKeysSlide = GuiManager::getInstance().findOrCreateSlideForAction( getNameOfAction() );

        if ( gameKeysSlide.isNewAndEmpty() )
        {
                gameKeysSlide.setEscapeAction( new CreateOptionsMenu() );

                gameKeysSlide.placeHeadAndHeels( /* icons */ false, /* copyrights */ false );

                this->menuOfKeys = new MenuWithValues( '.', 5 );
                menuOfKeys->setVerticalOffset( 64 );

                LanguageStrings & languageStrings = GuiManager::getInstance().getOrMakeLanguageStrings();

                // add menu option for each key
                std::vector< std::string > userActions ;
                InputManager::getInstance().getAllActions( userActions );
                for ( unsigned int i = 0 ; i < userActions.size () ; ++ i )
                {
                        const std::string & theAction = userActions[ i ];
                        std::string xmlAction = ( theAction == "take&jump" ) ? "takeandjump" : theAction ;

                        Label* label = new Label( languageStrings.getTranslatedTextByAlias( xmlAction ).getText() );

                        std::string theKey = InputManager::getInstance().getUserKeyFor( theAction );
                        if ( theKey == "none" ) label->getFontToChange().setColor( "cyan" );

                        label->setAction( new RedefineKey( menuOfKeys, theAction ) );

                        menuOfKeys->addOption( label );
                        menuOfKeys->setValueOf( label, CreateKeysMenu::allegroKeyToMenuKey( theKey ) );
                }

                gameKeysSlide.addWidget( menuOfKeys );
        }

        // select the first menu option
        menuOfKeys->resetActiveOption() ;

        gameKeysSlide.setKeyHandler( menuOfKeys );

        GuiManager::getInstance().changeSlide( getNameOfAction(), true );
}
