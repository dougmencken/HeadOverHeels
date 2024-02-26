
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
void PropagateActivity::toAdjacentItems( Item & sender, const Activity & activity )
{
        Mediator* mediator = sender.getMediator();

        // as long as there are items collided with the sender
        while ( ! mediator->isStackOfCollisionsEmpty() )
        {
                std::string nameOfCollision = mediator->popCollision();
                ItemPtr itemMeetsSender = mediator->findItemByUniqueName( nameOfCollision );

                if ( itemMeetsSender != nilPointer )
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
                                                        if ( mediator->depthOfStackOfCollisions() <= 1 )
                                                        {
                                                                itemMeetsSender->getBehavior()->setCurrentActivity( activities::Activity::MetLethalItem );
                                                        }
                                                }
                                        }
                                        // if the sender is the character and the colliding one is mortal, then the character loses one life
                                        else if ( sender.whichItemClass() == "avatar item" && itemMeetsSender->isMortal() )
                                        {
                                                if ( sender.getBehavior()->getCurrentActivity() != activities::Activity::MetLethalItem
                                                        && sender.getBehavior()->getCurrentActivity() != activities::Activity::Vanishing )
                                                {
                                                        sender.getBehavior()->setCurrentActivity( activities::Activity::MetLethalItem );
                                                }
                                                if ( itemMeetsSender->getBehavior()->getCurrentActivity() != activities::Activity::Vanishing )
                                                {
                                                        itemMeetsSender->getBehavior()->changeActivityDueTo( activity, ItemPtr( &sender ) );
                                                }
                                        }
                                        // if not, just propagate activity to that item
                                        else
                                        {
                                                if ( itemMeetsSender->getBehavior()->getCurrentActivity() != activities::Activity::Vanishing )
                                                {
                                                        itemMeetsSender->getBehavior()->changeActivityDueTo( activity, ItemPtr( &sender ) );
                                                }
                                        }
                                }
                                // otherwise it is an item without behavior, which may be mortal too
                                else if ( sender.whichItemClass() == "avatar item" && itemMeetsSender->isMortal() )
                                {
                                        if ( sender.getBehavior()->getCurrentActivity() != activities::Activity::MetLethalItem
                                                && sender.getBehavior()->getCurrentActivity() != activities::Activity::Vanishing )
                                        {
                                                sender.getBehavior()->setCurrentActivity( activities::Activity::MetLethalItem );
                                        }
                                }
                        }
                }
                // is it the character leaving the room via some door
                else if ( sender.whichItemClass() == "avatar item" &&
                        ( ( nameOfCollision == "north limit"  &&  mediator->getRoom()->hasDoorAt( "north" ) ) ||
                          ( nameOfCollision == "south limit"  &&  mediator->getRoom()->hasDoorAt( "south" ) ) ||
                          ( nameOfCollision == "east limit"  &&  mediator->getRoom()->hasDoorAt( "east" ) ) ||
                          ( nameOfCollision == "west limit"  &&  mediator->getRoom()->hasDoorAt( "west" ) ) ||
                          ( nameOfCollision == "northeast limit"  &&  mediator->getRoom()->hasDoorAt( "northeast" ) ) ||
                          ( nameOfCollision == "northwest limit"  &&  mediator->getRoom()->hasDoorAt( "northwest" ) ) ||
                          ( nameOfCollision == "southeast limit"  &&  mediator->getRoom()->hasDoorAt( "southeast" ) ) ||
                          ( nameOfCollision == "southwest limit"  &&  mediator->getRoom()->hasDoorAt( "southwest" ) ) ||
                          ( nameOfCollision == "eastnorth limit"  &&  mediator->getRoom()->hasDoorAt( "eastnorth" ) ) ||
                          ( nameOfCollision == "eastsouth limit"  &&  mediator->getRoom()->hasDoorAt( "eastsouth" ) ) ||
                          ( nameOfCollision == "westnorth limit"  &&  mediator->getRoom()->hasDoorAt( "westnorth" ) ) ||
                          ( nameOfCollision == "westsouth limit"  &&  mediator->getRoom()->hasDoorAt( "westsouth" ) ) ) )
                {
                        AvatarItem & character = dynamic_cast< AvatarItem & >( sender );

                        if ( nameOfCollision == "north limit" )
                                character.setWayOfExit( "north" );
                        else if ( nameOfCollision == "south limit" )
                                character.setWayOfExit( "south" );
                        else if ( nameOfCollision == "east limit" )
                                character.setWayOfExit( "east" );
                        else if ( nameOfCollision == "west limit" )
                                character.setWayOfExit( "west" );
                        else if ( nameOfCollision == "northeast limit" )
                                character.setWayOfExit( "northeast" );
                        else if ( nameOfCollision == "northwest limit" )
                                character.setWayOfExit( "northwest" );
                        else if ( nameOfCollision == "southeast limit" )
                                character.setWayOfExit( "southeast" );
                        else if ( nameOfCollision == "southwest limit" )
                                character.setWayOfExit( "southwest" );
                        else if ( nameOfCollision == "eastnorth limit" )
                                character.setWayOfExit( "eastnorth" );
                        else if ( nameOfCollision == "eastsouth limit" )
                                character.setWayOfExit( "eastsouth" );
                        else if ( nameOfCollision == "westnorth limit" )
                                character.setWayOfExit( "westnorth" );
                        else if ( nameOfCollision == "westsouth limit" )
                                character.setWayOfExit( "westsouth" );
                }
        }
}

