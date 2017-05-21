
#include "ConveyorBelt.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "DisplaceState.hpp"
#include "FallState.hpp"
#include "Mediator.hpp"
#include "SoundManager.hpp"


namespace isomot
{

ConveyorBelt::ConveyorBelt( Item* item, const BehaviorId& id ) : Behavior( item, id )
{
        stateId = StateWait;
        active = false;
        speedTimer = new HPC();
        speedTimer->start();
}

ConveyorBelt::~ConveyorBelt( )
{
        delete speedTimer;
}

bool ConveyorBelt::update ()
{
        Mediator* mediator = item->getMediator();

        switch ( stateId )
        {
                case StateWait:
                // Si está parado comprueba si hay elementos encima
                if ( speedTimer->getValue() > item->getSpeed() )
                {
                        if ( ! item->checkPosition( 0, 0, 1, Add ) )
                        {
                                // Copia la pila de colisiones
                                std::stack< int > topItems;
                                while ( ! mediator->isStackOfCollisionsEmpty() )
                                {
                                        topItems.push( mediator->popCollision() );
                                }

                                // Mientras haya elementos encima de este elemento se comprobarán
                                // las condiciones para ver si pueden cambiar de estado
                                while ( ! topItems.empty () )
                                {
                                        // Identificador del primer elemento de la pila de colisiones
                                        int id = topItems.top();
                                        topItems.pop();

                                        // El elemento tiene que ser un elemento libre
                                        if ( id >= FirstFreeId && ( id & 1 ) )
                                        {
                                                FreeItem* topItem = dynamic_cast< FreeItem * >( mediator->findItemById( id ) );

                                                // El elemento debe tener comportamiento
                                                if ( topItem != 0 && topItem->getBehavior() != 0 )
                                                {
                                                        // El ancla del elemento debe ser esta cinta transportadora para proceder a arrastrarlo
                                                        if ( topItem->getAnchor() == 0 || item->getId() == topItem->getAnchor()->getId() )
                                                        {
                                                                if ( this->id == ConveyorBeltNortheast )
                                                                {
                                                                        if ( item->getDirection() == South )
                                                                        {
                                                                                if ( topItem->getBehavior()->getStateId() != StateRegularJump && topItem->getBehavior()->getStateId() != StateHighJump )
                                                                                {
                                                                                        topItem->getBehavior()->changeStateId( StateForceDisplaceNorth );
                                                                                }
                                                                        }
                                                                        else if ( item->getDirection() == West )
                                                                        {
                                                                                if ( topItem->getBehavior()->getStateId() != StateRegularJump && topItem->getBehavior()->getStateId() != StateHighJump )
                                                                                {
                                                                                        topItem->getBehavior()->changeStateId( StateForceDisplaceEast );
                                                                                }
                                                                        }
                                                                }
                                                                if ( this->id == ConveyorBeltSouthwest )
                                                                {
                                                                        if ( item->getDirection() == South )
                                                                        {
                                                                                if ( topItem->getBehavior()->getStateId() != StateRegularJump && topItem->getBehavior()->getStateId() != StateHighJump )
                                                                                {
                                                                                        topItem->getBehavior()->changeStateId( StateForceDisplaceSouth );
                                                                                }
                                                                        }
                                                                        else if ( item->getDirection() == West )
                                                                        {
                                                                                if ( topItem->getBehavior()->getStateId() != StateRegularJump && topItem->getBehavior()->getStateId() != StateHighJump )
                                                                                {
                                                                                        topItem->getBehavior()->changeStateId( StateForceDisplaceWest );
                                                                                }
                                                                        }
                                                                }

                                                                // Reproduce el sonido de la cinta transportadora
                                                                SoundManager::getInstance()->play( item->getLabel(), StateActive );
                                                        }
                                                }
                                        }
                                }
                        }

                        // Reinicia el cronómetro para el siguiente ciclo
                        speedTimer->reset();
                }
                break;

                default:
                        stateId = StateWait;
        }

        return false;
}

}
