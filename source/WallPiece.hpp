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

        WallPiece( bool trueXfalseY, int index, Picture* image ) ;

        virtual ~WallPiece( ) ;

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

        bool onX;

        /**
         * Position of this piece on the wall, the smaller the closer to the room’s origin
         */
        int position ;

        std::pair < int, int > offset ;

        /**
         * Graphics of this wall segment
         */
        Picture * image ;

public:

        bool isOnX () const {  return onX ;  }

        bool isOnY () const {  return ! onX ;  }

        int getPosition () const {  return position ;  }

        Picture * getImage () const {  return image ;  }

} ;

////typedef multiptr < WallPiece > WallPiecePtr ;

#endif
