// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CreateMenuOfGraphicsSets_hpp_
#define CreateMenuOfGraphicsSets_hpp_

#include "Action.hpp"

#include <map>


namespace gui
{

class Menu;

/**
 * Create menu with list of graphic sets
 */

class CreateMenuOfGraphicsSets : public Action
{

public:

        /**
         * @param previous Action to do on escape
         */
        explicit CreateMenuOfGraphicsSets( Action * previous ) ;

        virtual ~CreateMenuOfGraphicsSets( ) ;

protected:

        /**
         * Show this menu
         */
        virtual void act () ;

private:

        Action * actionOnEscape ;

        Menu * menuOfGraphicsSets ;

        std::map < std::string, std::string > setsOfGraphics ;

};

}

#endif
