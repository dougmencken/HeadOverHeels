
#include "InputManager.hpp"

#include "util.hpp"
#include <iostream>


InputManager* InputManager::instance = nilPointer ;

InputManager::InputManager( )
{
        userKeys[ " 1 movenorth" ] = "Left" ;   // move left, izquierda, or isometric north, norte isométrico
        userKeys[ " 2 movesouth" ] = "Right" ;  // move right, derecha, or isometric south, sur isométrico
        userKeys[  " 3 moveeast" ] = "Up" ;     // move up, arriba, or isometric east, este isométrico
        userKeys[  " 4 movewest" ] = "Down" ;   // move down, abajo, or isometric west, oeste isométrico
        userKeys[      " 5 jump" ] = "Space" ;  // saltar
        userKeys[      " 6 take" ] = "c" ;      // coger y dejar objetos
        userKeys[ " 7 take&jump" ] = "b" ;      // coger/dejar un objeto y saltar
        userKeys[  " 8 doughnut" ] = "n" ;      // release a doughnut
        userKeys[      " 9 swap" ] = "x" ;      // to the next character
        userKeys[     "10 pause" ] = "m" ;      // detener el juego
        userKeys[   "11 automap" ] = "Tab" ;    // show~hide automap
}

InputManager& InputManager::getInstance()
{
        if ( instance == nilPointer )
                instance = new InputManager( );

        return *instance;
}

void InputManager::getAllActions( std::vector< std::string > & listOfActions ) const
{
        for ( std::map< std::string, std::string >::const_iterator cit = userKeys.begin () ; cit != userKeys.end () ; ++ cit )
                listOfActions.push_back( cit->first.substr( 3 ) );
}

std::string InputManager::getUserKeyFor( const std::string & action ) const
{
        for ( std::map< std::string, std::string >::const_iterator cit = userKeys.begin () ; cit != userKeys.end () ; ++ cit )
        {
                if ( cit->first.substr( 3 ) == action )
                        return cit->second ;
        }

        return "none" ;
}

void InputManager::changeUserKey( const std::string & action, const std::string & key )
{
        for ( std::map< std::string, std::string >::const_iterator cit = userKeys.begin () ; cit != userKeys.end () ; ++ cit )
                if ( cit->first.substr( 3 ) == action ) {
                        userKeys[ cit->first ] = key ;
                        break ;
                }
}

bool InputManager::movenorthTyped() const
{
        return allegro::isKeyPushed( getUserKeyFor( "movenorth" ) )
                        && ! allegro::isKeyPushed( getUserKeyFor( "movesouth" ) )
                        && ! allegro::isKeyPushed( getUserKeyFor( "moveeast" ) )
                        && ! allegro::isKeyPushed( getUserKeyFor( "movewest" ) ) ;
}

bool InputManager::movesouthTyped() const
{
        return allegro::isKeyPushed( getUserKeyFor( "movesouth" ) )
                        && ! allegro::isKeyPushed( getUserKeyFor( "movenorth" ) )
                        && ! allegro::isKeyPushed( getUserKeyFor( "moveeast" ) )
                        && ! allegro::isKeyPushed( getUserKeyFor( "movewest" ) ) ;
}

bool InputManager::moveeastTyped() const
{
        return allegro::isKeyPushed( getUserKeyFor( "moveeast" ) )
                        && ! allegro::isKeyPushed( getUserKeyFor( "movenorth" ) )
                        && ! allegro::isKeyPushed( getUserKeyFor( "movesouth" ) )
                        && ! allegro::isKeyPushed( getUserKeyFor( "movewest" ) ) ;
}

bool InputManager::movewestTyped() const
{
        return allegro::isKeyPushed( getUserKeyFor( "movewest" ) )
                        && ! allegro::isKeyPushed( getUserKeyFor( "movenorth" ) )
                        && ! allegro::isKeyPushed( getUserKeyFor( "movesouth" ) )
                        && ! allegro::isKeyPushed( getUserKeyFor( "moveeast" ) ) ;
}

bool InputManager::jumpTyped() const
{
        return allegro::isKeyPushed( getUserKeyFor( "jump" ) );
}

bool InputManager::takeTyped() const
{
        return allegro::isKeyPushed( getUserKeyFor( "take" ) );
}

bool InputManager::doughnutTyped() const
{
        return allegro::isKeyPushed( getUserKeyFor( "doughnut" ) );
}

bool InputManager::takeAndJumpTyped() const
{
        return allegro::isKeyPushed( getUserKeyFor( "take&jump" ) );
}

bool InputManager::swapTyped() const
{
        return allegro::isKeyPushed( getUserKeyFor( "swap" ) );
}

bool InputManager::pauseTyped() const
{
        return allegro::isKeyPushed( getUserKeyFor( "pause" ) );
        ///return ( allegro::nextKey() == getUserKeyFor( "pause" ) );
}

bool InputManager::automapTyped() const
{
        return allegro::isKeyPushed( getUserKeyFor( "automap" ) );
}

void InputManager::releaseKeyFor( const std::string & keyAction ) const
{
        const std::string & key = getUserKeyFor( keyAction ) ;
        if ( key != "none" )
                allegro::releaseKey( key ) ;
}
