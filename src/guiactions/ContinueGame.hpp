// The free and open source remake of Head over Heels
//
// Copyright © 2019 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef ContinueGame_h_
#define ContinueGame_h_

#include "Action.hpp"


namespace gui
{

class ContinueGame : public Action
{

public:

        /**
         * @param gameInProgress Indica si hay una partida en curso o comienza una nueva
         */
        explicit ContinueGame( bool gameInProgress ) ;

        virtual std::string getNameOfAction () const {  return "ContinueGame" ;  }

protected:

        /**
         * Begin the game
         */
        virtual void doAction () ;

private:

        /**
         * Indica si hay una partida en curso o comienza una nueva
         */
        bool gameInProgress ;

};

}

#endif
