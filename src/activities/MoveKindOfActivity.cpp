
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

KindOfActivity * MoveKindOfActivity::instance = nilPointer ;

KindOfActivity* MoveKindOfActivity::getInstance()
{
        if ( instance == nilPointer )
        {
                instance = new MoveKindOfActivity();
        }

        return instance;
}

MoveKindOfActivity::MoveKindOfActivity( ) : KindOfActivity()
{

}

MoveKindOfActivity::~MoveKindOfActivity( )  { }

bool MoveKindOfActivity::move( Behavior* behavior, ActivityOfItem* activity, bool canFall )
{
        bool moved = false;

        ActivityOfItem displaceActivity = Wait;

        FreeItem* freeItem = nilPointer;
        Mediator* mediator = nilPointer;

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
                        if ( freeItem != nilPointer )
                        {
                                freeItem->changeOrientation( North );
                                moved = freeItem->addToX( -1 );
                                displaceActivity = DisplaceNorth;
                        }
                        break;

                case MoveSouth:
                case AutoMoveSouth:
                        if ( freeItem != nilPointer )
                        {
                                freeItem->changeOrientation( South );
                                moved = freeItem->addToX( 1 );
                                displaceActivity = DisplaceSouth;
                        }
                        break;

                case MoveEast:
                case AutoMoveEast:
                        if ( freeItem != nilPointer )
                        {
                                freeItem->changeOrientation( East );
                                moved = freeItem->addToY( -1 );
                                displaceActivity = DisplaceEast;
                        }
                        break;

                case MoveWest:
                case AutoMoveWest:
                        if ( freeItem != nilPointer )
                        {
                                freeItem->changeOrientation( West );
                                moved = freeItem->addToY( 1 );
                                displaceActivity = DisplaceWest;
                        }
                        break;

                case MoveNortheast:
                        if ( freeItem != nilPointer )
                        {
                                moved = freeItem->addToPosition( -1, -1, 0 );
                                displaceActivity = DisplaceNortheast;
                        }
                        break;

                case MoveNorthwest:
                        if ( freeItem != nilPointer )
                        {
                                moved = freeItem->addToPosition( -1, 1, 0 );
                                displaceActivity = DisplaceNorthwest;
                        }
                        break;

                case MoveSoutheast:
                        if ( freeItem != nilPointer )
                        {
                                moved = freeItem->addToPosition( 1, -1, 0 );
                                displaceActivity = DisplaceSoutheast;
                        }
                        break;

                case MoveSouthwest:
                        if ( freeItem != nilPointer )
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

                                        if ( topItem != nilPointer && freeItem->getWidthX() + freeItem->getWidthY() >= topItem->getWidthX() + topItem->getWidthY() )
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

                                        if ( topItem != nilPointer )
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

        if ( freeItem != nilPointer )
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
        if ( canFall && *activity != Wait )
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
        if ( freeItem->getBehavior() != nilPointer )
        {
                // don’t ascent elevator
                if ( freeItem->getBehavior()->getNameOfBehavior () != "behavior of elevator" )
                {
                        // when there’s something above this item
                        // then raise every not bigger free item above
                        if ( ! freeItem->addToZ( z ) )
                        {
                                Mediator* mediator = freeItem->getMediator();

                                while ( ! mediator->isStackOfCollisionsEmpty() )
                                {
                                        FreeItem* topItem = dynamic_cast< FreeItem * >( mediator->findCollisionPop( ) );

                                        if ( topItem != nilPointer &&
                                                freeItem->getWidthX() + freeItem->getWidthY() >= topItem->getWidthX() + topItem->getWidthY() )
                                        {
                                                // raise recursively
                                                ascent( topItem, z );
                                        }
                                }

                                // yet it can ascend
                                freeItem->addToZ( z );
                        }

                        if ( freeItem->whichKindOfItem() == "player item" )
                        {       // when item is player
                                PlayerItem* playerItem = dynamic_cast< PlayerItem * >( freeItem );
                                if ( playerItem->getZ() >= MaxLayers * LayerHeight )
                                {
                                        // player reaches maximum height of room to possibly go to room above it
                                        playerItem->setWayOfExit( Up );
                                }
                        }
                }
        }
}

void MoveKindOfActivity::descend( FreeItem* freeItem, int z )
{
        if ( freeItem->getBehavior() != nilPointer )
        {
                // are there items on top
                bool loading = ! freeItem->checkPosition( 0, 0, z, Add );

                // if item may descend itself then lower every item above it
                if ( freeItem->addToZ( -1 ) && loading )
                {
                        Mediator* mediator = freeItem->getMediator();

                        // make copy of stack of collisions
                        std::stack< int > topItems;
                        while ( ! mediator->isStackOfCollisionsEmpty() )
                        {
                                topItems.push( mediator->popCollision() );
                        }

                        // descend by one at a time, otherwise there may be collision which hinders descend
                        for ( int i = 0; i < ( z - 1 ); i++ )
                        {
                                freeItem->addToZ( -1 );
                        }

                        // for each item above
                        while ( ! mediator->isStackOfCollisionsEmpty() )
                        {
                                FreeItem* topItem = dynamic_cast< FreeItem * >( mediator->findItemById( topItems.top() ) );
                                topItems.pop();

                                if ( topItem != nilPointer )
                                {
                                        // lower recursively
                                        descend( topItem, z );
                                }
                        }
                }
        }
}

}
