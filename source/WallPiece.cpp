
#include "WallPiece.hpp"

#include "Mediator.hpp"


WallPiece::WallPiece( bool trueXfalseY, int index, Picture* image )
        : Mediated()
        , onX( trueXfalseY )
        , position( index )
        , image( image )
{

}

WallPiece::~WallPiece()
{
          delete image ;
          image = nilPointer ;
}

void WallPiece::calculateOffset()
{
        assert( mediator != nilPointer );

        Room * room = mediator->getRoom() ;
        int tileSize = static_cast< int >( room->getSizeOfOneTile() );

        if ( onX )
                this->offset.first = ( this->position << 1 ) * tileSize + 1 + room->getX0();
        else
                this->offset.first = 1 - tileSize * ( ( this->position + 2 ) << 1 ) + room->getX0();

        this->offset.second = ( this->position + 1 ) * tileSize - this->image->getHeight() - 1 + room->getY0();

        if ( isOnX() && ( room->hasDoorAt( "east" ) ||
                                room->hasDoorAt( "eastnorth" ) || room->hasDoorAt( "eastsouth" )
                                        || ! room->hasFloor() ) )
        {
                this->offset.first -= ( tileSize << 1 );
                this->offset.second += tileSize;
        }

        if ( isOnY() && ( room->hasDoorAt( "north" ) ||
                                room->hasDoorAt( "northeast" ) || room->hasDoorAt( "northwest" )
                                        || ! room->hasFloor() ) )
        {
                this->offset.first += ( tileSize << 1 );
                this->offset.second += tileSize;
        }
}

void WallPiece::draw ()
{
        if ( this->image != nilPointer )
                allegro::drawSprite( this->image->getAllegroPict(), this->offset.first, this->offset.second );
}
