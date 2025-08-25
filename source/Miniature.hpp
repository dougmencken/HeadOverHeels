// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
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

class Color ;


/**
 * A miniature of game’s room
 */

class Miniature : public Drawable
{

public :

        Miniature( const Room & roomForMiniature, unsigned short singleTileSize = the_default_size_of_tile, bool withRoomInfo = false ) ;

        virtual ~Miniature( ) {  binTheImage() ;  }

        virtual void draw () ;

        void drawVignetteAboveOrBelow ( const allegro::Pict& where, int midX, int aboveY, int belowY, const Color& color, bool drawAbove, bool drawBelow ) ;

        void drawEastDoorOnMiniature ( const allegro::Pict & where, unsigned int tileX, unsigned int tileY, const Color & color ) ;
        void drawSouthDoorOnMiniature ( const allegro::Pict & where, unsigned int tileX, unsigned int tileY, const Color & color ) ;
        void drawNorthDoorOnMiniature ( const allegro::Pict & where, unsigned int tileX, unsigned int tileY, const Color & color ) ;
        void drawWestDoorOnMiniature ( const allegro::Pict & where, unsigned int tileX, unsigned int tileY, const Color & color ) ;

        void drawIsoSquare ( const allegro::Pict & where, std::pair< int, int > origin, unsigned int tilesX, unsigned int tilesY, const Color & color ) ;
        void drawIsoSquare ( const allegro::Pict & where, unsigned int tilesX, unsigned int tilesY, const Color & color )
                {  drawIsoSquare( where, getOriginOfRoom(), tilesX, tilesY, color ) ;  }

        void drawIsoTile ( const allegro::Pict & where, std::pair< int, int > origin, int tileX, int tileY, const Color & color, bool loX, bool loY, bool hiX, bool hiY ) ;
        void drawIsoTile ( const allegro::Pict & where, int tileX, int tileY, const Color & color, bool loX, bool loY, bool hiX, bool hiY )
                {  drawIsoTile( where, getOriginOfRoom(), tileX, tileY, color, loX, loY, hiX, hiY ) ;  }

        void fillIsoTile ( const allegro::Pict & where, std::pair< int, int > origin, int tileX, int tileY, const Color & color ) ;
        void fillIsoTile ( const allegro::Pict & where, int tileX, int tileY, const Color & color )
                {  fillIsoTile( where, getOriginOfRoom(), tileX, tileY, color ) ;  }

        void fillIsoTileInside ( const allegro::Pict & where, std::pair< int, int > origin, int tileX, int tileY, const Color & color, bool fullFill ) ;
        void fillIsoTileInside ( const allegro::Pict & where, int tileX, int tileY, const Color & color, bool fullFill )
                {  fillIsoTileInside( where, getOriginOfRoom(), tileX, tileY, color, fullFill ) ;  }

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
                return std::pair< int, int >( this->room.getTilesOnY() * ( getSizeOfTile() << 1 ), 0 ) ;
        }

        unsigned short getSizeOfTile () const {  return this->sizeOfTile ;  }

        void setSizeOfTile ( unsigned short newSize )
        {
                     if ( newSize < 2 ) newSize = 2 ;
                else if ( newSize > 16 ) newSize = 16 ;

                if ( newSize == this->sizeOfTile ) return ;

                this->sizeOfTile = newSize ;
                binTheImage() ;
        }

        const std::pair< int, int > & getOffsetOnScreen () const {  return this->offsetOnScreen ;  }

        void setOffsetOnScreen ( int leftX, int topY ) ;

        bool connectMiniature ( Miniature * that, const std::string & where, short gap = 0 ) ;

        static const short the_default_size_of_tile = 3 ;

        static const int room_info_shift_x = 2 ;
        static const int room_info_shift_y = 18 ;

protected :

        std::pair < unsigned int, unsigned int > calculateSize () const ;

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

        unsigned short sizeOfTile ; // 2...16

        bool withTextAboutRoom ;
        bool isThereRoomAbove ;

        std::pair < int, int > offsetOnScreen ;

        std::pair < int, int > northDoorEasternCorner ;
        std::pair < int, int > eastDoorNorthernCorner ;
        std::pair < int, int > southDoorEasternCorner ;
        std::pair < int, int > westDoorNorthernCorner ;

        static const int corner_not_set = 0x55555000 ;

} ;

#endif
