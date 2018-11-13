
#include "FloorTile.hpp"
#include "Mediator.hpp"

#include <cassert>


namespace iso
{

FloorTile::FloorTile( int cellX, int cellY, const Picture& graphicsOfTile )
        : Mediated (), Shady ()
        , uniqueName( "floor tile at cx=" + util::number2string( cellX ) + " cy=" + util::number2string( cellY ) )
        , rawImage( new Picture( graphicsOfTile ) )
        , shadyImage( new Picture( graphicsOfTile ) )
{
        this->coordinates.first = cellX;
        this->coordinates.second = cellY;
        this->offset.first = this->offset.second = 0;

        rawImage->setName( graphicsOfTile.getName() );
        shadyImage->setName( "shaded " + graphicsOfTile.getName() );

        setWantShadow( true );
}

void FloorTile::calculateOffset()
{
        assert( mediator != nilPointer );

        Room* room = mediator->getRoom();
        this->offset.first = room->getX0() + ( ( room->getSizeOfOneTile() * ( getCellX() - getCellY() - 1 ) ) << 1 ) + 1;
        this->offset.second = room->getY0() + room->getSizeOfOneTile() * ( getCellX() + getCellY() );
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

void FloorTile::freshShadyImage ()
{
        shadyImage = PicturePtr( new Picture( *rawImage ) );
        shadyImage->setName( "shaded " + rawImage->getName() );
}

int FloorTile::getColumn () const
{
        assert( mediator != nilPointer );
        return mediator->getRoom()->getTilesX() * getCellY() + getCellX();
}

}
