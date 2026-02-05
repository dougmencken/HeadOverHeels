// The free and open source remake of Head over Heels
//
// Copyright © 2026 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef TheMainMenu_hpp_
#define TheMainMenu_hpp_

#include "Menu.hpp"

namespace gui
{

class Label ;


class TheMainMenu : public Menu
{

public :

        TheMainMenu( ) ;

        virtual ~TheMainMenu( ) {}

private :

        Label *const newGame ;
        Label *const loadGame ;
        Label *const optionsMenu ;
        Label *const showCredits ;
        Label *const quitGame ;

} ;

}

#endif
