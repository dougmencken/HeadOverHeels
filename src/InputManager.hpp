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
#include <allegro.h>
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
        * El usuario está moviendo al jugador a la izquierda
        * @return true si pulsa la tecla indicada o mueve la palanca del joystick en ese sentido
        */
        bool movenorth () ;

       /**
        * El usuario está moviendo al jugador a la derecha
        * @return true si pulsa la tecla indicada o mueve la palanca del joystick en ese sentido
        */
        bool movesouth () ;

       /**
        * El usuario está moviendo al jugador arriba
        * @return true si pulsa la tecla indicada o mueve la palanca del joystick en ese sentido
        */
        bool moveeast () ;

       /**
        * El usuario está moviendo al jugador abajo
        * @return true si pulsa la tecla indicada o mueve la palanca del joystick en ese sentido
        */
        bool movewest () ;

       /**
        * El usuario está cogiendo o dejando algún objeto
        * @return true si pulsa la tecla o el botón indicado
        */
        bool take () ;

       /**
        * El usuario está saltando
        * @return true si pulsa la tecla o el botón indicado
        */
        bool jump () ;

       /**
        * El usuario está disparando
        * @return true si pulsa la tecla o el botón indicado
        */
        bool doughnut () ;

       /**
        * El usuario está cogiendo o dejando algún objeto y saltando
        * @return true si pulsa la tecla o el botón indicado
        */
        bool takeAndJump () ;

       /**
        * El usuario está cogiendo cambiando de personaje
        * @return true si pulsa la tecla o el botón indicado
        */
        bool swap () ;

       /**
        * El usuario está parando el juego
        * @return true si pulsa la tecla o el botón indicado
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
