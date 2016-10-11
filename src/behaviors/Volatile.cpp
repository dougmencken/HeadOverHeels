#include "Volatile.hpp"
#include "Item.hpp"
#include "ItemData.hpp"
#include "FreeItem.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "SoundManager.hpp"

namespace isomot
{

Volatile::Volatile(Item* item, const BehaviorId& id) : Behavior(item, id)
{
  this->solid = false;
  stateId = StateWait;
  destroyTimer = new HPC();
  destroyTimer->start();
}

Volatile::~Volatile()
{
  delete destroyTimer;
}

bool Volatile::update()
{
  bool destroy = false;
  Mediator* mediator = item->getMediator();

  switch(stateId)
  {
    case StateWait:
    case StateWakeUp:
      // En este estado tiene que ser forzosamente volátil
      this->solid = false;

      // Si es volátil por contacto o por peso y tiene un elemento encima entonces puede destruirse
      if((id == VolatileTouchBehavior || id == VolatileWeightBehavior || id == VolatileHeavyBehavior) &&
         !item->checkPosition(0, 0, 1, Add))
      {
        bool mustBeDestroy = false;

        // Copia la pila de colisiones
        std::stack<Item*> topItems;

        // Se inspeccionan todos los elementos que haya encima para ver
        // si la destrucción se puede llevar a cabo
        while(!mediator->isCollisionStackEmpty())
        {
          // Identificador del primer elemento de la pila de colisiones
          int id = mediator->popCollision();

          // El elemento tiene que se un elemento libre
          if(id >= FirstFreeId && (id & 1))
          {
            Item* item = mediator->findItem(id);

            // Si el elemento existe, se mira si es volátil o especial porque
            // el elemento se destruirá a no ser que ese elemento esté apoyándose en otro
            if(item != 0 && item->getBehavior() != 0 &&
               item->getBehavior()->getId() != VolatileWeightBehavior &&
               item->getBehavior()->getId() != VolatileHeavyBehavior &&
               item->getBehavior()->getId() != VolatileTouchBehavior &&
               item->getBehavior()->getId() != SpecialBehavior)
            {
              mustBeDestroy = true;
              topItems.push(item);
            }
          }
        }

        if(mustBeDestroy)
        {
          Item* topItem = topItems.top();
          topItems.pop();
          topItem->checkPosition(0, 0, -1, Add);

          // Si el elemento que desencadena la destrucción está unicamente sobre el volátil
          // entonces la destrucción es segura. Si está sobre varios elementos, se tienen que dar
          // ciertas condiciones para poder destruilo
          if(mediator->collisionStackSize() > 1)
          {
            while(!mediator->isCollisionStackEmpty())
            {
              Item* bottomItem = mediator->findItem(mediator->popCollision());

              if(bottomItem != 0)
              {
                // El volátil no se destruirá si está apoyándose:
                // 1. Sobre un elemento sin comportamiento, o
                // 2. Sobre un elemento que no sea volátil, o
                // 3. Sobre un elemento que está destruyéndose
                if((bottomItem->getBehavior() == 0) ||
                   (bottomItem->getBehavior() != 0 && bottomItem->getBehavior()->getId() != VolatileWeightBehavior && bottomItem->getBehavior()->getId() != VolatileTouchBehavior && bottomItem->getBehavior()->getId() != SpecialBehavior) ||
                   (bottomItem->getBehavior() != 0 && bottomItem->getBehavior()->getStateId() == StateDestroy))
                {
                  mustBeDestroy = false;
                }
              }
            }
          }
        }

        // Cambio de estado si se han cumplido las condiciones
        if(mustBeDestroy)
        {
          stateId = StateDestroy;
          destroyTimer->reset();
        }
      }
      // Es un perrito del silencio. Desaparece si Head o el jugador compuesto están en la sala
      else if(id == VolatilePuppyBehavior)
      {
        if(mediator->findItem(short(Head)) != 0 || mediator->findItem(short(HeadAndHeels)) != 0)
        {
          stateId = StateDestroy;
          destroyTimer->reset();
        }
      }
      // Es volátil por tiempo, es decir, el elemento creado al destruirse el volátil
      else if(id == VolatileTimeBehavior)
      {
        destroy = (destroyTimer->getValue() > item->getFramesDelay() * double(item->getFramesNumber()));
        item->animate();
      }
      break;

    case StateDisplaceNorth:
    case StateDisplaceSouth:
    case StateDisplaceEast:
    case StateDisplaceWest:
    case StateDisplaceNortheast:
    case StateDisplaceSoutheast:
    case StateDisplaceSouthwest:
    case StateDisplaceNorthwest:
      // Si un elemento volátil por contacto es desplazado entonces se destruye
      if(!solid)
      {
        switch(id)
        {
          case VolatileTouchBehavior:
            stateId = StateDestroy;
            break;

          case VolatilePuppyBehavior:
            if(mediator->findItem(short(Head)) != 0 || mediator->findItem(short(HeadAndHeels)) != 0)
            {
              stateId = StateDestroy;
            }
            break;

          default:
            stateId = StateWait;
        }
      }
      else
      {
        stateId = StateFreeze;
      }
      break;

    case StateDestroy:
      if( ( id != VolatileWeightBehavior && id != VolatileHeavyBehavior && id != VolatilePuppyBehavior ) ||
         ( id == VolatileWeightBehavior && destroyTimer->getValue() > 0.030 ) ||
          ( id == VolatileHeavyBehavior && destroyTimer->getValue() > 0.600 ) ||
         ( id == VolatilePuppyBehavior && destroyTimer->getValue() > 0.500 ) )
      {
        destroy = true;

        // Emite el sonido de destrucción
        this->soundManager->play( item->getLabel(), stateId );

        // Crea el elemento en la misma posición que el volátil y a su misma altura
        FreeItem* freeItem = new FreeItem( bubblesData,
                                           item->getX(), item->getY(), item->getZ(),
                                           NoDirection );

        freeItem->assignBehavior( VolatileTimeBehavior, 0 );
        freeItem->setCollisionDetector( false );

        // Se añade a la sala actual
        mediator->getRoom()->addItem( freeItem );
      }
      break;

    case StateFreeze:
      this->solid = true;
      break;

    default:
      ;
  }

  return destroy;
}

void Volatile::setExtraData( void* data )
{
  this->bubblesData = reinterpret_cast< ItemData* >( data );
}

}
