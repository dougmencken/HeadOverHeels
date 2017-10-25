
#include "CreateKeyboardMenu.hpp"
#include "GuiManager.hpp"
#include "LanguageManager.hpp"
#include "LanguageText.hpp"
#include "InputManager.hpp"
#include "Font.hpp"
#include "Screen.hpp"
#include "MenuWithValues.hpp"
#include "Label.hpp"
#include "CreateMainMenu.hpp"
#include "RedefineKey.hpp"

using gui::CreateKeyboardMenu;
using isomot::InputManager;


CreateKeyboardMenu::CreateKeyboardMenu( BITMAP* picture ) :
        Action( ) ,
        where( picture )
{

}

void CreateKeyboardMenu::doAction ()
{
        Screen* screen = GuiManager::getInstance()->findOrCreateScreenForAction( this, this->where );
        if ( screen->countWidgets() == 0 )
        {
                screen->setEscapeAction( new CreateMainMenu( this->where ) );

                screen->placeHeadAndHeels( /* icons */ false, /* copyrights */ false );

                LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();

                this->menuOfKeys = new MenuWithValues( '.', 5 );
                menuOfKeys->setVerticalOffset( 78 );

                // create one option for each key used in the game
                for ( size_t i = 0; i < InputManager::numberOfKeys; i++ )
                {
                        std::string nameOfThisKey = InputManager::namesOfKeys[ i ];
                        std::string nameOfTranslation = ( nameOfThisKey == "take&jump" ? "takeandjump" : nameOfThisKey );

                        Label* label = new Label( languageManager->findLanguageString( nameOfTranslation )->getText() );

                        int scancode = InputManager::getInstance()->getUserKey( nameOfThisKey );
                        if ( scancode == 0 )
                        {
                                label->changeFontAndColor( label->getFontName (), "cyan" );
                        }

                        // action is possibility to change key
                        label->setAction( new RedefineKey( menuOfKeys, nameOfThisKey ) );

                        menuOfKeys->addOption( label );
                        menuOfKeys->setValueOf( label, scancode_to_name( scancode ) );
                }

                screen->addWidget( menuOfKeys );
                screen->setKeyHandler( menuOfKeys );
        }
        else
        {
                // select first option of menu
                menuOfKeys->resetActiveOption();
        }

        GuiManager::getInstance()->changeScreen( screen );
}
