// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CreateGraphicsAreaSizeMenu_hpp_
#define CreateGraphicsAreaSizeMenu_hpp_

#include "ActionWithHandlingKeys.hpp"

#include "Menu.hpp"
#include "Label.hpp"
#include "Timer.hpp"

#include <string>
#include <vector>


namespace gui
{

class CreateGraphicsAreaSizeMenu : public ActionWithHandlingKeys
{

public:

        CreateGraphicsAreaSizeMenu( Action* previous )
                : ActionWithHandlingKeys()
                , actionOnEscape( previous ) {}

        virtual ~CreateGraphicsAreaSizeMenu( ) {}

        virtual void handleKey ( const std::string & key ) ;

        static std::vector< std::pair< unsigned int, unsigned int > > popularScreenSizes ;

protected:

        virtual void act () ;

private:

        void fillSlide ( Slide & slideToFill ) ;

        void pickCustomSize () ;

        void updateOptions () ;

        Menu * menuOfScreenSizes ;

        Label * customSize ;

        Timer customSizeBlinkTimer ;

        bool pickingWidth ; // true for the width, false for the height

        Action * actionOnEscape ;

};

}

#endif
