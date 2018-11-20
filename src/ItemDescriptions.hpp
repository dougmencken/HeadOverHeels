// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef ItemDescriptions_hpp_
#define ItemDescriptions_hpp_

#include <string>
#include <map>

#include "DescriptionOfItem.hpp"


namespace iso
{

class ItemDescriptions
{

public:

        ItemDescriptions( ) { }

        virtual ~ItemDescriptions( ) ;

        /**
         * Load description of items from XML file
         */
        void readDescriptionOfItemsFrom ( const std::string& nameOfXMLFile ) ;

        void freeDescriptionOfItems () ;

        /**
         * @return description of item or nil when it is absent
         */
        const DescriptionOfItem * getDescriptionByLabel ( const std::string& label ) const ;

private:

        std::map < std::string, const DescriptionOfItem * > descriptionOfItems ;

};

}

#endif
