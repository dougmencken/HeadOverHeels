// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef WallPiece_hpp_
#define WallPiece_hpp_

#include <WrappersAllegro.hpp>

#include "Picture.hpp"
#include "Drawable.hpp"
#include "Mediated.hpp"


/**
 * The segment of the room’s wall
 */

class WallPiece : public Drawable, public Mediated
{

public:

        /**
         * @param trueXfalseY is this a piece of the wall along X or not
         * @param index where’s this piece on the wall, the number from zero onwards
         * @param pictureFile the name of picture file
         */
        WallPiece( bool trueXfalseY, int index, const std::string & pictureFile )
                : Mediated()
                , alongX( trueXfalseY )
                , position( index )
                , nameOfImage( pictureFile ) {}

        virtual ~WallPiece( ) {}

        virtual void draw () ;

        virtual void setMediator ( Mediator* mediator )
                {  Mediated::setMediator( mediator ); calculateOffset ();  }

        virtual bool operator < ( const WallPiece & other ) const
                {  return getPosition() < other.getPosition() ;  }

        static bool compareWallPointers ( const WallPiece * first, const WallPiece * second )
                {  return ( first != nilPointer && second != nilPointer ) ? *first < *second : false ;  }

private:

        // calculates the offset of this wall piece’s graphics in the room
        void calculateOffset () ;

        // if true, then this piece is part of the wall along X, otherwise along Y
        bool alongX ;

        // the position of this piece, the smaller the closer to the room’s origin
        int position ;

        std::pair < int, int > offset ;

        // name of the picture file for this part of the wall
        std::string nameOfImage ;

public:

        bool isAlongX () const {  return   this->alongX ;  }
        bool isAlongY () const {  return ! this->alongX ;  }

        int getPosition () const {  return this->position ;  }

        const std::string & getNameOfImage () const {  return this->nameOfImage ;  }

} ;

////typedef multiptr < WallPiece > WallPiecePtr ;

#endif
