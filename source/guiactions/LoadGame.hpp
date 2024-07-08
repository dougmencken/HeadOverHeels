// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef LoadGame_hpp_
#define LoadGame_hpp_

#include "Action.hpp"


namespace gui
{

class LoadGame : public Action
{

public:

        /**
         * @param slot Number of file to load
         */
        explicit LoadGame( unsigned int slot )
                : Action( )
                , slot( slot ) {}

        virtual ~LoadGame( ) {}

protected:

        /**
         * Load the game and begin it
         */
        virtual void act () ;

private:

        /**
         * Number of file to load
         */
        unsigned int slot ;

};

}

#endif
