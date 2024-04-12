// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef SaveGame_hpp_
#define SaveGame_hpp_

#include "Action.hpp"


namespace gui
{

class SaveGame : public Action
{

public:

        /**
         * @param picture Image where to draw the user interface
         * @param slot Number of file to load
         */
        explicit SaveGame( unsigned int slot ) ;

        virtual std::string getNameOfAction () const {  return "SaveGame" ;  }

protected:

        /**
         * Save the game and back to play
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
