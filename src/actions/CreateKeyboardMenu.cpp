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


CreateKeyboardMenu::CreateKeyboardMenu( BITMAP* picture )
: Action(),
  where( picture )
{

}

void CreateKeyboardMenu::doIt ()
{
        Screen* screen = new Screen( 0, 0, this->where );
        screen->setBackground( GuiManager::getInstance()->findImage( "background" ) );
        screen->setAction( new CreateMainMenu( this->where ) );

        CreateMainMenu::placeHeadAndHeels( screen, /* icons */ false, /* copyrights */ true );

        Label* label = 0;
        LanguageText* langString = 0;
        LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();

        // Las opciones del menú
        std::string textKey[ ] = {  "left", "right", "up", "down", "take", "jump", "shoot", "take-jump", "swap", "halt"  };
        isomot::GameKey gameKey[ ] = {  isomot::KeyNorth, isomot::KeySouth, isomot::KeyEast, isomot::KeyWest,
                                        isomot::KeyTake, isomot::KeyJump, isomot::KeyShoot, isomot::KeyTakeAndJump,
                                        isomot::KeySwap, isomot::KeyHalt  };

        // La primera opción almacena la posición del menú en pantalla
        langString = languageManager->findLanguageString( "left" );
        Menu* menu = new Menu( langString->getX(), langString->getY() );

        InputManager* inputManager = InputManager::getInstance();

        // Creación de las opciones: una por cada tecla empleada en el juego
        for ( int i = 0; i < 10; i++ )
        {
                // Código de la tecla asignada
                int scancode = inputManager->readUserKey( gameKey[i] );

                // Descripción del uso de la tecla
                std::string keyText = languageManager->findLanguageString( textKey[i] )->getText();

                // Se crea la opción de menú con la descripción de la tecla y la propia tecla
                label = new Label( keyText + scancode_to_name( scancode ) );

                // Se asigna como acción el cambio de la tecla por parte del usuario
                label->setAction( new RedefineKey( menu, keyText, scancode ) );

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

        // Crea la cadena de responsabilidad
        screen->setSucessor( menu );

        // Cambia la pantalla mostrada en la interfaz
        GuiManager::getInstance()->changeScreen( screen );
}
