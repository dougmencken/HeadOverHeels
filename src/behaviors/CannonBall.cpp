#include "CannonBall.hpp"
#include "FreeItem.hpp"
#include "Mediator.hpp"
#include "MoveState.hpp"
#include "Mediator.hpp"
#include "Room.hpp"

namespace isomot
{

CannonBall::CannonBall(Item* item, const BehaviorId& id) : Behavior(item, id)
{
  stateId = StateWait;
  speedTimer = new HPC();
  speedTimer->start();
}

CannonBall::~CannonBall()
{
  delete speedTimer;
}

bool CannonBall::update()
{
  FreeItem* freeItem = dynamic_cast<FreeItem*>(this->item);
  bool destroy = false;

  switch(stateId)
  {
    case StateWait:
      // Asigna el estado de movimiento
      this->changeStateId(StateMoveNorth);
      break;

    case StateMoveNorth:
      if(speedTimer->getValue() > freeItem->getSpeed())
      {
        // Almacena en la pila de colisiones los elementos que hay al norte
        freeItem->checkPosition(-1, 0, 0, Add);

        // Si no hay colisión, la bola se mueve
        if(freeItem->getMediator()->isCollisionStackEmpty())
        {
          state->move(this, &stateId, false);
        }
        else
        {
          // En caso de colisión con cualquier elemento (excepto el jugador), el disparo desaparece
          destroy = true;

          // Crea el elemento en la misma posición que el volátil y a su misma altura
          FreeItem* freeItem = new FreeItem(bubblesData,
                                            item->getX(), item->getY(), item->getZ(),
                                            NoDirection);

          freeItem->assignBehavior(VolatileTimeBehavior, 0);
          freeItem->setCollisionDetector(false);

          // Se añade a la sala actual
          item->getMediator()->getRoom()->addComponent(freeItem);
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

void CannonBall::setExtraData(void* data)
{
  this->bubblesData = reinterpret_cast<ItemData*>(data);
}

}
