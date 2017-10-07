
#include "KindOfActivity.hpp"
#include "Behavior.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "GameManager.hpp"


namespace isomot
{

KindOfActivity::KindOfActivity( )
{

}

KindOfActivity::~KindOfActivity( )
{

}

bool KindOfActivity::move( Behavior * behavior, ActivityOfItem * activity, bool canFall )
{
        return true;
}

bool KindOfActivity::displace( Behavior * behavior, ActivityOfItem * activity, bool canFall )
{
        return true;
}

bool KindOfActivity::fall( Behavior * behavior )
{
        return true;
}

bool KindOfActivity::jump( Behavior * behavior, ActivityOfItem * activity, const std::vector< JumpMotion >& jumpMatrix, int * jumpIndex )
{
        return true;
}

void KindOfActivity::changeKindOfActivity ( Behavior * behavior, KindOfActivity * newKind )
{
        behavior->changeActivityTo( newKind ) ;
}

void KindOfActivity::propagateActivityToAdjacentItems( Item * sender, const ActivityOfItem& activity )
{
        Mediator* mediator = sender->getMediator();

        // as long as there are items collided with sender
        while ( ! mediator->isStackOfCollisionsEmpty() )
        {
                // pop item out of stack of collisions
                int id = mediator->popCollision();

                // is it free item or grid item
                if ( ( id >= FirstFreeId && ( id & 1 ) ) || ( id >= FirstGridId && ! ( id & 1 ) ) )
                {
                        Item * itemMeetsSender = mediator->findItemById( id );

                        if ( itemMeetsSender != 0 )
                        {
                                // is it item with behavior
                                if ( itemMeetsSender->getBehavior() != 0 )
                                {
                                        // if it’s player item and sender is mortal, then player loses its life
                                        if ( dynamic_cast< PlayerItem * >( itemMeetsSender ) && sender->isMortal() &&
                                                        dynamic_cast< PlayerItem * >( itemMeetsSender )->getShieldTime() <= 0 )
                                        {
                                                if ( itemMeetsSender->getBehavior()->getActivityOfItem() != MeetMortalItem &&
                                                                itemMeetsSender->getBehavior()->getActivityOfItem() != Vanish )
                                                {
                                                        if ( ! GameManager::getInstance()->isImmuneToCollisionsWithMortalItems () )
                                                        {
                                                                std::cout << "mortal item \"" << sender->getLabel() << "\" just met player" << std::endl ;

                                                                // is it direct contact
                                                                if ( mediator->depthOfStackOfCollisions() <= 1 )
                                                                {
                                                                        itemMeetsSender->getBehavior()->changeActivityOfItem( MeetMortalItem );
                                                                }
                                                        }
                                                        /* else std::cout << "right to inviolability granted when item \"" << sender->getLabel() << "\" met player" << std::endl ; */
                                                }
                                        }
                                        // if sender is player and colliding one is mortal, then player loses its life
                                        else if ( dynamic_cast< PlayerItem * >( sender ) && itemMeetsSender->isMortal() &&
                                                        dynamic_cast< PlayerItem * >( sender )->getShieldTime() <= 0 )
                                        {
                                                if ( sender->getBehavior()->getActivityOfItem() != MeetMortalItem &&
                                                                itemMeetsSender->getBehavior()->getActivityOfItem() != Vanish )
                                                {
                                                        if ( ! GameManager::getInstance()->isImmuneToCollisionsWithMortalItems () )
                                                        {
                                                                std::cout << "player just met mortal item \"" << itemMeetsSender->getLabel() << "\"" << std::endl ;

                                                                sender->getBehavior()->changeActivityOfItem( MeetMortalItem );
                                                                itemMeetsSender->getBehavior()->changeActivityOfItem( activity, sender );
                                                        }
                                                        /* else std::cout << "right to inviolability granted when player met item \"" << itemMeetsSender->getLabel() << "\"" << std::endl ; */
                                                }
                                        }
                                        // if not, propagate activity to that item
                                        else
                                        {
                                                if ( itemMeetsSender->getBehavior()->getActivityOfItem() != Vanish )
                                                {
                                                        itemMeetsSender->getBehavior()->changeActivityOfItem( activity, sender );
                                                }
                                        }
                                }
                                // otherwise it is item without behavior, which may be mortal too
                                else if ( dynamic_cast< PlayerItem * >( sender ) && itemMeetsSender->isMortal() &&
                                                dynamic_cast< PlayerItem * >( sender )->getShieldTime() <= 0 )
                                {
                                        if ( sender->getBehavior()->getActivityOfItem() != MeetMortalItem &&
                                                        sender->getBehavior()->getActivityOfItem() != Vanish )
                                        {
                                                if ( ! GameManager::getInstance()->isImmuneToCollisionsWithMortalItems () )
                                                {
                                                        sender->getBehavior()->changeActivityOfItem( MeetMortalItem );
                                                }
                                        }
                                }
                        }
                }
                // is it player which leaves room via some door
                else if ( dynamic_cast< PlayerItem * >( sender ) &&
                                ( ( id == NorthBorder  &&  mediator->getRoom()->getDoor( North )  != 0 ) ||
                                  ( id == SouthBorder  &&  mediator->getRoom()->getDoor( South )  != 0 ) ||
                                  ( id == EastBorder  &&  mediator->getRoom()->getDoor( East )  != 0 ) ||
                                  ( id == WestBorder  &&  mediator->getRoom()->getDoor( West )  != 0 ) ||
                                  ( id == NortheastBorder  &&  mediator->getRoom()->getDoor( Northeast )  != 0 ) ||
                                  ( id == NorthwestBorder  &&  mediator->getRoom()->getDoor( Northwest )  != 0 ) ||
                                  ( id == SoutheastBorder  &&  mediator->getRoom()->getDoor( Southeast )  != 0 ) ||
                                  ( id == SouthwestBorder  &&  mediator->getRoom()->getDoor( Southwest )  != 0 ) ||
                                  ( id == EastnorthBorder  &&  mediator->getRoom()->getDoor( Eastnorth )  != 0 ) ||
                                  ( id == EastsouthBorder  &&  mediator->getRoom()->getDoor( Eastsouth )  != 0 ) ||
                                  ( id == WestnorthBorder  &&  mediator->getRoom()->getDoor( Westnorth )  != 0 ) ||
                                  ( id == WestsouthBorder  &&  mediator->getRoom()->getDoor( Westsouth )  != 0 ) ) )
                {
                        PlayerItem * player = dynamic_cast< PlayerItem * >( sender );

                        switch ( id )
                        {
                                case NorthBorder:
                                        player->setDirectionOfExit( North );
                                        break;

                                case SouthBorder:
                                        player->setDirectionOfExit( South );
                                        break;

                                case EastBorder:
                                        player->setDirectionOfExit( East );
                                        break;

                                case WestBorder:
                                        player->setDirectionOfExit( West );
                                        break;

                                case NortheastBorder:
                                        player->setDirectionOfExit( Northeast );
                                        break;

                                case NorthwestBorder:
                                        player->setDirectionOfExit( Northwest );
                                        break;

                                case SoutheastBorder:
                                        player->setDirectionOfExit( Southeast );
                                        break;

                                case SouthwestBorder:
                                        player->setDirectionOfExit( Southwest );
                                        break;

                                case EastnorthBorder:
                                        player->setDirectionOfExit( Eastnorth );
                                        break;

                                case EastsouthBorder:
                                        player->setDirectionOfExit( Eastsouth );
                                        break;

                                case WestnorthBorder:
                                        player->setDirectionOfExit( Westnorth );
                                        break;

                                case WestsouthBorder:
                                        player->setDirectionOfExit( Westsouth );
                                        break;

                                default:
                                        ;
                        }
                }
        }
}

void KindOfActivity::propagateActivityToItemsAbove( Item * sender, const ActivityOfItem& activity )
{
        Mediator* mediator = sender->getMediator();

        // is there anything above
        if ( ! sender->checkPosition( 0, 0, 1, Add ) )
        {
                // copy stack of collisions
                std::stack< int > itemsAbove;
                while ( ! mediator->isStackOfCollisionsEmpty() )
                {
                        itemsAbove.push( mediator->popCollision() );
                }

                while ( ! itemsAbove.empty() )
                {
                        // get first item
                        int id = itemsAbove.top();
                        itemsAbove.pop();

                        // is it free item
                        if ( id >= FirstFreeId && ( id & 1 ) )
                        {
                                FreeItem* freeItemAbove = dynamic_cast< FreeItem * >( mediator->findItemById( id ) );

                                // is it item with behavior
                                if ( freeItemAbove != 0 && freeItemAbove->getBehavior() != 0 )
                                {
                                        // look for collisions of that free item with items below it
                                        if ( ! freeItemAbove->checkPosition( 0, 0, -1, Add ) )
                                        {
                                                // propagate activity when there’s no more than one item below or when sender is anchor of that item
                                                if ( mediator->depthOfStackOfCollisions() <= 1 || freeItemAbove->getAnchor() == sender )
                                                {
                                                        if ( freeItemAbove->getBehavior()->getActivityOfItem() != Vanish )
                                                        {
                                                                // if it’s player item above sender and sender is mortal, then player loses its life
                                                                if ( dynamic_cast< PlayerItem * >( freeItemAbove ) && sender->isMortal() &&
                                                                        dynamic_cast< PlayerItem * >( freeItemAbove )->getShieldTime() <= 0 )
                                                                {
                                                                        if ( freeItemAbove->getBehavior()->getActivityOfItem() != MeetMortalItem )
                                                                        {
                                                                                if ( ! GameManager::getInstance()->isImmuneToCollisionsWithMortalItems () )
                                                                                {
                                                                                        std::cout << "player is above mortal item \"" << sender->getLabel() << "\"" << std::endl ;
                                                                                        freeItemAbove->getBehavior()->changeActivityOfItem( MeetMortalItem );
                                                                                }
                                                                                /* else std::cout << "right to inviolability granted when player is above item \"" << sender->getLabel() << "\"" << std::endl ; */
                                                                        }
                                                                }
                                                                // if not, propagate activity to that item above
                                                                else
                                                                {
                                                                        ActivityOfItem currentActivity = freeItemAbove->getBehavior()->getActivityOfItem();
                                                                        if ( currentActivity != DisplaceNorth && currentActivity != DisplaceSouth &&
                                                                                currentActivity != DisplaceEast && currentActivity != DisplaceWest )
                                                                        {
                                                                                freeItemAbove->getBehavior()->changeActivityOfItem( activity, freeItemAbove );
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

}
