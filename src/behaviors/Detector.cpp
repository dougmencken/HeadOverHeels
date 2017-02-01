#include "Detector.hpp"
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

Detector::Detector( Item * item, const BehaviorId & id ) :
	Behavior( item, id )
{
	stateId = StateWait;
	speedTimer = new HPC();
	fallTimer = new HPC();
	speedTimer->start();
	fallTimer->start();
}

Detector::~Detector( )
{
	delete speedTimer;
	delete fallTimer;
}

bool Detector::update ()
{
  FreeItem* freeItem = dynamic_cast<FreeItem*>(this->item);
  PlayerItem* playerItem = freeItem->getMediator()->getActivePlayer();
  bool destroy = false;

  if(playerItem != 0)
  {
    switch(stateId)
    {
      // Si está parado intenta detectar a un jugador
      case StateWait:
        // Un jugador se cruza en el eje X del "perseguidor"
        if(freeItem->getX() >= playerItem->getX() - 1 && freeItem->getX() <= playerItem->getX() + 1)
        {
          // ...a la derecha de él
          if(playerItem->getY() <= freeItem->getY())
          {
            changeStateId(StateMoveEast);
          }
          else
          // ...a la izquierda de él
          if(playerItem->getY() >= freeItem->getY())
          {
            changeStateId(StateMoveWest);
          }
        }
        else
        // Un jugador se cruza en el eje Y del "perseguidor"
        if(freeItem->getY() >= playerItem->getY()-1 && freeItem->getY() <= playerItem->getY()+1)
        {
          // ...a la derecha de él
          if(playerItem->getX() <= freeItem->getX())
          {
            changeStateId(StateMoveNorth);
          }
          else
          // ...a la izquierda de él
          if(playerItem->getX() >= freeItem->getX())
          {
            changeStateId(StateMoveSouth);
          }
        }

        // Emite el sonido de activación del movimiento
        if(stateId != StateWait)
        {
          this->soundManager->play(freeItem->getLabel(), stateId);
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
        else if(fallTimer->getValue() > freeItem->getWeight())
        {
          if(!state->fall(this))
          {
            stateId = StateWait;
          }

          // Se pone a cero el cronómetro para el siguiente ciclo
          fallTimer->reset();
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
  }

  return destroy;
}

}
