// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef ItemDataManager_hpp_
#define ItemDataManager_hpp_

#include <string>
#include <list>
#include <iostream>

#include "ItemData.hpp"


namespace iso
{

/**
 * Manages items of game
 */

class ItemDataManager
{

public:

        ItemDataManager( ) { }

        virtual ~ItemDataManager( ) ;

        /**
         * Load data about items from XML file with definitions of these items
         */
        void readDataAboutItems ( const std::string& nameOfXMLFile ) ;

        void freeDataAboutItems () ;

        /**
         * Look for item by label
         * @return data of item or nil when not found
         */
        const ItemData * findDataByLabel ( const std::string& label ) const ;

private:

        std::list < const ItemData * > dataOfItems ;

};

}

#endif
