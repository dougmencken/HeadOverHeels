// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
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
         * @param inProgress false for a completely new game
         */
        explicit ContinueGame( bool inProgress )
                : Action( )
                , gameInProgress( inProgress ) {}

protected:

        /**
         * continue or begin the game
         */
        virtual void act () ;

private:

        /**
         * is game already in progress (true) or (false) it is a new game that's beginning
         */
        bool gameInProgress ;

};

}

#endif
