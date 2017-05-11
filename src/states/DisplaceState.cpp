
#include "DisplaceState.hpp"
#include "FallState.hpp"
#include "Behavior.hpp"
#include "Item.hpp"
#include "GridItem.hpp"
#include "FreeItem.hpp"


namespace isomot
{

BehaviorState* DisplaceState::instance = 0;

DisplaceState::DisplaceState() : BehaviorState()
{

}

DisplaceState::~DisplaceState()
{
}

BehaviorState* DisplaceState::getInstance()
{
        if ( instance == 0 )
        {
                instance = new DisplaceState();
        }

        return instance;
}

bool DisplaceState::displace( Behavior* behavior, StateId* substate, bool canFall )
{
        bool changedData = false;
        FreeItem* freeItem = 0;
        StateId displaceStateId = *substate;

        // Acceso al elemento que hay que mover si dicho elemento es libre
        if ( behavior->getItem()->getId() & 1 )
        {
                freeItem = dynamic_cast < FreeItem * > ( behavior->getItem () );
        }

        switch ( *substate )
        {
                case StateDisplaceNorth:
                case StateForceDisplaceNorth:
                        if ( freeItem != 0 )
                        {
                                changedData = freeItem->addToX( -1 );
                        }
                        displaceStateId = StateDisplaceNorth;
                        break;

                case StateDisplaceSouth:
                case StateForceDisplaceSouth:
                        if ( freeItem != 0 )
                        {
                                changedData = freeItem->addToX( 1 );
                        }
                        displaceStateId = StateDisplaceSouth;
                        break;

                case StateDisplaceEast:
                case StateForceDisplaceEast:
                        if ( freeItem != 0 )
                        {
                                changedData = freeItem->addToY( -1 );
                        }
                        displaceStateId = StateDisplaceEast;
                        break;

                case StateDisplaceWest:
                case StateForceDisplaceWest:
                        if ( freeItem != 0 )
                        {
                                changedData = freeItem->addToY( 1 );
                        }
                        displaceStateId = StateDisplaceWest;
                        break;

                case StateDisplaceNortheast:
                        if ( freeItem != 0 )
                        {
                                changedData = freeItem->addToPosition( -1, -1, 0 );
                        }
                        break;

                case StateDisplaceNorthwest:
                        if ( freeItem != 0 )
                        {
                                changedData = freeItem->addToPosition( -1, 1, 0 );
                        }
                        break;

                case StateDisplaceSoutheast:
                        if ( freeItem != 0 )
                        {
                                changedData = freeItem->addToPosition( 1, -1, 0 );
                        }
                        break;

                case StateDisplaceSouthwest:
                        if ( freeItem != 0 )
                        {
                                changedData = freeItem->addToPosition( 1, 1, 0 );
                        }
                        break;

                case StateDisplaceUp:
                        changedData = behavior->getItem()->addToZ( 1 );
                        break;

                default:
                        ;
        }

        if ( freeItem != 0 )
        {
                // En caso de colisión en los ejes X o Y se desplaza a los elementos implicados
                if ( ! changedData )
                {
                        this->propagateStateAdjacentItems( freeItem, displaceStateId );
                }
                // En caso de que el elemento se haya movido se comprueba si hay que desplazar los elementos
                // que pueda tener encima
                else
                {
                        this->propagateStateTopItems( freeItem, *substate );
                }
        }

        // Si el elemento puede caer entonces se comprueba si hay que cambiar el estado
        if ( canFall )
        {
                if ( FallState::getInstance()->fall( behavior ) )
                {
                        changeState( behavior, FallState::getInstance() );
                        *substate = StateFall;
                        changedData = true;
                }
        }

        return changedData;
}

}
