#include "MoveState.hpp"
#include "FallState.hpp"
#include "Behavior.hpp"
#include "Item.hpp"
#include "GridItem.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "Mediator.hpp"

#include <iostream>

namespace isomot
{

BehaviorState* MoveState::instance = 0;

MoveState::MoveState() : BehaviorState()
{

}

MoveState::~MoveState()
{

}

BehaviorState* MoveState::getInstance()
{
  if(instance == 0)
  {
    instance = new MoveState();
  }

  return instance;
}

bool MoveState::move(Behavior* behavior, StateId* substate, bool canFall)
{
  bool changedData = false;
  bool loading = false;
  StateId displaceStateId = StateWait;
  std::stack<int> topItems;
  FreeItem* freeItem = 0;
  Mediator* mediator = 0;

  // Acceso al elemento que hay que mover si dicho elemento es libre
  if(behavior->getItem()->getId() & 1)
  {
    freeItem = dynamic_cast<FreeItem*>(behavior->getItem());
    mediator = freeItem->getMediator();
  }

  switch(*substate)
  {
    case StateMoveNorth:
    case StateAutoMoveNorth:
      if(freeItem != 0)
      {
        if(freeItem->getDirection() != North)
        {
          freeItem->changeDirection(North);
        }

        changedData = freeItem->addX(-1);
        displaceStateId = StateDisplaceNorth;
      }
      break;

    case StateMoveSouth:
    case StateAutoMoveSouth:
      if(freeItem != 0)
      {
        if(freeItem->getDirection() != South)
        {
          freeItem->changeDirection(South);
        }

        changedData = freeItem->addX(1);
        displaceStateId = StateDisplaceSouth;
      }
      break;

    case StateMoveEast:
    case StateAutoMoveEast:
      if(freeItem != 0)
      {
        if(freeItem->getDirection() != East)
        {
          freeItem->changeDirection(East);
        }

        changedData = freeItem->addY(-1);
        displaceStateId = StateDisplaceEast;
      }
      break;

    case StateMoveWest:
    case StateAutoMoveWest:
      if(freeItem != 0)
      {
        if(freeItem->getDirection() != West)
        {
          freeItem->changeDirection(West);
        }

        changedData = freeItem->addY(1);
        displaceStateId = StateDisplaceWest;
      }
      break;

    case StateMoveNortheast:
      if(freeItem != 0)
      {
        changedData = freeItem->addPosition(-1, -1, 0);
        displaceStateId = StateDisplaceNortheast;
      }
      break;

    case StateMoveNorthwest:
      if(freeItem != 0)
      {
        changedData = freeItem->addPosition(-1, 1, 0);
        displaceStateId = StateDisplaceNorthwest;
      }
      break;

    case StateMoveSoutheast:
      if(freeItem != 0)
      {
        changedData = freeItem->addPosition(1, -1, 0);
        displaceStateId = StateDisplaceSoutheast;
      }
      break;

    case StateMoveSouthwest:
      if(freeItem != 0)
      {
        changedData = freeItem->addPosition(1, 1, 0);
        displaceStateId = StateDisplaceSouthwest;
      }
      break;

    case StateMoveUp:
      // Si no ha podido ascender, levanta a todos los elementos que pudiera tener encima
      if(!(changedData = freeItem->addZ(1)))
      {
        while(!mediator->isCollisionStackEmpty())
        {
          FreeItem* topItem = dynamic_cast<FreeItem*>(mediator->findItem(mediator->popCollision()));

          if(topItem != 0 && freeItem->getWidthX() + freeItem->getWidthY() >= topItem->getWidthX() + topItem->getWidthY())
          {
            ascent(topItem, 1);
          }
        }

        // Ahora ya puede ascender
        changedData = freeItem->addZ(1);
      }
      break;

    case StateMoveDown:
      // Comprueba si tiene elementos encima
      loading = !freeItem->checkPosition(0, 0, 2, Add);

      // Copia la pila de colisiones
      while(!mediator->isCollisionStackEmpty())
      {
        topItems.push(mediator->popCollision());
      }

      // Si pudo descender entonces deben descender con él los elementos que tiene encima
      if((changedData = freeItem->addZ(-1)) && loading)
      {
        while(!topItems.empty())
        {
          FreeItem* topItem = dynamic_cast<FreeItem*>(mediator->findItem(topItems.top()));
          topItems.pop();

          if(topItem != 0)
          {
            descend(topItem, 2);
          }
        }
      }
      break;

    case StateCancelDisplaceNorth:
      if(freeItem->getDirection() != South)
      {
        freeItem->changeDirection(South);
      }
      break;

    case StateCancelDisplaceSouth:
      if(freeItem->getDirection() != North)
      {
        freeItem->changeDirection(North);
      }
      break;

    case StateCancelDisplaceEast:
      if(freeItem->getDirection() != West)
      {
        freeItem->changeDirection(West);
      }
      break;

    case StateCancelDisplaceWest:
      if(freeItem->getDirection() != East)
      {
        freeItem->changeDirection(East);
      }
      break;

    default:
      ;
  }

  if(freeItem != 0)
  {
    // En caso de colisión en los ejes X o Y se desplaza a los elementos implicados
    if(!changedData)
    {
      this->propagateStateAdjacentItems(freeItem, displaceStateId);
    }
    // En caso de que el elemento se haya movido se comprueba si hay que desplazar los elementos
    // que pueda tener encima. La excepción es para el movimiento vertical porque de lo contrario
    // se cambiaría el estado de los elementos situados encima de un ascensor
    else if(*substate != StateMoveUp && *substate != StateMoveDown)
    {
      this->propagateStateTopItems(freeItem, displaceStateId);
    }
  }

  // Si el elemento puede caer entonces se comprueba si hay que cambiar el estado
  if(canFall && *substate)
  {
    if(FallState::getInstance()->fall(behavior))
    {
      changeState(behavior, FallState::getInstance());
      *substate = StateFall;
      changedData = true;
    }
  }

  return changedData;
}

void MoveState::ascent(FreeItem* freeItem, int z)
{
  // El elemento debe poder cambiar de estado
  if(freeItem->getBehavior() != 0)
  {
    // Si el elemento no es el ascensor entonces se levanta
    if(freeItem->getBehavior()->getId() != ElevatorBehavior)
    {
      // Si no se puede levantar, se toma el elemento con el que choca para levantarlo
      if(!freeItem->addZ(z))
      {
        Mediator* mediator = freeItem->getMediator();

        // Para todo elemento que pueda tener encima
        while(!mediator->isCollisionStackEmpty())
        {
          FreeItem* topItem = dynamic_cast<FreeItem*>(mediator->findItem(mediator->popCollision()));

          if(topItem != 0 && freeItem->getWidthX() + freeItem->getWidthY() >= topItem->getWidthX() + topItem->getWidthY())
          {
            // Levanta recursivamente a todos los elementos
            ascent(topItem, z);
          }
        }

        // Ahora ya puede ascender
        freeItem->addZ(z);
      }

      // Si el elemento es un jugador y supera la altura máxima de la sala entonces pasa a
      // la sala de arriba. Se supone que no hay posibilidad de alcanzar la altura máxima
      // de una sala que no conduce a otra situada sobre ella
      PlayerItem* playerItem = dynamic_cast<PlayerItem*>(freeItem);
      if(playerItem != 0 && playerItem->getZ() >= MaxLayers * LayerHeight)
      {
        playerItem->setExit(Up);
        playerItem->setOrientation(playerItem->getDirection());
      }
    }
  }
}

void MoveState::descend(FreeItem* freeItem, int z)
{
  // El elemento debe poder cambiar de estado
  if(freeItem->getBehavior() != 0)
  {
    Mediator* mediator = freeItem->getMediator();

    // Comprueba si tiene elementos encima
    bool loading = !freeItem->checkPosition(0, 0, z, Add);

    // Copia la pila de colisiones
    std::stack<int> topItems;
    while(!mediator->isCollisionStackEmpty())
    {
      topItems.push(mediator->popCollision());
    }

    // Si puede descender entonces hace bajar a todos los elementos apilados encima
    if(freeItem->addZ(-1) && loading)
    {
      // Desciende el resto de unidades. Se hace de una en una porque de lo contrario
      // se podría detectar colisión y no descendería ninguna unidad
      for(int i = 0; i < z - 1; i++)
      {
        freeItem->addZ(-1);
      }

      // Para todo elemento que pueda tener encima
      while(!mediator->isCollisionStackEmpty())
      {
        FreeItem* topItem = dynamic_cast<FreeItem*>(mediator->findItem(topItems.top()));
        topItems.pop();

        if(topItem != 0)
        {
          // Desciende recursivamente a todos los elementos
          descend(topItem, z);
        }
      }
    }
  }
}

}
