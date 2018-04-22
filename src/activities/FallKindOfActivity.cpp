
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
        if ( behavior == nilPointer ) return false ;

        if ( behavior->getItem()->whichKindOfItem() == "player item" &&
                GameManager::getInstance()->charactersFly() && ! ( ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_PGDN ] ) )
        {
                return false ;
        }

        bool fallsNow = behavior->getItem()->addToZ( -1 );

        // when there’s something below
        if ( ! fallsNow )
        {
                Item* sender = behavior->getItem();
                Mediator* mediator = sender->getMediator();

                // copy stack of collisions into vector
                std::vector< std::string > itemsBelow;
                while ( ! mediator->isStackOfCollisionsEmpty() )
                {
                        itemsBelow.push_back( mediator->popCollision() );
                }

                if ( sender->whichKindOfItem() == "free item" || sender->whichKindOfItem() == "player item" )
                        this->assignAnchor( dynamic_cast< FreeItem * >( sender ), itemsBelow );

                // as long as there’re items collided with sender
                while ( ! itemsBelow.empty() )
                {
                        std::string name = itemsBelow.back();
                        itemsBelow.pop_back();

                        Item* itemBelow = mediator->findItemByUniqueName( name );

                        if ( itemBelow != nilPointer )
                        {
                                // is it free item or grid item
                                if ( itemBelow->whichKindOfItem() == "grid item" ||
                                        itemBelow->whichKindOfItem() == "free item" || itemBelow->whichKindOfItem() == "player item" )
                                {
                                        if ( dynamic_cast< PlayerItem * >( itemBelow ) && sender->isMortal() )
                                        {
                                                if ( sender->checkPosition( 0, 0, -1, Add ) )
                                                {
                                                        if ( ! GameManager::getInstance()->isImmuneToCollisionsWithMortalItems () )
                                                        {
                                                                itemBelow->getBehavior()->changeActivityOfItem( MeetMortalItem );
                                                        }
                                                }
                                        }
                                        else if ( sender->whichKindOfItem() == "player item" && itemBelow->isMortal() )
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
                        else if ( sender->whichKindOfItem() == "player item" && name == "some tile of floor" )
                        {
                                PlayerItem* playerItem = dynamic_cast< PlayerItem * >( sender );

                                if ( mediator->getRoom()->getKindOfFloor() == "none" )
                                {
                                        playerItem->setWayOfExit( "down" );
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

void FallKindOfActivity::assignAnchor( FreeItem* freeItem, const std::vector< std::string >& items )
{
        if ( freeItem != nilPointer )
        {
                Mediator* mediator = freeItem->getMediator();

                // set anchor when item falls and is placed on some other one, its anchor

                // when item falls on several items below it, priority to choose anchor is
                //    grid item before free item
                //    harmless item before mortal item
                //    item with higher spatial coordinates

                Item* anchor = nilPointer;
                Item* oldAnchor = freeItem->getAnchor();

                unsigned int count = 0;

                // search for anchor of this item
                for ( std::vector< std::string >::const_iterator it = items.begin () ; it != items.end () ; ++ it )
                {
                        Item* item = mediator->findItemByUniqueName( *it );
                        count++ ;

                        // in case when item is anchored previously
                        if ( oldAnchor != nilPointer && item != nilPointer && oldAnchor == item )
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
                                // if it is grid item and current anchor is not grid item then pick grid item as anchor
                                if ( item->whichKindOfItem() == "grid item" && anchor->whichKindOfItem() != "grid item" )
                                {
                                        anchor = item;
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

        #ifdef DEBUG
                if ( anchor != nilPointer && anchor != oldAnchor )
                {
                        std::cout << anchor->whichKindOfItem() << " \"" << anchor->getUniqueName() << "\" at" <<
                                        " x=" << anchor->getX() << " y=" << anchor->getY() << " z=" << anchor->getZ() <<
                                        " is set as anchor for " << freeItem->whichKindOfItem() << " \"" << freeItem->getUniqueName() <<
                                        "\" at" << " x=" << freeItem->getX() << " y=" << freeItem->getY() << " z=" << freeItem->getZ()
                                  << std::endl ;
                }
        #endif
        }
}

}
