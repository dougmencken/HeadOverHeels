
#include "Icon.hpp"


namespace gui
{

Icon::Icon( int x, int y, BITMAP* image ) :
        Widget( x, y ) ,
        icon( image )
{

}

Icon::~Icon()
{
        delete icon ;
        icon = 0 ;
}

void Icon::draw( BITMAP* where )
{
        if ( icon != 0 )
        {
                // dibuja el icono en la imagen destino
                draw_sprite( where, icon, getX(), getY() );
        }
}

}
