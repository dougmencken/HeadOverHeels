// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
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

        static Motion2D rest () {  return Motion2D( 0, 0 ) ;  }

        static Motion2D movingSouth () {  return Motion2D( 1, 0 ) ;  }
        static Motion2D movingWest () {  return Motion2D( 0, 1 ) ;  }
        static Motion2D movingNorth () {  return Motion2D( -1, 0 ) ;  }
        static Motion2D movingEast () {  return Motion2D( 0, -1 ) ;  }

        static Motion2D movingNortheast () {  return Motion2D( -1, -1 ) ;  }
        static Motion2D movingNorthwest () {  return Motion2D( -1, 1 ) ;  }
        static Motion2D movingSoutheast () {  return Motion2D( 1, -1 ) ;  }
        static Motion2D movingSouthwest () {  return Motion2D( 1, 1 ) ;  }

        int getMotionAlongX () const {  return this->alongX ;  }
        int getMotionAlongY () const {  return this->alongY ;  }

        bool equals ( const Motion2D & that )
        {
                return this->alongX == that.alongX && this->alongY == that.alongY ;
        }
        bool operator == ( const Motion2D & that ) {  return   this->equals( that ) ;  }
        bool operator != ( const Motion2D & that ) {  return ! this->equals( that ) ;  }

        Motion2D& add ( const Motion2D & that )
        {
                this->alongX += that.alongX ;
                this->alongY += that.alongY ;
                return *this ;
        }

        Motion2D& multiplyBy ( double multiplier )
        {
                this->alongX *= multiplier ;
                this->alongY *= multiplier ;
                return *this ;
        }

        Motion2D& reverse ()
        {
                this->alongX = - this->alongX ;
                this->alongY = - this->alongY ;
                return *this ;
        }

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

        bool isMovingNortheast () const {  return isMovingNorth() && isMovingEast() ;  }
        bool isMovingNorthwest () const {  return isMovingNorth() && isMovingWest() ;  }
        bool isMovingSoutheast () const {  return isMovingSouth() && isMovingEast() ;  }
        bool isMovingSouthwest () const {  return isMovingSouth() && isMovingWest() ;  }

        void resetX () {  this->alongX = 0 ;  }
        void resetY () {  this->alongY = 0 ;  }

} ;

#endif
