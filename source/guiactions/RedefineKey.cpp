
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
        Label * choice = menu->getActiveOption ();
        choice->changeFontFamilyAndColor( "big", "yellow" );
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
                                const std::string & thatKey = InputManager::getInstance().getUserKeyFor( this->whatKeyDoes );
                                if ( thatKey != newKey && /* any key but */ newKey != "PrintScreen" )
                                {
                                        // if this new key was already in use, change that previous one to "none"
                                        std::vector< std::string > userActions ;
                                        InputManager::getInstance().getAllActions( userActions );
                                        std::string previousAction( "" );
                                        for ( unsigned int i = 0 ; i < userActions.size () ; ++ i )
                                        {
                                                const std::string & action = userActions[ i ];
                                                const std::string & key = InputManager::getInstance().getUserKeyFor( action );
                                                if ( key == newKey ) {
                                                        previousAction = action ;
                                                        break ;
                                                }
                                        }

                                        if ( previousAction.length () > 0 )
                                        {
                                                InputManager::getInstance().changeUserKey( previousAction, "none" );

                                                // update menu
                                                std::string toLook = ( previousAction == "take&jump" ) ? "takeandjump" : previousAction ;
                                                std::string textOfThatKey = GuiManager::getInstance().getLanguageManager()->findLanguageStringForAlias( toLook )->getText();

                                                std::list < Label * > allOptions = menu->getEveryOption ();
                                                for ( std::list< Label * >::iterator o = allOptions.begin (); o != allOptions.end (); ++ o )
                                                {
                                                        if ( ( *o )->getText() == textOfThatKey )
                                                        {
                                                                menu->setValueOf( *o, "none" );
                                                                ( *o )->changeColor( "cyan" );

                                                                std::cout << "NO key for \"" << previousAction << "\" yet" << std::endl ;
                                                                break;
                                                        }
                                                }
                                                menu->redraw ();
                                        }

                                        std::cout << "the key for \"" << this->whatKeyDoes << "\" was \"" << thatKey << "\" now is \"" << newKey << "\"" << std::endl ;

                                        menu->setValueOf( choice, newKey );

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
                choice->changeFontFamilyAndColor( "big", "white" );
        } else {
                choice->changeFontFamilyAndColor( "big", "cyan" );
        }

        menu->redraw ();
}
