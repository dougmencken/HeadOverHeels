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

#include "WrappersAllegro.hpp"

#include "Ism.hpp"


namespace iso
{

class InputManager
{

public:

        static const size_t numberOfKeys = 10 ;

        static const std::string actionsOfKeys[ ] ;

private:

        InputManager( ) ;

public:

        ~InputManager( ) ;

        static InputManager & getInstance () ;

       /**
        * Returns name of key defined by user for given action
        */
        std::string getUserKeyFor ( const std::string& action ) {  return this->userKeys[ action ] ;  }

        void changeUserKey ( const std::string& action, const std::string& name ) {  this->userKeys[ action ] = name ;  }

        std::string findActionOfKeyByName ( const std::string& keyName ) ;

       /**
        * Move character to the north
        */
        bool movenorthTyped () ;

       /**
        * Move character to the south
        */
        bool movesouthTyped () ;

       /**
        * Move character to the east
        */
        bool moveeastTyped () ;

       /**
        * Move character to the west
        */
        bool movewestTyped () ;

        bool anyMoveTyped () ;

       /**
        * Take or drop some object
        */
        bool takeTyped () ;

       /**
        * Character jumps
        */
        bool jumpTyped () ;

        bool takeAndJumpTyped () ;

       /**
        * Release doughnut
        */
        bool doughnutTyped () ;

       /**
        * Swap characters
        */
        bool swapTyped () ;

       /**
        * Pause game
        */
        bool pauseTyped () ;

        void releaseKeyFor ( const std::string& keyAction ) ;

private:

       /**
        * Único objeto de esta clase para toda la aplicación
        */
        static InputManager * instance ;

       /**
        * Associative map of action of key to key’s name for keys chosen by user
        */
        std::map < /* action */ std::string, /* name */ std::string > userKeys ;

};

}

#endif
