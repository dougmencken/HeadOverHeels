// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
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
#include "Picture.hpp"
#include "FreeItem.hpp"


class ItemDescriptions ;


/**
 * A door to the room. It is really the three free items, the two jambs and the lintel
 */

class Door : public Mediated
{

public:

       /**
        * @param label the label of door
        * @param cx the door's cell on X
        * @param cy the door's cell on Y
        * @param z the position on Z, that's how far from the ground
        * @param way where is the door, in the north of the room, in the south, in the west or in the east
        */
        Door( const std::string & label, int cx, int cy, int z, const std::string & where ) ;

        virtual ~Door( ) ;

        /**
         * Get image of lintel from image of door
         */
        static Picture * cutOutLintel ( const allegro::Pict & door, unsigned int widthX, unsigned int widthY, unsigned int height,
                                        unsigned int leftJambWidthX, unsigned int leftJambWidthY,
                                        unsigned int rightJambWidthX, unsigned int rightJambWidthY,
                                        const std::string& at ) ;

        /**
         * Get image of left jamb from image of door
         */
        static Picture * cutOutLeftJamb ( const allegro::Pict & door, unsigned int widthX, unsigned int widthY, unsigned int height,
                                                /* unsigned int lintelWidthX, */ unsigned int lintelWidthY, unsigned int lintelHeight,
                                                const std::string& at ) ;

        /**
         * Get image of right jamb from image of door
         */
        static Picture * cutOutRightJamb ( const allegro::Pict & door, unsigned int widthX, unsigned int widthY, unsigned int height,
                                                unsigned int lintelWidthX, /* unsigned int lintelWidthY, */ unsigned int lintelHeight,
                                                const std::string& at ) ;

private:

        std::string labelOfDoor ;

        int cellX ;

        int cellY ;

        int z ;

        int rightLimit ;

        int leftLimit ;

        std::string whereIsDoor ;

        Picture * leftJambImage ;

        Picture * rightJambImage ;

        Picture * lintelImage ;

        FreeItemPtr leftJamb ;

        FreeItemPtr rightJamb ;

        FreeItemPtr lintel ;

public:

        bool isUnderDoor ( int x, int y, int z ) const ;

        std::string getWhereIsDoor () const {  return whereIsDoor ;  }

        FreeItemPtr getLeftJamb () ;

        FreeItemPtr getRightJamb () ;

        FreeItemPtr getLintel () ;

        int getCellX () const {  return cellX ;  }

        int getCellY () const {  return cellY ;  }

        int getZ () const {  return z ;  }

        std::string getLabel() const {  return labelOfDoor ;  }

} ;

typedef multiptr < Door > DoorPtr ;

#endif
