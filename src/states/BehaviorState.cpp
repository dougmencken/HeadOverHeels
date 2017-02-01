
#include "BehaviorState.hpp"
#include "Behavior.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "Room.hpp"
#include "Mediator.hpp"


namespace isomot
{

BehaviorState::BehaviorState()
{

}

BehaviorState::~BehaviorState()
{

}

bool BehaviorState::move( Behavior * behavior, StateId * substate, bool canFall )
{
        return true;
}

bool BehaviorState::displace( Behavior * behavior, StateId * substate, bool canFall )
{
        return true;
}

bool BehaviorState::fall( Behavior * behavior )
{
        return true;
}

bool BehaviorState::jump( Behavior * behavior, StateId * substate, const std::vector< JumpMotion >& jumpMatrix, int * jumpIndex )
{
        return true;
}

void BehaviorState::changeState( Behavior * behavior, BehaviorState * state )
{
        behavior->changeState( state );
}

void BehaviorState::propagateStateAdjacentItems( Item * sender, const StateId& stateId )
{
        Mediator* mediator = sender->getMediator();

        // Mientras haya elementos que hayan chocado con el emisor
        while ( ! mediator->isStackOfCollisionsEmpty() )
        {
                // Identificador del primer elemento de la pila de colisiones
                int id = mediator->popCollision();

                // El elemento tiene que ser un elemento libre o uno rejilla
                if ( ( id >= FirstFreeId && ( id & 1 ) ) || ( id >= FirstGridId && ! ( id & 1 ) ) )
                {
                        Item * item = mediator->findItemById( id );

                        // Si el elemento se ha encontrado entonces:
                        if ( item != 0 )
                        {
                                // Si tiene comportamiento se cambia su estado
                                if ( item->getBehavior() != 0 )
                                {
                                        // Si el elemento es un jugador y el emisor es mortal entonces el jugador muere
                                        if ( dynamic_cast< PlayerItem * >( item ) && sender->isMortal() && dynamic_cast< PlayerItem * >( item )->getShieldTime() <= 0 )
                                        {
                                                // Si el emisor ha chocado con más elementos el jugador no morirá porque
                                                // dichos elementos harán de tope
                                                if ( mediator->depthOfStackOfCollisions() == 1 )
                                                {
                                                        if ( item->getBehavior()->getStateId() != StateStartDestroy &&
                                                                item->getBehavior()->getStateId() != StateDestroy )
                                                        {
                                                                item->getBehavior()->changeStateId( StateStartDestroy );
                                                        }
                                                }
                                        }
                                        // Si el emisor es un jugador y el elemento es mortal entonces el jugador muere
                                        else if ( dynamic_cast< PlayerItem * >( sender ) && item->isMortal() &&
                                                        dynamic_cast< PlayerItem * >( sender )->getShieldTime() <= 0 )
                                        {
                                                if ( sender->getBehavior()->getStateId() != StateStartDestroy && item->getBehavior()->getStateId() != StateDestroy )
                                                {
                                                        sender->getBehavior()->changeStateId( StateStartDestroy );
                                                        item->getBehavior()->changeStateId( stateId, sender );
                                                }
                                        }
                                        // Si no, se comunica el estado de desplazamiento al elemento
                                        else
                                        {
                                                if ( item->getBehavior()->getStateId() != StateDestroy )
                                                {
                                                        item->getBehavior()->changeStateId( stateId, sender );
                                                }
                                        }
                                }
                                // Si no tiene comportamiento pero es mortal y el emisor es un jugador
                                // entonces el que cambia de estado es el jugador, ya que muere
                                else if ( dynamic_cast< PlayerItem * >( sender ) && item->isMortal() && dynamic_cast< PlayerItem * >( sender )->getShieldTime() <= 0 )
                                {
                                        if ( sender->getBehavior()->getStateId() != StateStartDestroy && sender->getBehavior()->getStateId() != StateDestroy )
                                        {
                                                sender->getBehavior()->changeStateId( StateStartDestroy );
                                        }
                                }
                        }
                }
                // Si es un elemento especial se comprueba si el elemento que propaga el estado es
                // un jugador y si puede haber llegado a los límites de la sala
                else if ( dynamic_cast< PlayerItem * >( sender ) &&
                                ( ( id == NorthBorder  &&  mediator->getRoom()->getDoor( North )  != 0 ) ||
                                  ( id == SouthBorder  &&  mediator->getRoom()->getDoor( South )  != 0 ) ||
                                  ( id == EastBorder  &&  mediator->getRoom()->getDoor( East )  != 0 ) ||
                                  ( id == WestBorder  &&  mediator->getRoom()->getDoor( West )  != 0 ) ||
                                  ( id == NortheastBorder  &&  mediator->getRoom()->getDoor( Northeast )  != 0 ) ||
                                  ( id == NorthwestBorder  &&  mediator->getRoom()->getDoor( Northwest )  != 0 ) ||
                                  ( id == SoutheastBorder  &&  mediator->getRoom()->getDoor( Southeast )  != 0 ) ||
                                  ( id == SouthwestBorder  &&  mediator->getRoom()->getDoor( Southwest )  != 0 ) ||
                                  ( id == EastnorthBorder  &&  mediator->getRoom()->getDoor( Eastnorth )  != 0 ) ||
                                  ( id == EastsouthBorder  &&  mediator->getRoom()->getDoor( Eastsouth )  != 0 ) ||
                                  ( id == WestnorthBorder  &&  mediator->getRoom()->getDoor( Westnorth )  != 0 ) ||
                                  ( id == WestsouthBorder  &&  mediator->getRoom()->getDoor( Westsouth )  != 0 ) ) )
                {
                        PlayerItem * player = dynamic_cast< PlayerItem * >( sender );

                        // El jugador está saliendo de la sala. Se comunica la dirección de salida
                        switch ( id )
                        {
                                case NorthBorder:
                                        player->setExit( North);
                                        player->setOrientation( North );
                                        break;

                                case SouthBorder:
                                        player->setExit( South );
                                        player->setOrientation( South );
                                        break;

                                case EastBorder:
                                        player->setExit( East );
                                        player->setOrientation( East );
                                        break;

                                case WestBorder:
                                        player->setExit( West );
                                        player->setOrientation( West );
                                        break;

                                case NortheastBorder:
                                        player->setExit( Northeast );
                                        player->setOrientation( North );
                                        break;

                                case NorthwestBorder:
                                        player->setExit( Northwest );
                                        player->setOrientation( North );
                                        break;

                                case SoutheastBorder:
                                        player->setExit( Southeast );
                                        player->setOrientation( South );
                                        break;

                                case SouthwestBorder:
                                        player->setExit( Southwest );
                                        player->setOrientation( South );
                                        break;

                                case EastnorthBorder:
                                        player->setExit( Eastnorth );
                                        player->setOrientation( East );
                                        break;

                                case EastsouthBorder:
                                        player->setExit( Eastsouth );
                                        player->setOrientation( East );
                                        break;

                                case WestnorthBorder:
                                        player->setExit( Westnorth );
                                        player->setOrientation( West );
                                        break;

                                case WestsouthBorder:
                                        player->setExit( Westsouth );
                                        player->setOrientation( West );
                                        break;

                                default:
                                        ;
                        }
                }
        }
}

void BehaviorState::propagateStateTopItems( Item * sender, const StateId& stateId )
{
        // Acceso al mediador
        Mediator* mediator = sender->getMediator();

        // Almacena en la pila de colisiones los elementos que tiene encima
        if ( ! sender->checkPosition( 0, 0, 1, Add ) )
        {
                // Copia la pila de colisiones
                std::stack< int > topItems;
                while ( ! mediator->isStackOfCollisionsEmpty() )
                {
                        topItems.push( mediator->popCollision() );
                }

                // Mientras haya elementos encima de este elemento se comprobarán
                // las condiciones para ver si pueden cambiar de estado
                while ( ! topItems.empty() )
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
                                        // Si debajo del elemento que está encima del elemento que pretende propagar su estado hay
                                        // más elementos, se busca el ancla
                                        if ( ! topItem->checkPosition( 0, 0, -1, Add ) )
                                        {
                                                // Si sólo hay un elemento debajo o debajo está el ancla, el estado se propaga
                                                if ( mediator->depthOfStackOfCollisions() == 1 || topItem->getAnchor() == sender )
                                                {
                                                        if ( topItem->getBehavior()->getStateId() != StateDestroy )
                                                        {
                                                                // Si el elemento es un jugador y el emisor es mortal entonces el jugador muere
                                                                if ( dynamic_cast< PlayerItem * >( topItem ) && sender->isMortal() &&
                                                                        dynamic_cast< PlayerItem * >( topItem )->getShieldTime() <= 0 )
                                                                {
                                                                        if ( topItem->getBehavior()->getStateId() != StateStartDestroy )
                                                                        {
                                                                                topItem->getBehavior()->changeStateId( StateStartDestroy );
                                                                        }
                                                                }
                                                                // Si no, se comunica el estado de desplazamiento al elemento
                                                                else
                                                                {
                                                                        // Se envía el propio elemento como emisor para saber que está
                                                                        // siendo desplazado por un elemento situado debajo de él
                                                                        StateId currentStateId = topItem->getBehavior()->getStateId();
                                                                        if ( currentStateId != StateDisplaceNorth && currentStateId != StateDisplaceSouth &&
                                                                                currentStateId != StateDisplaceEast && currentStateId != StateDisplaceWest )
                                                                        {
                                                                                topItem->getBehavior()->changeStateId( stateId, topItem );
                                                                        }
                                                                }
                                                        }
                                                }
                                        }
                                }
                        }
                }
        }
}

}
