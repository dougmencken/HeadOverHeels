
#include "Switch.hpp"
#include "Item.hpp"
#include "PlayerItem.hpp"
#include "Mediator.hpp"
#include "SoundManager.hpp"


namespace isomot
{

Switch::Switch( Item * item, const BehaviorId & id ) :
        Behavior( item, id ),
        itemOver( false )
{
	this->stateId = StateWait;
}

Switch::~Switch( )
{

}

bool Switch::update ()
{
        Mediator * mediator = item->getMediator();
        std::vector< Item * > sideItems;

        switch ( stateId )
        {
                case StateWait:
                        // Comprueba si hay elementos a los lados
                        if ( checkSideItems( sideItems ) )
                        {
                                // Si algún elemento que cambió el estado del interruptor ya no está junto a él, o bien
                                // ese elemento está junto a él pero es un jugador y está detenido, entonces se elimina de
                                // la lista de activadores para que dicho elemento pueda volver a activar el interruptor
                                for ( size_t i = 0; i < triggerItems.size(); i++ )
                                {
                                        Item* tempItem = triggerItems[ i ];

                                        if ( std::find_if( sideItems.begin (), sideItems.end (), std::bind2nd( EqualItemId(), tempItem->getId() ) ) == sideItems.end() ||
                                                ( dynamic_cast< PlayerItem * >( tempItem ) && tempItem->getBehavior()->getStateId() == StateWait ) )
                                        {
                                                triggerItems.erase( std::remove_if(
                                                        triggerItems.begin (), triggerItems.end (),
                                                        std::bind2nd( EqualItemId(), tempItem->getId() )
                                                ), triggerItems.end () );
                                        }
                                }
                        }
                        else
                        {
                                triggerItems.clear();
                        }

                        // Comprueba si hay elementos encima
                        if ( ! item->checkPosition( 0, 0, 1, Add ) )
                        {
                                // Copia la pila de colisiones
                                std::stack< int > topItems;
                                while ( ! mediator->isStackOfCollisionsEmpty() )
                                {
                                        topItems.push( mediator->popCollision() );
                                }

                                // Mientras haya elementos encima de este elemento se comprobarán
                                // las condiciones para ver si se puede activar
                                while ( ! topItems.empty() )
                                {
                                        // Identificador del primer elemento de la pila de colisiones
                                        int id = topItems.top();
                                        topItems.pop();

                                        // El elemento tiene que ser un elemento libre
                                        if ( id >= FirstFreeId && ( id & 1 ) )
                                        {
                                                Item* topItem = mediator->findItemById( id );

                                                // El elemento debe tener comportamiento y debe estar exclusivamente sobre el interruptor
                                                // Además no puede activarse por el "roce" de un salto, sino que debe estar completamente posado
                                                // sobre el interruptor
                                                if ( topItem != 0 && topItem->getBehavior() != 0 && ! topItem->checkPosition( 0, 0, -1, Add ) &&
                                                        topItem->getBehavior()->getStateId() != StateRegularJump &&
                                                        topItem->getBehavior()->getStateId() != StateHighJump)
                                                {
                                                        // Si sólo hay un elemento debajo entonces se activa
                                                        if ( ! itemOver && mediator->depthOfStackOfCollisions() == 1 )
                                                        {
                                                                itemOver = true;
                                                                item->animate();
                                                                // Comunica a la sala el cambio de estado del interruptor
                                                                mediator->changeSwitchState();
                                                                // Activa el sonido de conmutación
                                                                this->soundManager->play( item->getLabel(), StateSwitch );
                                                        }
                                                }
                                        }
                                }
                        }
                        else
                        {
                                itemOver = false;
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
                        // Si el elemento no estaba junto al interruptor entonces ahora lo activa y ya no lo podrá
                        // volver a activar a no ser que se cumplan las condiciones fijadas en el estado inicial
                        if ( std::find_if( triggerItems.begin (), triggerItems.end (), std::bind2nd( EqualItemId(), sender->getId() ) ) == triggerItems.end () )
                        {
                                triggerItems.push_back( sender );
                                item->animate();
                                // Comunica a la sala el cambio de estado del interruptor
                                mediator->changeSwitchState();
                                // Activa el sonido de conmutación
                                this->soundManager->play( item->getLabel(), StateSwitch );
                        }

                        // Vuelta al estado inicial
                        stateId = StateWait;
                        break;

                default:
                        ;
        }

        return false;
}

bool Switch::checkSideItems( std::vector< Item * >& sideItems )
{
        Mediator* mediator = item->getMediator();

        // Comprueba si hay elementos al norte
        if ( ! item->checkPosition( -1, 0, 0, Add ) )
        {
                while ( ! mediator->isStackOfCollisionsEmpty() )
                {
                        sideItems.push_back( mediator->findCollisionPop() );
                }
        }

        // Comprueba si hay elementos al sur
        if ( ! item->checkPosition( 1, 0, 0, Add ) )
        {
                while ( ! mediator->isStackOfCollisionsEmpty() )
                {
                        sideItems.push_back( mediator->findCollisionPop() );
                }
        }

        // Comprueba si hay elementos al este
        if ( ! item->checkPosition( 0, -1, 0, Add ) )
        {
                while ( ! mediator->isStackOfCollisionsEmpty() )
                {
                        sideItems.push_back( mediator->findCollisionPop() );
                }
        }

        // Comprueba si hay elementos al oeste
        if ( ! item->checkPosition( 0, 1, 0, Add ) )
        {
                while ( ! mediator->isStackOfCollisionsEmpty() )
                {
                        sideItems.push_back( mediator->findCollisionPop() );
                }
        }

        return ! sideItems.empty();
}

}
