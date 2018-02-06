
#include "Wall.hpp"
#include "Mediator.hpp"


namespace isomot
{

Wall::Wall( bool trueXfalseY, int index, BITMAP* image )
        : Mediated()
        , onX( trueXfalseY )
        , index( index )
        , image( image )
{

}

Wall::~Wall()
{
          destroy_bitmap( image );
}

void Wall::calculateOffset()
{
        if ( onX )
                this->offset.first = ( this->index << 1 ) * mediator->getRoom()->getSizeOfOneTile() + 1 + mediator->getRoom()->getX0();
        else
                this->offset.first = 1 - mediator->getRoom()->getSizeOfOneTile() * ( ( this->index + 2 ) << 1 ) + mediator->getRoom()->getX0();

        this->offset.second = ( this->index + 1 ) * mediator->getRoom()->getSizeOfOneTile() - this->image->h - 1 + mediator->getRoom()->getY0();
}

void Wall::draw( BITMAP* where )
{
        if ( this->image )
        {
                draw_sprite( where, this->image, this->offset.first, this->offset.second );
        }
}

}
