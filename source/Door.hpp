// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Door_hpp_
#define Door_hpp_

#include "Way.hpp"
#include "Mediated.hpp"
#include "NamedPicture.hpp"
#include "FreeItem.hpp"


class ItemDescriptions ;


/**
 * A door to the room. It is really the three free items, the two jambs and the lintel
 */

class Door : public Mediated
{

public:

       /**
        * @param kind the kind of door
        * @param cx the door's cell on X
        * @param cy the door's cell on Y
        * @param z the position on Z, that's how far from the ground
        * @param where is the door, in the north of the room, in the south, in the west or in the east
        */
        Door( const std::string & kind, int cx, int cy, int z, const std::string & where ) ;

        virtual ~Door( ) ;

        const std::string & getWhereIsDoor () const {  return this->whereIsDoor ;  }

        const FreeItemPtr & getLeftJamb () ;

        const FreeItemPtr & getRightJamb () ;

        const FreeItemPtr & getLintel () ;

        int getCellX () const {  return this->cellX ;  }

        int getCellY () const {  return this->cellY ;  }

        int getZ () const {  return this->theZ ;  }

        const std::string & getKind () const {  return this->kindOfDoor ;  }

        bool isUnderDoor ( int x, int y, int z ) const ;

        bool isUnderDoor ( const FreeItem & item ) const
        {
                return isUnderDoor( item.getX(), item.getY(), item.getZ() );
        }

private:

        std::string kindOfDoor ;

        int cellX ;

        int cellY ;

        int theZ ;

        std::string whereIsDoor ;

        int rightLimit ;

        int leftLimit ;

        NamedPicture * leftJambImage ;

        NamedPicture * rightJambImage ;

        NamedPicture * lintelImage ;

        FreeItemPtr leftJamb ;

        FreeItemPtr rightJamb ;

        FreeItemPtr lintel ;

        /**
         * Get the image of lintel from the image of door
         */
        static NamedPicture * cutOutLintel ( const allegro::Pict & door, unsigned int widthX, unsigned int widthY, unsigned int height,
                                                unsigned int leftJambWidthX, unsigned int leftJambWidthY,
                                                unsigned int rightJambWidthX, unsigned int rightJambWidthY,
                                                const std::string& at ) ;

        /**
         * Get the image of left jamb from the image of door
         */
        static NamedPicture * cutOutLeftJamb ( const allegro::Pict & door, unsigned int widthX, unsigned int widthY, unsigned int height,
                                                /* unsigned int lintelWidthX, */ unsigned int lintelWidthY, unsigned int lintelHeight,
                                                const std::string& at ) ;

        /**
         * Get the image of right jamb from the image of door
         */
        static NamedPicture * cutOutRightJamb ( const allegro::Pict & door, unsigned int widthX, unsigned int widthY, unsigned int height,
                                                unsigned int lintelWidthX, /* unsigned int lintelWidthY, */ unsigned int lintelHeight,
                                                const std::string& at ) ;

} ;

typedef multiptr < Door > DoorPtr ;

#endif
