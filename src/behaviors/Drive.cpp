#include "Drive.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "DisplaceState.hpp"
#include "FallState.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"

namespace isomot
{

Drive::Drive(Item* item, const BehaviorId& id) : Behavior(item, id)
{
  stateId = StateWait;
  running = false;
  speedTimer = new HPC();
  fallenTimer = new HPC();
  speedTimer->start();
  fallenTimer->start();
}

Drive::~Drive()
{
  delete speedTimer;
  delete fallenTimer;
}

bool Drive::update()
{
  FreeItem* freeItem = dynamic_cast<FreeItem*>(this->item);
  Mediator* mediator = freeItem->getMediator();
  bool destroy = false;
  bool playerFound = false;

  switch(stateId)
  {
    case StateWait:
      // Si está en marcha sigue avanzando según su orientación actual
      if(running)
      {
        switch(freeItem->getDirection())
        {
          case North:
            changeStateId(StateMoveNorth);
            break;

          case South:
            changeStateId(StateMoveSouth);
            break;

          case East:
            changeStateId(StateMoveEast);
            break;

          case West:
            changeStateId(StateMoveWest);
            break;

          default:
            ;
        }
      }
      // Si está parado comprueba si hay un jugador encima y toma su orientación para empezar a avanzar
      else
      {
        if(!freeItem->checkPosition(0, 0, 1, Add))
        {
          while(!mediator->isCollisionStackEmpty() && !playerFound)
          {
            Item* item = mediator->findItem(mediator->popCollision());

            if(dynamic_cast<PlayerItem*>(item))
            {
              playerFound = true;
              running = true;

              switch(dynamic_cast<PlayerItem*>(item)->getDirection())
              {
                case North:
                  changeStateId(StateMoveNorth);
                  break;

                case South:
                  changeStateId(StateMoveSouth);
                  break;

                case East:
                  changeStateId(StateMoveEast);
                  break;

                case West:
                  changeStateId(StateMoveWest);
                  break;

                default:
                  ;
              }
            }
          }
        }
      }
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
          // El elemento se mueve. Si colisiona vuelve al estado inicial para tomar una nueva dirección
          if(!state->move(this, &stateId, true))
          {
            running = false;
            stateId = StateWait;
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

}
