// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef SelectLanguage_hpp_
#define SelectLanguage_hpp_

#include <string>
#include <allegro.h>
#include "Action.hpp"


namespace gui
{

class SelectLanguage: public Action
{

public:

        SelectLanguage( BITMAP* picture, const std::string& language ) ;

        std::string getNameOfAction () const {  return "SelectLanguage" ;  }

protected:

        /**
         * Set the language of the game
         */
        virtual void doAction () ;

private:

        BITMAP* where ;

        std::string language ;

};

}

#endif
