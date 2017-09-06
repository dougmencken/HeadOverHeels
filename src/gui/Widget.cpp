
#include "Widget.hpp"


namespace gui
{

Widget::Widget( )
        : xy ( std::make_pair( 0, 0 ) )
{

}

Widget::Widget( int x, int y )
        : xy ( std::make_pair( x, y ) )
{

}

Widget::~Widget( )
{

}

void Widget::moveTo( int x, int y )
{
        xy = std::make_pair( x, y ) ;
}

}
