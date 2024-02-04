// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Switch_hpp_
#define Switch_hpp_

#include <vector>

#include "Behavior.hpp"


namespace behaviors
{

class Switch : public Behavior
{

public:

        Switch( const ItemPtr & item, const std::string & behavior ) ;

        virtual ~Switch( ) ;

        virtual bool update () ;

private:

       /**
        * See if there are items near the switch
        * @param Where to put items
        * @return true if any item was found or false otherwise
        */
        bool lookForItemsNearby ( std::vector< ItemPtr > & itemsNearby ) ;

private:

        bool isItemAbove ;

        /**
         * Items that triggered this switch
         */
        std::vector< ItemPtr > triggers ;

};

}

#endif
