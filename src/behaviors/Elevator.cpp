#include "Elevator.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "MoveState.hpp"
#include "SoundManager.hpp"

namespace isomot
{

Elevator::Elevator( Item * item, const BehaviorId & id ) :
        Behavior( item, id )
{
        stateId = StateWait;
        speedTimer = new HPC();
        stopTimer = new HPC();
        speedTimer->start();
        stopTimer->start();
}

Elevator::~Elevator( )
{
        delete speedTimer;
        delete stopTimer;
}

bool Elevator::update ()
{
  FreeItem* freeItem = dynamic_cast<FreeItem*>(this->item);

  switch(stateId)
  {
    case StateWait:
      changeStateId(ascent ? StateMoveUp : StateMoveDown);
      validState = stateId;
      break;

    case StateMoveUp:
      if(speedTimer->getValue() > freeItem->getSpeed())
      {
        // El elemento se mueve
        state->move(this, &stateId, false);

        // Se pone a cero el cronómetro para el siguiente ciclo
        speedTimer->reset();

        // Si ha llegado el techo entonces empieza el descenso
        if(freeItem->getZ() > top * LayerHeight)
        {
          stateId = StateStopTop;
          validState = stateId;
          stopTimer->reset();
        }
      }

      // Anima el elemento
      freeItem->animate();
      break;

    case StateMoveDown:
      if(speedTimer->getValue() > freeItem->getSpeed())
      {
        // El elemento se mueve
        state->move(this, &stateId, false);

        // Se pone a cero el cronómetro para el siguiente ciclo
        speedTimer->reset();

        // Si ha llegado al suelo entonces empieza el ascenso
        if(freeItem->getZ() <= bottom * LayerHeight)
        {
          stateId = StateStopBottom;
          validState = stateId;
          stopTimer->reset();
        }
      }

      // Anima el elemento
      freeItem->animate();
      break;

    // Detiene el ascensor un instante cuando alcanza la altura mínima
    case StateStopBottom:
      if(stopTimer->getValue() >= 0.250)
      {
        changeStateId(StateMoveUp);
        validState = stateId;
      }

      // Anima el elemento
      freeItem->animate();
      break;

    // Detiene el ascensor un instante cuando alcanza la altura máxima
    case StateStopTop:
      if(stopTimer->getValue() >= 0.250)
      {
        changeStateId(StateMoveDown);
        validState = stateId;
      }

      // Anima el elemento
      freeItem->animate();
      break;

    default:
      stateId = validState;
      state = MoveState::getInstance();
  }

  // Emite el sonido correspondiente
  this->soundManager->play(freeItem->getLabel(), stateId);

  return false;
}

void Elevator::setMoreData( void * data )
{
        int * values = reinterpret_cast< int * >( data );

        this->top = values[ 0 ];
        this->bottom = values[ 1 ];
        this->ascent = bool( values[ 2 ] );
}

}
