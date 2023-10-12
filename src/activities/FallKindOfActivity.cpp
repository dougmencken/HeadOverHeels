
#include "FallKindOfActivity.hpp"

#include "Behavior.hpp"
#include "PlayerItem.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "GameManager.hpp"

#ifdef DEBUG
#  define DEBUG_ANCHORING       0
#endif


namespace iso
{

FallKindOfActivity * FallKindOfActivity::instance = nilPointer ;

FallKindOfActivity& FallKindOfActivity::getInstance()
{
        if ( instance == nilPointer )
        {
                instance = new FallKindOfActivity();
        }

        return *instance;
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
                game::GameManager::getInstance().charactersFly() &&
                ! ( allegro::isShiftKeyPushed() && allegro::isKeyPushed( "PageDown" ) ) )
        {
                return false ;
        }

        bool mayFall = behavior->getItem()->addToZ( -1 );

        // when there’s something below
        if ( ! mayFall )
        {
                Item& sender = *( behavior->getItem() );
                Mediator* mediator = sender.getMediator();

                // copy stack of collisions into vector
                std::vector< std::string > itemsBelow;
                while ( ! mediator->isStackOfCollisionsEmpty() )
                {
                        itemsBelow.push_back( mediator->popCollision() );
                }

                if ( sender.whichKindOfItem() == "free item" || sender.whichKindOfItem() == "player item" )
                {
                        this->assignAnchor( sender.getUniqueName(), sender.getMediator(), itemsBelow );
                }

                // as long as there’re items collided with the sender
                while ( ! itemsBelow.empty() )
                {
                        std::string name = itemsBelow.back();
                        itemsBelow.pop_back();

                        ItemPtr itemBelow = mediator->findItemByUniqueName( name );

                        if ( itemBelow != nilPointer )
                        {
                                // is it grid item or free item
                                if ( itemBelow->whichKindOfItem() == "grid item" ||
                                        itemBelow->whichKindOfItem() == "free item" ||
                                        itemBelow->whichKindOfItem() == "player item" )
                                {
                                        if ( itemBelow->whichKindOfItem() == "player item" && sender.isMortal() )
                                        {
                                                if ( sender.canAdvanceTo( 0, 0, -1 ) )
                                                {
                                                        if ( ! game::GameManager::getInstance().isImmuneToCollisionsWithMortalItems () )
                                                        {
                                                                itemBelow->getBehavior()->changeActivityOfItem( Activity::MeetMortalItem );
                                                        }
                                                }
                                        }
                                        else if ( sender.whichKindOfItem() == "player item" && itemBelow->isMortal() )
                                        {
                                                if ( sender.canAdvanceTo( 0, 0, -1 ) )
                                                {
                                                        if ( ! game::GameManager::getInstance().isImmuneToCollisionsWithMortalItems () )
                                                        {
                                                                sender.getBehavior()->changeActivityOfItem( Activity::MeetMortalItem );
                                                        }
                                                }
                                                else
                                                {
                                                        bool onlyMortal = true;

                                                        // look if some item underneath the character is not mortal
                                                        while ( ! mediator->isStackOfCollisionsEmpty() )
                                                        {
                                                                if ( ! mediator->findCollisionPop()->isMortal() )
                                                                {
                                                                        onlyMortal = false;
                                                                }
                                                        }

                                                        // if every one is mortal then the character loses its life
                                                        if ( onlyMortal )
                                                        {
                                                                if ( ! game::GameManager::getInstance().isImmuneToCollisionsWithMortalItems () )
                                                                {
                                                                        sender.getBehavior()->changeActivityOfItem( Activity::MeetMortalItem );
                                                                }
                                                        }
                                                }
                                        }
                                }
                        }
                        // the character reaches floor
                        else if ( sender.whichKindOfItem() == "player item" && name == "some tile of floor" )
                        {
                                PlayerItem& characterItem = dynamic_cast< PlayerItem& >( sender );

                                if ( ! mediator->getRoom()->hasFloor() )
                                {
                                        characterItem.setWayOfExit( "below" );
                                }
                                else
                                if ( mediator->getRoom()->getKindOfFloor() == "mortal" )
                                {
                                        if ( ! game::GameManager::getInstance().isImmuneToCollisionsWithMortalItems () )
                                        {
                                                characterItem.getBehavior()->changeActivityOfItem( Activity::MeetMortalItem );
                                        }
                                }
                        }
                }
        }

