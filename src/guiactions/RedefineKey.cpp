
#include "RedefineKey.hpp"
#include "InputManager.hpp"
#include "ConfigurationManager.hpp"
#include "GuiManager.hpp"
#include "LanguageManager.hpp"
#include "LanguageText.hpp"
#include "MenuWithValues.hpp"
#include "Label.hpp"

using gui::RedefineKey;
using gui::ConfigurationManager;
using isomot::InputManager;


RedefineKey::RedefineKey( MenuWithValues* menu, std::string name )
        : Action(),
        menu( menu ),
        nameOfKey( name )
{

}

void RedefineKey::doAction ()
{
        Label * activeLabel = menu->getActiveOption();
        activeLabel->changeFontFamilyAndColor( "big", "yellow" );
        menu->redraw ();

        clear_keybuf();

        bool exitLoop = false;
        while ( ! exitLoop )
        {
                if ( keypressed() )
                {
                        // Tecla asignada por el usuario
                        int newKey = readkey() >> 8;

                        if ( newKey == KEY_ESC )
                        {
                                exitLoop = true;
                        }
                        else
                        {
                                int codeOfKey = InputManager::getInstance()->getUserKey( this->nameOfKey );
                                if ( codeOfKey != newKey )
                                {
                                        // if this new key was already in use, change that previous one to "none"
                                        std::string nameOfPrevious = InputManager::getInstance()->findNameOfKeyByCode( newKey );
                                        std::string nameForText = ( nameOfPrevious == "take&jump" ? "takeandjump" : nameOfPrevious );

                                        if ( nameOfPrevious != InputManager::nameOfAbsentKey )
                                        {
                                                InputManager::getInstance()->changeUserKey( nameOfPrevious, 0 );

                                                // update menu
                                                std::string textOfThatKey = GuiManager::getInstance()->getLanguageManager()->findLanguageString( nameForText )->getText();
                                                std::list < Label * > everyLabel = menu->getEveryOption ();
                                                for ( std::list< Label * >::iterator o = everyLabel.begin (); o != everyLabel.end (); ++o )
                                                {
                                                        if ( ( *o )->getText() == textOfThatKey )
                                                        {
                                                                menu->setValueOf( *o, scancode_to_name( 0 ) );
                                                                ( *o )->changeColor( "cyan" );

                                                                std::cout << "key \"" << nameOfPrevious << "\" is now NONE ( 0 )" << std::endl ;
                                                                break;
                                                        }
                                                }
                                                menu->redraw ();
                                        }

                                        std::cout << "key \"" << this->nameOfKey
                                                  << "\" was " << scancode_to_name( codeOfKey ) << " ( " << codeOfKey << " ) "
                                                  << "now is " << scancode_to_name( newKey ) << " ( " << newKey << " )"
                                                  << std::endl ;

                                        nameForText = ( this->nameOfKey == "take&jump" ? "takeandjump" : this->nameOfKey );

                                        menu->setValueOf( activeLabel, scancode_to_name( newKey ) );

                                        InputManager::getInstance()->changeUserKey( this->nameOfKey, newKey );
                                }

                                exitLoop = true;
                        }

                        clear_keybuf();
                }

                // no te comas la CPU
                // do not eat the CPU
                sleep( 20 );
        }

        if ( InputManager::getInstance()->getUserKey( this->nameOfKey ) != 0 ) {
                activeLabel->changeFontFamilyAndColor( "big", "white" );
        } else {
                activeLabel->changeFontFamilyAndColor( "big", "cyan" );
        }

        menu->redraw ();
}
