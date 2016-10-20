
#include "Widget.hpp"
#include "actions/Action.hpp"

namespace gui
{

Widget::Widget( unsigned int x, unsigned int y )
: x( x ),
  y( y ),
  next( 0 )
{

}

Widget::~Widget( )
{

}

void Widget::handleKey( int key )
{
        if ( this->next != 0 )
        {
                this->next->handleKey( key );
        }
}

}
