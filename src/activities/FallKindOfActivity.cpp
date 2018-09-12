
#include "FallKindOfActivity.hpp"
#include "Behavior.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "GameManager.hpp"

#ifdef DEBUG
#  define DEBUG_ANCHORING       0
#endif


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

        bool mayFall = behavior->getItem()->addToZ( -1 );

        // when there’s something below
        if ( ! mayFall )
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
                                // is it grid item or free item
                                if ( itemBelow->whichKindOfItem() == "grid item" ||
                                        itemBelow->whichKindOfItem() == "free item" ||
                                        itemBelow->whichKindOfItem() == "player item" )
                                {
                                        if ( itemBelow->whichKindOfItem() == "player item" && sender->isMortal() )
                                        {
                                                if ( sender->canAdvanceTo( 0, 0, -1 ) )
                                                {
                                                        if ( ! GameManager::getInstance()->isImmuneToCollisionsWithMortalItems () )
                                                        {
                                                                itemBelow->getBehavior()->changeActivityOfItem( MeetMortalItem );
                                                        }
                                                }
                                        }
                                        else if ( sender->whichKindOfItem() == "player item" && itemBelow->isMortal() )
                                        {
                                                if ( sender->canAdvanceTo( 0, 0, -1 ) )
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

        return mayFall ;
}

void FallKindOfActivity::assignAnchor( FreeItem* freeItem, const std::vector< std::string >& itemsBelow )
{
        if ( freeItem == nilPointer ) return ;

        Item* anchor = nilPointer;
        Item* oldAnchor = freeItem->getAnchor();

        unsigned int count = 0;

        Mediator* mediator = freeItem->getMediator();

        // when item falls on several ones below it, priority to choose anchor is
        //    grid item before free item
        //    harmless item before mortal item
        //    item with higher spatial coordinates

        for ( std::vector< std::string >::const_iterator it = itemsBelow.begin () ; it != itemsBelow.end () ; ++ it )
        {
        #if defined( DEBUG_ANCHORING ) && DEBUG_ANCHORING
                std::cout << "assignAnchor got item " << *it << " below " << freeItem->getUniqueName() << std::endl ;
        #endif

                Item* itemBelow = mediator->findItemByUniqueName( *it );
                count++ ;

                // in case when item is already anchored
                if ( oldAnchor != nilPointer && itemBelow != nilPointer && oldAnchor == itemBelow )
                {
                        anchor = oldAnchor;
                        break;
                }

                if ( count == 1 )
                {
                        // just pick that first item as first choice of anchor
                        anchor = itemBelow;
                }
                else if ( itemBelow != nilPointer && anchor != nilPointer )
                {
                        // if it is grid item and current anchor is not grid item then pick grid item as anchor
                        if ( itemBelow->whichKindOfItem() == "grid item" && anchor->whichKindOfItem() != "grid item" )
                        {
                                anchor = itemBelow;
                        }

                        if ( anchor != itemBelow )
                        {
                                if ( ! itemBelow->isMortal() )
                                {
                                        // when current anchor is mortal then pick harmless item as anchor
                                        if ( anchor->isMortal() )
                                        {
                                                anchor = itemBelow;
                                        }
                                        // current anchor is harmless too, pick as anchor item with higher coordinates
                                        else
                                        {
                                                if ( anchor->getX() + anchor->getY() < itemBelow->getX() + itemBelow->getY() )
                                                {
                                                        anchor = itemBelow;
                                                }
                                        }
                                }
                        }
                }
        }

        if ( anchor != oldAnchor )
        {
                freeItem->setAnchor( anchor );

        #if defined( DEBUG_ANCHORING ) && DEBUG_ANCHORING
                if ( anchor != nilPointer )
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
