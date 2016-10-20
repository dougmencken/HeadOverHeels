
#include "Widget.hpp"
#include "actions/Action.hpp"


namespace gui
{

Widget::Widget( unsigned int x, unsigned int y )
: xy ( std::make_pair( x, y ) ),
  next ( 0 )
{

}

Widget::~Widget( )
{

}

void Widget::changePosition( unsigned int x, unsigned int y )
{
        xy = std::make_pair( x, y ) ;
}

void Widget::handleKey( int key )
{
        if ( this->next != 0 )
        {
                this->next->handleKey( key );
        }
}

}
