
#include "Icon.hpp"

namespace gui
{

Icon::Icon( unsigned int x, unsigned int y, BITMAP* image ) : Widget( x, y )
{
        this->image = image;
}

Icon::~Icon()
{

}

void Icon::draw( BITMAP* where )
{
        // Dibuja el icono en la imagen destino
        draw_sprite( where, image, this->getX(), this->getY() );
}

}
