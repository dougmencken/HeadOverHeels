#include "RemoteControl.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "DisplaceState.hpp"
#include "FallState.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"

namespace isomot
{

RemoteControl::RemoteControl( Item * item, const BehaviorId & id ) : Behavior( item, id )
{
        stateId = StateWait;
        controlledItem = 0;

        // Sólo se mueve el elemento controlado, el controlador es fijo
        if ( id == SteerBehavior )
        {
                speedTimer = new HPC();
                fallTimer = new HPC();
                speedTimer->start();
                fallTimer->start();
        }
}

RemoteControl::~RemoteControl()
{
        if ( id == SteerBehavior )
        {
                delete speedTimer;
                delete fallTimer;
        }
}

bool RemoteControl::update ()
{
  FreeItem* freeItem = dynamic_cast< FreeItem * >( this->item );
  bool destroy = false;

  // Si este es el elemento controlador, busca al controlado
  if ( id == RemoteControlBehavior && controlledItem == 0 )
  {
    controlledItem = static_cast< FreeItem * >( freeItem->getMediator()->findItemByBehavior( SteerBehavior ) );
  }

  switch ( stateId )
  {
    case StateWait:
      break;

    case StateMoveNorth:
    case StateMoveSouth:
    case StateMoveEast:
    case StateMoveWest:
      // Si este es el elemento controlado y está activo y ha llegado el momento de moverse, entonces:
      if ( id == SteerBehavior )
      {
        if ( speedTimer->getValue() > freeItem->getSpeed() )
        {
          // El elemento se mueve. Si colisiona vuelve al estado inicial para tomar una nueva dirección
          state->move( this, &stateId, true );
          if ( stateId != StateFall )
          {
            stateId = StateWait;
          }
          // Se pone a cero el cronómetro para el siguiente ciclo
          speedTimer->reset();
        }

        // Anima el elemento
        freeItem->animate();
      }
      break;

    case StateDisplaceNorth:
    case StateDisplaceSouth:
    case StateDisplaceEast:
    case StateDisplaceWest:
    case StateDisplaceNortheast:
    case StateDisplaceNorthwest:
    case StateDisplaceSoutheast:
    case StateDisplaceSouthwest:
      // Si este es el elemento controlado y está activo y ha llegado el momento de moverse, entonces:
      if ( id == SteerBehavior )
      {
        if ( speedTimer->getValue() > freeItem->getSpeed() )
        {
          // Emite el sonido de de desplazamiento si está siendo empujado, no desplazado
          // por un elemento que haya debajo
          if(this->sender == 0 || (this->sender != 0 && this->sender != this->item))
          {
            this->soundManager->play(freeItem->getLabel(), stateId);
          }

          // Desplaza el elemento una unidad
          state->displace(this, &stateId, true);
          if(stateId != StateFall)
          {
            stateId = StateWait;
          }

          // Se pone a cero el cronómetro para el siguiente ciclo
          speedTimer->reset();
        }

        // Anima el elemento
        freeItem->animate();
      }

      // Para los cuatro puntos cardinales básicos el elemento controlador debe cambiar el estado
      // del elemento controlado
      if(stateId == StateDisplaceNorth || stateId == StateDisplaceSouth || stateId == StateDisplaceEast || stateId == StateDisplaceWest)
      {
        if(id == RemoteControlBehavior)
        {
          StateId motionStateId = StateWait;

          switch(stateId)
          {
            case StateDisplaceNorth:
              motionStateId = StateMoveNorth;
              break;

            case StateDisplaceSouth:
              motionStateId = StateMoveSouth;
              break;

            case StateDisplaceEast:
              motionStateId = StateMoveEast;
              break;

            case StateDisplaceWest:
              motionStateId = StateMoveWest;
              break;

            default:
              ;
          }

          dynamic_cast<RemoteControl*>(controlledItem->getBehavior())->changeStateId(motionStateId);
          stateId = StateWait;
        }
      }
      break;

    case StateFall:
      // Se comprueba si ha topado con el suelo en una sala sin suelo
      if(freeItem->getZ() == 0 && freeItem->getMediator()->getRoom()->getFloorType() == NoFloor)
      {
        // El elemento desaparece
        destroy = true;
      }
      // Si este es el elemento controlado y ha llegado el momento de caer entonces
      // el elemento desciende una unidad
      else if ( id == SteerBehavior && fallTimer->getValue() > freeItem->getWeight() )
      {
        if ( ! state->fall( this ) )
        {
          // Emite el sonido de caída
          this->soundManager->play( freeItem->getLabel(), stateId );
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
