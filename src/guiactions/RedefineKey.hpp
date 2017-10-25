// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef RedefineKey_hpp_
#define RedefineKey_hpp_

#include <string>
#include <allegro.h>
#ifdef __WIN32
  #include <winalleg.h>
#endif
#include "Action.hpp"


namespace gui
{

class MenuWithValues ;

/**
 * Cambia una tecla para el control del juego por otra indicada por el usuario
 */

class RedefineKey : public Action
{

public:

       /**
        * @param name Name of key like "jump" or "movesouth"
        */
        RedefineKey( MenuWithValues * menu, std::string name ) ;

        std::string getNameOfAction () const {  return "RedefineKey" ;  }

protected:

        virtual void doAction () ;

private:

        MenuWithValues * menu ;

        std::string nameOfKey ;

};

}

#endif
