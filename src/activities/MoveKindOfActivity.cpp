
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
        bool moved = false;

        ActivityOfItem displaceActivity = Wait;

        FreeItem* freeItem = 0;
        Mediator* mediator = 0;

        // is item free or player
        if ( behavior->getItem()->whichKindOfItem() == "free item" || behavior->getItem()->whichKindOfItem() == "player item" )
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
                                freeItem->changeOrientation( North );
                                moved = freeItem->addToX( -1 );
                                displaceActivity = DisplaceNorth;
                        }
                        break;

                case MoveSouth:
                case AutoMoveSouth:
                        if ( freeItem != 0 )
                        {
                                freeItem->changeOrientation( South );
                                moved = freeItem->addToX( 1 );
                                displaceActivity = DisplaceSouth;
                        }
                        break;

                case MoveEast:
                case AutoMoveEast:
                        if ( freeItem != 0 )
                        {
                                freeItem->changeOrientation( East );
                                moved = freeItem->addToY( -1 );
                                displaceActivity = DisplaceEast;
                        }
                        break;

                case MoveWest:
                case AutoMoveWest:
                        if ( freeItem != 0 )
                        {
                                freeItem->changeOrientation( West );
                                moved = freeItem->addToY( 1 );
                                displaceActivity = DisplaceWest;
                        }
                        break;

                case MoveNortheast:
                        if ( freeItem != 0 )
                        {
                                moved = freeItem->addToPosition( -1, -1, 0 );
                                displaceActivity = DisplaceNortheast;
                        }
                        break;

                case MoveNorthwest:
                        if ( freeItem != 0 )
                        {
                                moved = freeItem->addToPosition( -1, 1, 0 );
                                displaceActivity = DisplaceNorthwest;
                        }
                        break;

                case MoveSoutheast:
                        if ( freeItem != 0 )
                        {
                                moved = freeItem->addToPosition( 1, -1, 0 );
                                displaceActivity = DisplaceSoutheast;
                        }
                        break;

                case MoveSouthwest:
                        if ( freeItem != 0 )
                        {
                                moved = freeItem->addToPosition( 1, 1, 0 );
                                displaceActivity = DisplaceSouthwest;
                        }
                        break;

                case MoveUp:
                        moved = freeItem->addToZ( 1 );

                        // if can’t move up, raise items above
                        if ( ! moved )
                        {
                                while ( ! mediator->isStackOfCollisionsEmpty() )
                                {
                                        FreeItem* topItem = dynamic_cast< FreeItem * >( mediator->findCollisionPop( ) );

                                        if ( topItem != 0 && freeItem->getWidthX() + freeItem->getWidthY() >= topItem->getWidthX() + topItem->getWidthY() )
                                        {
                                                ascent( topItem, 1 );
                                        }
                                }

                                // now raise itself
                                moved = freeItem->addToZ( 1 );
                        }
                        break;

                case MoveDown:
                {
                        // is there any items above
                        bool loading = ! freeItem->checkPosition( 0, 0, 2, Add );

                        // copy stack of collisions
                        std::stack< int > topItems;
                        while ( ! mediator->isStackOfCollisionsEmpty() )
                        {
                                topItems.push( mediator->popCollision() );
                        }

                        moved = freeItem->addToZ( -1 );

                        // fall together with items above
                        if ( moved && loading )
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
                        freeItem->changeOrientation( South );
                        break;

                case CancelDisplaceSouth:
                        freeItem->changeOrientation( North );
                        break;

                case CancelDisplaceEast:
                        freeItem->changeOrientation( West );
                        break;

                case CancelDisplaceWest:
                        freeItem->changeOrientation( East );
                        break;

                default:
                        ;
        }

        if ( freeItem != 0 )
        {
                // move collided items when there’s horizontal collision
                if ( ! moved )
                {
                        this->propagateActivityToAdjacentItems( freeItem, displaceActivity );
                }
                // see if is it necessary to move items above
                // exception is for vertical movement to keep activity of items above elevator unchanged
                else if ( *activity != MoveUp && *activity != MoveDown )
                {
                        this->propagateActivityToItemsAbove( freeItem, displaceActivity );
                }
        }

        // item may fall
        if ( canFall && *activity )
        {
                if ( FallKindOfActivity::getInstance()->fall( behavior ) )
                {
                        behavior->changeActivityTo( FallKindOfActivity::getInstance() );
                        *activity = Fall;
                        moved = true;
                }
        }

        return moved ;
}

void MoveKindOfActivity::ascent( FreeItem* freeItem, int z )
{
        // El elemento debe poder cambiar de estado
        if ( freeItem->getBehavior() != 0 )
        {
                // Si el elemento no es el ascensor entonces se levanta
                if ( freeItem->getBehavior()->getNameOfBehavior () != "behavior of elevator" )
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
                                playerItem->setWayOfExit( Up );
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
