#include "InputManager.hpp"

using isomot::GameKey;
using isomot::InputManager;

InputManager* InputManager::instance = 0;

InputManager::InputManager()
{
  // Asigna una configuración por defecto al teclado
  userKeys.push_back( KEY_LEFT );
  userKeys.push_back( KEY_RIGHT );
  userKeys.push_back( KEY_UP );
  userKeys.push_back( KEY_DOWN );
  userKeys.push_back( KEY_M );
  userKeys.push_back( KEY_SPACE );
  userKeys.push_back( KEY_N );
  userKeys.push_back( KEY_V );
  userKeys.push_back( KEY_G );
  userKeys.push_back( KEY_ESC );
}

InputManager::~InputManager()
{
}

InputManager* InputManager::getInstance()
{
  if( instance == 0 )
  {
    instance = new InputManager();
  }

  return instance;
}

GameKey InputManager::findKeyType( int scancode ) const
{
  GameKey result = KeyNone;

  for( size_t i = 0; i < userKeys.size(); i++ )
  {
    if( userKeys[ i ] == scancode )
    {
      result = static_cast< GameKey >(i);
      break;
    }
  }

  return result;
}

bool InputManager::left()
{
  return key[ this->userKeys[ KeyNorth ] ];
}

bool InputManager::right()
{
  return key[ this->userKeys[ KeySouth ] ];
}

bool InputManager::up()
{
  return key[ this->userKeys[ KeyEast ] ];
}

bool InputManager::down()
{
  return key[ this->userKeys[ KeyWest ] ];
}

bool InputManager::take()
{
  return key[ this->userKeys[ KeyTake ] ];
}

bool InputManager::jump()
{
  return key[ this->userKeys[ KeyJump ] ];
}

bool InputManager::shoot()
{
  return key[ this->userKeys[ KeyShoot ] ];
}

bool InputManager::takeAndJump()
{
  return key[ this->userKeys[ KeyTakeAndJump ] ];
}

bool InputManager::swap()
{
  return key[ this->userKeys[ KeySwap ] ];
}

bool InputManager::halt()
{
  return key[ this->userKeys[ KeyHalt ] ];
}

void InputManager::noRepeat(const GameKey& gameKey)
{
  key[ this->userKeys[ gameKey ] ] = 0;
}
