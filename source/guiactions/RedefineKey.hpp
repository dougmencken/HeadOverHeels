// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef RedefineKey_hpp_
#define RedefineKey_hpp_

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
        * @param keyAction what the key does, like "jump" or "movesouth"
        */
        RedefineKey( MenuWithValues * menu, const std::string & keyAction ) ;

        virtual std::string getNameOfAction () const {  return "RedefineKey" ;  }

protected:

        virtual void doAction () ;

private:

        MenuWithValues * menu ;

        std::string whatKeyDoes ;

};

}

#endif
