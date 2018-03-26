
#include "Wall.hpp"
#include "Mediator.hpp"

#include <cassert>


namespace isomot
{

Wall::Wall( bool trueXfalseY, int index, BITMAP* image )
        : Mediated()
        , onX( trueXfalseY )
        , position( index )
        , image( image )
{

}

Wall::~Wall()
{
          allegro::destroyBitmap( image );
}

void Wall::calculateOffset()
{
        assert( mediator != nilPointer );

        int tileSize = mediator->getRoom()->getSizeOfOneTile();

        if ( onX )
                this->offset.first = ( this->position << 1 ) * tileSize + 1 + mediator->getRoom()->getX0();
        else
                this->offset.first = 1 - tileSize * ( ( this->position + 2 ) << 1 ) + mediator->getRoom()->getX0();

        this->offset.second = ( this->position + 1 ) * tileSize - this->image->h - 1 + mediator->getRoom()->getY0();

        if ( isOnX() && ( mediator->getRoom()->hasDoorAt( "east" ) || mediator->getRoom()->getKindOfFloor() == "none" ) )
        {
                this->offset.first -= ( tileSize << 1 );
                this->offset.second += tileSize;
        }

        if ( isOnY() && ( mediator->getRoom()->hasDoorAt( "north" ) || mediator->getRoom()->getKindOfFloor() == "none" ) )
        {
                this->offset.first += ( tileSize << 1 );
                this->offset.second += tileSize;
        }
}

void Wall::draw( BITMAP* where )
{
        if ( this->image )
        {
                allegro::drawSprite( where, this->image, this->offset.first, this->offset.second );
        }
}

}
