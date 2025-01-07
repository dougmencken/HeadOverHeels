// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
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

class SlideWithHeadAndHeels ;


class CreateGraphicsAreaSizeMenu : public ActionWithHandlingKeys
{

public:

        CreateGraphicsAreaSizeMenu( Action* previous )
                : ActionWithHandlingKeys()
                , menuOfScreenSizes( )
                , actionOnEscape( previous )
        {
                this->menuOfScreenSizes.setVerticalOffset( 40 );
        }

        virtual ~CreateGraphicsAreaSizeMenu( ) {}

        virtual void handleKey ( const std::string & key ) ;

        static std::vector< std::pair< unsigned int, unsigned int > > popularScreenSizes ;

protected:

        virtual void act () ;

private:

        void fillSlide ( SlideWithHeadAndHeels & slideToFill ) ;

        void pickCustomSize () ;

        // note that it deletes all the previously created slides to update them to the new size
        static bool applySize ( const std::string & chosenWidth, const std::string & chosenHeight ) ;

        void updateOptions () ;

        Menu menuOfScreenSizes ;

        Label * customSize ;

        Action * actionOnEscape ;

};

}

#endif
