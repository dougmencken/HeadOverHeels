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

#include "Picture.hpp"
#include "Drawable.hpp"

#include <map>
#include <string>


class Color ;

class Room ;
class ConnectedRooms ;


class Miniature : public Drawable
{

public :

        Miniature( const Room & roomForMiniature, int leftX, int topY, unsigned int sizeOfTileForMiniature ) ;

        virtual ~Miniature( ) ;

        virtual void draw () ;

        void drawVignetteForRoomAboveOrBelow ( const allegro::Pict& where, int midX, int aboveY, int belowY, const Color& color, bool drawAbove, bool drawBelow ) ;

        void drawEastDoorOnMiniature ( const allegro::Pict & where, int x0, int y0, unsigned int tileX, unsigned int tileY, const Color& color ) ;
        void drawSouthDoorOnMiniature ( const allegro::Pict & where, int x0, int y0, unsigned int tileX, unsigned int tileY, const Color& color ) ;
        void drawNorthDoorOnMiniature ( const allegro::Pict & where, int x0, int y0, unsigned int tileX, unsigned int tileY, const Color& color ) ;
        void drawWestDoorOnMiniature ( const allegro::Pict & where, int x0, int y0, unsigned int tileX, unsigned int tileY, const Color& color ) ;

        void drawIsoSquare ( const allegro::Pict & where, int x0, int y0, unsigned int tilesX, unsigned int tilesY, const Color& color ) ;

        void drawIsoTile ( const allegro::Pict & where, int x0, int y0, int tileX, int tileY, const Color & color, bool loX, bool loY, bool hiX, bool hiY ) ;

        void fillIsoTile ( const allegro::Pict & where, int x0, int y0, int tileX, int tileY, const Color & color ) ;

        void fillIsoTileInside ( const allegro::Pict & where, int x0, int y0, int tileX, int tileY, const Color & color, bool fullFill ) ;

        const std::pair< int, int > & getEastDoorNorthernCorner () const {  return this->eastDoorNorthernCorner ;  }
        const std::pair< int, int > & getWestDoorNorthernCorner () const {  return this->westDoorNorthernCorner ;  }
        const std::pair< int, int > & getNorthDoorEasternCorner () const {  return this->northDoorEasternCorner ;  }
        const std::pair< int, int > & getSouthDoorEasternCorner () const {  return this->southDoorEasternCorner ;  }

        void setEastDoorNorthernCorner( int x, int y ) {  this->eastDoorNorthernCorner = std::pair< int, int >( x, y ) ;  }
        void setWestDoorNorthernCorner( int x, int y ) {  this->westDoorNorthernCorner = std::pair< int, int >( x, y ) ;  }
        void setNorthDoorEasternCorner( int x, int y ) {  this->northDoorEasternCorner = std::pair< int, int >( x, y ) ;  }
        void setSouthDoorEasternCorner( int x, int y ) {  this->southDoorEasternCorner = std::pair< int, int >( x, y ) ;  }

        std::pair < int, int > calculatePositionOfConnectedMiniature ( const std::string & where, unsigned short gap = 1 ) ;

        std::pair < unsigned int, unsigned int > getImageSize () const
        {
                if ( theMiniature == nilPointer ) return std::pair < unsigned int, unsigned int >( 0, 0 ) ;

                return std::pair < unsigned int, unsigned int >( this->theMiniature->getWidth(), this->theMiniature->getHeight() ) ;
        }

        unsigned int getSizeOfTile () const {  return this->sizeOfTile ;  }

        void setSizeOfTile ( unsigned int newSize ) {  this->sizeOfTile = ( newSize >= 2 ) ? newSize : 2 ;  }

        const Room & getRoom () const {  return room ;  }

protected :

        std::pair < unsigned int, unsigned int > calculateSize () const ;

        void composeMiniature () ;

private :

        Picture * theMiniature ;

        const Room & room ;

        unsigned int sizeOfTile ;

        std::pair < int, int > offset ;

        std::pair < int, int > northDoorEasternCorner ;
        std::pair < int, int > eastDoorNorthernCorner ;
        std::pair < int, int > southDoorEasternCorner ;
        std::pair < int, int > westDoorNorthernCorner ;

} ;

#endif
