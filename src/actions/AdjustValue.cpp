#include "AdjustValue.hpp"
#include "GuiManager.hpp"
#include "SoundManager.hpp"
#include "Menu.hpp"
#include "Label.hpp"

using gui::AdjustValue;
using isomot::SoundManager;

AdjustValue::AdjustValue( Menu* menu, const std::string& text, int value )
        : menu( menu ),
          text( text ),
          value( value )
{

}

void AdjustValue::doIt ()
{
        std::stringstream ss;
        int pressedKey = KEY_MAX;

        ss << value;
        Label* label = new Label( this->text + ss.str() );
        label->changeFont( "big", "yellow" );
        menu->changeOption( label->getText(), label );
        GuiManager::getInstance()->refresh();
        clear_keybuf();

        while ( pressedKey != KEY_ESC && pressedKey != KEY_ENTER && pressedKey != KEY_UP && pressedKey != KEY_DOWN )
        {
                if ( keypressed () )
                {
                        int previousValue = value;

                        // Tecla pulsada por el usuario
                        pressedKey = readkey() >> 8;

                        // Si se pulsa el cursor izquierdo se baja el volumen
                        if ( pressedKey == KEY_LEFT )
                        {
                                value = ( value > 0 ? value - 1 : 0 );
                        }
                        // Si se pulsa el cursor derecho se sube el volumen
                        else if ( pressedKey == KEY_RIGHT )
                        {
                                value = ( value < 99 ? value + 1 : 99 );
                        }

                        if ( value != previousValue )
                        {
                                ss.str( std::string() );
                                ss << value;
                                std::string previous( label->getText() );
                                label = new Label( this->text + ss.str() );
                                label->changeFont( "big", "yellow" );
                                menu->changeOption( previous, label );
                                GuiManager::getInstance()->refresh();
                        }
                }

                // No te comas la CPU
                // Do not eat the CPU
                sleep( 20 );
        }

        if( pressedKey == KEY_ENTER )
        {
                std::string previous( label->getText() );
                label = new Label( previous );
                label->changeFont( "big", "white" );
                menu->changeOption( previous, label );
                GuiManager::getInstance()->refresh();
        }
        else
        {
                simulate_keypress( pressedKey << 8 );
        }
}
