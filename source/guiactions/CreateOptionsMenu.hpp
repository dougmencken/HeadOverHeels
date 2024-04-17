// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CreateOptionsMenu_hpp_
#define CreateOptionsMenu_hpp_

#include "Action.hpp"


namespace gui
{

class CreateOptionsMenu : public Action
{

public:

        CreateOptionsMenu( ) : Action() {}

protected:

        virtual void act () ;

};

}

#endif
