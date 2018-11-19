// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef MenuWithValues_hpp_
#define MenuWithValues_hpp_

#include <vector>
#include "Menu.hpp"


namespace gui
{

class Label ;

/**
 * Menu where every option has its value
 */

class MenuWithValues : public Menu
{

public:

        /**
         * @param fill Symbol to fill gaps between label and value
         * @param minSpaces How many space letters to place before value after longest label
         */
        MenuWithValues( char fill = ' ', unsigned short minSpaces = 3 );

        virtual ~MenuWithValues( );

        virtual void draw () ;

        std::vector < std::string > getValues () {  return this->listOfValues ;  }

        std::string getValueOf( Label* option ) const ;

        void setValueOf( Label* option, const std::string& value ) ;

        virtual unsigned int getWidthOfMenu () const ;

protected:

        virtual void previousOption () ;

        virtual void nextOption () ;

        /*
         * Values are stored here
         */
        std::vector < std::string > listOfValues ;

        char symbolToFill ;

        unsigned short minSpacesBeforeValue ;

};

}

#endif
