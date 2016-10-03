#include "Hunter.hpp"
#include "Item.hpp"
#include "ItemData.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "DisplaceState.hpp"
#include "FallState.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"
#include "GameManager.hpp"

namespace isomot
{

Hunter::Hunter(Item* item, const BehaviorId& id) : Behavior(item, id)
{
  stateId = StateWait;
  speedTimer = new HPC();
  speedTimer->start();
}

Hunter::~Hunter()
{
  delete speedTimer;
}

bool Hunter::update()
{
  FreeItem* freeItem = dynamic_cast<FreeItem*>(this->item);
  PlayerItem* playerItem = freeItem->getMediator()->getActivePlayer();
  Mediator* mediator = freeItem->getMediator();
  bool destroy = false;

  switch(stateId)
  {
    case StateWait:
      // Si el elemento no tiene que esperar la cercanía del jugador, se activa sin más
      if(id == Hunter4Behavior || id == Hunter8Behavior)
      {
        this->soundManager->play(freeItem->getLabel(), stateId);
        stateId = calculateDirection(stateId);
      }
      // Se comprueba la cercanía del jugador para activar el elemento
      else
      {
        int delta = mediator->getTileSize() * 3;

        // Si el jugador está dentro del rectángulo definido en torno al cazador entonces el cazador se activa
        if(playerItem != 0 &&
           playerItem->getX() > freeItem->getX() - delta &&
           playerItem->getX() < freeItem->getX() + freeItem->getWidthX() + delta &&
           playerItem->getY() > freeItem->getY() - delta &&
           playerItem->getY() < freeItem->getY() + freeItem->getWidthY() + delta)
        {
          stateId = calculateDirection(stateId);
        }

        // Si se mueve en ocho direcciones emite sonido cuando está detenido
        if(id == HunterWaiting8Behavior)
        {
          this->soundManager->play(freeItem->getLabel(), stateId);
        }

        // Anima el elemento aunque esté detenido
        freeItem->animate();
      }
      break;

    case StateMoveNorth:
    case StateMoveSouth:
    case StateMoveEast:
    case StateMoveWest:
      // Si se crea el guarda completo entonces el elemento actual debe destruirse
      if(id == HunterWaiting4Behavior && createFullBody())
      {
        destroy = true;
      }
      // Si el elemento está activo y ha llegado el momento de moverse, entonces:
      else if(!freeItem->isDead())
      {
        if(speedTimer->getValue() > freeItem->getSpeed())
        {
          // El elemento se mueve
          state->move(this, &stateId, false);
          // Se pone a cero el cronómetro para el siguiente ciclo
          speedTimer->reset();
          // Comprueba si hay cambio de dirección
          stateId = calculateDirection(stateId);
          // Cae si tiene que hacerlo
          if(freeItem->getWeight() > 0)
          {
            FallState::getInstance()->fall(this);
          }
        }

        // Anima el elemento
        freeItem->animate();

        // Emite el sonido de movimiento
        this->soundManager->play(freeItem->getLabel(), stateId);
      }
      break;

    case StateMoveNortheast:
    case StateMoveNorthwest:
    case StateMoveSoutheast:
    case StateMoveSouthwest:
      // Si el elemento está activo y ha llegado el momento de moverse, entonces:
      if(!freeItem->isDead())
      {
        if(speedTimer->getValue() > freeItem->getSpeed())
        {
          // El elemento se mueve
          if(!state->move(this, &stateId, false))
          {
            if(stateId == StateMoveNortheast || stateId == StateMoveNorthwest)
            {
              StateId tempStateId = StateMoveNorth;
              if(!state->move(this, &tempStateId, false))
              {
                stateId = (stateId == StateMoveNortheast ? StateMoveEast : StateMoveWest);
                if(freeItem->getWeight() > 0)
                {
                  FallState::getInstance()->fall(this);
                }
              }
            }
            else
            {
              StateId tempStateId = StateMoveSouth;
              if(!state->move(this, &tempStateId, false))
              {
                stateId = (stateId == StateMoveSoutheast ? StateMoveEast : StateMoveWest);
                if(freeItem->getWeight() > 0)
                {
                  FallState::getInstance()->fall(this);
                }
              }
            }
          }
          else
          {
            // Comprueba si hay cambio de dirección
            stateId = calculateDirection(stateId);
          }

          // Se pone a cero el cronómetro para el siguiente ciclo
          speedTimer->reset();

        }

        // Anima el elemento
        freeItem->animate();

        // Emite el sonido de movimiento
        this->soundManager->play(freeItem->getLabel(), stateId);
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
        // El elemento se mueve
        state->displace(this, &stateId, false);
        stateId = StateWait;
        // Se pone a cero el cronómetro para el siguiente ciclo
        speedTimer->reset();
      }

      // Si el elemento estaba inactivo, después del desplazamiento seguirá estando
      if(freeItem->isDead())
      {
        stateId = StateFreeze;
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

StateId Hunter::calculateDirection(const StateId& stateId)
{
  if(id == Hunter4Behavior || id == HunterWaiting4Behavior)
  {
    return calculateDirection4(stateId);
  }
  else if(id == Hunter8Behavior || id == HunterWaiting8Behavior)
  {
    return calculateDirection8(stateId);
  }

  return StateWait;
}

StateId Hunter::calculateDirection4(const StateId& stateId)
{
  FreeItem* freeItem = dynamic_cast<FreeItem*>(this->item);
  PlayerItem* playerItem = freeItem->getMediator()->getActivePlayer();

  // Se comprueba si el jugador activo está en la sala
  if(playerItem != 0)
  {
    int dx = freeItem->getX() - playerItem->getX();
    int dy = freeItem->getY() - playerItem->getY();

    if(abs(dy) > abs(dx))
    {
      // Se moverá al este o al oeste porque la distancia X al jugador es menor
      if(dx > 0)
      {
        // Si la distancia es positiva se mueve al norte
        changeStateId(StateMoveNorth);
      }
      else if(dx < 0)
      {
        // Si la distancia es negativa se mueve al sur
        changeStateId(StateMoveSouth);
      }
      else
      {
        if(dy > 0)
        {
          // Si la distancia es positiva se mueve al este
          changeStateId(StateMoveEast);
        }
        else if(dy < 0)
        {
          // Si la distancia es negativa se mueve al oeste
          changeStateId(StateMoveWest);
        }
      }
    }
    else if(abs(dy) < abs(dx))
    {
      // Se moverá al norte o al sur porque la distancia Y al jugador es menor
      if(dy > 0)
      {
        // Si la distancia es positiva se mueve al este
        changeStateId(StateMoveEast);
      }
      else if(dy < 0)
      {
        // Si la distancia es negativa se mueve al oeste
        changeStateId(StateMoveWest);
      }
      else
      {
        if(dx > 0)
        {
          // Si la distancia es positiva se mueve al norte
          changeStateId(StateMoveNorth);
        }
        else if(dx < 0)
        {
          // Si la distancia es negativa se mueve al sur
          changeStateId(StateMoveSouth);
        }
      }
    }
  }

  return stateId;
}

StateId Hunter::calculateDirection8(const StateId& stateId)
{
  FreeItem* freeItem = dynamic_cast<FreeItem*>(this->item);
  PlayerItem* playerItem = freeItem->getMediator()->getActivePlayer();

  // Se comprueba si el jugador activo está en la sala
  if(playerItem != 0)
  {
    // Distancia del cazador al jugador en los ejes X e Y
    int dx = freeItem->getX() - playerItem->getX();
    int dy = freeItem->getY() - playerItem->getY();

    // El siguiente algoritmo obtiene la dirección más adecuada, aquella que permita alcanzar al
    // jugador lo más rápido posible, en función de las distancias en los ejes X e Y entre el
    // cazador y el jugador
    if(abs(dy) > abs(dx))
    {
      if(dx > 1)
      {
        if(dy > 1)
        {
          changeStateId(StateMoveNortheast);
        }
        else if(dy < -1)
        {
          changeStateId(StateMoveNorthwest);
        }
        else
        {
          changeStateId(StateMoveNorth);
        }
      }
      else if(dx < -1)
      {
        if(dy > 1)
        {
          changeStateId(StateMoveSoutheast);
        }
        else if(dy < -1)
        {
          changeStateId(StateMoveSouthwest);
        }
        else
        {
          changeStateId(StateMoveSouth);
        }
      }
      else
      {
        if(dy > 0)
        {
          changeStateId(StateMoveEast);
        }
        else if(dy < 0)
        {
          changeStateId(StateMoveWest);
        }
      }
    }
    else if(abs(dy) < abs(dx))
    {
      if(dy > 1)
      {
        if(dx > 1)
        {
          changeStateId(StateMoveNortheast);
        }
        else if(dx < -1)
        {
          changeStateId(StateMoveSoutheast);
        }
        else
        {
          changeStateId(StateMoveEast);
        }
      }
      else if(dy < -1)
      {
        if(dx > 1)
        {
          changeStateId(StateMoveNorthwest);
        }
        else if(dx < -1)
        {
          changeStateId(StateMoveSouthwest);
        }
        else
        {
          changeStateId(StateMoveWest);
        }
      }
      else
      {
        if(dx > 0)
        {
          changeStateId(StateMoveNorth);
        }
        else if(dx < 0)
        {
          changeStateId(StateMoveSouth);
        }
      }
    }

    // El guardián del trono huirá del jugador si éste tiene cuatro coronas
    if(item->getLabel() == ThroneGuard && GameManager::getInstance()->getNumberFreePlanets() >= 4)
    {
       changeStateId(StateMoveSouthwest);
    }
  }

  return stateId;
}

bool Hunter::createFullBody()
{
  FreeItem* freeItem = dynamic_cast<FreeItem*>(this->item);
  bool created = false;

  if(freeItem->getLabel() == ImperialGuardHeadLabel && freeItem->checkPosition(0, 0, -LayerHeight, Add))
  {
    created = true;

    // Crea el elemento en la misma posición que el jugador y a su misma altura
    FreeItem* newItem = new FreeItem(guardData,
                                     freeItem->getX(), freeItem->getY(), freeItem->getZ() - LayerHeight,
                                     freeItem->getDirection());

    newItem->assignBehavior(Hunter4Behavior, 0);

    // El elemento actual debe dejar de detectar colisiones porque,
    // de lo contrariom no se podrá crear el guarda completo
    freeItem->setCollisionDetector(false);
    // Se añade a la sala actual
    freeItem->getMediator()->getRoom()->addComponent(newItem);
  }

  return created;
}

void Hunter::setExtraData(void* data)
{
  this->guardData = reinterpret_cast<ItemData*>(data);
}

}
