// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CreateVideoMenu_hpp_
#define CreateVideoMenu_hpp_

#include "Action.hpp"

#include <allegro.h>


namespace gui
{

class Menu;
class Label;

/**
 * Create menu to modify video & graphics
 */

class CreateVideoMenu : public Action
{

public:

        /**
         * Constructor
         * @param picture Image where to draw
         */
        CreateVideoMenu( BITMAP* picture ) ;

        std::string getNameOfAction () const {  return "CreateVideoMenu" ;  }

        static const size_t positionOfSetting = 20 ;

protected:

        /**
         * Show this menu
         */
        virtual void doAction () ;

private:

        void updateLabels () ;

        BITMAP * where ;

        Menu * listOfOptions ;

        Label * labelFullscreen ;
        Label * labelDrawShadows ;
        Label * labelDrawBackground ;
        Label * labelGraphicSet ;

};

}

#endif