        return mayFall ;
}

void FallKindOfActivity::assignAnchor( const std::string & uniqueNameOfItem, Mediator * mediator, const std::vector< std::string >& itemsBelow )
{
        assert( mediator != nilPointer );

        FreeItem& freeItem = dynamic_cast< FreeItem& >( * mediator->findItemByUniqueName( uniqueNameOfItem ) );

        std::string anchor ;
        std::string oldAnchor = freeItem.getAnchor();

        // when item falls on several ones below it, priority to choose anchor is
        //    grid item before free item
        //    harmless item before mortal item
        //    item with higher spatial coordinates

        bool isFirst = true ;

        for ( std::vector< std::string >::const_iterator it = itemsBelow.begin () ; it != itemsBelow.end () ; ++ it )
        {
        #if defined( DEBUG_ANCHORING ) && DEBUG_ANCHORING
                std::cout << "assignAnchor got item " << *it << " below " << uniqueNameOfItem << std::endl ;
        #endif

                ItemPtr itemBelow = mediator->findItemByUniqueName( *it );

                // in case when item is already anchored
                if ( itemBelow != nilPointer && oldAnchor == itemBelow->getUniqueName() )
                {
                        anchor = oldAnchor;
                        break;
                }

                if ( isFirst )
                {
                        // just pick that first item as first choice of anchor
                        if ( itemBelow != nilPointer )
                                anchor = itemBelow->getUniqueName() ;

                        isFirst = false ;
                }
                else if ( itemBelow != nilPointer && ! anchor.empty() )
                {
                        // if it is grid item and current anchor is not grid item then pick grid item as anchor
                        if ( itemBelow->whichKindOfItem() == "grid item" && mediator->findItemByUniqueName( anchor )->whichKindOfItem() != "grid item" )
                        {
                                anchor = itemBelow->getUniqueName() ;
                        }

                        if ( anchor != itemBelow->getUniqueName() )
                        {
                                if ( ! itemBelow->isMortal() )
                                {
                                        // when current anchor is mortal then pick harmless item as anchor
                                        if ( mediator->findItemByUniqueName( anchor )->isMortal() )
                                        {
                                                anchor = itemBelow->getUniqueName() ;
                                        }
                                        // current anchor is harmless too, pick as anchor item with higher coordinates
                                        else
                                        {
                                                ItemPtr anchorItem = mediator->findItemByUniqueName( anchor );

                                                if ( anchorItem->getX() + anchorItem->getY() < itemBelow->getX() + itemBelow->getY() )
                                                {
                                                        anchor = itemBelow->getUniqueName() ;
                                                }
                                        }
                                }
                        }
                }
        }

        if ( anchor != oldAnchor )
        {
                freeItem.setAnchor( anchor );

        #if defined( DEBUG_ANCHORING ) && DEBUG_ANCHORING
                ItemPtr anchorItem = mediator->findItemByUniqueName( anchor );

                if ( anchor != nilPointer )
                {
                        std::cout << anchorItem->whichKindOfItem() << " \"" << anchorItem->getUniqueName() << "\" at" <<
                                        " x=" << anchorItem->getX() << " y=" << anchorItem->getY() << " z=" << anchorItem->getZ() <<
                                        " is set as anchor for " << freeItem.whichKindOfItem() << " \"" << freeItem.getUniqueName() <<
                                        "\" at" << " x=" << freeItem.getX() << " y=" << freeItem.getY() << " z=" << freeItem.getZ()
                                  << std::endl ;
                }
        #endif
        }
}

}
