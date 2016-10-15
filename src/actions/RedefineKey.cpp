#include "RedefineKey.hpp"
#include "GuiManager.hpp"
#include "InputManager.hpp"
#include "ConfigurationManager.hpp"
#include "Menu.hpp"
#include "Label.hpp"

using gui::RedefineKey;
using gui::ConfigurationManager;
using isomot::InputManager;
using isomot::GameKey;

RedefineKey::RedefineKey( Menu* menu, std::string keyText, int assignedKey )
: Action(),
  menu( menu ),
  keyText( keyText ),
  assignedKey( assignedKey )
{

}

void RedefineKey::doIt ()
{
        bool keyChanged = false;
        Label* label = new Label( this->keyText );
        menu->changeOption( this->keyText + scancode_to_name( this->assignedKey ), label );
        GuiManager::getInstance()->refresh();

        clear_keybuf();

        while ( ! keyChanged )
        {
                if ( keypressed() )
                {
                        // Consulta para qué sirve la tecla
                        GameKey gameKey = InputManager::getInstance()->findKeyType( this->assignedKey );
                        // Tecla asignada por el usuario
                        int newKey = readkey() >> 8;

                        // Si la nueva tecla no se está usando entonces se asigna y se presenta en pantalla
                        if ( InputManager::getInstance()->findKeyType( newKey ) == isomot::KeyNone ||
                                InputManager::getInstance()->findKeyType( newKey ) == gameKey )
                        {
                                this->assignedKey = newKey;
                                menu->changeOption( label->getText(), new Label( this->keyText + scancode_to_name( this->assignedKey ) ) );
                                InputManager::getInstance()->changeUserKey( gameKey, newKey );
                                GuiManager::getInstance()->getConfigurationManager()->setKey( gameKey, newKey );
                                keyChanged = true;
                        }

                        clear_keybuf();
                }

                // No te comas la CPU
                // Do not eat the CPU
                sleep( 20 );
        }
}
