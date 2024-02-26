
#include "Moving.hpp"

#include "Falling.hpp"
#include "AvatarItem.hpp"
#include "Mediator.hpp"
#include "PropagateActivity.hpp"

#include <stack>


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


bool Moving::move( behaviors::Behavior* behavior, Activity* activity, bool itFalls )
{
        bool moved = false ;

        Activity toItemsNearby = activities::Activity::Waiting ;

        ItemPtr item = behavior->getItem();
        if ( item == nilPointer ) return false ;
        Mediator* mediator = item->getMediator();

        switch ( *activity )
        {
                case activities::Activity::MovingNorth:
                case activities::Activity::AutomovingNorth:
                        item->changeOrientation( "north" );
                        moved = item->addToX( -1 );
                        toItemsNearby = activities::Activity::PushedNorth ;
                        break;

                case activities::Activity::MovingSouth:
                case activities::Activity::AutomovingSouth:
                        item->changeOrientation( "south" );
                        moved = item->addToX( 1 );
                        toItemsNearby = activities::Activity::PushedSouth ;
                        break;

                case activities::Activity::MovingEast:
                case activities::Activity::AutomovingEast:
                        item->changeOrientation( "east" );
                        moved = item->addToY( -1 );
                        toItemsNearby = activities::Activity::PushedEast ;
                        break;

                case activities::Activity::MovingWest:
                case activities::Activity::AutomovingWest:
                        item->changeOrientation( "west" );
                        moved = item->addToY( 1 );
                        toItemsNearby = activities::Activity::PushedWest ;
                        break;

                case activities::Activity::MovingNortheast:
                        moved = item->addToPosition( -1, -1, 0 );
                        toItemsNearby = activities::Activity::PushedNortheast ;
                        break;

                case activities::Activity::MovingNorthwest:
                        moved = item->addToPosition( -1, 1, 0 );
                        toItemsNearby = activities::Activity::PushedNorthwest ;
                        break;

                case activities::Activity::MovingSoutheast:
                        moved = item->addToPosition( 1, -1, 0 );
                        toItemsNearby = activities::Activity::PushedSoutheast ;
                        break;

                case activities::Activity::MovingSouthwest:
                        moved = item->addToPosition( 1, 1, 0 );
                        toItemsNearby = activities::Activity::PushedSouthwest ;
                        break;

                case activities::Activity::GoingUp:
                        moved = item->addToZ( 1 );

                        // if can’t move up, raise the items above
                        if ( ! moved )
                        {
                                while ( ! mediator->isStackOfCollisionsEmpty() )
                                {
                                        ItemPtr aboveItem = mediator->findCollisionPop ();

                                        // when there’s something above
                                        if ( aboveItem != nilPointer
                                                // that moves freely
                                                && ( aboveItem->whichItemClass() == "free item" || aboveItem->whichItemClass() == "avatar item" )
                                                // and isn’t bigger
                                                && ( item->getWidthX() + item->getWidthY() >= aboveItem->getWidthX() + aboveItem->getWidthY() ) )
                                        {
                                                        // then raise it
                                                        ascent( dynamic_cast< FreeItem & >( *aboveItem ), 1 );
                                        }
                                }

                                // now raise itself
                                moved = item->addToZ( 1 );
                        }
                        break;

                case activities::Activity::GoingDown:
                {
                        // is there anything above
                        bool loaded = ! item->canAdvanceTo( 0, 0, /* 2 */ 1 );

                        // collect the stack of such items
                        std::stack< std::string > itemsAbove ;
                        while ( ! mediator->isStackOfCollisionsEmpty() )
                        {
                                itemsAbove.push( mediator->popCollision() );
                        }

                        moved = item->addToZ( -1 );

                        // fall together with the items above
                        if ( moved && loaded )
                        {
                                while ( ! itemsAbove.empty() )
                                {
                                        ItemPtr onTop = mediator->findItemByUniqueName( itemsAbove.top() );
                                        itemsAbove.pop();

                                        if ( onTop->whichItemClass() == "free item" || onTop->whichItemClass() == "avatar item" )
                                        {       // lower it, to not fall when on elevator and it goes down
                                                descend( dynamic_cast< FreeItem & >( *onTop ), /* 2 */ 1 );
                                        }
                                }
                        }
                }
                        break;

                case activities::Activity::CancelDragNorth:
                        item->changeOrientation( "south" );
                        break;

                case activities::Activity::CancelDragSouth:
                        item->changeOrientation( "north" );
                        break;

                case activities::Activity::CancelDragEast:
                        item->changeOrientation( "west" );
                        break;

                case activities::Activity::CancelDragWest:
                        item->changeOrientation( "east" );
                        break;

                default:
                        ;
        }

        // if the item can move freely
        if ( item->whichItemClass() == "free item" || item->whichItemClass() == "avatar item" )
        {
                if ( /* not moving up or down, to not change the activity of items on elevator */
                        ( *activity != activities::Activity::GoingUp && *activity != activities::Activity::GoingDown )
                                || /* there’s a collision */ ! moved )
                {
                        // move adjacent items
                        PropagateActivity::toAdjacentItems( *item, toItemsNearby );
                }
        }

        if ( itFalls /* doesn’t fly */ && *activity != activities::Activity::Waiting )
        {
                if ( Falling::getInstance().fall( behavior ) )
                {
                        *activity = activities::Activity::Falling;
                        moved = true;
                }
        }

        return moved ;
}

