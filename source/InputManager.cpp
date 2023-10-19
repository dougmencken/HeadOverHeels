
#include "InputManager.hpp"

#include "util.hpp"
#include <iostream>


InputManager* InputManager::instance = nilPointer ;

InputManager::InputManager( )
{
        userKeys.push_back( std::pair< std::string, std::string >( "movenorth", "Left" ) );   // tecla para mover el jugador al norte, izquierda
        userKeys.push_back( std::pair< std::string, std::string >( "movesouth", "Right" ) );  // tecla para mover el jugador al sur, derecha
        userKeys.push_back( std::pair< std::string, std::string >( "moveeast", "Up" ) );      // tecla para mover el jugador al este, arriba
        userKeys.push_back( std::pair< std::string, std::string >( "movewest", "Down" ) );    // tecla para mover el jugador al oeste, abajo
        userKeys.push_back( std::pair< std::string, std::string >( "jump", "Space" ) );       // tecla para saltar
        userKeys.push_back( std::pair< std::string, std::string >( "take", "c" ) );           // tecla para coger y dejar objetos
        userKeys.push_back( std::pair< std::string, std::string >( "take&jump", "b" ) );      // tecla para coger/dejar un objeto y saltar
        userKeys.push_back( std::pair< std::string, std::string >( "doughnut", "n" ) );       // tecla para disparar
        userKeys.push_back( std::pair< std::string, std::string >( "swap", "x" ) );           // tecla para cambiar de jugador
        userKeys.push_back( std::pair< std::string, std::string >( "pause", "m" ) );          // tecla para detener el juego
        userKeys.push_back( std::pair< std::string, std::string >( "automap", "Tab" ) );      // tecla para show~hide automap
}

InputManager& InputManager::getInstance()
{
        if ( instance == nilPointer )
                instance = new InputManager( );

        return *instance;
}

std::string InputManager::getUserKeyFor( const std::string& action ) const
{
        for ( std::vector< std::pair< std::string, std::string > >::const_iterator cit = userKeys.begin () ; cit != userKeys.end () ; ++ cit )
        {
                if ( cit->first == action )
                        return cit->second ;
        }

        return "unknown" ;
}

std::string InputManager::getActionOfKeyByName( const std::string& keyName ) const
{
        for ( std::vector< std::pair< std::string, std::string > >::const_iterator cit = userKeys.begin () ; cit != userKeys.end () ; ++ cit )
        {
                if ( cit->second == keyName )
                        return cit->first ;
        }

        return "none" ;
}

void InputManager::changeUserKey( const std::string& action, const std::string& name )
{
        for ( std::vector< std::pair< std::string, std::string > >::iterator it = userKeys.begin () ; it != userKeys.end () ; ++ it )
        {
                if ( it->first == action )
                        it->second = name ;
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

bool InputManager::anyMoveTyped() const
{
        return allegro::isKeyPushed( getUserKeyFor( "movenorth" ) ) ||
                allegro::isKeyPushed( getUserKeyFor( "movesouth" ) ) ||
                allegro::isKeyPushed( getUserKeyFor( "moveeast" ) ) ||
                allegro::isKeyPushed( getUserKeyFor( "movewest" ) ) ;
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
}

bool InputManager::automapTyped() const
{
        return allegro::isKeyPushed( getUserKeyFor( "automap" ) );
}

void InputManager::releaseKeyFor( const std::string& keyAction ) const
{
        const std::string& key = getUserKeyFor( keyAction ) ;
        if ( key != "unknown" )
                allegro::releaseKey( key ) ;
}
