#include "Shot.hpp"
#include "FreeItem.hpp"
#include "Mediator.hpp"
#include "MoveState.hpp"
#include "PlayerItem.hpp"
#include "UserControlled.hpp"

namespace isomot
{

Shot::Shot(Item* item, const BehaviorId& id) : Behavior(item, id)
{
  stateId = StateWait;
  playerItem = 0;
  speedTimer = new HPC();
  speedTimer->start();
}

Shot::~Shot()
{
  delete speedTimer;
}

bool Shot::update()
{
  FreeItem* freeItem = dynamic_cast<FreeItem*>(this->item);
  bool destroy = false;

  switch(stateId)
  {
    // El estado inicial establece la dirección
    case StateWait:
      switch(freeItem->getDirection())
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

      // Asigna el estado de movimiento
      state = MoveState::getInstance();
      break;

    case StateMoveNorth:
      if(speedTimer->getValue() > freeItem->getSpeed())
      {
        // Almacena en la pila de colisiones los elementos que hay al norte
        freeItem->setCollisionDetector(true);
        freeItem->checkPosition(-1, 0, 0, Add);
        // Si no hay colisión o ha chocado con el jugador, el disparo se mueve
        if(freeItem->getMediator()->isCollisionStackEmpty() || freeItem->getMediator()->collisionWith(playerItem->getLabel()))
        {
          freeItem->setCollisionDetector(false);
          state->move(this, &stateId, false);
        }
        else
        {
          // Si ha chocado con un elemento enemigo entonces lo paraliza
          if(freeItem->getMediator()->collisionWithBadBoy())
          {
            propagateState(this->item, StateFreeze);
          }

          // En caso de colisión con cualquier elemento (excepto el jugador), el disparo desaparece
          dynamic_cast<UserControlled*>(playerItem->getBehavior())->setShotPresent(false);
          destroy = true;
        }

        // Se pone a cero el cronómetro para el siguiente ciclo
        speedTimer->reset();

        // Anima el elemento
        freeItem->animate();
      }
      break;

    case StateMoveSouth:
      if(speedTimer->getValue() > freeItem->getSpeed())
      {
        // Almacena en la pila de colisiones los elementos que hay al sur
        freeItem->setCollisionDetector(true);
        freeItem->checkPosition(1, 0, 0, Add);
        // Si no hay colisión o ha chocado con el jugador, el disparo se mueve
        if(freeItem->getMediator()->isCollisionStackEmpty() || freeItem->getMediator()->collisionWith(playerItem->getLabel()))
        {
          freeItem->setCollisionDetector(false);
          state->move(this, &stateId, false);
        }
        else
        {
          // Si ha chocado con un elemento enemigo entonces lo paraliza
          if(freeItem->getMediator()->collisionWithBadBoy())
          {
            propagateState(this->item, StateFreeze);
          }

          // En caso de colisión con cualquier elemento (excepto el jugador), el disparo desaparece
          dynamic_cast<UserControlled*>(playerItem->getBehavior())->setShotPresent(false);
          destroy = true;
        }

        // Se pone a cero el cronómetro para el siguiente ciclo
        speedTimer->reset();

        // Anima el elemento
        freeItem->animate();
      }
      break;

    case StateMoveEast:
      if(speedTimer->getValue() > freeItem->getSpeed())
      {
        // Almacena en la pila de colisiones los elementos que hay al este
        freeItem->setCollisionDetector(true);
        freeItem->checkPosition(0, -1, 0, Add);
        // Si no hay colisión o ha chocado con el jugador, el disparo se mueve
        if(freeItem->getMediator()->isCollisionStackEmpty() || freeItem->getMediator()->collisionWith(playerItem->getLabel()))
        {
          freeItem->setCollisionDetector(false);
          state->move(this, &stateId, false);
        }
        else
        {
          // Si ha chocado con un elemento enemigo entonces lo paraliza
          if(freeItem->getMediator()->collisionWithBadBoy())
          {
            propagateState(this->item, StateFreeze);
          }

          // En caso de colisión con cualquier elemento (excepto el jugador), el disparo desaparece
          dynamic_cast<UserControlled*>(playerItem->getBehavior())->setShotPresent(false);
          destroy = true;
        }

        // Se pone a cero el cronómetro para el siguiente ciclo
        speedTimer->reset();

        // Anima el elemento
        freeItem->animate();
      }
      break;

    case StateMoveWest:
      if(speedTimer->getValue() > freeItem->getSpeed())
      {
        // Almacena en la pila de colisiones los elementos que hay al oeste
        freeItem->setCollisionDetector(true);
        freeItem->checkPosition(0, 1, 0, Add);
        // Si no hay colisión o ha chocado con el jugador, el disparo se mueve
        if(freeItem->getMediator()->isCollisionStackEmpty() || freeItem->getMediator()->collisionWith(playerItem->getLabel()))
        {
          freeItem->setCollisionDetector(false);
          state->move(this, &stateId, false);
        }
        else
        {
          // Si ha chocado con un elemento enemigo entonces lo paraliza
          if(freeItem->getMediator()->collisionWithBadBoy())
          {
            propagateState(this->item, StateFreeze);
          }

          // En caso de colisión con cualquier elemento (excepto el jugador), el disparo desaparece
          dynamic_cast<UserControlled*>(playerItem->getBehavior())->setShotPresent(false);
          destroy = true;
        }

        // Se pone a cero el cronómetro para el siguiente ciclo
        speedTimer->reset();

        // Anima el elemento
        freeItem->animate();
      }
      break;

    default:
      ;
  }

  return destroy;
}

void Shot::setPlayerItem(PlayerItem* playerItem)
{
  this->playerItem = playerItem;
}

}
