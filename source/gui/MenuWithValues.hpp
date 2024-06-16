// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef MenuWithValues_hpp_
#define MenuWithValues_hpp_

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
         * @param fill The symbol to fill the gap between the label and the value
         * @param minSpaces How many space letters to add before the value after the longest label
         */
        MenuWithValues( char fill = ' ', unsigned short minSpaces = 3 );

        virtual ~MenuWithValues( );

        virtual void draw () ;

        const std::vector < std::string > & getValues () const {  return this->listOfValues ;  }

        std::string getValueOf( const std::string & textOfOption ) const ;

        void setValueOf( const Label * const option, const std::string & value ) ;

        virtual unsigned int getWidthOfMenu () const ;

protected:

        virtual unsigned int getIndexOfActiveOption () const ;

private:

        /*
         * Values are stored here
         */
        std::vector < std::string > listOfValues ;

        char symbolToFill ;

        unsigned short minSpacesBeforeValue ;

};

}

#endif
