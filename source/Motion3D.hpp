// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Motion3D_hpp_
#define Motion3D_hpp_

#include "Motion2D.hpp"

class Motion3D : public Motion2D
{

private:

        int alongZ ;

public:

        Motion3D( int dx, int dy, int dz )
                : Motion2D( dx, dy )
                , alongZ( dz )
        {}

        Motion3D( const Motion3D & that )
                : Motion2D( that )
                , alongZ( that.alongZ )
        {}

        Motion3D( const Motion2D & that )
                : Motion2D( that )
                , alongZ( 0 )
        {}

        virtual ~Motion3D( ) {}

        static Motion3D restWithGravity () {  return Motion3D( 0, 0, -1 ) ;  }
        static Motion3D fallingDown () {  return restWithGravity() ;  }

        static Motion3D glidingSouthWithGravity () {  return Motion3D( 1, 0, -1 ) ;  }
        static Motion3D glidingWestWithGravity () {  return Motion3D( 0, 1, -1 ) ;  }
        static Motion3D glidingNorthWithGravity () {  return Motion3D( -1, 0, -1 ) ;  }
        static Motion3D glidingEastWithGravity () {  return Motion3D( 0, -1, -1 ) ;  }

        int getMotionAlongZ () const {  return this->alongZ ;  }

        Motion2D to2D () const
        {
                return Motion2D( getMotionAlongX(), getMotionAlongY() );
        }

        bool isRest () const
        {
                return getMotionAlongX() == 0 && getMotionAlongY() == 0 && getMotionAlongZ() == 0 ;
        }

        bool isFallingDown () const
        {
                return getMotionAlongX() == 0 && getMotionAlongY() == 0 && getMotionAlongZ() < 0 ;
        }

        bool isRestWithGravity () const {  return isFallingDown() ;  }

        bool isMovingOnlyNorth () const
        {
                return isMovingNorth() && getMotionAlongY() == 0 && getMotionAlongZ() == 0 ;
        }

        bool isMovingOnlySouth () const
        {
                return isMovingSouth() && getMotionAlongY() == 0 && getMotionAlongZ() == 0 ;
        }

        bool isMovingOnlyEast () const
        {
                return isMovingEast() && getMotionAlongX() == 0 && getMotionAlongZ() == 0 ;
        }

        bool isMovingOnlyWest () const
        {
                return isMovingWest() && getMotionAlongX() == 0 && getMotionAlongZ() == 0 ;
        }

} ;

#endif
