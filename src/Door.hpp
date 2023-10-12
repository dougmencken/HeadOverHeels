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


namespace iso
{

class ItemDescriptions ;


/**
 * A door to the room. It is really three free items: two jambs and lintel
 */

class Door : public Mediated
{

public:

       /**
        * @param itemDescriptions to find three parts of door
        * @param label label of door
        * @param cx cell of door on X
        * @param cy cell of door on Y
        * @param z position on Z or how far is item from ground
        * @param way orientation of door
        */
        Door( const ItemDescriptions & itemDescriptions, const std::string & label, int cx, int cy, int z, const std::string & way ) ;

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

        const ItemDescriptions & itemDescriptions ;

        std::string labelOfDoor ;

        int cellX ;

        int cellY ;

        int z ;

        int rightLimit ;

        int leftLimit ;

        std::string positionOfDoor ;

        Picture * leftJambImage ;

        Picture * rightJambImage ;

        Picture * lintelImage ;

        FreeItemPtr leftJamb ;

        FreeItemPtr rightJamb ;

        FreeItemPtr lintel ;

public:

        bool isUnderDoor ( int x, int y, int z ) const ;

        std::string getWhereIsDoor () const {  return positionOfDoor ;  }

        FreeItemPtr getLeftJamb () ;

        FreeItemPtr getRightJamb () ;

        FreeItemPtr getLintel () ;

        int getCellX () const {  return cellX ;  }

        int getCellY () const {  return cellY ;  }

        int getZ () const {  return z ;  }

        std::string getLabel() const {  return labelOfDoor ;  }

};

typedef safeptr < Door > DoorPtr ;

}

#endif