void Moving::ascent( FreeItem & freeItem, int z )
{
        if ( freeItem.getBehavior() != nilPointer )
        {
                if ( freeItem.getBehavior()->getNameOfBehavior () != "behavior of elevator" ) // don’t ascent an elevator
                {
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

                                                        if ( character.isActiveCharacter() ) {
                                                                // the active character reached the maximum height of room
                                                                character.setWayOfExit( "above" );
                                                        }

                                                        continue ;
                                                }
                                        }

                                        ItemPtr aboveItem = mediator->findItemByUniqueName( collision );

                                        // when there’s something above
                                        if ( aboveItem != nilPointer
                                                // that moves freely
                                                && ( aboveItem->whichItemClass() == "free item" || aboveItem->whichItemClass() == "avatar item" )
                                                // and isn’t bigger
                                                && ( freeItem.getWidthX() + freeItem.getWidthY() >= aboveItem->getWidthX() + aboveItem->getWidthY() ) )
                                        {
                                                // then raise it, recursively
                                                ascent( dynamic_cast< FreeItem & >( *aboveItem ), z );
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
                // is there anything above
                bool loaded = ! freeItem.canAdvanceTo( 0, 0, z );

                // if the item may descend itself then lower every item above it
                if ( freeItem.addToZ( -1 ) && loaded )
                {
                        Mediator* mediator = freeItem.getMediator();

                        // collect the stack of items above
                        std::stack< std::string > itemsAbove ;
                        while ( ! mediator->isStackOfCollisionsEmpty() )
                        {
                                itemsAbove.push( mediator->popCollision() );
                        }

                        // descend by the one at a time, otherwise there may be a collision which hinders descending
                        for ( int i = 0; i < ( z - 1 ); i++ )
                        {
                                freeItem.addToZ( -1 );
                        }

                        // for each item above
                        while ( ! mediator->isStackOfCollisionsEmpty() )
                        {
                                ItemPtr aboveItem = mediator->findItemByUniqueName( itemsAbove.top () );
                                itemsAbove.pop ();

                                if ( aboveItem != nilPointer &&
                                        ( aboveItem->whichItemClass() == "free item" || aboveItem->whichItemClass() == "avatar item" ) )
                                {
                                        // lower it, recursively
                                        descend( dynamic_cast< FreeItem & >( *aboveItem ), z );
                                }
                        }
                }
        }
}

}
