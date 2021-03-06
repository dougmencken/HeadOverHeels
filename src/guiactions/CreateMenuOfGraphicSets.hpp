// The free and open source remake of Head over Heels
//
// Copyright © 2019 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CreateMenuOfGraphicSets_hpp_
#define CreateMenuOfGraphicSets_hpp_

#include "Action.hpp"

#include <map>


namespace gui
{

class Menu;

/**
 * Create menu with list of graphic sets
 */

class CreateMenuOfGraphicSets : public Action
{

public:

        /**
         * @param previous Action to do on escape
         */
        explicit CreateMenuOfGraphicSets( Action * previous ) ;

        virtual ~CreateMenuOfGraphicSets( ) ;

        virtual std::string getNameOfAction () const {  return "CreateMenuOfGraphicSets" ;  }

protected:

        /**
         * Show this menu
         */
        virtual void doAction () ;

private:

        Action * actionOnEscape ;

        Menu * menuOfGraphicSets;

        std::map < std::string, std::string > graphicSets ;

};

}

#endif
