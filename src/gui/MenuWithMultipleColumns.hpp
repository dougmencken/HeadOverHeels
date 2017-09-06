// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef MenuWithMultipleColumns_hpp_
#define MenuWithMultipleColumns_hpp_

#include "Menu.hpp"


namespace gui
{

/*
 * Two-column menu
 */

class MenuWithMultipleColumns : public Menu
{

public:

        /**
         * Constructor
         * @param space Space between columns
         */
        MenuWithMultipleColumns( unsigned int space );

        virtual ~MenuWithMultipleColumns( );

        void draw ( BITMAP* where ) ;

        virtual unsigned int getWidthOfMenu () const ;

        virtual unsigned int getHeightOfMenu () const ;

private:

        unsigned int spaceBetweenColumns;

};

}

#endif
