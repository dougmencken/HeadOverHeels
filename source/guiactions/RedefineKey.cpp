
#include "RedefineKey.hpp"

#include "sleep.hpp"

#include "InputManager.hpp"
#include "GuiManager.hpp"
#include "LanguageManager.hpp"
#include "LanguageText.hpp"
#include "MenuWithValues.hpp"
#include "Label.hpp"

using gui::RedefineKey ;


RedefineKey::RedefineKey( MenuWithValues* menu, const std::string& keyAction )
        : Action( )
        , menu( menu )
        , whatKeyDoes( keyAction )
{

}

void RedefineKey::doAction ()
{
        Label * activeLabel = menu->getActiveOption();
        activeLabel->changeFontFamilyAndColor( "big", "yellow" );
        menu->redraw ();

        allegro::emptyKeyboardBuffer();

        bool exitLoop = false;
        while ( ! exitLoop )
        {
                if ( allegro::areKeypushesWaiting() )
                {
                        std::string newKey = allegro::nextKey() ;

                        if ( newKey == "Escape" )
                        {
                                exitLoop = true;
                        }
                        else
                        {
                                std::string thatKey = InputManager::getInstance().getUserKeyFor( this->whatKeyDoes );
                                if ( thatKey != newKey && /* print screen is used to toggle recording of game */ newKey != "PrintScreen" )
                                {
                                        // if this new key was already in use, change that previous one to "none"
                                        std::string previousAction = InputManager::getInstance().getActionOfKeyByName( newKey );
                                        std::string toLook = ( previousAction == "take&jump" ? "takeandjump" : previousAction );

                                        if ( previousAction != "unknown" )
                                        {
                                                InputManager::getInstance().changeUserKey( previousAction, "none" );

                                                // update menu
                                                std::string textOfThatKey = GuiManager::getInstance().getLanguageManager()->findLanguageStringForAlias( toLook )->getText();
                                                std::list < Label * > everyLabel = menu->getEveryOption ();
                                                for ( std::list< Label * >::iterator o = everyLabel.begin (); o != everyLabel.end (); ++o )
                                                {
                                                        if ( ( *o )->getText() == textOfThatKey )
                                                        {
                                                                menu->setValueOf( *o, "none" );
                                                                ( *o )->changeColor( "cyan" );

                                                                std::cout << "key \"" << previousAction << "\" is now NONE" << std::endl ;
                                                                break;
                                                        }
                                                }
                                                menu->redraw ();
                                        }

                                        std::cout << "key for \"" << this->whatKeyDoes << "\" was \"" << thatKey << "\" now is \"" << newKey << "\"" << std::endl ;

                                        menu->setValueOf( activeLabel, newKey );

                                        InputManager::getInstance().changeUserKey( this->whatKeyDoes, newKey );
                                }

                                exitLoop = true;
                        }

                        allegro::emptyKeyboardBuffer();
                }

                // no te comas la CPU
                // do not eat the CPU
                somn::milliSleep( 20 );
        }

        if ( InputManager::getInstance().getUserKeyFor( this->whatKeyDoes ) != "none" ) {
                activeLabel->changeFontFamilyAndColor( "big", "white" );
        } else {
                activeLabel->changeFontFamilyAndColor( "big", "cyan" );
        }

        menu->redraw ();
}
