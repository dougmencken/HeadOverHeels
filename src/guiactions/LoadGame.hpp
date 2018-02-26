// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
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
         * Constructor
         * @param picture Image where to draw user interface
         * @param slot Number of file to load
         */
        LoadGame( BITMAP * picture, int slot ) ;

        std::string getNameOfAction () const {  return "LoadGame" ;  }

protected:

        /**
         * Load the game and begin it
         */
        virtual void doAction () ;

private:

        /**
         * Number of file to load
         */
        int slot ;

};

}

#endif
