
#include "FloorTile.hpp"
#include "Mediator.hpp"

#include <sstream>
#include <cassert>


namespace isomot
{

FloorTile::FloorTile( int cellX, int cellY, BITMAP* image )
        : Mediated (), Shady ()
        , rawImage( image )
        , shadyImage( nilPointer )
{
        this->coordinates.first = cellX;
        this->coordinates.second = cellY;
        this->offset.first = this->offset.second = 0;

        std::ostringstream at;
        at << "cx=" << coordinates.first << " cy=" << coordinates.second;
        this->uniqueName = "floor tile at " + at.str();
}

FloorTile::~FloorTile()
{
        allegro::destroyBitmap( rawImage );
        allegro::destroyBitmap( shadyImage );
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

void FloorTile::draw( BITMAP* where )
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

void FloorTile::setShadyImage( BITMAP* newShady )
{
        if ( shadyImage != newShady )
        {
                allegro::destroyBitmap( shadyImage /*~ , "shady image of " + uniqueName + " via FloorTile::setShadyImage" ~*/ );
                shadyImage = newShady;
        }
}

void FloorTile::freshShadyImage ()
{
        if ( shadyImage != nilPointer )
        {
                allegro::destroyBitmap( shadyImage /*~ , "shady image of " + uniqueName + " via FloorTile::freshShadyImage" ~*/ );

                BITMAP* shady = create_bitmap_ex( 32, rawImage->w, rawImage->h );
                blit( rawImage, shady, 0, 0, 0, 0, rawImage->w, rawImage->h );
                shadyImage = shady;
        }
}

int FloorTile::getColumn () const
{
        assert( mediator != nilPointer );
        return mediator->getRoom()->getTilesX() * getCellY() + getCellX();
}

}
