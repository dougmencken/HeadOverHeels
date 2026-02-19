
#include "WallPiece.hpp"

#include "Mediator.hpp"
#include "PoolOfPictures.hpp"


/* private */
void WallPiece::calculateOffset()
{
        assert( getMediator() != nilPointer );

        Room & room = getMediator()->getRoom() ;
        int oneCell = static_cast< int >( room.getSizeOfOneCell() );

        int x = room.getX0 ();
        int y = room.getY0 ();

        if ( isAlongX() )
                x += ( this->position << 1 ) * oneCell + 1 ;
        else
                x += 1 - oneCell * ( ( this->position + 2 ) << 1 ) ;

        PicturePtr image = PoolOfPictures::getPoolOfPictures().getOrLoadAndGet( this->nameOfImageFile );
        y += ( this->position + 1 ) * oneCell - image->getHeight() - 1 ;

        if ( isAlongX() && ( room.hasDoorOn( "east" ) ||
                                room.hasDoorOn( "eastnorth" ) || room.hasDoorOn( "eastsouth" )
                                        || ! room.hasFloor() ) ) {
                x -= ( oneCell << 1 );
                y += oneCell ;
        }

        if ( isAlongY() && ( room.hasDoorOn( "north" ) ||
                                room.hasDoorOn( "northeast" ) || room.hasDoorOn( "northwest" )
                                        || ! room.hasFloor() ) ) {
                x += ( oneCell << 1 );
                y += oneCell ;
        }

        this->offset.first = x ;
        this->offset.second = y ;
}

void WallPiece::draw ()
{
        PicturePtr image = PoolOfPictures::getPoolOfPictures().getOrLoadAndGet( this->nameOfImageFile );
        if ( image != nilPointer )
                allegro::drawSprite( image->getAllegroPict(), this->offset.first, this->offset.second );
}
