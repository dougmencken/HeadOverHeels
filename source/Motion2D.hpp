// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Motion2D_hpp_
#define Motion2D_hpp_

class Motion2D
{

private:

        int alongX ;
        int alongY ;

public:

        Motion2D( int dx, int dy )
                : alongX( dx )
                , alongY( dy )
        {}

        Motion2D( const Motion2D & that )
                : alongX( that.alongX )
                , alongY( that.alongY )
        {}

        virtual ~Motion2D( ) {}

        static Motion2D* newRest () {  return new Motion2D( 0, 0 ) ;  }

        static Motion2D* newMovingSouth () {  return new Motion2D( 1, 0 ) ;  }
        static Motion2D* newMovingWest () {  return new Motion2D( 0, 1 ) ;  }
        static Motion2D* newMovingNorth () {  return new Motion2D( -1, 0 ) ;  }
        static Motion2D* newMovingEast () {  return new Motion2D( 0, -1 ) ;  }

        int getMotionAlongX () const {  return this->alongX ;  }
        int getMotionAlongY () const {  return this->alongY ;  }

        bool isRest () const
        {
                return getMotionAlongX() == 0 && getMotionAlongY() == 0 ;
        }

        bool isMovingNorth () const {  return getMotionAlongX() < 0 ;  }
        bool isMovingOnlyNorth () const {  return isMovingNorth() && getMotionAlongY() == 0 ;  }

        bool isMovingSouth () const {  return getMotionAlongX() > 0 ;  }
        bool isMovingOnlySouth () const {  return isMovingSouth() && getMotionAlongY() == 0 ;  }

        bool isMovingEast () const {  return getMotionAlongY() < 0 ;  }
        bool isMovingOnlyEast () const {  return isMovingEast() && getMotionAlongX() == 0 ;  }

        bool isMovingWest () const {  return getMotionAlongY() > 0 ;  }
        bool isMovingOnlyWest () const {  return isMovingWest() && getMotionAlongX() == 0 ;  }

} ;

#endif
