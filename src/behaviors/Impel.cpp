#include "Impel.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "DisplaceState.hpp"
#include "FallState.hpp"
#include "Mediator.hpp"
#include "Room.hpp"

namespace isomot
{

Impel::Impel( Item * item, const BehaviorId & id ) :
        Behavior( item, id )
{
        stateId = StateWait;
        speedTimer = new HPC();
        fallTimer = new HPC();
        speedTimer->start();
        fallTimer->start();
}

Impel::~Impel()
{
        delete speedTimer;
        delete fallTimer;
}

bool Impel::update ()
{
  FreeItem * freeItem = dynamic_cast< FreeItem * >( this->item );
  bool destroy = false;

  switch ( stateId )
  {
    case StateWait:
      break;

    case StateDisplaceNorth:
    case StateDisplaceSouth:
    case StateDisplaceEast:
    case StateDisplaceWest:
    case StateDisplaceNortheast:
    case StateDisplaceNorthwest:
    case StateDisplaceSoutheast:
    case StateDisplaceSouthwest:
      // Si el elemento está activo y ha llegado el momento de moverse, entonces:
      if(speedTimer->getValue() > freeItem->getSpeed())
      {
        // El elemento se mueve hasta detectar un colisión
        if(!state->displace(this, &stateId, true))
        {
          stateId = StateWait;
        }

        // Se pone a cero el cronómetro para el siguiente ciclo
        speedTimer->reset();
      }

      // Anima el elemento
      freeItem->animate();
      break;

    case StateFall:
      // Se comprueba si ha topado con el suelo en una sala sin suelo
      if(freeItem->getZ() == 0 && freeItem->getMediator()->getRoom()->getFloorType() == NoFloor)
      {
        // El elemento desaparece
        destroy = true;
      }
      // Si ha llegado el momento de caer entonces el elemento desciende una unidad
      else if ( fallTimer->getValue() > freeItem->getWeight() )
      {
        if ( ! state->fall( this ) )
        {
          stateId = StateWait;
        }

        // Se pone a cero el cronómetro para el siguiente ciclo
        fallTimer->reset();
      }
      break;

    default:
      ;
  }

  return destroy;
}

}
