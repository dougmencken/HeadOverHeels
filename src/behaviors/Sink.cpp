#include "Sink.hpp"
#include "Item.hpp"
#include "GridItem.hpp"
#include "FallState.hpp"
#include "SoundManager.hpp"

namespace isomot
{

Sink::Sink(Item* item, const BehaviorId& id) : Behavior(item, id)
{
  stateId = StateWait;
  fallenTimer = new HPC();
  fallenTimer->start();
}

Sink::~Sink()
{
  delete fallenTimer;
}

bool Sink::update()
{
  GridItem* gridItem = dynamic_cast<GridItem*>(this->item);

  switch(stateId)
  {
    case StateWait:
      // Si tiene un elemento encima entonces el elemento empieza a descender
      if(!gridItem->checkPosition(0, 0, 1, Add))
      {
        this->changeStateId(StateFall);
      }
      break;

    case StateFall:
      // Si ha llegado el momento de caer entonces el elemento desciende una unidad
      if(fallenTimer->getValue() > gridItem->getWeight())
      {
        // Si no puede seguir descendiendo o ya no hay ningún elemento encima entonces se detiene
        if(!state->fall(this) || gridItem->checkPosition(0, 0, 1, Add))
        {
          stateId = StateWait;
        }

        // Se pone a cero el cronómetro para el siguiente ciclo
        fallenTimer->reset();
      }
      break;

    default:
      stateId = StateWait;
  }

  // Emite el sonido acorde al estado
  this->soundManager->play(gridItem->getLabel(), stateId);

  return false;
}

}
