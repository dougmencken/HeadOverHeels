
#include "Wall.hpp"
#include "Mediator.hpp"

#include <cassert>


namespace iso
{

Wall::Wall( bool trueXfalseY, int index, Picture* image )
        : Mediated()
        , onX( trueXfalseY )
        , position( index )
        , image( image )
{

}

Wall::~Wall()
{
          delete image ;
          image = nilPointer ;
}

void Wall::calculateOffset()
{
        assert( mediator != nilPointer );

        int tileSize = mediator->getRoom()->getSizeOfOneTile();

        if ( onX )
                this->offset.first = ( this->position << 1 ) * tileSize + 1 + mediator->getRoom()->getX0();
        else
                this->offset.first = 1 - tileSize * ( ( this->position + 2 ) << 1 ) + mediator->getRoom()->getX0();

        this->offset.second = ( this->position + 1 ) * tileSize - this->image->getHeight() - 1 + mediator->getRoom()->getY0();

        if ( isOnX() && ( mediator->getRoom()->hasDoorAt( "east" ) ||
                          mediator->getRoom()->hasDoorAt( "eastnorth" ) || mediator->getRoom()->hasDoorAt( "eastsouth" ) ||
                          ! mediator->getRoom()->hasFloor() ) )
        {
                this->offset.first -= ( tileSize << 1 );
                this->offset.second += tileSize;
        }

        if ( isOnY() && ( mediator->getRoom()->hasDoorAt( "north" ) ||
                          mediator->getRoom()->hasDoorAt( "northeast" ) || mediator->getRoom()->hasDoorAt( "northwest" ) ||
                          ! mediator->getRoom()->hasFloor() ) )
        {
                this->offset.first += ( tileSize << 1 );
                this->offset.second += tileSize;
        }
}

void Wall::draw ()
{
        if ( this->image != nilPointer )
        {
                allegro::drawSprite( this->image->getAllegroPict(), this->offset.first, this->offset.second );
        }
}

}
