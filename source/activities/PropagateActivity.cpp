
#include "PropagateActivity.hpp"

#include "Behavior.hpp"
#include "FreeItem.hpp"
#include "AvatarItem.hpp"
#include "Room.hpp"
#include "Mediator.hpp"

#include <stack>


namespace activities
{

/* static */
void PropagateActivity::spreadEasily( const AbstractItem & sender, const Activity & activity, const Motion2D & velocity )
{
        Mediator* mediator = sender.getMediator() ;
        while ( mediator->isThereAnyCollision () ) // there are items collided with the sender
        {
                DescribedItemPtr collidedItem = mediator->findCollisionPop() ;
                if ( collidedItem != nilPointer && collidedItem->getBehavior() != nilPointer )
                        collidedItem->getBehavior()->setCurrentActivity( activity, velocity );
        }
}

/* static */
void PropagateActivity::toAdjacentItems( DescribedItem & sender, const Activity & activity, const Motion2D & velocity )
{
        Mediator* mediator = sender.getMediator() ;

        // as long as there are items collided with the sender
        while ( mediator->isThereAnyCollision() )
        {
                const std::string & nameOfCollision = mediator->popCollision();
                // not mediator->findCollisionPop() because a collision may be not only with an item,
                // to look at the collision name further
                DescribedItemPtr itemMeetsSender = mediator->findItemByUniqueName( nameOfCollision );

                if ( itemMeetsSender != nilPointer ) // a collision with an item
                {
                        // is it free item or grid item
                        if ( itemMeetsSender->whichItemClass() == "grid item" ||
                                itemMeetsSender->whichItemClass() == "free item" || itemMeetsSender->whichItemClass() == "avatar item" )
                        {
                                // is it item with behavior
                                if ( itemMeetsSender->getBehavior() != nilPointer )
                                {
                                        // if it’s the character and the sender is mortal, then the character loses one life
                                        if ( itemMeetsSender->whichItemClass() == "avatar item" && sender.isMortal() )
                                        {
                                                if ( itemMeetsSender->getBehavior()->getCurrentActivity() != activities::Activity::MetLethalItem
                                                        && itemMeetsSender->getBehavior()->getCurrentActivity() != activities::Activity::Vanishing )
                                                {
                                                        // is the contact direct
                                                        if ( mediator->howManyCollisions() <= 1 )
                                                                itemMeetsSender->getBehavior()->setCurrentActivity( activities::Activity::MetLethalItem, Motion2D::rest() );
                                                }
                                        }
                                        // if the sender is the character and the colliding one is mortal, then the character loses one life
                                        else if ( sender.whichItemClass() == "avatar item" && itemMeetsSender->isMortal() )
                                        {
                                                const Activity & activityOfSender = sender.getBehavior()->getCurrentActivity() ;
                                                if ( activityOfSender != activities::Activity::MetLethalItem
                                                                && activityOfSender != activities::Activity::Vanishing )
                                                {
                                                        sender.getBehavior()->setCurrentActivity( activities::Activity::MetLethalItem, Motion2D::rest() );
                                                }
                                                if ( itemMeetsSender->getBehavior()->getCurrentActivity() != activities::Activity::Vanishing )
                                                {
                                                        itemMeetsSender->getBehavior()->changeActivityDueTo( activity, velocity, DescribedItemPtr( &sender ) );
                                                }
                                        }
                                        // if not, just propagate activity to that item
                                        else
                                        {
                                                if ( itemMeetsSender->getBehavior()->getCurrentActivity() != activities::Activity::Vanishing )
                                                {
                                                        itemMeetsSender->getBehavior()->changeActivityDueTo( activity, velocity, DescribedItemPtr( &sender ) );
                                                }
                                        }
                                }
                                // otherwise it is an item without behavior, which may be mortal too
                                else if ( sender.whichItemClass() == "avatar item" && itemMeetsSender->isMortal() )
                                {
                                        const Activity & activityOfSender = sender.getBehavior()->getCurrentActivity() ;
                                        if ( activityOfSender != activities::Activity::MetLethalItem
                                                        && activityOfSender != activities::Activity::Vanishing )
                                                sender.getBehavior()->setCurrentActivity( activities::Activity::MetLethalItem, Motion2D::rest() );
                                }
                        }
                }
                // a collision isn’t with an item
                else
                if ( sender.whichItemClass() == "avatar item" )
                {
                        AvatarItem & character = dynamic_cast< AvatarItem & >( sender );

                        // maybe the character is leaving the room thru a door
                        for ( unsigned int on = 0 ; on < Room::Sides ; ++ on ) {
                                const std::string & where = Room::Sides_Of_Room[ on ];
                                if ( nameOfCollision == ( where + " door" ) && mediator->getRoom().hasDoorOn( where ) ) {
                                        character.setWayOfExit( where );
                                        return ;
                                }
                        }
                }
        }
}

/* static */
void PropagateActivity::toItemsAbove( DescribedItem & sender, const Activity & activity, const Motion2D & velocity )
{
        Mediator* mediator = sender.getMediator();

        // is there’s anything above
        if ( ! sender.canAdvanceTo( 0, 0, 1 ) )
        {
                // copy the stack of collisions
                std::stack< std::string > itemsAbove;
                while ( mediator->isThereAnyCollision() )
                        itemsAbove.push( mediator->popCollision() );

                while ( ! itemsAbove.empty() )
                {
                        // get first item
                        std::string uniqueName = itemsAbove.top();
                        itemsAbove.pop();

                        DescribedItemPtr itemAbove = mediator->findItemByUniqueName( uniqueName );
                        if ( itemAbove == nilPointer ) continue ;

                        // is it free item
                        if ( itemAbove->whichItemClass() == "free item" || itemAbove->whichItemClass() == "avatar item" )
                        {
                                FreeItem & freeItemAbove = dynamic_cast< FreeItem & >( *itemAbove );
                                if ( freeItemAbove.getBehavior() == nilPointer ) continue ; // nothing for an item without behavior

                                // look for collisions of that free item with the items below it
                                if ( ! freeItemAbove.canAdvanceTo( 0, 0, -1 ) )
                                {
                                        // propagate activity when there’s no more than one item below or when the sender carries that item
                                        if ( mediator->howManyCollisions() <= 1 || sender.getUniqueName() == freeItemAbove.getCarrier() )
                                        {
                                                if ( freeItemAbove.getBehavior()->getCurrentActivity() != activities::Activity::Vanishing )
                                                {
                                                        // if it’s the character above the mortal sender, then the character loses its life
                                                        if ( freeItemAbove.whichItemClass() == "avatar item" && sender.isMortal() )
                                                        {
                                                                freeItemAbove.getBehavior()->setCurrentActivity( activities::Activity::MetLethalItem, Motion2D::rest() );
                                                        }
                                                        // if not, propagate activity to that item above
                                                        else {
                                                                Activity currentActivity = freeItemAbove.getBehavior()->getCurrentActivity();
                                                                if ( currentActivity != activities::Activity::Pushed )
                                                                        freeItemAbove.getBehavior()->changeActivityDueTo( activity, velocity, DescribedItemPtr( &sender ) );
                                                        }
                                                }
                                        }
                                }
                        }
                }
        }
}

}
