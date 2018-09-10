// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef InputManager_hpp_
#define InputManager_hpp_

#include <map>
#include <string>

#include <WrappersAllegro.hpp>

#include "Ism.hpp"


namespace isomot
{

/**
 * Gestor de los dispositivos de entrada. Almacena las teclas empleadas para el control del
 * juego así como la configuración del joystick ( siempre que haya uno conectado al sistema )
 */

class InputManager
{

public:

        static const size_t numberOfKeys = 10 ;

        static const std::string namesOfKeys[ ] ;

        static const std::string nameOfAbsentKey ;

private:

        InputManager( ) ;

public:

        ~InputManager( ) ;

       /**
        * Único objeto de esta clase para toda la aplicación
        * @return Un puntero al objeto único
        */
        static InputManager * getInstance () ;

       /**
        * Asigna el código de una tecla definida por el usuario para el control del juego
        * @param gameKey Tipo de tecla
        * @param scancode Un código Allegro de una tecla
        */
        void changeUserKey ( const std::string& nameOfKey, int scancode ) {  this->userKeys[ nameOfKey ] = scancode ;  }

       /**
        * Devuelve el código de una tecla definida por el usuario para el control del juego
        * @param gameKey Tipo de tecla
        * @return Un código Allegro de una tecla
        */
        int getUserKey ( const std::string& nameOfKey ) {  return this->userKeys[ nameOfKey ] ;  }

        std::string findNameOfKeyByCode ( int scancode ) ;

       /**
        * Move character to the north
        * @return true if user types this key or moves joystick there
        */
        bool movenorth () ;

       /**
        * Move character to the south
        * @return true if user types this key or moves joystick there
        */
        bool movesouth () ;

       /**
        * Move character to the east
        * @return true if user types this key or moves joystick there
        */
        bool moveeast () ;

       /**
        * Move character to the west
        * @return true if user types this key or moves joystick there
        */
        bool movewest () ;

       /**
        * Take or drop some object
        * @return true if user types this key or joystick button
        */
        bool take () ;

       /**
        * Character jumps
        * @return true if user types this key or joystick button
        */
        bool jump () ;

        bool takeAndJump () ;

       /**
        * Release doughnut
        * @return true if user types this key or joystick button
        */
        bool doughnut () ;

       /**
        * Change characters
        * @return true if user types this key or joystick button
        */
        bool swap () ;

       /**
        * Pause game
        * @return true if user types this key or joystick button
        */
        bool pause () ;

       /**
        * Deletes stored keystrokes for a key
        * @param keyName A key associated with an action
        */
        void noRepeat ( const std::string& keyName ) ;

private:

       /**
        * Único objeto de esta clase para toda la aplicación
        */
        static InputManager * instance ;

       /**
        * Associative map with name of key and Allegro "scan code" for keys defined by user
        */
        std::map < std::string, int > userKeys ;

};

}

#endif
