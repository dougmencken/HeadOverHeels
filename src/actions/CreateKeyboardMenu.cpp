
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
        Screen* screen = new Screen( 0, 0, this->where );
        screen->setBackground( GuiManager::getInstance()->findImage( "background" ) );
        screen->setEscapeAction( new CreateMainMenu( this->where ) );

        CreateMainMenu::placeHeadAndHeels( screen, /* icons */ false, /* copyrights */ false );

        Label* label = 0;
        LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();

        // La primera opción almacena la posición del menú en pantalla
        LanguageText* langString = languageManager->findLanguageString( "movenorth" );
        Menu* menu = new Menu( langString->getX(), langString->getY() );

        InputManager* supportOfInput = InputManager::getInstance();

        // Create one option for each key used in the game
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

                // Assign as action the possibility to change the key
                label->setAction( new RedefineKey( menu, nameOfThisKey ) );

                // La primera opción es la opción activa
                if ( i == 0 )
                {
                        menu->addActiveOption( label );
                }
                else
                {
                        menu->addOption( label );
                }
        }

        // El menú que aparece en pantalla
        screen->addWidget( menu );
        screen->setNext( menu );

        GuiManager::getInstance()->changeScreen( screen );
}
