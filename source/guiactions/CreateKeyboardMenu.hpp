// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CreateKeyboardMenu_hpp_
#define CreateKeyboardMenu_hpp_

#include "Action.hpp"

namespace gui
{

class MenuWithValues ;


/**
 * Create "select the keys" menu
 */

class CreateKeyboardMenu : public Action
{

public:

        CreateKeyboardMenu( ) : Action( ), menuOfKeys( nilPointer ) { }

        virtual std::string getNameOfAction () const {  return "CreateKeyboardMenu" ;  }

protected:

        /**
         * Show the keys menu
         */
        virtual void doAction () ;

private:

        MenuWithValues * menuOfKeys ;

};

}

#endif
