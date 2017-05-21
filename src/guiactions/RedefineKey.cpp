
#include "RedefineKey.hpp"
#include "InputManager.hpp"
#include "ConfigurationManager.hpp"
#include "GuiManager.hpp"
#include "LanguageManager.hpp"
#include "Menu.hpp"
#include "Label.hpp"

using gui::RedefineKey;
using gui::ConfigurationManager;
using isomot::InputManager;


RedefineKey::RedefineKey( Menu* menu, std::string name )
: Action(),
  menu( menu ),
  nameOfKey( name )
{

}

void RedefineKey::doIt ()
{
        Label * activeLabel = menu->getActiveOption();
        activeLabel->changeFontAndColor( "big", "yellow" );
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
                                        if ( nameOfPrevious != InputManager::nameOfAbsentKey )
                                        {
                                                InputManager::getInstance()->changeUserKey( nameOfPrevious, 0 );

                                                // update the menu now
                                                std::string textOfThatKey = GuiManager::getInstance()->getLanguageManager()->findLanguageString( nameOfPrevious )->getText();
                                                std::list < Label * > everyLabel = menu->getEveryOption ();
                                                for ( std::list< Label * >::iterator iter = everyLabel.begin (); iter != everyLabel.end (); ++iter )
                                                {
                                                        if ( ( *iter )->getText().find( textOfThatKey ) == 0 )
                                                        {  // this label begins with the text of that previous key
                                                                std::string dottedLabelForKey( textOfThatKey );
                                                                for ( size_t position = textOfThatKey.length() ; position < 16 ; ++position ) {
                                                                        dottedLabelForKey = dottedLabelForKey + ".";
                                                                }
                                                                ( *iter )->setText( dottedLabelForKey + scancode_to_name( 0 ) );
                                                                ( *iter )->changeFontAndColor( ( *iter )->getFontName (), "cyan" );
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

                                        std::string textOfKey = GuiManager::getInstance()->getLanguageManager()->findLanguageString( this->nameOfKey )->getText();
                                        std::string dottedTextOfKey( textOfKey );
                                        for ( size_t position = textOfKey.length() ; position < 16 ; ++position ) {
                                                dottedTextOfKey = dottedTextOfKey + ".";
                                        }
                                        activeLabel->setText( dottedTextOfKey + scancode_to_name( newKey ) );

                                        InputManager::getInstance()->changeUserKey( this->nameOfKey, newKey );
                                }

                                exitLoop = true;
                        }

                        clear_keybuf();
                }

                // No te comas la CPU
                // Do not eat the CPU
                sleep( 20 );
        }

        if ( InputManager::getInstance()->getUserKey( this->nameOfKey ) != 0 ) {
                activeLabel->changeFontAndColor( "big", "white" );
        } else {
                activeLabel->changeFontAndColor( "big", "cyan" );
        }

        menu->redraw ();
}
