
#include "Widget.hpp"


namespace gui
{

Widget::Widget( )
        : xy ( std::make_pair( 0, 0 ) )
        , onScreen( false )
{

}

Widget::Widget( int x, int y )
        : xy ( std::make_pair( x, y ) )
        , onScreen( false )
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
