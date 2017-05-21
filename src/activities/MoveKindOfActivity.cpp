
#include "MoveKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Behavior.hpp"
#include "Item.hpp"
#include "GridItem.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "Mediator.hpp"

#include <iostream>


namespace isomot
{

KindOfActivity * MoveKindOfActivity::instance = 0 ;


MoveKindOfActivity::MoveKindOfActivity( ) : KindOfActivity()
{

}

MoveKindOfActivity::~MoveKindOfActivity( )  { }


KindOfActivity* MoveKindOfActivity::getInstance()
{
        if ( instance == 0 )
        {
                instance = new MoveKindOfActivity();
        }

        return instance;
}

bool MoveKindOfActivity::move( Behavior* behavior, ActivityOfItem* activity, bool canFall )
{
        bool changedData = false;
        bool loading = false;
        ActivityOfItem displaceActivity = Wait;
        FreeItem* freeItem = 0;
        Mediator* mediator = 0;

        // Acceso al elemento que hay que mover si dicho elemento es libre
        if ( behavior->getItem()->getId() & 1 )
        {
                freeItem = dynamic_cast< FreeItem * >( behavior->getItem() );
                mediator = freeItem->getMediator();
        }

        switch ( *activity )
        {
                case MoveNorth:
                case AutoMoveNorth:
                        if ( freeItem != 0 )
                        {
                                if ( freeItem->getDirection() != North )
                                {
                                        freeItem->changeDirection( North );
                                }

                                changedData = freeItem->addToX( -1 );
                                displaceActivity = DisplaceNorth;
                        }
                        break;

                case MoveSouth:
                case AutoMoveSouth:
                        if ( freeItem != 0 )
                        {
                                if ( freeItem->getDirection() != South )
                                {
                                        freeItem->changeDirection( South );
                                }

                                changedData = freeItem->addToX( 1 );
                                displaceActivity = DisplaceSouth;
                        }
                        break;

                case MoveEast:
                case AutoMoveEast:
                        if ( freeItem != 0 )
                        {
                                if ( freeItem->getDirection() != East )
                                {
                                        freeItem->changeDirection( East );
                                }

                                changedData = freeItem->addToY( -1 );
                                displaceActivity = DisplaceEast;
                        }
                        break;

                case MoveWest:
                case AutoMoveWest:
                        if ( freeItem != 0 )
                        {
                                if ( freeItem->getDirection() != West )
                                {
                                        freeItem->changeDirection( West );
                                }

                                changedData = freeItem->addToY( 1 );
                                displaceActivity = DisplaceWest;
                        }
                        break;

                case MoveNortheast:
                        if ( freeItem != 0 )
                        {
                                changedData = freeItem->addToPosition( -1, -1, 0 );
                                displaceActivity = DisplaceNortheast;
                        }
                        break;

                case MoveNorthwest:
                        if ( freeItem != 0 )
                        {
                                changedData = freeItem->addToPosition( -1, 1, 0 );
                                displaceActivity = DisplaceNorthwest;
                        }
                        break;

                case MoveSoutheast:
                        if ( freeItem != 0 )
                        {
                                changedData = freeItem->addToPosition( 1, -1, 0 );
                                displaceActivity = DisplaceSoutheast;
                        }
                        break;

                case MoveSouthwest:
                        if ( freeItem != 0 )
                        {
                                changedData = freeItem->addToPosition( 1, 1, 0 );
                                displaceActivity = DisplaceSouthwest;
                        }
                        break;

                case MoveUp:
                        // Si no ha podido ascender, levanta a todos los elementos que pudiera tener encima
                        if ( ! ( changedData = freeItem->addToZ( 1 ) ) )
                        {
                                while ( ! mediator->isStackOfCollisionsEmpty() )
                                {
                                        FreeItem* topItem = dynamic_cast< FreeItem * >( mediator->findCollisionPop( ) );

                                        if ( topItem != 0 && freeItem->getWidthX() + freeItem->getWidthY() >= topItem->getWidthX() + topItem->getWidthY() )
                                        {
                                                ascent( topItem, 1 );
                                        }
                                }

                                // Ahora ya puede ascender
                                changedData = freeItem->addToZ( 1 );
                        }
                        break;

                case MoveDown:
                {
                        // Comprueba si tiene elementos encima
                        loading = !freeItem->checkPosition( 0, 0, 2, Add );

                        // Copia la pila de colisiones
                        std::stack< int > topItems;
                        while ( ! mediator->isStackOfCollisionsEmpty() )
                        {
                                topItems.push( mediator->popCollision() );
                        }

                        // Si pudo descender entonces deben descender con él los elementos que tiene encima
                        if ( ( changedData = freeItem->addToZ( -1 ) ) && loading )
                        {
                                while ( ! topItems.empty() )
                                {
                                        FreeItem* topItem = dynamic_cast< FreeItem * >( mediator->findItemById( topItems.top() ) );
                                        topItems.pop();

                                        if ( topItem != 0 )
                                        {
                                                descend( topItem, 2 );
                                        }
                                }
                        }
                }
                        break;

                case CancelDisplaceNorth:
                        if ( freeItem->getDirection() != South )
                        {
                                freeItem->changeDirection( South );
                        }
                        break;

                case CancelDisplaceSouth:
                        if ( freeItem->getDirection() != North )
                        {
                                freeItem->changeDirection( North );
                        }
                        break;

                case CancelDisplaceEast:
                        if ( freeItem->getDirection() != West )
                        {
                                freeItem->changeDirection( West );
                        }
                        break;

                case CancelDisplaceWest:
                        if ( freeItem->getDirection() != East )
                        {
                                freeItem->changeDirection( East );
                        }
                        break;

                default:
                        ;
        }

        if ( freeItem != 0 )
        {
                // En caso de colisión en los ejes X o Y se desplaza a los elementos implicados
                if ( ! changedData )
                {
                        this->propagateActivityToAdjacentItems( freeItem, displaceActivity );
                }
                // En caso de que el elemento se haya movido se comprueba si hay que desplazar los elementos
                // que pueda tener encima. La excepción es para el movimiento vertical porque de lo contrario
                // se cambiaría el estado de los elementos situados encima de un ascensor
                else if ( *activity != MoveUp && *activity != MoveDown )
                {
                        this->propagateActivityToTopItems( freeItem, displaceActivity );
                }
        }

        // Si el elemento puede caer entonces se comprueba si hay que cambiar el estado
        if ( canFall && *activity )
        {
                if ( FallKindOfActivity::getInstance()->fall( behavior ) )
                {
                        changeKindOfActivity( behavior, FallKindOfActivity::getInstance() );
                        *activity = Fall;
                        changedData = true;
                }
        }

        return changedData;
}

void MoveKindOfActivity::ascent( FreeItem* freeItem, int z )
{
        // El elemento debe poder cambiar de estado
        if ( freeItem->getBehavior() != 0 )
        {
                // Si el elemento no es el ascensor entonces se levanta
                if ( freeItem->getBehavior()->getBehaviorOfItem () != ElevatorBehavior )
                {
                        // Si no se puede levantar, se toma el elemento con el que choca para levantarlo
                        if ( ! freeItem->addToZ( z ) )
                        {
                                Mediator* mediator = freeItem->getMediator();

                                // Para todo elemento que pueda tener encima
                                while ( ! mediator->isStackOfCollisionsEmpty() )
                                {
                                        FreeItem* topItem = dynamic_cast< FreeItem * >( mediator->findCollisionPop( ) );

                                        if ( topItem != 0 && freeItem->getWidthX() + freeItem->getWidthY() >= topItem->getWidthX() + topItem->getWidthY() )
                                        {
                                                // Levanta recursivamente a todos los elementos
                                                ascent( topItem, z );
                                        }
                                }

                                // Ahora ya puede ascender
                                freeItem->addToZ( z );
                        }

                        // Si el elemento es un jugador y supera la altura máxima de la sala entonces pasa a
                        // la sala de arriba. Se supone que no hay posibilidad de alcanzar la altura máxima
                        // de una sala que no conduce a otra situada sobre ella
                        PlayerItem* playerItem = dynamic_cast< PlayerItem * >( freeItem );
                        if ( playerItem != 0 && playerItem->getZ() >= MaxLayers * LayerHeight )
                        {
                                playerItem->setExit( Up );
                                playerItem->setOrientation( playerItem->getDirection() );
                        }
                }
        }
}

void MoveKindOfActivity::descend( FreeItem* freeItem, int z )
{
        // El elemento debe poder cambiar de estado
        if ( freeItem->getBehavior() != 0 )
        {
                Mediator* mediator = freeItem->getMediator();

                // Comprueba si tiene elementos encima
                bool loading = ! freeItem->checkPosition( 0, 0, z, Add );

                // Copia la pila de colisiones
                std::stack< int > topItems;
                while ( ! mediator->isStackOfCollisionsEmpty() )
                {
                        topItems.push( mediator->popCollision() );
                }

                // Si puede descender entonces hace bajar a todos los elementos apilados encima
                if ( freeItem->addToZ( -1 ) && loading )
                {
                        // Desciende el resto de unidades. Se hace de una en una porque de lo contrario
                        // se podría detectar colisión y no descendería ninguna unidad
                        for ( int i = 0; i < ( z - 1 ); i++ )
                        {
                                freeItem->addToZ( -1 );
                        }

                        // Para todo elemento que pueda tener encima
                        while ( ! mediator->isStackOfCollisionsEmpty() )
                        {
                                FreeItem* topItem = dynamic_cast< FreeItem * >( mediator->findItemById( topItems.top() ) );
                                topItems.pop();

                                if ( topItem != 0 )
                                {
                                        // Desciende recursivamente a todos los elementos
                                        descend( topItem, z );
                                }
                        }
                }
        }
}

}
