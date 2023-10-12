
#include "MoveKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Behavior.hpp"
#include "PlayerItem.hpp"
#include "Mediator.hpp"

#include <iostream>


namespace iso
{

MoveKindOfActivity * MoveKindOfActivity::instance = nilPointer ;

MoveKindOfActivity& MoveKindOfActivity::getInstance()
{
        if ( instance == nilPointer )
        {
                instance = new MoveKindOfActivity();
        }

        return *instance;
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

        ItemPtr item = behavior->getItem();
        if ( item == nilPointer ) return false ;
        Mediator* mediator = item->getMediator();

        switch ( *activity )
        {
                case Activity::MoveNorth:
                case Activity::AutoMoveNorth:
                        item->changeOrientation( "north" );
                        moved = item->addToX( -1 );
                        displaceActivity = Activity::DisplaceNorth ;
                        break;

                case Activity::MoveSouth:
                case Activity::AutoMoveSouth:
                        item->changeOrientation( "south" );
                        moved = item->addToX( 1 );
                        displaceActivity = Activity::DisplaceSouth ;
                        break;

                case Activity::MoveEast:
                case Activity::AutoMoveEast:
                        item->changeOrientation( "east" );
                        moved = item->addToY( -1 );
                        displaceActivity = Activity::DisplaceEast ;
                        break;

                case Activity::MoveWest:
                case Activity::AutoMoveWest:
                        item->changeOrientation( "west" );
                        moved = item->addToY( 1 );
                        displaceActivity = Activity::DisplaceWest ;
                        break;

                case Activity::MoveNortheast:
                        moved = item->addToPosition( -1, -1, 0 );
                        displaceActivity = Activity::DisplaceNortheast ;
                        break;

                case Activity::MoveNorthwest:
                        moved = item->addToPosition( -1, 1, 0 );
                        displaceActivity = Activity::DisplaceNorthwest ;
                        break;

                case Activity::MoveSoutheast:
                        moved = item->addToPosition( 1, -1, 0 );
                        displaceActivity = Activity::DisplaceSoutheast ;
                        break;

                case Activity::MoveSouthwest:
                        moved = item->addToPosition( 1, 1, 0 );
                        displaceActivity = Activity::DisplaceSouthwest ;
                        break;

                case Activity::MoveUp:
                        moved = item->addToZ( 1 );

                        // if can’t move up, raise free items above
                        if ( ! moved )
                        {
                                while ( ! mediator->isStackOfCollisionsEmpty() )
                                {
                                        ItemPtr topItem = mediator->findCollisionPop( );
                                        if ( topItem != nilPointer &&
                                                ( topItem->whichKindOfItem() == "free item" || topItem->whichKindOfItem() == "player item" ) )
                                        {
                                                if ( item->getWidthX() + item->getWidthY() >= topItem->getWidthX() + topItem->getWidthY() )
                                                {
                                                        ascent( dynamic_cast< FreeItem& >( *topItem ), 1 );
                                                }
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
                                        ItemPtr topItem = mediator->findItemByUniqueName( topItems.top() );
                                        topItems.pop();

                                        if ( topItem->whichKindOfItem() == "free item" || topItem->whichKindOfItem() == "player item" )
                                        {
                                                descend( dynamic_cast< FreeItem& >( *topItem ), 2 );
                                        }
                                }
                        }
                }
                        break;

                case Activity::CancelDisplaceNorth:
                        item->changeOrientation( "south" );
                        break;

                case Activity::CancelDisplaceSouth:
                        item->changeOrientation( "north" );
                        break;

                case Activity::CancelDisplaceEast:
                        item->changeOrientation( "west" );
                        break;

                case Activity::CancelDisplaceWest:
                        item->changeOrientation( "east" );
                        break;

                default:
                        ;
        }

        if ( item->whichKindOfItem() == "free item" || item->whichKindOfItem() == "player item" )
        {
                // move collided items when there’s horizontal collision
                if ( ! moved ||
                        ( *activity != Activity::MoveUp && *activity != Activity::MoveDown ) )
                {
                        // see if is it necessary to move items above
                        // exception is for vertical movement to keep activity of items above elevator unchanged
                        this->propagateActivityToAdjacentItems( *item, displaceActivity );
                }
        }

        // item may fall
        if ( canFall && *activity != Activity::Wait )
        {
                if ( FallKindOfActivity::getInstance().fall( behavior ) )
                {
                        *activity = Activity::Fall;
                        moved = true;
                }
        }

        return moved ;
}

void MoveKindOfActivity::ascent( FreeItem & freeItem, int z )
{
        if ( freeItem.getBehavior() != nilPointer )
        {
                // don’t ascent elevator
                if ( freeItem.getBehavior()->getNameOfBehavior () != "behavior of elevator" )
                {
                        // when there’s something above this item
                        // then raise every not bigger free item above
                        if ( ! freeItem.addToZ( z ) )
                        {
                                Mediator* mediator = freeItem.getMediator();

                                while ( ! mediator->isStackOfCollisionsEmpty() )
                                {
                                        std::string collision = mediator->popCollision ();

                                        if ( freeItem.whichKindOfItem() == "player item" )
                                        {
                                                if ( collision == "ceiling" )
                                                {
                                                        PlayerItem & character = dynamic_cast< PlayerItem & >( freeItem );

                                                        if ( character.isActiveCharacter() )
                                                        {
                                                                // active character reached maximum height of room
                                                                character.setWayOfExit( "above" );
                                                        }

                                                        continue ;
                                                }
                                        }

                                        ItemPtr topItem = mediator->findItemByUniqueName( collision );

                                        if ( topItem != nilPointer &&
                                                ( topItem->whichKindOfItem() == "free item" || topItem->whichKindOfItem() == "player item" ) )
                                        {
                                                if ( freeItem.getWidthX() + freeItem.getWidthY() >= topItem->getWidthX() + topItem->getWidthY() )
                                                {
                                                        // raise recursively
                                                        ascent( dynamic_cast< FreeItem& >( *topItem ), z );
                                                }
                                        }
                                }

                                // yet it can ascend
                                freeItem.addToZ( z );
                        }
                }
        }
}

void MoveKindOfActivity::descend( FreeItem & freeItem, int z )
{
        if ( freeItem.getBehavior() != nilPointer )
        {
                // are there items on top
                bool loading = ! freeItem.canAdvanceTo( 0, 0, z );

                // if item may descend itself then lower every item above it
                if ( freeItem.addToZ( -1 ) && loading )
                {
                        Mediator* mediator = freeItem.getMediator();

                        // make copy of stack of collisions
                        std::stack< std::string > topItems;
                        while ( ! mediator->isStackOfCollisionsEmpty() )
                        {
                                topItems.push( mediator->popCollision() );
                        }

                        // descend by one at a time, otherwise there may be collision which hinders descend
                        for ( int i = 0; i < ( z - 1 ); i++ )
                        {
                                freeItem.addToZ( -1 );
                        }

                        // for each item above
                        while ( ! mediator->isStackOfCollisionsEmpty() )
                        {
                                ItemPtr topItem = mediator->findItemByUniqueName( topItems.top() );
                                topItems.pop();

                                if ( topItem != nilPointer &&
                                        ( topItem->whichKindOfItem() == "free item" || topItem->whichKindOfItem() == "player item" ) )
                                {
                                        // lower recursively
                                        descend( dynamic_cast< FreeItem& >( *topItem ), z );
                                }
                        }
                }
        }
}

}
