
#include "InputManager.hpp"
#include <iostream>

using isomot::InputManager;


InputManager* InputManager::instance = nilPointer ;

const std::string InputManager::namesOfKeys[ ] =

                        {  "movenorth", "movesouth", "moveeast", "movewest",
                           "jump", "take", "take&jump", "doughnut",
                           "swap", "pause"  } ;

const std::string InputManager::nameOfAbsentKey = "unknown" ;


InputManager::InputManager()
{
        userKeys.insert( std::pair< std::string, int >( "movenorth", KEY_LEFT ) );   // tecla para mover el jugador al norte, izquierda
        userKeys.insert( std::pair< std::string, int >( "movesouth", KEY_RIGHT ) );  // tecla para mover el jugador al sur, derecha
        userKeys.insert( std::pair< std::string, int >( "moveeast", KEY_UP ) );      // tecla para mover el jugador al este, arriba
        userKeys.insert( std::pair< std::string, int >( "movewest", KEY_DOWN ) );    // tecla para mover el jugador al oeste, abajo
        userKeys.insert( std::pair< std::string, int >( "jump", KEY_SPACE ) );       // tecla para saltar
        userKeys.insert( std::pair< std::string, int >( "take", KEY_C ) );           // tecla para coger y dejar objetos
        userKeys.insert( std::pair< std::string, int >( "take&jump", KEY_B ) );      // tecla para coger/dejar un objeto y saltar
        userKeys.insert( std::pair< std::string, int >( "doughnut", KEY_N ) );       // tecla para disparar
        userKeys.insert( std::pair< std::string, int >( "swap", KEY_X ) );           // tecla para cambiar de jugador
        userKeys.insert( std::pair< std::string, int >( "pause", KEY_M ) );          // tecla para detener el juego

        std::cout << "InputManager provides by default\n" ;
        for ( std::map< std::string, int >::const_iterator iter = userKeys.begin (); iter != userKeys.end (); ++iter )
        { // iter->first is name of key, iter->second is its scancode
                std::cout << "   key with name \"" << iter->first
                          << "\" paired to " << scancode_to_name( iter->second ) << " ( " << iter->second << " )"
                          << std::endl ;
        }
}

InputManager::~InputManager()
{
}

InputManager* InputManager::getInstance()
{
        if ( instance == nilPointer )
        {
                instance = new InputManager();
        }

        return instance;
}

std::string InputManager::findNameOfKeyByCode( int scanCode )
{
        for ( std::map< std::string, int >::const_iterator iter = userKeys.begin (); iter != userKeys.end (); ++iter )
        {
                if ( iter->second == scanCode )
                {
                        return iter->first;
                }
        }

        return InputManager::nameOfAbsentKey ;
}

bool InputManager::movenorth()
{
        return key[ getUserKey( "movenorth" ) ];
}

bool InputManager::movesouth()
{
        return key[ getUserKey( "movesouth" ) ];
}

bool InputManager::moveeast()
{
        return key[ getUserKey( "moveeast" ) ];
}

bool InputManager::movewest()
{
        return key[ getUserKey( "movewest" ) ];
}

bool InputManager::jump()
{
        return key[ getUserKey( "jump" ) ];
}

bool InputManager::take()
{
        return key[ getUserKey( "take" ) ];
}

bool InputManager::doughnut()
{
        return key[ getUserKey( "doughnut" ) ];
}

bool InputManager::takeAndJump()
{
        return key[ getUserKey( "take&jump" ) ];
}

bool InputManager::swap()
{
        return key[ getUserKey( "swap" ) ];
}

bool InputManager::pause()
{
        return key[ getUserKey( "pause" ) ];
}

void InputManager::noRepeat( const std::string& nameOfKey )
{
        key[ getUserKey( nameOfKey ) ] = false;
}
