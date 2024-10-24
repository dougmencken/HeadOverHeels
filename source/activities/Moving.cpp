
#include "Moving.hpp"

#include "Activity.hpp"
#include "Falling.hpp"
#include "PropagateActivity.hpp"
#include "FreeItem.hpp"
#include "AvatarItem.hpp"
#include "Mediator.hpp"
#include "Elevator.hpp"

#include <stack>


namespace activities
{

Moving * Moving::instance = nilPointer ;

Moving& Moving::getInstance()
{
        if ( instance == nilPointer )
                instance = new Moving () ;

        return *instance;
}

bool Moving::move( behaviors::Behavior & behavior, bool itFalls )
{
        bool moved = false ;

        Item & whatMoves = behavior.getItem() ;
        Mediator * mediator = whatMoves.getMediator();

        Activity toItemsNearby = activities::Activity::Waiting ;

        Activity activity = behavior.getCurrentActivity() ;
        Motion2D velocity = behavior.get2DVelocityVector() ;

        switch ( activity )
        {
                case activities::Activity::Moving:
                case activities::Activity::Automoving:
                        if ( velocity.isMovingOnlyNorth() || velocity.isMovingOnlySouth() ) {
                                if ( velocity.isMovingOnlyNorth() ) {
                                        whatMoves.changeHeading( "north" );
                                        toItemsNearby = activities::Activity::PushedNorth ;
                                } else if ( velocity.isMovingOnlySouth() ) {
                                        whatMoves.changeHeading( "south" );
                                        toItemsNearby = activities::Activity::PushedSouth ;
                                }

                                moved = whatMoves.addToX( velocity.getMotionAlongX() );
                        }
                        else
                        if ( velocity.isMovingOnlyEast() || velocity.isMovingOnlyWest() ) {
                                if ( velocity.isMovingOnlyEast() ) {
                                        whatMoves.changeHeading( "east" );
                                        toItemsNearby = activities::Activity::PushedEast ;
                                } else if ( velocity.isMovingOnlyWest() ) {
                                        whatMoves.changeHeading( "west" );
                                        toItemsNearby = activities::Activity::PushedWest ;
                                }

                                moved = whatMoves.addToY( velocity.getMotionAlongY() );
                        }
                        else {
                                     if ( velocity.isMovingNortheast() ) toItemsNearby = activities::Activity::PushedNortheast ;
                                else if ( velocity.isMovingNorthwest() ) toItemsNearby = activities::Activity::PushedNorthwest ;
                                else if ( velocity.isMovingSoutheast() ) toItemsNearby = activities::Activity::PushedSoutheast ;
                                else if ( velocity.isMovingSouthwest() ) toItemsNearby = activities::Activity::PushedSouthwest ;

                                moved = whatMoves.addToPosition( velocity.getMotionAlongX(), velocity.getMotionAlongY(), 0 );
                        }

                        break ;

                case activities::ActivityOfElevator::GoingUp :
                        moved = whatMoves.addToZ( 1 );

                        // if can’t move up, raise the items above
                        if ( ! moved )
                        {
                                while ( mediator->isThereAnyCollision() )
                                {
                                        ItemPtr aboveItem = mediator->findCollisionPop ();

                                        // when there’s something above
                                        if ( aboveItem != nilPointer
                                                // that moves freely
                                                && ( aboveItem->whichItemClass() == "free item" || aboveItem->whichItemClass() == "avatar item" )
                                                // and isn’t bigger
                                                && ( whatMoves.getWidthX() + whatMoves.getWidthY() >= aboveItem->getWidthX() + aboveItem->getWidthY() ) )
                                        {
                                                        // then raise it
                                                        ascent( dynamic_cast< FreeItem & >( *aboveItem ), 1 );
                                        }
                                }

                                // now raise itself
                                moved = whatMoves.addToZ( 1 );
                        }
                        break;

                case activities::ActivityOfElevator::GoingDown :
                {
                        // is there anything above
                        bool loaded = ! whatMoves.canAdvanceTo( 0, 0, /* 2 */ 1 );

                        // collect the stack of such items
                        std::stack< std::string > itemsAbove ;
                        while ( mediator->isThereAnyCollision() )
                        {
                                itemsAbove.push( mediator->popCollision() );
                        }

                        moved = whatMoves.addToZ( -1 );

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

                case activities::Activity::CancelDragging:
                        // not moving
                        break;

                default:
                        ;
        }

        // if the item can move freely
        if ( whatMoves.whichItemClass() == "free item" || whatMoves.whichItemClass() == "avatar item" )
        {
                bool onElevator = ( activity == activities::ActivityOfElevator::GoingUp || activity == activities::ActivityOfElevator::GoingDown );
                if ( /* don’t affect activity of items on elevator */ ! onElevator )
                {
                        if ( /* there’s a collision */ ! moved ) {
                                // move adjacent items
                                PropagateActivity::toAdjacentItems( whatMoves, toItemsNearby );
                        }
                        else {
                                // maybe there’s something above
                                PropagateActivity::toItemsAbove( whatMoves, toItemsNearby );
                        }
                }
        }

        if ( itFalls /* doesn’t fly */ && activity != activities::Activity::Waiting )
        {
                if ( Falling::getInstance().fall( behavior ) ) {
                        behavior.setCurrentActivity( activities::Activity::Falling );
                        moved = true ;
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

                                while ( mediator->isThereAnyCollision() )
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
                        while ( mediator->isThereAnyCollision() )
                                itemsAbove.push( mediator->popCollision() );

                        // descend by the one at a time, otherwise there may be a collision which hinders descending
                        for ( int i = 0; i < ( z - 1 ); i++ )
                        {
                                freeItem.addToZ( -1 );
                        }

                        // for each item above
                        while ( mediator->isThereAnyCollision() )
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
