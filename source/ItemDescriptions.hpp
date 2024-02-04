// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef ItemDescriptions_hpp_
#define ItemDescriptions_hpp_

#include <string>
#include <map>

#include <tinyxml2.h>

#include "DescriptionOfItem.hpp"


/**
 * All the descriptions of the game's items as read from items.xml
 */

class ItemDescriptions
{

public:

        static const std::string The_File_Full_Of_Item_Descriptions ;

        ItemDescriptions( ) : alreadyRead( false ), descriptionsOfItems( ) { }

        virtual ~ItemDescriptions( ) ;

        /**
         * Read the descriptions of items from the XML file
         */
        void readDescriptionsFromFile ( const std::string & nameOfXMLFile, bool reRead = false ) ;

        /**
         * @return the description of item or nil if there's no description for such an item
         */
        const DescriptionOfItem * getDescriptionByKind ( const std::string & kind ) /* const */ ;

        static ItemDescriptions & descriptions () {  return *theDescriptions ;  }

private:

        void readDescriptionFurther( const tinyxml2::XMLElement & item, DescriptionOfItem & description ) ;

        static autouniqueptr< ItemDescriptions > theDescriptions ;

        bool alreadyRead ;

        std::map < std::string, const DescriptionOfItem * > descriptionsOfItems ;

        void binDescriptions () ;

} ;

#endif
