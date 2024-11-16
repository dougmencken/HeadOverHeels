
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

        DescribedItem & whatMoves = dynamic_cast< DescribedItem & >( behavior.getItem() );
        Mediator * mediator = whatMoves.getMediator();

        Activity toItemsNearby = activities::Activity::Waiting ;

        Activity activity = behavior.getCurrentActivity() ;
        Motion2D velocity = behavior.get2DVelocityVector() ;

        switch ( activity )
        {
                case activities::Activity::Moving :
                case activities::Activity::Automoving :
                        if ( whatMoves.whichItemClass() == "free item" || whatMoves.whichItemClass() == "avatar item" )
                        {
                                FreeItem & itMoves = dynamic_cast< FreeItem & >( whatMoves );

                                if ( velocity.isMovingOnlyNorth() || velocity.isMovingOnlySouth() ) {
                                        if ( velocity.isMovingOnlyNorth() )
                                                itMoves.changeHeading( "north" );
                                        else if ( velocity.isMovingOnlySouth() )
                                                itMoves.changeHeading( "south" );

                                        moved = whatMoves.addToX( velocity.getMotionAlongX() );
                                }
                                else
                                if ( velocity.isMovingOnlyEast() || velocity.isMovingOnlyWest() ) {
                                        if ( velocity.isMovingOnlyEast() )
                                                itMoves.changeHeading( "east" );
                                        else if ( velocity.isMovingOnlyWest() )
                                                itMoves.changeHeading( "west" );

                                        moved = whatMoves.addToY( velocity.getMotionAlongY() );
                                }
                                else
                                        moved = whatMoves.addToPosition( velocity.getMotionAlongX(), velocity.getMotionAlongY(), 0 );
                        }

                        toItemsNearby = activities::Activity::Pushed ;

                        break ;

                case activities::ActivityOfElevator::GoingUp :
                        moved = whatMoves.addToZ( 1 );

                        // if can’t move up, raise the items above
                        if ( ! moved )
                        {
                                while ( mediator->isThereAnyCollision() )
                                {
                                        DescribedItemPtr aboveItem = mediator->findCollisionPop ();

                                        // when there’s something above
                                        if ( aboveItem != nilPointer
                                                // that moves freely
                                                && ( aboveItem->whichItemClass() == "free item" || aboveItem->whichItemClass() == "avatar item" )
                                                // and isn’t bigger
                                                && ( whatMoves.getWidthX() + whatMoves.getWidthY() >= aboveItem->getWidthX() + aboveItem->getWidthY() ) )
                                        {
                                                        // then raise an above item
                                                        ascend( dynamic_cast< FreeItem & >( *aboveItem ), 1 );
                                        }
                                }

                                // now raise itself
                                moved = whatMoves.addToZ( 1 );
                        }
                        break ;

                case activities::ActivityOfElevator::GoingDown :
                {
                        // is there anything above
                        bool loaded = ! whatMoves.canAdvanceTo( 0, 0, 1 );

                        // collect the items stacked above
                        std::stack< std::string > itemsAbove ;
                        while ( mediator->isThereAnyCollision() )
                                itemsAbove.push( mediator->popCollision() );

                        moved = whatMoves.addToZ( -1 );

                        // fall together with the items above
                        if ( moved && loaded ) {
                                while ( ! itemsAbove.empty() )
                                {
                                        DescribedItemPtr onTop = mediator->findItemByUniqueName( itemsAbove.top() );
                                        itemsAbove.pop() ;

                                        if ( onTop->whichItemClass() == "free item" || onTop->whichItemClass() == "avatar item" )
                                        {       // lower it, to not fall when on an elevator and it goes down
                                                descend( dynamic_cast< FreeItem & >( *onTop ), /* 2 */ 1 );
                                        }
                                }
                        }
                }
                        break ;

                case activities::Activity::CancelDragging :
                        // not moving
                        break ;

                default:
                        ;
        }

        // if the item can move freely
        if ( whatMoves.whichItemClass() == "free item" || whatMoves.whichItemClass() == "avatar item" )
        {
                bool onElevator = ( activity == activities::ActivityOfElevator::GoingUp
                                        || activity == activities::ActivityOfElevator::GoingDown );

                if ( /* don’t affect activity of items on elevator */ ! onElevator ) {
                        if ( /* there’s a collision */ ! moved ) {
                                // move adjacent items
                                PropagateActivity::toAdjacentItems( whatMoves, toItemsNearby, velocity );
                        }
                        else {
                                // maybe there’s something above
                                PropagateActivity::toItemsAbove( whatMoves, toItemsNearby, velocity );
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

void Moving::ascend( FreeItem & freeItem, int dz )
{
        if ( freeItem.getBehavior() == nilPointer ) return ;
        /* if ( freeItem.getBehavior()->getNameOfBehavior() != "behavior of elevator" ) return ; // don’t ascend an elevator */

        if ( freeItem.canAdvanceTo( 0, 0, dz ) && freeItem.addToZ( dz ) ) return ; // it is done, there was no collision above

        // otherwise, the item can’t raise by dz because there’s a collision which hinders ascending

        std::cout << "↑↑ ascending " << freeItem.getUniqueName() << " by " << dz << std::endl ;

        Mediator* mediator = freeItem.getMediator();

        while ( mediator->isThereAnyCollision() )
        {
                std::string collision = mediator->popCollision ();

                if ( freeItem.whichItemClass() == "avatar item" && collision == "ceiling" ) {
                        AvatarItem & character = dynamic_cast< AvatarItem & >( freeItem );

                        if ( character.isActiveCharacter() ) {
                                // the active character reached the maximum height of room
                                character.setWayOfExit( "above" );
                        }

                        continue ;
                }

                DescribedItemPtr aboveItem = mediator->findItemByUniqueName( collision );

                // when there’s something above
                if ( aboveItem != nilPointer
                        // that can move freely
                        && ( aboveItem->whichItemClass() == "free item" || aboveItem->whichItemClass() == "avatar item" )
                        // and isn’t bigger
                        && ( freeItem.getWidthX() + freeItem.getWidthY() >= aboveItem->getWidthX() + aboveItem->getWidthY() ) )
                {
                        // then raise that thing, recursively
                        ascend( dynamic_cast< FreeItem & >( *aboveItem ), dz );
                }
        }

        // and now the item can lift up
        freeItem.addToZ( dz );
}

void Moving::descend( FreeItem & freeItem, int dz )
{
        if ( freeItem.getBehavior() == nilPointer ) return ;
        /* if ( freeItem.getBehavior()->getNameOfBehavior() != "behavior of elevator" ) return ; // don’t descend an elevator */

        if ( freeItem.canAdvanceTo( 0, 0, -dz ) && freeItem.addToZ( -dz ) ) return ; // it is done, there was no collision below

        // the item can’t lower by dz because there’s a collision which hinders descending
        // but if the item may lower by a single unit then try to descend the items below it
        if ( freeItem.addToZ( -1 ) )
        {
                std::cout << "↓↓ descending " << freeItem.getUniqueName() << " by " << dz << std::endl ;

                Mediator* mediator = freeItem.getMediator();

                // collect the stack of items below
                std::stack< std::string > itemsBelow ;
                while ( mediator->isThereAnyCollision() )
                        itemsBelow.push( mediator->popCollision() );

                // try to descend as low as possible by one at a time at most dz-1 times
                int n = 0 ;
                bool mayLower = true ;
                while ( mayLower && ( ++ n ) < dz )
                        mayLower = freeItem.addToZ( -1 );

                // for each item below
                while ( ! itemsBelow.empty() )
                {
                        DescribedItemPtr belowItem = mediator->findItemByUniqueName( itemsBelow.top () );
                        itemsBelow.pop ();

                        if ( belowItem != nilPointer
                                && ( belowItem->whichItemClass() == "free item" || belowItem->whichItemClass() == "avatar item" ) )
                        {
                                // recursively lower such an item
                                descend( dynamic_cast< FreeItem & >( *belowItem ), dz );
                        }
                }
        }
}

}
