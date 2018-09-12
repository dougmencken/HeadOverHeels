
#include "FloorTile.hpp"
#include "Mediator.hpp"

#include <cassert>


namespace isomot
{

FloorTile::FloorTile( int cellX, int cellY, allegro::Pict* image )
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
        allegro::binPicture( rawImage );
        allegro::binPicture( shadyImage );
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

void FloorTile::draw( allegro::Pict* where )
{
        if ( shadyImage != nilPointer )
        {       // draw tile with shadow
                allegro::drawSprite( where, shadyImage, offset.first, offset.second /*~ , "shady image of " + uniqueName + " via FloorTile::draw" ~*/ );
        }
        else if ( rawImage != nilPointer )
        {       // draw tile, just tile
                allegro::drawSprite( where, rawImage, offset.first, offset.second /*~ , "raw image of " + uniqueName + " via FloorTile::draw" ~*/ );
        }
}

void FloorTile::setShadyImage( allegro::Pict* newShady )
{
        if ( shadyImage != newShady )
        {
                allegro::binPicture( shadyImage );
                shadyImage = newShady;
        }
}

void FloorTile::freshShadyImage ()
{
        if ( shadyImage != nilPointer )
        {
                allegro::binPicture( shadyImage );

                allegro::Pict* shady = allegro::createPicture( rawImage->w, rawImage->h );
                allegro::bitBlit( rawImage, shady );
                shadyImage = shady;
        }
}

int FloorTile::getColumn () const
{
        assert( mediator != nilPointer );
        return mediator->getRoom()->getTilesX() * getCellY() + getCellX();
}

}
