
#include "CreateKeysMenu.hpp"

#include "GuiManager.hpp"
#include "InputManager.hpp"
#include "RedefineKey.hpp"
#include "Font.hpp"
#include "Label.hpp"
#include "Screen.hpp"
#include "MenuWithValues.hpp"
#include "CreateOptionsMenu.hpp"
#include "LanguageStrings.hpp"


namespace gui
{

void CreateKeysMenu::act ()
{
        Screen & screen = * GuiManager::getInstance().findOrCreateScreenForAction( *this );

        if ( screen.isNewAndEmpty() )
        {
                screen.setEscapeAction( new CreateOptionsMenu() );

                screen.placeHeadAndHeels( /* icons */ false, /* copyrights */ false );

                this->menuOfKeys = new MenuWithValues( '.', 5 );
                menuOfKeys->setVerticalOffset( 64 );

                LanguageStrings* languageStrings = GuiManager::getInstance().getLanguageStrings();

                // add menu option for each key
                std::vector< std::string > userActions ;
                InputManager::getInstance().getAllActions( userActions );
                for ( unsigned int i = 0 ; i < userActions.size () ; ++ i )
                {
                        const std::string & theAction = userActions[ i ];
                        std::string xmlAction = ( theAction == "take&jump" ) ? "takeandjump" : theAction ;

                        Label* label = new Label( languageStrings->getTranslatedTextByAlias( xmlAction )->getText() );

                        std::string theKey = InputManager::getInstance().getUserKeyFor( theAction );
                        if ( theKey == "none" ) label->changeColor( "cyan" );

                        label->setAction( new RedefineKey( menuOfKeys, theAction ) );

                        menuOfKeys->addOption( label );
                        menuOfKeys->setValueOf( label, CreateKeysMenu::allegroKeyToMenuKey( theKey ) );
                }

                screen.addWidget( menuOfKeys );
                screen.setNextKeyHandler( menuOfKeys );
        }
        else    // select the first menu option
                menuOfKeys->resetActiveOption();

        GuiManager::getInstance().changeScreen( screen, true );
}

}
