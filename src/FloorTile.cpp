
#include "FloorTile.hpp"
#include "Mediator.hpp"


namespace isomot
{

FloorTile::FloorTile( int column, int cellX, int cellY, BITMAP* image )
        : Mediated (), Shady ()
        , column( column )
        , rawImage( image )
        , shadyImage( nilPointer )
{
        this->coordinates.first = cellX;
        this->coordinates.second = cellY;
        this->offset.first = this->offset.second = 0;
}

FloorTile::~FloorTile()
{
        destroy_bitmap( rawImage );
        destroy_bitmap( shadyImage );
}

void FloorTile::calculateOffset()
{
        if ( mediator != nilPointer )
        {
                Room* room = mediator->getRoom();
                this->offset.first = room->getX0() + ( ( room->getSizeOfOneTile() * ( coordinates.first - coordinates.second - 1 ) ) << 1 ) + 1;
                this->offset.second = room->getY0() + room->getSizeOfOneTile() * ( coordinates.first + coordinates.second );
        }
}

void FloorTile::draw( BITMAP* where )
{
        if ( shadyImage != nilPointer )
        {       // draw tile with shadow
                draw_sprite( where, shadyImage, offset.first, offset.second );
        }
        else if ( rawImage != nilPointer )
        {       // draw tile, just tile
                draw_sprite( where, rawImage, offset.first, offset.second );
        }
}

void FloorTile::setShadyImage( BITMAP* newShady )
{
        if ( shadyImage != newShady )
        {
                destroy_bitmap( shadyImage );
                shadyImage = newShady;
        }
}

}
