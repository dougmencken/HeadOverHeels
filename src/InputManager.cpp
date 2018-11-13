
#include "InputManager.hpp"
#include <iostream>

using iso::InputManager;


InputManager* InputManager::instance = nilPointer ;

const std::string InputManager::actionsOfKeys[ ] =

                        {  "movenorth", "movesouth", "moveeast", "movewest",
                           "jump", "take", "take&jump", "doughnut",
                           "swap", "pause"  } ;


InputManager::InputManager()
{
        userKeys.insert( std::pair< std::string, std::string >( "movenorth", "Left" ) );   // tecla para mover el jugador al norte, izquierda
        userKeys.insert( std::pair< std::string, std::string >( "movesouth", "Right" ) );  // tecla para mover el jugador al sur, derecha
        userKeys.insert( std::pair< std::string, std::string >( "moveeast", "Up" ) );      // tecla para mover el jugador al este, arriba
        userKeys.insert( std::pair< std::string, std::string >( "movewest", "Down" ) );    // tecla para mover el jugador al oeste, abajo
        userKeys.insert( std::pair< std::string, std::string >( "jump", "Space" ) );       // tecla para saltar
        userKeys.insert( std::pair< std::string, std::string >( "take", "c" ) );           // tecla para coger y dejar objetos
        userKeys.insert( std::pair< std::string, std::string >( "take&jump", "b" ) );      // tecla para coger/dejar un objeto y saltar
        userKeys.insert( std::pair< std::string, std::string >( "doughnut", "n" ) );       // tecla para disparar
        userKeys.insert( std::pair< std::string, std::string >( "swap", "x" ) );           // tecla para cambiar de jugador
        userKeys.insert( std::pair< std::string, std::string >( "pause", "m" ) );          // tecla para detener el juego
}

InputManager::~InputManager()
{
}

InputManager& InputManager::getInstance()
{
        if ( instance == nilPointer )
        {
                instance = new InputManager();
        }

        return *instance;
}

std::string InputManager::findActionOfKeyByName( const std::string& keyName )
{
        for ( std::map< std::string, std::string >::const_iterator iter = userKeys.begin (); iter != userKeys.end (); ++iter )
        {
                if ( iter->second == keyName )
                        return iter->first;
        }

        return "unknown" ;
}

bool InputManager::movenorthTyped()
{
        return allegro::isKeyPushed( getUserKeyFor( "movenorth" ) )
                && ! allegro::isKeyPushed( getUserKeyFor( "movesouth" ) )
                && ! allegro::isKeyPushed( getUserKeyFor( "moveeast" ) )
                && ! allegro::isKeyPushed( getUserKeyFor( "movewest" ) ) ;
}

bool InputManager::movesouthTyped()
{
        return allegro::isKeyPushed( getUserKeyFor( "movesouth" ) )
                && ! allegro::isKeyPushed( getUserKeyFor( "movenorth" ) )
                && ! allegro::isKeyPushed( getUserKeyFor( "moveeast" ) )
                && ! allegro::isKeyPushed( getUserKeyFor( "movewest" ) ) ;
}

bool InputManager::moveeastTyped()
{
        return allegro::isKeyPushed( getUserKeyFor( "moveeast" ) )
                && ! allegro::isKeyPushed( getUserKeyFor( "movenorth" ) )
                && ! allegro::isKeyPushed( getUserKeyFor( "movesouth" ) )
                && ! allegro::isKeyPushed( getUserKeyFor( "movewest" ) ) ;
}

bool InputManager::movewestTyped()
{
        return allegro::isKeyPushed( getUserKeyFor( "movewest" ) )
                && ! allegro::isKeyPushed( getUserKeyFor( "movenorth" ) )
                && ! allegro::isKeyPushed( getUserKeyFor( "movesouth" ) )
                && ! allegro::isKeyPushed( getUserKeyFor( "moveeast" ) ) ;
}

bool InputManager::anyMoveTyped()
{
        return allegro::isKeyPushed( getUserKeyFor( "movenorth" ) ) ||
                allegro::isKeyPushed( getUserKeyFor( "movesouth" ) ) ||
                allegro::isKeyPushed( getUserKeyFor( "moveeast" ) ) ||
                allegro::isKeyPushed( getUserKeyFor( "movewest" ) ) ;
}

bool InputManager::jumpTyped()
{
        return allegro::isKeyPushed( getUserKeyFor( "jump" ) );
}

bool InputManager::takeTyped()
{
        return allegro::isKeyPushed( getUserKeyFor( "take" ) );
}

bool InputManager::doughnutTyped()
{
        return allegro::isKeyPushed( getUserKeyFor( "doughnut" ) );
}

bool InputManager::takeAndJumpTyped()
{
        return allegro::isKeyPushed( getUserKeyFor( "take&jump" ) );
}

bool InputManager::swapTyped()
{
        return allegro::isKeyPushed( getUserKeyFor( "swap" ) );
}

bool InputManager::pauseTyped()
{
        return allegro::isKeyPushed( getUserKeyFor( "pause" ) );
}

void InputManager::releaseKeyFor( const std::string& keyAction )
{
        if ( userKeys.find( keyAction ) != userKeys.end () )
                allegro::releaseKey( userKeys[ keyAction ] ) ;
}
