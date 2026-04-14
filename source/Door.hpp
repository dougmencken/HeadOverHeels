// The free and open source remake of Head over Heels
//
// Copyright © 2026 Douglas Mencken dougmencken@gmail.com
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

public :

       /**
        * @param kind the kind of door
        * @param cx the door's cell on X
        * @param cy the door's cell on Y
        * @param z the position on Z, that's how far from the ground
        * @param on the side of the room where the door is
        */
        Door( const std::string & kind, int cx, int cy, int z, const std::string & on ) ;

        virtual ~Door( ) {}

        const std::string & getRoomSide () const {  return this->onWhichSide ;  }

        const FreeItemPtr & getLeftJamb () ;

        const FreeItemPtr & getRightJamb () ;

        const FreeItemPtr & getLintel () ;

        int getCellX () const {  return this->cellX ;  }

        int getCellY () const {  return this->cellY ;  }

        int getElevation () const {  return this->elevation ;  }

        const std::string & getKind () const {  return this->kindOfDoor ;  }

        bool isUnderDoor ( int x, int y, int z ) const ;

        bool isUnderDoor ( const FreeItem & item ) const
        {
                return isUnderDoor( item.getX(), item.getY(), item.getZ() );
        }

private :

        // the door item’s kind is %scenery%-door-%on%
        std::string kindOfDoor ;

        // the room’s grid cell where this door is located
        int cellX ;
        int cellY ;

        // how far is this door from the ground
        int elevation ;

        // on which side of the room is this door located
        // for a single room, the sides are north, south, west or east
        std::string onWhichSide ;

        int leftLimit ;
        int rightLimit ;

        FreeItemPtr leftJamb ;
        FreeItemPtr rightJamb ;
        FreeItemPtr lintel ;

        // useful for doors in big rooms
        std::string onWhichSideOfTheFour () const
        {
                const std::string & side = this->onWhichSide ;

                if ( side == "northeast" || side == "northwest" ) return "north" ;
                if ( side == "southeast" || side == "southwest" ) return "south" ;
                if ( side == "eastnorth" || side == "eastsouth" ) return "east" ;
                if ( side == "westnorth" || side == "westsouth" ) return "west" ;

                return side ;
        }

        /**
         * Get the image of lintel from the image of door
         */
        static NamedPicture * cutOutLintel ( const NamedPicturePtr & doorImage,
                                                unsigned int widthX, unsigned int widthY, unsigned int height,
                                                unsigned int leftJambWidthX, unsigned int leftJambWidthY,
                                                unsigned int rightJambWidthX, unsigned int rightJambWidthY,
                                                const std::string & on ) ;
        /**
         * Get the image of left jamb from the image of door
         */
        static NamedPicture * cutOutLeftJamb ( const NamedPicturePtr & doorImage,
                                                unsigned int widthX, unsigned int widthY, unsigned int height,
                                                /* unsigned int lintelWidthX, */ unsigned int lintelWidthY, unsigned int lintelHeight,
                                                const std::string & on ) ;
        /**
         * Get the image of right jamb from the image of door
         */
        static NamedPicture * cutOutRightJamb ( const NamedPicturePtr & doorImage,
                                                unsigned int widthX, unsigned int widthY, unsigned int height,
                                                unsigned int lintelWidthX, /* unsigned int lintelWidthY, */ unsigned int lintelHeight,
                                                const std::string & on ) ;

} ;

typedef multiptr < Door > DoorPtr ;

#endif
