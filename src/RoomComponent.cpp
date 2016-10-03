#include "RoomComponent.hpp"
#include "Mediator.hpp"

namespace isomot
{

RoomComponent::RoomComponent()
{
  this->mediator = 0;
}

void RoomComponent::setMediator(Mediator* mediator)
{
  this->mediator = mediator;
}

Mediator* RoomComponent::getMediator()
{
  return mediator;
}

}
