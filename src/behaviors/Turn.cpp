#include "Turn.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "MoveState.hpp"
#include "DisplaceState.hpp"
#include "FallState.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"

namespace isomot
{

Turn::Turn(Item* item, const BehaviorId& id) : Behavior(item, id)
{
  stateId = StateWait;
  speedTimer = new HPC();
  fallenTimer = new HPC();
  speedTimer->start();
  fallenTimer->start();
}

Turn::~Turn()
{
  delete speedTimer;
  delete fallenTimer;
}

bool Turn::update()
{
  bool destroy = false;
  FreeItem* freeItem = dynamic_cast<FreeItem*>(this->item);

  switch(stateId)
  {
    case StateWait:
      start();
      break;

    case StateMoveNorth:
    case StateMoveSouth:
    case StateMoveEast:
    case StateMoveWest:
      // Si el elemento está activo y ha llegado el momento de moverse, entonces:
      if(!freeItem->isDead())
      {
        if(speedTimer->getValue() > freeItem->getSpeed())
        {
          // El elemento se mueve. Si el movimiento no se pudo realizar por colisión entonces
          // se desplaza a los elementos con los que pudiera haber chocado y el elemento da media
          // vuelta cambiando su estado a otro de movimiento
          if(!state->move(this, &stateId, true))
          {
            turn();
            // Emite el sonido de colisión
            this->soundManager->play(freeItem->getLabel(), StateCollision);
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
    case StateDisplaceSoutheast:
    case StateDisplaceSouthwest:
    case StateDisplaceNorthwest:
      // Emite el sonido de de desplazamiento
      this->soundManager->play(freeItem->getLabel(), stateId);

      // El elemento es deplazado por otro. Si el desplazamiento no se pudo realizar por
      // colisión entonces el estado se propaga a los elementos con los que ha chocado
      state->displace(this, &stateId, true);

      // Una vez se ha completado el desplazamiento el elemento vuelve a su comportamiento normal
      stateId = StateWait;

      // Si el elemento estaba inactivo, después del desplazamiento seguirá estando
      if(freeItem->isDead())
      {
        stateId = StateFreeze;
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
      else if(fallenTimer->getValue() > freeItem->getWeight())
      {
        if(!state->fall(this))
        {
          // Emite el sonido de caída
          this->soundManager->play(freeItem->getLabel(), stateId);
          stateId = StateWait;
        }

        // Se pone a cero el cronómetro para el siguiente ciclo
        fallenTimer->reset();
      }
      break;

    case StateFreeze:
      freeItem->setDead(true);
      break;

    case StateWakeUp:
      freeItem->setDead(false);
      stateId = StateWait;
      break;

    default:
      ;
  }

  return destroy;
}

void Turn::start()
{
  switch(dynamic_cast<FreeItem*>(this->item)->getDirection())
  {
    case North:
      stateId = StateMoveNorth;
      break;

    case South:
      stateId = StateMoveSouth;
      break;

    case East:
      stateId = StateMoveEast;
      break;

    case West:
      stateId = StateMoveWest;
      break;

    default:
      ;
  }

  state = MoveState::getInstance();
}

void Turn::turn()
{
  FreeItem* freeItem = dynamic_cast<FreeItem*>(this->item);

  switch(freeItem->getDirection())
  {
    case North:
      stateId = (id == TurnLeftBehavior ? StateMoveWest : StateMoveEast);
      if(freeItem->getDirectionFrames() > 1)
      {
        freeItem->changeDirection(id == TurnLeftBehavior ? West : East);
      }
      break;

    case South:
      stateId = (id == TurnLeftBehavior ? StateMoveEast : StateMoveWest);
      if(freeItem->getDirectionFrames() > 1)
      {
        freeItem->changeDirection(id == TurnLeftBehavior ? East : West);
      }
      break;

    case East:
      stateId = (id == TurnLeftBehavior ? StateMoveNorth : StateMoveSouth);
      if(freeItem->getDirectionFrames() > 1)
      {
        freeItem->changeDirection(id == TurnLeftBehavior ? North : South);
      }
      break;

    case West:
      stateId = (id == TurnLeftBehavior ? StateMoveSouth : StateMoveNorth);
      if(freeItem->getDirectionFrames() > 1)
      {
        freeItem->changeDirection(id == TurnLeftBehavior ? South : North);
      }
      break;

    default:
      ;
  }
}

}
