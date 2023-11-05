
#include "Moving.hpp"

#include "Falling.hpp"
#include "Behavior.hpp"
#include "AvatarItem.hpp"
#include "Mediator.hpp"

#include <iostream>


namespace activities
{

Moving * Moving::instance = nilPointer ;

Moving& Moving::getInstance()
{
        if ( instance == nilPointer )
        {
                instance = new Moving();
        }

        return *instance;
}


bool Moving::move( behaviors::Behavior* behavior, ActivityOfItem* activity, bool canFall )
{
        bool moved = false;

        ActivityOfItem displaceActivity = activities::Activity::Wait;

        ItemPtr item = behavior->getItem();
        if ( item == nilPointer ) return false ;
        Mediator* mediator = item->getMediator();

        switch ( *activity )
        {
                case activities::Activity::MoveNorth:
                case activities::Activity::AutoMoveNorth:
                        item->changeOrientation( "north" );
                        moved = item->addToX( -1 );
                        displaceActivity = activities::Activity::DisplaceNorth ;
                        break;

                case activities::Activity::MoveSouth:
                case activities::Activity::AutoMoveSouth:
                        item->changeOrientation( "south" );
                        moved = item->addToX( 1 );
                        displaceActivity = activities::Activity::DisplaceSouth ;
                        break;

                case activities::Activity::MoveEast:
                case activities::Activity::AutoMoveEast:
                        item->changeOrientation( "east" );
                        moved = item->addToY( -1 );
                        displaceActivity = activities::Activity::DisplaceEast ;
                        break;

                case activities::Activity::MoveWest:
                case activities::Activity::AutoMoveWest:
                        item->changeOrientation( "west" );
                        moved = item->addToY( 1 );
                        displaceActivity = activities::Activity::DisplaceWest ;
                        break;

                case activities::Activity::MoveNortheast:
                        moved = item->addToPosition( -1, -1, 0 );
                        displaceActivity = activities::Activity::DisplaceNortheast ;
                        break;

                case activities::Activity::MoveNorthwest:
                        moved = item->addToPosition( -1, 1, 0 );
                        displaceActivity = activities::Activity::DisplaceNorthwest ;
                        break;

                case activities::Activity::MoveSoutheast:
                        moved = item->addToPosition( 1, -1, 0 );
                        displaceActivity = activities::Activity::DisplaceSoutheast ;
                        break;

                case activities::Activity::MoveSouthwest:
                        moved = item->addToPosition( 1, 1, 0 );
                        displaceActivity = activities::Activity::DisplaceSouthwest ;
                        break;

                case activities::Activity::MoveUp:
                        moved = item->addToZ( 1 );

                        // if can’t move up, raise free items above
                        if ( ! moved )
                        {
                                while ( ! mediator->isStackOfCollisionsEmpty() )
                                {
                                        ItemPtr topItem = mediator->findCollisionPop( );
                                        if ( topItem != nilPointer &&
                                                ( topItem->whichItemClass() == "free item" || topItem->whichItemClass() == "avatar item" ) )
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

                case activities::Activity::MoveDown:
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

                                        if ( topItem->whichItemClass() == "free item" || topItem->whichItemClass() == "avatar item" )
                                        {
                                                descend( dynamic_cast< FreeItem& >( *topItem ), 2 );
                                        }
                                }
                        }
                }
                        break;

                case activities::Activity::CancelDisplaceNorth:
                        item->changeOrientation( "south" );
                        break;

                case activities::Activity::CancelDisplaceSouth:
                        item->changeOrientation( "north" );
                        break;

                case activities::Activity::CancelDisplaceEast:
                        item->changeOrientation( "west" );
                        break;

                case activities::Activity::CancelDisplaceWest:
                        item->changeOrientation( "east" );
                        break;

                default:
                        ;
        }

        if ( item->whichItemClass() == "free item" || item->whichItemClass() == "avatar item" )
        {
                // move collided items when there’s horizontal collision
                if ( ! moved ||
                        ( *activity != activities::Activity::MoveUp && *activity != activities::Activity::MoveDown ) )
                {
                        // see if is it necessary to move items above
                        // exception is for vertical movement to keep activity of items above elevator unchanged
                        this->propagateActivityToAdjacentItems( *item, displaceActivity );
                }
        }

        // item may fall
        if ( canFall && *activity != activities::Activity::Wait )
        {
                if ( Falling::getInstance().fall( behavior ) )
                {
                        *activity = activities::Activity::Fall;
                        moved = true;
                }
        }

        return moved ;
}

void Moving::ascent( FreeItem & freeItem, int z )
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

                                        if ( freeItem.whichItemClass() == "avatar item" )
                                        {
                                                if ( collision == "ceiling" )
                                                {
                                                        AvatarItem & character = dynamic_cast< AvatarItem & >( freeItem );

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
                                                ( topItem->whichItemClass() == "free item" || topItem->whichItemClass() == "avatar item" ) )
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

void Moving::descend( FreeItem & freeItem, int z )
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
                                        ( topItem->whichItemClass() == "free item" || topItem->whichItemClass() == "avatar item" ) )
                                {
                                        // lower recursively
                                        descend( dynamic_cast< FreeItem& >( *topItem ), z );
                                }
                        }
                }
        }
}

}
