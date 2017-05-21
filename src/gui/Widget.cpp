
#include "Widget.hpp"


namespace gui
{

Widget::Widget( int x, int y )
: xy ( std::make_pair( x, y ) )
{

}

Widget::~Widget( )
{

}

void Widget::changePosition( int x, int y )
{
        xy = std::make_pair( x, y ) ;
}

}
