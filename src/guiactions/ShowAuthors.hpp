// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef ShowAuthors_hpp_
#define ShowAuthors_hpp_

#include <allegro.h>
#include "Action.hpp"


namespace gui
{

class TextField;

/**
 * Show authors of this game
 */

class ShowAuthors : public Action
{

public:

        /**
         * Constructor
         * @param picture Where to draw the graphical interface
         */
        ShowAuthors( BITMAP* picture ) ;

        void doIt () ;

        std::string getNameOfAction ()  {  return "ShowAuthors" ;  }

private:

        BITMAP * where ;

        TextField * linesOfCredits ;

        unsigned int initialY ;

};

}

#endif
