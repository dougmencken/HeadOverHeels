// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef TheMainMenuSlide_hpp_
#define TheMainMenuSlide_hpp_

#include "SlideWithHeadAndHeels.hpp"
#include "TheMainMenu.hpp"


namespace gui
{

class TheMainMenuSlide : public SlideWithHeadAndHeels
{

public :

        TheMainMenuSlide( )
                : SlideWithHeadAndHeels( )
                , mainMenu( )
        {
                placeHeadAndHeels( /* icons */ true, /* copyrights */ true );

                this->mainMenu.setVerticalOffset( 12 );

                addWidget( & this->mainMenu );
                setKeyHandler( & this->mainMenu );
        }

private :

        TheMainMenu mainMenu ;

} ;

}

#endif
