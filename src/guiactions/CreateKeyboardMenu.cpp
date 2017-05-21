
#include "CreateKeyboardMenu.hpp"
#include "GuiManager.hpp"
#include "LanguageManager.hpp"
#include "InputManager.hpp"
#include "Font.hpp"
#include "Screen.hpp"
#include "Menu.hpp"
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

void CreateKeyboardMenu::doIt ()
{
        Screen* screen = GuiManager::getInstance()->findOrCreateScreenForAction( this, this->where );
        if ( screen->countWidgets() == 0 )
        {
                screen->setEscapeAction( new CreateMainMenu( this->where ) );

                screen->placeHeadAndHeels( /* icons */ false, /* copyrights */ false );

                Label* label = 0;
                LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();

                // la primera opción almacena la posición del menú en pantalla
                LanguageText* langString = languageManager->findLanguageString( "movenorth" );
                this->menuOfKeys = new Menu( langString->getX(), langString->getY() );

                InputManager* supportOfInput = InputManager::getInstance();

                // create one option for each key used in the game
                for ( size_t i = 0; i < InputManager::numberOfKeys; i++ )
                {
                        std::string nameOfThisKey = InputManager::namesOfKeys[ i ];

                        // Código de la tecla asignada
                        int scancode = supportOfInput->getUserKey( nameOfThisKey );

                        // Descripción del uso de la tecla
                        std::string textOfKey = languageManager->findLanguageString( nameOfThisKey )->getText();
                        std::string dottedTextOfKey( textOfKey );
                        for ( size_t position = textOfKey.length() ; position < 16 ; ++position ) {
                                dottedTextOfKey = dottedTextOfKey + ".";
                        }

                        // The menu option is made of the description of the key and the key itself
                        label = new Label( dottedTextOfKey + scancode_to_name( scancode ) );
                        if ( scancode == 0 )
                        {
                                label->changeFontAndColor( label->getFontName (), "cyan" );
                        }

                        // assign as action the possibility to change the key
                        label->setAction( new RedefineKey( menuOfKeys, nameOfThisKey ) );

                        menuOfKeys->addOption( label );
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
