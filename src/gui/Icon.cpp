#include "Icon.hpp"

namespace gui
{

Icon::Icon(unsigned int x, unsigned int y, BITMAP* image) : Widget(x, y)
{
  this->image = image;
}

Icon::~Icon()
{

}

void Icon::draw(BITMAP* destination)
{
  // Dibuja el icono en la imagen destino
  draw_sprite(destination, image, this->x, this->y);
}

}
