
#include "Widget.hpp"
#include "actions/Action.hpp"


namespace gui
{

Widget::Widget( unsigned int x, unsigned int y )
: xy ( std::make_pair( x, y ) )
{

}

Widget::~Widget( )
{

}

void Widget::changePosition( unsigned int x, unsigned int y )
{
        xy = std::make_pair( x, y ) ;
}

}
