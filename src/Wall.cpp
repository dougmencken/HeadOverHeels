#include "Wall.hpp"
#include "Mediator.hpp"

namespace isomot
{

Wall::Wall( Axis axis, int index, BITMAP* image ) : Mediated()
{
        this->axis = axis;
        this->index = index;
        this->image = image;
}

Wall::~Wall()
{
          destroy_bitmap( image );
}

void Wall::calculateOffset()
{
        switch( this->axis )
        {
                case AxisX:
                        this->offset.first = ( this->index << 1 ) * mediator->getRoom()->getSizeOfOneTile() + 1 + mediator->getRoom()->getX0();
                        break;

                case AxisY:
                        this->offset.first = 1 - mediator->getRoom()->getSizeOfOneTile() * ( ( this->index + 2 ) << 1 ) + mediator->getRoom()->getX0();
                        break;
        }

        this->offset.second = ( this->index + 1 ) * mediator->getRoom()->getSizeOfOneTile() - this->image->h - 1 + mediator->getRoom()->getY0();
}

void Wall::draw( BITMAP* where )
{
        if( this->image )
        {
                draw_sprite( where, this->image, this->offset.first, this->offset.second );
        }
}

}
