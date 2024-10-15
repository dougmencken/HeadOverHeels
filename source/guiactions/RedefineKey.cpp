
#include "RedefineKey.hpp"

#include "sleep.hpp"

#include "InputManager.hpp"
#include "GuiManager.hpp"
#include "LanguageStrings.hpp"
#include "MenuWithValues.hpp"
#include "CreateKeysMenu.hpp"
#include "Label.hpp"


namespace gui {

RedefineKey::RedefineKey( MenuWithValues & menu, const std::string & keyAction )
        : Action( )
        , keysMenu( menu )
        , whatKeyDoes( keyAction )
{}

void RedefineKey::act ()
{
        Label * choice = keysMenu.getActiveOption() ;
        choice->getFontToChange().setColor( "yellow" );
        choice->getFontToChange().setDoubleHeight( true );
        keysMenu.redraw ();

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
                                                const std::string & textOfThatKey = GuiManager::getInstance().getOrMakeLanguageStrings().getTranslatedTextByAlias( toLook ).getText();

                                                const std::vector < Label * > & allOptions = keysMenu.getEveryOption ();
                                                for ( unsigned int o = 0 ; o < allOptions.size() ; ++ o )
                                                {
                                                        Label* option = allOptions[ o ];
                                                        if ( option->getText() == textOfThatKey )
                                                        {
                                                                keysMenu.setValueOf( option->getText(), "none" );
                                                                option->getFontToChange().setColor( "cyan" );

                                                                std::cout << "NO key for \"" << previousAction << "\" is yet defined" << std::endl ;
                                                                break;
                                                        }
                                                }
                                                keysMenu.redraw ();
                                        }

                                        std::cout << "the key for \"" << this->whatKeyDoes << "\" was \"" << thatKey << "\" now is \"" << newKey << "\"" << std::endl ;

                                        keysMenu.setValueOf( choice->getText(), CreateKeysMenu::allegroKeyToMenuKey( newKey ) );

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

        choice->getFontToChange().setColor( InputManager::getInstance().getUserKeyFor( this->whatKeyDoes ) != "none" ? "white" : "cyan" );

        keysMenu.redraw ();
}

}
