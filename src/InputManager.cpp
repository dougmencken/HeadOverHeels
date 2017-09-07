
#include "InputManager.hpp"
#include <iostream>

using isomot::InputManager;


InputManager* InputManager::instance = 0 ;

const std::string InputManager::namesOfKeys[ ] =

                        {  "movenorth", "movesouth", "moveeast", "movewest",
                           "take", "jump", "doughnut", "take-jump",
                           "swap", "halt"  } ;

const std::string InputManager::nameOfAbsentKey = "unknown" ;


InputManager::InputManager()
{
        userKeys.insert( std::pair< std::string, int >( "movenorth", KEY_LEFT ) );   // Tecla para mover el jugador al norte, izquierda
        userKeys.insert( std::pair< std::string, int >( "movesouth", KEY_RIGHT ) );  // Tecla para mover el jugador al sur, derecha
        userKeys.insert( std::pair< std::string, int >( "moveeast", KEY_UP ) );      // Tecla para mover el jugador al este, arriba
        userKeys.insert( std::pair< std::string, int >( "movewest", KEY_DOWN ) );    // Tecla para mover el jugador al oeste, abajo
        userKeys.insert( std::pair< std::string, int >( "take", KEY_C ) );           // Tecla para coger y dejar objetos
        userKeys.insert( std::pair< std::string, int >( "jump", KEY_SPACE ) );       // Tecla para saltar
        userKeys.insert( std::pair< std::string, int >( "doughnut", KEY_N ) );       // Tecla para disparar
        userKeys.insert( std::pair< std::string, int >( "take-jump", KEY_B ) );      // Tecla para coger/dejar un objeto y saltar
        userKeys.insert( std::pair< std::string, int >( "swap", KEY_X ) );           // Tecla para cambiar de jugador
        userKeys.insert( std::pair< std::string, int >( "halt", KEY_M ) );           // Tecla para detener el juego

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
        if ( instance == 0 )
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

bool InputManager::take()
{
        return key[ getUserKey( "take" ) ];
}

bool InputManager::jump()
{
        return key[ getUserKey( "jump" ) ];
}

bool InputManager::doughnut()
{
        return key[ getUserKey( "doughnut" ) ];
}

bool InputManager::takeAndJump()
{
        return key[ getUserKey( "take-jump" ) ];
}

bool InputManager::swap()
{
        return key[ getUserKey( "swap" ) ];
}

bool InputManager::halt()
{
        return key[ getUserKey( "halt" ) ];
}

void InputManager::noRepeat( const std::string& nameOfKey )
{
        key[ getUserKey( nameOfKey ) ] = 0;
}
