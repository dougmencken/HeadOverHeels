
#include "FallKindOfActivity.hpp"
#include "Behavior.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "GameManager.hpp"


namespace isomot
{

FallKindOfActivity * FallKindOfActivity::instance = nilPointer ;

FallKindOfActivity* FallKindOfActivity::getInstance()
{
        if ( instance == nilPointer )
        {
                instance = new FallKindOfActivity();
        }

        return instance;
}


FallKindOfActivity::FallKindOfActivity( ) : KindOfActivity()
{

}

FallKindOfActivity::~FallKindOfActivity( )
{

}

bool FallKindOfActivity::fall( Behavior * behavior )
{
        bool fallsNow = behavior->getItem()->addToZ( -1 );

        // when don’t fall anymore see is this item a player and does it collide with some mortal item
        if ( ! fallsNow )
        {
                Item* sender = behavior->getItem();
                Mediator* mediator = sender->getMediator();

                // copy stack of collisions
                std::stack< int > itemsBelow;
                while ( ! mediator->isStackOfCollisionsEmpty() )
                {
                        itemsBelow.push( mediator->popCollision() );
                }

                this->assignAnchor( dynamic_cast< FreeItem * >( sender ), itemsBelow );

                // as long as there’re items collided with sender
                while ( ! itemsBelow.empty() )
                {
                        int id = itemsBelow.top();
                        itemsBelow.pop();

                        // is it free item or grid item
                        if ( ( id >= FirstFreeId && ( id & 1 ) ) || ( id >= FirstGridId && ! ( id & 1 ) ) )
                        {
                                Item* item = mediator->findItemById( id );

                                if ( item != nilPointer )
                                {
                                        if ( dynamic_cast< PlayerItem * >( item ) && sender->isMortal() )
                                        {
                                                if ( sender->checkPosition( 0, 0, -1, Add ) )
                                                {
                                                        if ( ! GameManager::getInstance()->isImmuneToCollisionsWithMortalItems () )
                                                        {
                                                                item->getBehavior()->changeActivityOfItem( MeetMortalItem );
                                                        }
                                                }
                                        }
                                        else if ( dynamic_cast< PlayerItem * >( sender ) && item->isMortal() )
                                        {
                                                if ( sender->checkPosition( 0, 0, -1, Add ) )
                                                {
                                                        if ( ! GameManager::getInstance()->isImmuneToCollisionsWithMortalItems () )
                                                        {
                                                                sender->getBehavior()->changeActivityOfItem( MeetMortalItem );
                                                        }
                                                }
                                                else
                                                {
                                                        bool allMortal = true;

                                                        // look if some item underneath player is not mortal
                                                        while ( ! mediator->isStackOfCollisionsEmpty() )
                                                        {
                                                                if ( ! mediator->findCollisionPop()->isMortal() )
                                                                {
                                                                        allMortal = false;
                                                                }
                                                        }

                                                        // if every one is mortal then player loses its life
                                                        if ( allMortal )
                                                        {
                                                                if ( ! GameManager::getInstance()->isImmuneToCollisionsWithMortalItems () )
                                                                {
                                                                        sender->getBehavior()->changeActivityOfItem( MeetMortalItem );
                                                                }
                                                        }
                                                }
                                        }
                                }
                        }
                        // player reaches floor
                        else if ( dynamic_cast< PlayerItem * >( sender ) && id == Floor )
                        {
                                PlayerItem* playerItem = dynamic_cast< PlayerItem * >( sender );

                                if ( mediator->getRoom()->getKindOfFloor() == "none" )
                                {
                                        playerItem->setWayOfExit( Down );
                                }
                                else
                                if ( mediator->getRoom()->getKindOfFloor() == "mortal" )
                                {
                                        if ( ! GameManager::getInstance()->isImmuneToCollisionsWithMortalItems () )
                                        {
                                                playerItem->getBehavior()->changeActivityOfItem( MeetMortalItem );
                                        }
                                }
                                else
                                /* if ( mediator->getRoom()->getKindOfFloor() == "plain" ) */
                                {
                                        // nothing to do
                                }
                        }
                }
        }

        return fallsNow ;
}

void FallKindOfActivity::assignAnchor( FreeItem* freeItem, std::stack< int > items )
{
        if ( freeItem != nilPointer )
        {
                Mediator* mediator = freeItem->getMediator();

                int count = 0;

                // set anchor when item falls and is placed on some other one, its anchor

                // in case when item falls on several items below it the priority to anchor is
                //    grid item before free item
                //    harmless item before mortal item
                //    item with higher spatial coordinates

                Item* anchor = nilPointer;
                Item* oldAnchor = freeItem->getAnchor();

                // search for anchor of this item
                while ( ! items.empty() )
                {
                        int id = items.top();
                        items.pop();
                        Item* item = mediator->findItemById( id );
                        count++ ;

                        // in case when item is already anchored
                        if ( oldAnchor != nilPointer && item != nilPointer && oldAnchor->getId() == item->getId() )
                        {
                                anchor = oldAnchor;
                                break;
                        }

                        if ( count == 1 )
                        {
                                // just pick that first item as first choice of anchor
                                anchor = item;
                        }
                        else
                        {
                                // if it is grid item
                                if ( id >= FirstGridId && ! ( id & 1 ) )
                                {
                                        // when current anchor is not grid item then select this grid item as new anchor
                                        if ( ! ( anchor->getId() >= FirstGridId && ! ( anchor->getId() & 1 ) ) )
                                        {
                                                anchor = item;
                                        }
                                }

                                if ( anchor != item )
                                {
                                        if ( ! item->isMortal() )
                                        {
                                                // when current anchor is mortal then select this harmless item as new anchor
                                                if ( anchor->isMortal() )
                                                {
                                                        anchor = item;
                                                }
                                                // current anchor is harmless too, so look at positions
                                                else
                                                {
                                                        // if item has higher coordinates then select it as new anchor
                                                        if ( anchor->getX() + anchor->getY() < item->getX() + item->getY() )
                                                        {
                                                                anchor = item;
                                                        }
                                                }
                                        }
                                }
                        }
                }

                freeItem->setAnchor( anchor );

                /* if ( anchor != nilPointer && anchor != oldAnchor )
                {
                        std::cout << "item \"" << anchor->getLabel() << "\" at" <<
                                        " x=" << anchor->getX() << " y=" << anchor->getY() << " z=" << anchor->getZ() <<
                                        " is set as anchor for item \"" << freeItem->getLabel() << "\" at" <<
                                        " x=" << freeItem->getX() << " y=" << freeItem->getY() << " z=" << freeItem->getZ()
                                  << std::endl ;
                } */
        }
}

}
