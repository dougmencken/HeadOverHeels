#include "Trampoline.hpp"
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

Trampoline::Trampoline(Item* item, const BehaviorId& id) : Behavior(item, id)
{
  stateId = StateWait;
  folded = rebounding = false;
  regularFrame = 0;
  foldedFrame = 1;
  speedTimer = new HPC();
  fallenTimer = new HPC();
  reboundTimer = new HPC();
  speedTimer->start();
  fallenTimer->start();
  reboundTimer->start();
}

Trampoline::~Trampoline()
{
  delete speedTimer;
  delete fallenTimer;
  delete reboundTimer;
}

bool Trampoline::update()
{
  FreeItem* freeItem = dynamic_cast<FreeItem*>(this->item);
  bool destroy = false;

  switch(stateId)
  {
    case StateWait:
      // Si hay elementos encima el trampolín se pliega
      if(!freeItem->checkPosition(0, 0, 1, Add))
      {
        folded = true;
        rebounding = false;
        freeItem->changeFrame(foldedFrame);
      }
      else
      {
        // Si el trampolín está rebotando se anima hasta que finalice el tiempo
        if(rebounding && reboundTimer->getValue() < 0.600)
        {
          freeItem->animate();

          // Emite el sonido de rebote
          if(reboundTimer->getValue() > 0.100)
          {
            this->soundManager->play(freeItem->getLabel(), StateActive);
          }
        }
        else
        {
          // Si no hay elementos encima pero los había entonces el trampolín rebota
          if(folded)
          {
            rebounding = true;
            reboundTimer->reset();
          }

          // Ya no está pleglado
          folded = false;

          freeItem->changeFrame(regularFrame);
        }
      }

      // Se comprueba si el elemento debe empezar a caer
      if(FallState::getInstance()->fall(this))
      {
        fallenTimer->reset();
        stateId = StateFall;
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
        // Emite el sonido de de desplazamiento
        this->soundManager->play(freeItem->getLabel(), stateId);

        // Actualiza el estado
        this->changeStateId(stateId);
        this->state->displace(this, &stateId, true);

        // Si no está cayendo entonces vuelve al estado inicial
        if(stateId != StateFall)
        {
          stateId = StateWait;
        }

        // Se pone a cero el cronómetro para el siguiente ciclo
        speedTimer->reset();
      }
      break;

    case StateFall:
      // Se comprueba si ha topado con el suelo en una sala sin suelo
      if(item->getZ() == 0 && item->getMediator()->getRoom()->getFloorType() == NoFloor)
      {
        // El elemento desaparece
        destroy = true;
      }
      // Si ha llegado el momento de caer entonces el elemento desciende una unidad
      else if(fallenTimer->getValue() > freeItem->getWeight())
      {
        // El elemento cae
        this->changeStateId(stateId);
        if(!state->fall(this))
        {
          // Emite el sonido de caída
          soundManager->play(freeItem->getLabel(), stateId);
          stateId = StateWait;
        }

        // Se pone a cero el cronómetro para el siguiente ciclo
        fallenTimer->reset();
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
