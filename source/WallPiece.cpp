
#include "WallPiece.hpp"

#include "Mediator.hpp"
#include "PoolOfPictures.hpp"


void WallPiece::calculateOffset()
{
        assert( getMediator() != nilPointer );

        Room * room = getMediator()->getRoom() ;

        this->offset.first  = room->getX0 ();
        this->offset.second = room->getY0 ();

        int tileSize = static_cast< int >( room->getSizeOfOneTile() );

        if ( isAlongX() )
                this->offset.first += ( this->position << 1 ) * tileSize + 1 ;
        else
                this->offset.first += 1 - tileSize * ( ( this->position + 2 ) << 1 ) ;

        PicturePtr image = PoolOfPictures::getPoolOfPictures().getOrLoadAndGet( this->nameOfImage );
        this->offset.second += ( this->position + 1 ) * tileSize - image->getHeight() - 1 ;

        if ( isAlongX() && ( room->hasDoorOn( "east" ) ||
                                room->hasDoorOn( "eastnorth" ) || room->hasDoorOn( "eastsouth" )
                                        || ! room->hasFloor() ) ) {
                this->offset.first -= ( tileSize << 1 );
                this->offset.second += tileSize ;
        }

        if ( isAlongY() && ( room->hasDoorOn( "north" ) ||
                                room->hasDoorOn( "northeast" ) || room->hasDoorOn( "northwest" )
                                        || ! room->hasFloor() ) ) {
                this->offset.first += ( tileSize << 1 );
                this->offset.second += tileSize ;
        }
}

void WallPiece::draw ()
{
        PicturePtr image = PoolOfPictures::getPoolOfPictures().getOrLoadAndGet( this->nameOfImage );
        if ( image != nilPointer )
                allegro::drawSprite( image->getAllegroPict(), this->offset.first, this->offset.second );
}
