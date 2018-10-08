
#include "FloorTile.hpp"
#include "Mediator.hpp"

#include <cassert>


namespace isomot
{

FloorTile::FloorTile( int cellX, int cellY, Picture* image )
        : Mediated (), Shady ()
        , uniqueName( "floor tile at cx=" + isomot::numberToString( cellX ) + " cy=" + isomot::numberToString( cellY ) )
        , rawImage( image )
        , shadyImage( nilPointer )
{
        this->coordinates.first = cellX;
        this->coordinates.second = cellY;
        this->offset.first = this->offset.second = 0;
}

FloorTile::~FloorTile()
{
        delete rawImage ;
        delete shadyImage ;
}

void FloorTile::calculateOffset()
{
        if ( mediator != nilPointer )
        {
                Room* room = mediator->getRoom();
                this->offset.first = room->getX0() + ( ( room->getSizeOfOneTile() * ( getCellX() - getCellY() - 1 ) ) << 1 ) + 1;
                this->offset.second = room->getY0() + room->getSizeOfOneTile() * ( getCellX() + getCellY() );
        }
}

void FloorTile::draw( const allegro::Pict& where )
{
        if ( shadyImage != nilPointer )
        {       // draw tile with shadow
                allegro::drawSprite( where, shadyImage->getAllegroPict(), offset.first, offset.second );
        }
        else if ( rawImage != nilPointer )
        {       // draw tile, just tile
                allegro::drawSprite( where, rawImage->getAllegroPict(), offset.first, offset.second );
        }
}

void FloorTile::setShadyImage( Picture* newShady )
{
        if ( shadyImage != newShady )
        {
                delete shadyImage ;
                shadyImage = newShady;
        }
}

void FloorTile::freshShadyImage ()
{
        if ( shadyImage != nilPointer )
        {
                delete shadyImage ;
                shadyImage = new Picture( *rawImage );
        }
}

int FloorTile::getColumn () const
{
        assert( mediator != nilPointer );
        return mediator->getRoom()->getTilesX() * getCellY() + getCellX();
}

}
