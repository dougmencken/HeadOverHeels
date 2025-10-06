// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef DescriptionOfDoor_hpp_
#define DescriptionOfDoor_hpp_

#include "DescriptionOfItem.hpp"


/**
 * The description of a door
 *
 * For a door, the item kind is %scenery%-door-%on% with the image file %scenery%-door-%on%.png
 */

class DescriptionOfDoor : public DescriptionOfItem
{

public:

        static const unsigned int  WIDTH_OF_DOOR_IMAGE = 82 ;
        static const unsigned int HEIGHT_OF_DOOR_IMAGE = 140 ;

        DescriptionOfDoor( const std::string & scene, const std::string & where ) ;

        virtual ~DescriptionOfDoor () ;

private:

        std::string scenery ;

        std::string doorOn ;

        // the three parts of door are the lintel, the left jamb and the right jamb
        DescriptionOfItem * lintel ;
        DescriptionOfItem * leftJamb ;
        DescriptionOfItem * rightJamb ;

        DescriptionOfItem * cloneAsLintel() ;
        DescriptionOfItem * cloneAsLeftJamb() ;
        DescriptionOfItem * cloneAsRightJamb() ;

public:

        const DescriptionOfItem * getLintel () const {  return this->lintel ;  }
        const DescriptionOfItem * getLeftJamb () const {  return this->leftJamb ;  }
        const DescriptionOfItem * getRightJamb () const {  return this->rightJamb ;  }

        const std::string & getScenery () const {  return this->scenery ;  }

        const std::string & getWhereOn () const {  return this->doorOn ;  }

} ;

#endif
