#include "UserControlled.hpp"
#include "Item.hpp"
#include "ItemData.hpp"
#include "ItemDataManager.hpp"
#include "PlayerItem.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "MoveState.hpp"
#include "DisplaceState.hpp"
#include "FallState.hpp"
#include "JumpState.hpp"
#include "Shot.hpp"
#include "GameManager.hpp"
#include "SoundManager.hpp"

namespace isomot
{

UserControlled::UserControlled(Item* item, const BehaviorId& id) : Behavior(item, id)
{
  jumpIndex = 0;
  jumpFrames = highJumpFrames = 0;
  automaticSteps = automaticStepsCounter = highSpeedSteps = shieldSteps = 0;
  fallFrames[North] = fallFrames[South] = fallFrames[East] = fallFrames[West] = 0xffffffff;
  speedTimer = 0;
  fallenTimer = 0;
  glideTimer = 0;
  blinkingTimer = 0;
  takenItemData = 0;
  takenItemImage = 0;
  takenItemBehaviorId = NoBehavior;
}

UserControlled::~UserControlled()
{

}

void UserControlled::changeStateId(const StateId& stateId, Item* sender)
{
  this->stateId = stateId;
  this->sender = sender;

  // Asigna el estado en función del identificador
  switch(stateId)
  {
    case StateMoveNorth:
    case StateMoveSouth:
    case StateMoveEast:
    case StateMoveWest:
    case StateMoveNortheast:
    case StateMoveNorthwest:
    case StateMoveSoutheast:
    case StateMoveSouthwest:
    case StateMoveUp:
    case StateMoveDown:
      state = MoveState::getInstance();
      break;

    case StateDisplaceNorth:
    case StateDisplaceSouth:
    case StateDisplaceEast:
    case StateDisplaceWest:
    case StateDisplaceNortheast:
    case StateDisplaceSoutheast:
    case StateDisplaceSouthwest:
    case StateDisplaceNorthwest:
    case StateForceDisplaceNorth:
    case StateForceDisplaceSouth:
    case StateForceDisplaceEast:
    case StateForceDisplaceWest:
      state = DisplaceState::getInstance();
      break;

    case StateFall:
      state = FallState::getInstance();
      break;

    case StateStartWayInTeletransport:
      this->item->changeFrame(this->nullFrame);
      break;

    default:
      ;
  }
}

void UserControlled::wait(PlayerItem* playerItem)
{
  playerItem->wait();

  // Se comprueba si el jugador debe empezar a caer
  if(FallState::getInstance()->fall(this))
  {
    speedTimer->reset();
    stateId = StateFall;

    // Si el jugador se detiene entonces se para la cuenta atrás
    // del conejo que proporciona doble velocidad
    if(playerItem->getLabel() == PlayerId(Head) && playerItem->getHighSpeed() > 0 && this->highSpeedSteps < 4)
    {
      this->highSpeedSteps = 0;
    }
  }
}

void UserControlled::move(PlayerItem* playerItem)
{
  // El elemento debe estar activo
  if(!playerItem->isDead())
  {
    // Si el jugador ha cogido velocidad extra entonces se divide su velocidad habitual
    double speed = playerItem->getSpeed() / (playerItem->getLabel() == PlayerId(Head) && playerItem->getHighSpeed() > 0 ? 2 : 1);

    // Si ha llegado el momento de moverse
    if(speedTimer->getValue() > speed)
    {
      state = MoveState::getInstance();

      // El elemento se mueve. Si el movimiento no se pudo realizar por colisión entonces
      // se desplaza a los elementos con los que pudiera haber chocado
      bool moved = state->move(this, &stateId, true);

      // Si se ha movido y no ha cambiado de estado entonces se cuenta un paso
      if(playerItem->getLabel() == PlayerId(Head) && playerItem->getHighSpeed() > 0 && moved && stateId != StateFall)
      {
        this->highSpeedSteps++;

        if(this->highSpeedSteps == 8)
        {
          playerItem->decreaseHighSpeed();
          this->highSpeedSteps = 4;
        }
      }

      // Se pone a cero el cronómetro para el siguiente ciclo
      speedTimer->reset();

      // Anima el elemento
      playerItem->animate();
    }
  }
}

void UserControlled::autoMove(PlayerItem* playerItem)
{
  // Si el jugador ha cogido velocidad extra entonces se divide su velocidad habitual
  double speed = playerItem->getSpeed() / (playerItem->getLabel() == PlayerId(Head) && playerItem->getHighSpeed() > 0 ? 2 : 1);

  // Si el elemento está activo y ha llegado el momento de moverse, entonces:
  if(speedTimer->getValue() > speed)
  {
    state = MoveState::getInstance();

    // El elemento se mueve. Si el movimiento no se pudo realizar por colisión entonces
    // se desplaza a los elementos con los que pudiera haber chocado
    state->move(this, &stateId, true);

    // Se pone a cero el cronómetro para el siguiente ciclo
    speedTimer->reset();

    // Anima el elemento
    playerItem->animate();

    // Se comprueba si seguirá moviéndose de forma automática
    if(--automaticStepsCounter < 0)
    {
      automaticStepsCounter = automaticSteps;
      stateId = StateWait;
    }
  }

  // Si deja de moverse de forma automática se detiene el sonido correspondiente
  if(stateId == StateWait)
  {
    this->soundManager->stop(playerItem->getLabel(), StateAutoMove);
  }
}

void UserControlled::displace(PlayerItem* playerItem)
{
  // El elemento es deplazado por otro. Si el desplazamiento no se pudo realizar por
  // colisión entonces el estado se propaga a los elementos con los que ha chocado
  if(speedTimer->getValue() > playerItem->getSpeed())
  {
    state->displace(this, &stateId, true);
    // Una vez se ha completado el desplazamiento el elemento vuelve a su comportamiento normal
    stateId = StateWait;
    // Se pone a cero el cronómetro para el siguiente ciclo
    speedTimer->reset();
  }
}

void UserControlled::cancelDisplace(PlayerItem* playerItem)
{
  // Si el elemento está activo y ha llegado el momento de moverse, entonces:
  if(!playerItem->isDead())
  {
    if(speedTimer->getValue() > playerItem->getSpeed())
    {
      state = MoveState::getInstance();

      // El elemento se mueve. Si el movimiento no se pudo realizar por colisión entonces
      // se desplaza a los elementos con los que pudiera haber chocado
      state->move(this, &stateId, false);

      // Se pone a cero el cronómetro para el siguiente ciclo
      speedTimer->reset();

      // Anima el elemento
      playerItem->animate();
    }
  }
}

void UserControlled::fall(PlayerItem* playerItem)
{
  // Si ha llegado el momento de caer entonces el elemento desciende una unidad
  if(fallenTimer->getValue() > playerItem->getWeight())
  {
    state = FallState::getInstance();

    // Si no hay colisión ahora ni en el siguiente ciclo, selecciona el fotograma de caída del personaje
    if(state->fall(this))
    {
      if(playerItem->checkPosition(0, 0, -1, Add))
      {
        playerItem->changeFrame(fallFrames[playerItem->getDirection()]);
      }
    }
    // Si hay colisión deja de caer y vuelve al estado inicial siempre que
    // el jugador no haya sido destruido por la colisión con un elemento mortal
    else if(stateId != StateStartDestroy || (stateId == StateStartDestroy && playerItem->getShieldTime() > 0))
    {
      stateId = StateWait;
    }

    // Se pone a cero el cronómetro para el siguiente ciclo
    fallenTimer->reset();
  }

  // Si deja de caer se detiene el sonido correspondiente
  if(stateId != StateFall)
  {
    this->soundManager->stop(playerItem->getLabel(), StateFall);
  }
}

void UserControlled::jump(PlayerItem* playerItem)
{
  switch(stateId)
  {
    case StateJump:
      // Almacena en la pila de colisiones los elementos que tiene debajo
      playerItem->checkPosition(0, 0, -1, Add);
      // Si está sobre un trampolín o tiene el conejo de los grandes saltos, el jugador dará un gran salto
      stateId = (playerItem->getMediator()->collisionWith(TrampolineBehavior) || (playerItem->getHighJumps() > 0 && playerItem->getLabel() == PlayerId(Heels)) ? StateHighJump : StateRegularJump);
      // Si está sobre el trampolín emite el sonido propio de este elemento
      if(stateId == StateHighJump)
      {
        if(playerItem->getLabel() == PlayerId(Heels))
        {
          playerItem->decreaseHighJumps();
        }
        this->soundManager->play(playerItem->getLabel(), StateRebound);
      }
      break;

    case StateRegularJump:
      // Si ha llegado el momento de saltar:
      if(speedTimer->getValue() > playerItem->getSpeed())
      {
        // Salta en función del ciclo actual
        state = JumpState::getInstance();
        state->jump(this, &stateId, jumpMatrix, &jumpIndex);

        // Se pone a cero el cronómetro para el siguiente ciclo
        speedTimer->reset();

        // Anima el elemento
        playerItem->animate();
      }
      break;

    case StateHighJump:
      // Si ha llegado el momento de saltar:
      if(speedTimer->getValue() > playerItem->getSpeed())
      {
        // Salta en función del ciclo actual
        state = JumpState::getInstance();
        state->jump(this, &stateId, highJumpMatrix, &jumpIndex);

        // Se pone a cero el cronómetro para el siguiente ciclo
        speedTimer->reset();

        // Anima el elemento
        playerItem->animate();
      }
      break;

    default:
      ;
  }

  // Si deja de saltar se detiene el sonido correspondiente
  if(stateId != StateJump && stateId != StateRegularJump && stateId != StateHighJump)
  {
    this->soundManager->stop(playerItem->getLabel(), StateJump);
  }

  // Si el jugador supera la altura máxima de la sala entonces pasa a la sala de arriba
  // Se supone que no hay posibilidad de alcanzar la altura máxima de una sala que no
  // conduce a otra situada sobre ella
  if(playerItem->getZ() >= MaxLayers * LayerHeight)
  {
    playerItem->setExit(Up);
    playerItem->setOrientation(playerItem->getDirection());
  }
}

void UserControlled::glide(PlayerItem* playerItem)
{
  // Si ha pasado el tiempo necesario para que el elemento descienda:
  //if(glideTimer->getValue() > playerItem->getSpeed() / 2.0)
  if(glideTimer->getValue() > playerItem->getWeight())
  {
    state = FallState::getInstance();

    // Si hay colisión deja de caer y vuelve al estado inicial
    if(!state->fall(this) && (stateId != StateStartDestroy || (stateId == StateStartDestroy && playerItem->getShieldTime() > 0)))
    {
      stateId = StateWait;
    }

    // Se pone a cero el cronómetro para el siguiente ciclo
    glideTimer->reset();
  }

  // Si ha pasado el tiempo necesario para mover el elemento:
  if(speedTimer->getValue() > playerItem->getSpeed() * (playerItem->getLabel() == PlayerId(HeadAndHeels) ? 2 : 1))
  {
    state = MoveState::getInstance();
    StateId substate;

    // Subestado para ejecutar el movimiento
    switch(playerItem->getDirection())
    {
      case North:
        substate = StateMoveNorth;
        break;

      case South:
        substate = StateMoveSouth;
        break;

      case East:
        substate = StateMoveEast;
        break;

      case West:
        substate = StateMoveWest;
        break;

      default:
        ;
    }

    // El elemento se mueve. Si el movimiento no se pudo realizar por colisión entonces
    // se desplaza a los elementos con los que pudiera haber chocado y el elemento da media
    // vuelta cambiando su estado a otro de movimiento
    state->move(this, &substate, false);

    // Selecciona el fotograma de caída del personaje
    playerItem->changeFrame(fallFrames[playerItem->getDirection()]);

    // Se pone a cero el cronómetro para el siguiente ciclo
    speedTimer->reset();
  }

  // Si deja de planear se detiene el sonido correspondiente
  if(stateId != StateGlide)
  {
    this->soundManager->stop(playerItem->getLabel(), StateGlide);
  }
}

void UserControlled::wayInTeletransport(PlayerItem* playerItem)
{
  switch(stateId)
  {
    case StateStartWayInTeletransport:
      // Almacena la orientación de entrada
      playerItem->setOrientation(playerItem->getDirection());
      // Cambia a las burbujas pero conservando la etiqueta del jugador
      playerData = playerItem->getItemData();
      playerItem->changeItemData(itemDataManager->find(transitionDataLabel), NoDirection);
      // Las burbujas se animarán marcha atrás
      playerItem->setBackwardMotion();
      // Inicia el teletransporte
      stateId = StateWayInTeletransport;
      break;

    case StateWayInTeletransport:
      // Anima el elemento. Si ha llegado al final el jugador aparece
      if(playerItem->animate())
      {
        // Cambia al aspecto original del jugador
        playerItem->changeItemData(playerData, NoDirection);
        // Se recupera su orientación original
        playerItem->changeDirection(playerItem->getOrientation());
        // Estado inicial
        stateId = StateWait;
      }
      break;

    default:
      ;
  }
}

void UserControlled::wayOutTeletransport(PlayerItem* playerItem)
{
  switch(stateId)
  {
    case StateStartWayOutTeletransport:
      // Almacena la orientación de salida
      playerItem->setOrientation(playerItem->getDirection());
      // Cambia a las burbujas pero conservando la etiqueta del jugador
      playerItem->changeItemData(itemDataManager->find(transitionDataLabel), NoDirection);
      // Inicia el teletransporte
      stateId = StateWayOutTeletransport;
      break;

    case StateWayOutTeletransport:
      // Anima el elemento. Si ha llegado al final hay cambio de sala
      if(playerItem->animate())
      {
        playerItem->addZ(-1);
        playerItem->setExit(playerItem->getMediator()->collisionWith(TeleportLabel) ? ByTeleport : ByTeleport2);
      }
      break;

    default:
      ;
  }
}

void UserControlled::destroy(PlayerItem* playerItem)
{
  switch(stateId)
  {
    // El jugador ha sido destruido por un enemigo
    case StateStartDestroy:
      // La destrucción se ejecuta siempre y cuando no tenga inmunidad
      if(playerItem->getShieldTime() == 0)
      {
        // Cambia a las burbujas pero conservando la etiqueta del jugador
        playerItem->changeItemData(itemDataManager->find(transitionDataLabel), NoDirection);
        // Inicia la destrucción
        stateId = StateDestroy;
      }
      // Si tiene inmunidad vuelve al estado inicial
      else
      {
        stateId = StateWait;
      }
      break;

    case StateDestroy:
      // Anima el elemento. Si ha llegado al final la sala se reinicia
      if(playerItem->animate())
      {
        playerItem->setExit(Restart);
        playerItem->loseLife();
      }
      break;

    default:
      ;
  }
}

void UserControlled::shot(PlayerItem* playerItem)
{
  // El jugador puede disparar si tiene la bocina y rosquillas
  if(playerItem->hasTool(Horn) && playerItem->getAmmo() > 0)
  {
    this->shotPresent = true;

    // Se buscan los datos del elemento empleado como disparo
    ItemData* shotData = this->itemDataManager->find(shotDataLabel);

    if(shotData != 0)
    {
      // Detiene el sonido del disparo
      this->soundManager->stop(playerItem->getLabel(), StateShot);

      // Crea el elemento en la misma posición que el jugador y a su misma altura
      int z = playerItem->getZ() + playerItem->getHeight() - shotData->height;
      FreeItem* freeItem = new FreeItem(shotData,
                                        playerItem->getX(), playerItem->getY(),
                                        z < 0 ? 0 : z,
                                        playerItem->getDirection());

      freeItem->assignBehavior(ShotBehavior, 0);
      dynamic_cast<Shot*>(freeItem->getBehavior())->setPlayerItem(playerItem);

      // En un primer momento no detecta colisiones ya que parte de la misma posición del jugador
      freeItem->setCollisionDetector(false);

      // Se añade a la sala actual
      playerItem->getMediator()->getRoom()->addComponent(freeItem);

      // Se gasta una rosquillas
      playerItem->consumeAmmo();

      // Emite el sonido del disparo
      this->soundManager->play(playerItem->getLabel(), StateShot);
    }
  }
}

void UserControlled::take(PlayerItem* playerItem)
{
  // El jugador puede coger objetos si tiene el bolso
  if(playerItem->hasTool(Handbag))
  {
    Mediator* mediator = playerItem->getMediator();
    Item* takenItem = 0;

    // Comprueba si hay elementos debajo
    if(!playerItem->checkPosition(0, 0, -1, Add))
    {
      int coordinates = 0;

      // Selecciona elementos libres empujables de tamaño menor o igual a 3/4 el tamaño de la loseta
      // De haber varios se seleccionará aquel con las coordenadas espaciales mayores
      while(!mediator->isCollisionStackEmpty())
      {
        Item* bottomItem = mediator->findItem(mediator->popCollision());

        if(bottomItem != 0 && bottomItem->getBehavior() != 0 &&
           (bottomItem->getBehavior()->getId() == MobileBehavior || bottomItem->getBehavior()->getId() == TrampolineBehavior) &&
           bottomItem->getWidthX() <= (mediator->getTileSize() * 3) / 4 &&
           bottomItem->getWidthY() <= (mediator->getTileSize() * 3) / 4)
        {
          if(bottomItem->getX() + bottomItem->getY() > coordinates)
          {
            coordinates = bottomItem->getX() + bottomItem->getY();
            takenItem = bottomItem;
            takenItemBehaviorId = bottomItem->getBehavior()->getId();
          }
        }
      }

      // Guarda el elemento seleccionado y lo hace desaparecer de la sala
      if(takenItem != 0)
      {
        takenItemData = itemDataManager->find(takenItem->getLabel());
        takenItem->getBehavior()->changeStateId(StateDestroy);
        stateId = (stateId == StateTakeAndJump ? StateJump : StateTakenItem);

        // Comunica al gestor del juego el elemento que se ha cogido
        takenItemImage = takenItem->getImage();
        GameManager::getInstance()->setItemTaken(takenItemImage);
        // Emite el sonido correspondiente
        this->soundManager->play(playerItem->getLabel(), StateTakeItem);
      }
    }
  }

  // Estado inicial si no se ha cogido ningún elemento
  if(stateId != StateTakenItem && stateId != StateJump)
  {
    stateId = StateWait;
  }
}

void UserControlled::drop(PlayerItem* playerItem)
{
  // El jugador debe haber cogido algún elemento
  if(takenItemData != 0)
  {
    // El elemento se deja justo debajo. Si el jugador no puede ascender no es posible dejarlo
    if(playerItem->addZ(LayerHeight))
    {
      // Crea el elemento en la misma posición pero debajo del jugador
      FreeItem* freeItem = new FreeItem(takenItemData,
                                        playerItem->getX(), playerItem->getY(),
                                        playerItem->getZ() - LayerHeight,
                                        NoDirection);

      // Sólo pueden cogerse los elementos portátiles o el trampolín
      freeItem->assignBehavior(takenItemBehaviorId, 0);

      // Se añade a la sala actual
      playerItem->getMediator()->getRoom()->addComponent(freeItem);

      // El jugador ya no tiene el elemento
      takenItemData = 0;
      takenItemImage = 0;
      takenItemBehaviorId = NoBehavior;

      // Se actualiza el estado para que salte o se detenga
      stateId = (stateId == StateDropAndJump ? StateJump : StateWait);

      // Comunica al gestor del juego que el bolso está vacío
      GameManager::getInstance()->setItemTaken(takenItemImage);
      // Emite el sonido correspondiente
      this->soundManager->stop(playerItem->getLabel(), StateFall);
      this->soundManager->play(playerItem->getLabel(), StateDropItem);
    }
    else
    {
      // Se emite sonido de error
      this->soundManager->play(playerItem->getLabel(), StateError);
    }
  }

  // Estado inicial si no se ha dejado ningún elemento
  if(stateId != StateJump)
  {
    stateId = StateWait;
  }
}

void UserControlled::setExtraData(void* data)
{
  this->itemDataManager = reinterpret_cast<ItemDataManager*>(data);
}

void UserControlled::assignTakenItem(ItemData* itemData, BITMAP* takenItemImage, const BehaviorId& behaviorId)
{
  this->takenItemData = itemData;
  this->takenItemImage = takenItemImage;
  this->takenItemBehaviorId = behaviorId;
}

}
