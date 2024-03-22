
#include "WallPiece.hpp"

#include "Mediator.hpp"


WallPiece::WallPiece( bool trueXfalseY, int index, Picture* graphicsOfPiece )
        : Mediated()
        , alongX( trueXfalseY )
        , position( index )
        , image( graphicsOfPiece )
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

        this->offset.first  = room->getX0 ();
        this->offset.second = room->getY0 ();

        int tileSize = static_cast< int >( room->getSizeOfOneTile() );

        if ( isAlongX() )
                this->offset.first += ( this->position << 1 ) * tileSize + 1 ;
        else
                this->offset.first += 1 - tileSize * ( ( this->position + 2 ) << 1 ) ;

        this->offset.second += ( this->position + 1 ) * tileSize - this->image->getHeight() - 1 ;

        if ( isAlongX() && ( room->hasDoorOn( "east" ) ||
                                room->hasDoorOn( "eastnorth" ) || room->hasDoorOn( "eastsouth" )
                                        || ! room->hasFloor() ) )
        {
                this->offset.first -= ( tileSize << 1 );
                this->offset.second += tileSize ;
        }

        if ( isAlongY() && ( room->hasDoorOn( "north" ) ||
                                room->hasDoorOn( "northeast" ) || room->hasDoorOn( "northwest" )
                                        || ! room->hasFloor() ) )
        {
                this->offset.first += ( tileSize << 1 );
                this->offset.second += tileSize ;
        }
}

void WallPiece::draw ()
{
        if ( this->image != nilPointer )
                allegro::drawSprite( this->image->getAllegroPict(), this->offset.first, this->offset.second );
}
