// The free and open source remake of Head over Heels
//
// Copyright © 2026 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Miniature_hpp_
#define Miniature_hpp_

#include "WrappersAllegro.hpp"

#include "Drawable.hpp"
#include "NamedPicture.hpp"
#include "Room.hpp"

#include <map>
#include <string>

#include <pthread.h>

class Color ;


/**
 * A miniature of game’s room
 */

class Miniature : public Drawable
{

public :

        Miniature( const Room & roomForMiniature, unsigned short sizeOfSquare = the_default_square_size, bool withRoomInfo = false ) ;

        virtual ~Miniature( ) {  binTheImage() ;  }

        virtual void draw () ;

        void drawVignetteAboveOrBelow ( const allegro::Pict& where, int midX, int aboveY, int belowY, const Color& color, bool drawAbove, bool drawBelow ) ;

        void drawEastDoorOnMiniature ( const allegro::Pict & where, unsigned int x, unsigned int y, const Color & color ) ;
        void drawSouthDoorOnMiniature ( const allegro::Pict & where, unsigned int x, unsigned int y, const Color & color ) ;
        void drawNorthDoorOnMiniature ( const allegro::Pict & where, unsigned int x, unsigned int y, const Color & color ) ;
        void drawWestDoorOnMiniature ( const allegro::Pict & where, unsigned int x, unsigned int y, const Color & color ) ;

        void drawBigIsoRectangle ( const allegro::Pict & where, std::pair< int, int > origin, unsigned int squaresX, unsigned int squaresY, const Color & color ) ;
        void drawBigIsoRectangle ( const allegro::Pict & where, unsigned int squaresX, unsigned int squaresY, const Color & color )
                {  drawBigIsoRectangle( where, getOriginOfRoom(), squaresX, squaresY, color ) ;  }

        void drawIsoSquare ( const allegro::Pict & where, std::pair< int, int > origin, int x, int y, const Color & color, bool loX, bool loY, bool hiX, bool hiY ) ;
        void drawIsoSquare ( const allegro::Pict & where, int x, int y, const Color & color, bool loX, bool loY, bool hiX, bool hiY )
                {  drawIsoSquare( where, getOriginOfRoom(), x, y, color, loX, loY, hiX, hiY ) ;  }

        void fillIsoSquare ( const allegro::Pict & where, std::pair< int, int > origin, int x, int y, const Color & color ) ;
        void fillIsoSquare ( const allegro::Pict & where, int x, int y, const Color & color )
                {  fillIsoSquare( where, getOriginOfRoom(), x, y, color ) ;  }

        void fillIsoSquareInside ( const allegro::Pict & where, std::pair< int, int > origin, int x, int y, const Color & color, bool fullFill ) ;
        void fillIsoSquareInside ( const allegro::Pict & where, int x, int y, const Color & color, bool fullFill )
                {  fillIsoSquareInside( where, getOriginOfRoom(), x, y, color, fullFill ) ;  }

        const std::pair< int, int > & getEastDoorNorthernCorner ()
                {  if ( this->theImage == nilPointer ) composeImage () ;  return this->eastDoorNorthernCorner ;  }
        const std::pair< int, int > & getWestDoorNorthernCorner ()
                {  if ( this->theImage == nilPointer ) composeImage () ;  return this->westDoorNorthernCorner ;  }
        const std::pair< int, int > & getNorthDoorEasternCorner ()
                {  if ( this->theImage == nilPointer ) composeImage () ;  return this->northDoorEasternCorner ;  }
        const std::pair< int, int > & getSouthDoorEasternCorner ()
                {  if ( this->theImage == nilPointer ) composeImage () ;  return this->southDoorEasternCorner ;  }

        void setEastDoorNorthernCorner( int x, int y ) {  this->eastDoorNorthernCorner = std::pair< int, int >( x, y ) ;  }
        void setWestDoorNorthernCorner( int x, int y ) {  this->westDoorNorthernCorner = std::pair< int, int >( x, y ) ;  }
        void setNorthDoorEasternCorner( int x, int y ) {  this->northDoorEasternCorner = std::pair< int, int >( x, y ) ;  }
        void setSouthDoorEasternCorner( int x, int y ) {  this->southDoorEasternCorner = std::pair< int, int >( x, y ) ;  }

        NamedPicture & getImage ()
        {
                if ( this->theImage == nilPointer ) composeImage () ;
                return * this->theImage ;
        }

        unsigned int getImageWidth () const
        {
                return ( this->theImage != nilPointer ) ? this->theImage->getWidth() : 0 ;
        }

        unsigned int getImageHeight () const
        {
                return ( this->theImage != nilPointer ) ? this->theImage->getHeight() : 0 ;
        }

        const Room & getRoom () const {  return room ;  }

        std::pair< int, int > getOriginOfRoom () const
        {
                return std::pair< int, int >( this->room.getCellsAlongY() * ( getSquareSize() << 1 ), 0 ) ;
        }

        unsigned short getSquareSize () const {  return this->squareSize ;  }

        void setSquareSize ( unsigned short newSize )
        {
                     if ( newSize < 2 ) newSize = 2 ;
                else if ( newSize > 16 ) newSize = 16 ;

                if ( newSize != this->squareSize ) {
                        this->squareSize = newSize ;
                        binTheImage() ;
                }
        }

        const std::pair< int, int > & getDrawingOffset () const {  return this->drawingOffset ;  }

        void setDrawingOffset ( int leftX, int topY ) ;

        bool connectMiniature ( Miniature * that, const std::string & where, short gap = 0 ) ;

        static const short the_default_square_size = 3 ;

        static const int room_info_shift_x = 2 ;
        static const int room_info_shift_y = 18 ;

protected :

        std::pair < unsigned int, unsigned int > calculateSize () const ;

        void createImage () ;

        void composeImage () ;

private :

        NamedPicture * theImage ;

        void binTheImage ()
        {
                if ( this->theImage == nilPointer ) return ;

                delete this->theImage ;
                this->theImage = nilPointer ;
        }

        const Room & room ;

        // the size of a single square, in pixels
        unsigned short squareSize ; // 2...16

        bool withTextAboutRoom ;
        bool textAboutRoomIsWritten ;

        bool isThereRoomAbove ;
        bool isThereRoomBelow ;

        std::pair < int, int > drawingOffset ;

        std::pair < int, int > northDoorEasternCorner ;
        std::pair < int, int > eastDoorNorthernCorner ;
        std::pair < int, int > southDoorEasternCorner ;
        std::pair < int, int > westDoorNorthernCorner ;

        static const int corner_not_set = 0x55555000 ;

} ;

#endif
