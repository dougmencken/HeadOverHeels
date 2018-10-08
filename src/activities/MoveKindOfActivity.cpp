
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

MoveKindOfActivity * MoveKindOfActivity::instance = nilPointer ;

MoveKindOfActivity* MoveKindOfActivity::getInstance()
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

MoveKindOfActivity::~MoveKindOfActivity( )
{
}

bool MoveKindOfActivity::move( Behavior* behavior, ActivityOfItem* activity, bool canFall )
{
        bool moved = false;

        ActivityOfItem displaceActivity = Activity::Wait;

        Item* item = behavior->getItem();
        if ( item == nilPointer ) return false ;
        Mediator* mediator = item->getMediator();

        FreeItem* freeItem = nilPointer;
        if ( item->whichKindOfItem() == "free item" || item->whichKindOfItem() == "player item" )
                freeItem = dynamic_cast< FreeItem * >( item );

        switch ( *activity )
        {
                case Activity::MoveNorth:
                case Activity::AutoMoveNorth:
                        item->changeOrientation( Way::North );
                        moved = item->addToX( -1 );
                        displaceActivity = Activity::DisplaceNorth;
                        break;

                case Activity::MoveSouth:
                case Activity::AutoMoveSouth:
                        item->changeOrientation( Way::South );
                        moved = item->addToX( 1 );
                        displaceActivity = Activity::DisplaceSouth;
                        break;

                case Activity::MoveEast:
                case Activity::AutoMoveEast:
                        item->changeOrientation( Way::East );
                        moved = item->addToY( -1 );
                        displaceActivity = Activity::DisplaceEast;
                        break;

                case Activity::MoveWest:
                case Activity::AutoMoveWest:
                        item->changeOrientation( Way::West );
                        moved = item->addToY( 1 );
                        displaceActivity = Activity::DisplaceWest;
                        break;

                case Activity::MoveNortheast:
                        moved = item->addToPosition( -1, -1, 0 );
                        displaceActivity = Activity::DisplaceNortheast;
                        break;

                case Activity::MoveNorthwest:
                        moved = item->addToPosition( -1, 1, 0 );
                        displaceActivity = Activity::DisplaceNorthwest;
                        break;

                case Activity::MoveSoutheast:
                        moved = item->addToPosition( 1, -1, 0 );
                        displaceActivity = Activity::DisplaceSoutheast;
                        break;

                case Activity::MoveSouthwest:
                        moved = item->addToPosition( 1, 1, 0 );
                        displaceActivity = Activity::DisplaceSouthwest;
                        break;

                case Activity::MoveUp:
                        moved = item->addToZ( 1 );

                        // if can’t move up, raise free items above
                        if ( ! moved )
                        {
                                while ( ! mediator->isStackOfCollisionsEmpty() )
                                {
                                        Item* topItem = mediator->findCollisionPop( );
                                        FreeItem* topFreeItem = nilPointer;
                                        if ( topItem->whichKindOfItem() == "free item" || topItem->whichKindOfItem() == "player item" )
                                                topFreeItem = dynamic_cast< FreeItem * >( topItem );

                                        if ( topFreeItem != nilPointer && item->getWidthX() + item->getWidthY() >= topItem->getWidthX() + topItem->getWidthY() )
                                        {
                                                ascent( topFreeItem, 1 );
                                        }
                                }

                                // now raise itself
                                moved = item->addToZ( 1 );
                        }
                        break;

                case Activity::MoveDown:
                {
                        // is there any items above
                        bool loading = ! item->canAdvanceTo( 0, 0, 2 );

                        // copy stack of collisions
                        std::stack< std::string > topItems;
                        while ( ! mediator->isStackOfCollisionsEmpty() )
                        {
                                topItems.push( mediator->popCollision() );
                        }

                        moved = item->addToZ( -1 );

                        // fall together with items above
                        if ( moved && loading )
                        {
                                while ( ! topItems.empty() )
                                {
                                        Item* topItem = mediator->findItemByUniqueName( topItems.top() );
                                        topItems.pop();

                                        FreeItem* topFreeItem = nilPointer;
                                        if ( topItem->whichKindOfItem() == "free item" || topItem->whichKindOfItem() == "player item" )
                                                topFreeItem = dynamic_cast< FreeItem * >( topItem );

                                        if ( topFreeItem != nilPointer )
                                        {
                                                descend( topFreeItem, 2 );
                                        }
                                }
                        }
                }
                        break;

                case Activity::CancelDisplaceNorth:
                        item->changeOrientation( Way::South );
                        break;

                case Activity::CancelDisplaceSouth:
                        item->changeOrientation( Way::North );
                        break;

                case Activity::CancelDisplaceEast:
                        item->changeOrientation( Way::West );
                        break;

                case Activity::CancelDisplaceWest:
                        item->changeOrientation( Way::East );
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
                else if ( *activity != Activity::MoveUp && *activity != Activity::MoveDown )
                {
                        this->propagateActivityToItemsAbove( freeItem, displaceActivity );
                }
        }

        // item may fall
        if ( canFall && *activity != Activity::Wait )
        {
                if ( FallKindOfActivity::getInstance()->fall( behavior ) )
                {
                        *activity = Activity::Fall;
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
                        {
                                PlayerItem* playerItem = dynamic_cast< PlayerItem * >( freeItem );
                                if ( playerItem->isActiveCharacter() && playerItem->getZ() >= MaxLayers * LayerHeight )
                                {
                                        // active player reaches maximum height of room to possibly go to room above it
                                        playerItem->setWayOfExit( "up" );
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
                bool loading = ! freeItem->canAdvanceTo( 0, 0, z );

                // if item may descend itself then lower every item above it
                if ( freeItem->addToZ( -1 ) && loading )
                {
                        Mediator* mediator = freeItem->getMediator();

                        // make copy of stack of collisions
                        std::stack< std::string > topItems;
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
                                FreeItem* topItem = dynamic_cast< FreeItem * >( mediator->findItemByUniqueName( topItems.top() ) );
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
