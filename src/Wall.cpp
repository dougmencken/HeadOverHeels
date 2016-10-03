#include "Wall.hpp"
#include "Mediator.hpp"

namespace isomot
{

Wall::Wall(Axis axis, int index, BITMAP* image) : RoomComponent()
{
  this->axis = axis;
  this->index = index;
  this->image = image;
}

Wall::~Wall()
{
  destroy_bitmap(image);
}

void Wall::calculateOffset()
{
  switch(this->axis)
  {
    case AxisX:
      this->offset.first = (this->index << 1) * mediator->getTileSize() + 1 + mediator->getX0();
      break;

    case AxisY:
      this->offset.first = 1 - mediator->getTileSize() * ((this->index + 2) << 1) + mediator->getX0();
      break;
  }

  this->offset.second = (this->index + 1) * mediator->getTileSize() - this->image->h - 1 + mediator->getY0();
}

void Wall::draw(BITMAP* destination)
{
  if(this->image)
  {
    draw_sprite(destination, this->image, this->offset.first, this->offset.second);
  }
}

}
