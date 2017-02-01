#include "Mobile.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "DisplaceState.hpp"
#include "FallState.hpp"
#include "SoundManager.hpp"

namespace isomot
{

Mobile::Mobile( Item * item, const BehaviorId & id ) :
        Behavior( item, id )
{
        stateId = StateWait;
        speedTimer = new HPC();
        fallTimer = new HPC();
        speedTimer->start();
        fallTimer->start();
}

Mobile::~Mobile()
{
        delete speedTimer;
        delete fallTimer;
}

bool Mobile::update ()
{
  FreeItem* freeItem = dynamic_cast< FreeItem * >( this->item );
  bool destroy = false;

  switch ( stateId )
  {
    case StateWait:
      // Se comprueba si el elemento debe empezar a caer
      if(FallState::getInstance()->fall(this))
      {
        fallTimer->reset();
        stateId = StateFall;
      }
      break;

    case StateDisplaceNorth:
    case StateDisplaceSouth:
    case StateDisplaceEast:
    case StateDisplaceWest:
    case StateDisplaceNortheast:
    case StateDisplaceSoutheast:
    case StateDisplaceSouthwest:
    case StateDisplaceNorthwest:
      // El elemento es deplazado por otro. Si el desplazamiento no se pudo realizar por
      // colisión entonces el estado se propaga a los elementos con los que ha chocado
      if(speedTimer->getValue() > freeItem->getSpeed())
      {
        // Emite el sonido de de desplazamiento si está siendo empujado, no desplazado
        // por un elemento que haya debajo
        // TODO Hecho para portátiles y para Carlos ¿aplica a otros elementos?
        if(this->sender == 0 || (this->sender != 0 && this->sender != this->item))
        {
          this->soundManager->play(freeItem->getLabel(), stateId);
        }

        // Actualiza el estado
        this->changeStateId(stateId);
        state->displace(this, &stateId, true);

        // Una vez se ha completado el desplazamiento el elemento vuelve a su comportamiento normal
        stateId = StateWait;

        // Se pone a cero el cronómetro para el siguiente ciclo
        speedTimer->reset();
      }

      // Anima el elemento
      freeItem->animate();
      break;

    case StateForceDisplaceNorth:
    case StateForceDisplaceSouth:
    case StateForceDisplaceEast:
    case StateForceDisplaceWest:
      // El elemento es arrastrado porque está encima de una cinta transportadora
      if(speedTimer->getValue() > item->getSpeed())
      {
        state = DisplaceState::getInstance();
        state->displace(this, &stateId, true);

        // Una vez se ha completado el desplazamiento el elemento vuelve a su comportamiento normal
        stateId = StateFall;

        // Se pone a cero el cronómetro para el siguiente ciclo
        speedTimer->reset();
      }
      break;

    case StateFall:
      // Se comprueba si ha topado con el suelo en una sala sin suelo
      if(freeItem->getZ() == 0 && freeItem->getMediator()->getRoom()->getFloorType() == NoFloor)
      {
        // El elemento desaparece
        destroy = true;
      }
      // Si ha llegado el momento de caer entonces el elemento desciende una unidad
      else if(fallTimer->getValue() > freeItem->getWeight())
      {
        // Actualiza el estado
        this->changeStateId(stateId);
        if(!state->fall(this))
        {
          // Emite el sonido de caída
          this->soundManager->play(freeItem->getLabel(), stateId);
          stateId = StateWait;
        }

        // Se pone a cero el cronómetro para el siguiente ciclo
        fallTimer->reset();
      }
      break;

    case StateDestroy:
      // Se destruye cuando el elemento se coge
      destroy = true;
      break;

    default:
      ;
  }

  return destroy;
}

}