/* static */
void PropagateActivity::toItemsAbove( Item & sender, const Activity & activity )
{
        Mediator* mediator = sender.getMediator();

        // is there’s anything above
        if ( ! sender.canAdvanceTo( 0, 0, 1 ) )
        {
                // copy the stack of collisions
                std::stack< std::string > itemsAbove;
                while ( ! mediator->isStackOfCollisionsEmpty() )
                        itemsAbove.push( mediator->popCollision() );

                while ( ! itemsAbove.empty() )
                {
                        // get first item
                        std::string uniqueName = itemsAbove.top();
                        itemsAbove.pop();

                        ItemPtr itemAbove = mediator->findItemByUniqueName( uniqueName );
                        if ( itemAbove == nilPointer ) continue ;

                        // is it free item
                        if ( itemAbove->whichItemClass() == "free item" || itemAbove->whichItemClass() == "avatar item" )
                        {
                                FreeItem & freeItemAbove = dynamic_cast< FreeItem& >( *itemAbove );
                                if ( freeItemAbove.getBehavior() == nilPointer ) continue ; // nothing for an item without behavior

                                // look for collisions of that free item with the items below it
                                if ( ! freeItemAbove.canAdvanceTo( 0, 0, -1 ) )
                                {
                                        // propagate activity when there’s no more than one item below or when the sender carries that item
                                        if ( mediator->depthOfStackOfCollisions() <= 1 || sender.getUniqueName() == freeItemAbove.getCarrier() )
                                        {
                                                if ( freeItemAbove.getBehavior()->getCurrentActivity() != activities::Activity::Vanishing )
                                                {
                                                        // if it’s the character above the mortal sender, then the character loses its life
                                                        if ( freeItemAbove.whichItemClass() == "avatar item" && sender.isMortal() )
                                                        {
                                                                freeItemAbove.getBehavior()->setCurrentActivity( activities::Activity::MetLethalItem );
                                                        }
                                                        // if not, propagate activity to that item above
                                                        else {
                                                                Activity currentActivity = freeItemAbove.getBehavior()->getCurrentActivity();
                                                                if ( currentActivity != activities::Activity::PushedNorth &&
                                                                        currentActivity != activities::Activity::PushedSouth &&
                                                                        currentActivity != activities::Activity::PushedEast &&
                                                                        currentActivity != activities::Activity::PushedWest )
                                                                {
                                                                        freeItemAbove.getBehavior()->changeActivityDueTo( activity, ItemPtr( &freeItemAbove ) );
                                                                }
                                                        }
                                                }
                                        }
                                }
                        }
                }
        }
}

}
