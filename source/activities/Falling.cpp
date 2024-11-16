
#include "Falling.hpp"

#include "AvatarItem.hpp"
#include "Room.hpp"
#include "GameManager.hpp"

#ifdef DEBUG
#  define DEBUG_ANCHORING       0
#endif


namespace activities
{

Falling * Falling::instance = nilPointer ;

Falling & Falling::getInstance()
{
        if ( instance == nilPointer )
                instance = new Falling () ;

        return *instance;
}


bool Falling::fall( behaviors::Behavior & behavior )
{
        DescribedItem & whatFalls = dynamic_cast< DescribedItem & >( behavior.getItem () );

        if ( whatFalls.whichItemClass() == "avatar item"
                && GameManager::getInstance().charactersFly()
                        && ! ( allegro::isShiftKeyPushed() && allegro::isKeyPushed( "PageDown" ) ) )
        {
                return false ;
        }

        int lowerBy = -1 ;
        lowerBy -= behavior.getHowLongFalls() / Room::LayerHeight ; // accelerate

        bool mayFall = false ;
        while ( ! mayFall && lowerBy < 0 )
                mayFall = whatFalls.addToZ( lowerBy ++ );

        // when there’s something below
        if ( ! mayFall )
        {
                Mediator & mediator = * whatFalls.getMediator() ;

                // collect the collisions
                std::vector< std::string > itemsBelow ;
                while ( mediator.isThereAnyCollision() )
                        itemsBelow.push_back( mediator.popCollision() );

                if ( whatFalls.whichItemClass() == "free item" || whatFalls.whichItemClass() == "avatar item" )
                {
                        this->assignAnchor( whatFalls.getUniqueName(), whatFalls.getMediator(), itemsBelow );
                }

                while ( ! itemsBelow.empty() )
                {
                        std::string nameOfItemBelow = itemsBelow.back();
                        itemsBelow.pop_back();

                        DescribedItemPtr itemBelow = mediator.findItemByUniqueName( nameOfItemBelow );

                        if ( itemBelow != nilPointer )
                        {
                                // is it a grid item or a free item
                                if ( itemBelow->whichItemClass() == "grid item"
                                        || itemBelow->whichItemClass() == "free item"
                                                || itemBelow->whichItemClass() == "avatar item" )
                                {
                                        if ( itemBelow->whichItemClass() == "avatar item" && whatFalls.isMortal() )
                                        {
                                                if ( whatFalls.canAdvanceTo( 0, 0, -1 ) )
                                                {
                                                        itemBelow->getBehavior()->setCurrentActivity( activities::Activity::MetLethalItem );
                                                }
                                        }
                                        else if ( whatFalls.whichItemClass() == "avatar item" && itemBelow->isMortal() )
                                        {
                                                if ( whatFalls.canAdvanceTo( 0, 0, -1 ) )
                                                {
                                                        whatFalls.getBehavior()->setCurrentActivity( activities::Activity::MetLethalItem );
                                                }
                                                else
                                                {
                                                        bool onlyMortal = true;

                                                        // look if some item underneath the character is not mortal
                                                        while ( mediator.isThereAnyCollision() )
                                                                if ( ! mediator.findCollisionPop()->isMortal() )
                                                                {
                                                                        onlyMortal = false ;
                                                                        break ;
                                                                }

                                                        // if every one is mortal then the character loses its life
                                                        if ( onlyMortal )
                                                                whatFalls.getBehavior()->setCurrentActivity( activities::Activity::MetLethalItem );
                                                }
                                        }
                                }
                        }
                        // the character reaches the floor
                        else if ( whatFalls.whichItemClass() == "avatar item" && nameOfItemBelow == "some tile of floor" )
                        {
                                AvatarItem & characterItem = dynamic_cast< AvatarItem & >( whatFalls );

                                if ( ! mediator.getRoom()->hasFloor() )
                                {
                                        characterItem.setWayOfExit( "below" );

                                        AvatarItemPtr waitingOne = mediator.getWaitingCharacter() ;
                                        if ( waitingOne != nilPointer && waitingOne->getUniqueName() == characterItem.getUniqueName() ) {
                                                // only the active character can change rooms,
                                                // and falling in a floor-less room down into the room below
                                                // couldn’t happen without activating the character who falls
                                                std::cout << "inactive character \"" << waitingOne->getKind() << "\" falls down into another room,"
                                                                << " swapping the characters will activate the falling one" << std::endl ;
                                                mediator.pickNextCharacter () ;
                                        }
                                }
                                else
                                if ( mediator.getRoom()->getKindOfFloor() == "mortal" )
                                {
                                        characterItem.getBehavior()->setCurrentActivity( activities::Activity::MetLethalItem );
                                }
                        }
                }
        }

        if ( mayFall ) behavior.incrementHowLongFalls() ;
        else behavior.resetHowLongFalls() ;

        return mayFall ;
}

void Falling::assignAnchor( const std::string & uniqueNameOfItem, Mediator * mediator, const std::vector< std::string >& itemsBelow )
{
        assert( mediator != nilPointer );

        FreeItem& freeItem = dynamic_cast< FreeItem& >( * mediator->findItemByUniqueName( uniqueNameOfItem ) );

        std::string anchor ;
        const std::string & existingAnchor = freeItem.getCarrier() ;

        // when an item falls on something below it, the anchoring priority is
        //    a grid item over a free item
        //    a harmless item over a mortal item
        //    an item with bigger x+y coordinates in space

        bool isFirst = true ;

        for ( std::vector< std::string >::const_iterator it = itemsBelow.begin () ; it != itemsBelow.end () ; ++ it )
        {
                DescribedItemPtr itemBelow = mediator->findItemByUniqueName( *it );

                if ( itemBelow != nilPointer && itemBelow->getUniqueName() == existingAnchor ) {
                        // already anchored
                        anchor = existingAnchor ;
                        break ;
                }

        #if defined( DEBUG_ANCHORING ) && DEBUG_ANCHORING > 1
                std::cout << "assignAnchor got item " << *it << " below " << uniqueNameOfItem << std::endl ;
        #endif

                if ( isFirst )
                {
                        // just pick the first item as the initial candidate for anchor
                        if ( itemBelow != nilPointer )
                                anchor = itemBelow->getUniqueName() ;

                        isFirst = false ;
                }
                else if ( itemBelow != nilPointer && ! anchor.empty() )
                {
                        // if it’s a grid item and the current anchor is not grid item then a grid item becomes the anchor
                        if ( itemBelow->whichItemClass() == "grid item" && mediator->findItemByUniqueName( anchor )->whichItemClass() != "grid item" )
                        {
                                anchor = itemBelow->getUniqueName() ;
                        }

                        if ( anchor != itemBelow->getUniqueName() )
                        {
                                if ( ! itemBelow->isMortal() )
                                {
                                        // when the current anchor is mortal then a harmless item becomes the anchor
                                        if ( mediator->findItemByUniqueName( anchor )->isMortal() )
                                        {
                                                anchor = itemBelow->getUniqueName() ;
                                        }
                                        // the current anchor is harmless too
                                        else
                                        {       // then the anchor is an item with the larger sum of X and Y coordinates
                                                DescribedItemPtr anchorItem = mediator->findItemByUniqueName( anchor );

                                                if ( anchorItem->getX() + anchorItem->getY() < itemBelow->getX() + itemBelow->getY() )
                                                {
                                                        anchor = itemBelow->getUniqueName() ;
                                                }
                                        }
                                }
                        }
                }
        }

        if ( anchor != existingAnchor )
        {
                freeItem.setCarrier( anchor );

        #if defined( DEBUG_ANCHORING ) && DEBUG_ANCHORING
                ItemPtr anchorItem = mediator->findItemByUniqueName( anchor );

                if ( anchorItem != nilPointer ) {
                        std::cout << anchorItem->whichItemClass() << " \"" << anchorItem->getUniqueName() << "\" at" <<
                                        " x=" << anchorItem->getX() << " y=" << anchorItem->getY() << " z=" << anchorItem->getZ() <<
                                        " is set as anchor for " << freeItem.whichItemClass() << " \"" << freeItem.getUniqueName() <<
                                        "\" at" << " x=" << freeItem.getX() << " y=" << freeItem.getY() << " z=" << freeItem.getZ()
                                  << std::endl ;
                }
        #endif
        }
}

}
