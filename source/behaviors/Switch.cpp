
#include "Switch.hpp"

#include "Mediator.hpp"
#include "SoundManager.hpp"

#include <algorithm> // std::find


namespace behaviors
{

Switch::Switch( Item & item, const std::string & behavior )
        : Behavior( item, behavior )
        , switchedFromAbove( false )
{

}

bool Switch::update_returningdisappearance ()
{
        Item & switchItem = getItem () ;
        Mediator * mediator = switchItem.getMediator();

        switch ( getCurrentActivity() )
        {
                case activities::Activity::Waiting:
                {
                        std::set< std::string > itemsNearbyNow ;
                        lookForItemsNearby( itemsNearbyNow );

                        if ( ! itemsNearbyNow.empty () ) // if there’re items near the switch
                        {
                                for ( std::set< std::string >::iterator s = this->switchers.begin (), e = this->switchers.end () ; s != e ; )
                                {
                                        const std::string & switcher = *s ;
                                        ItemPtr switcherItem = mediator->findItemByUniqueName( switcher );

                                        // when an item that did the switch went away from it (is not near),
                                        // or disappeared from the room,
                                        // or that item may be near but it’s a character that is waiting
                                        if ( std::find( itemsNearbyNow.begin (), itemsNearbyNow.end (), switcher ) == itemsNearbyNow.end ()
                                                        || switcherItem == nilPointer ||
                                                        ( switcherItem->whichItemClass() == "avatar item" &&
                                                                switcherItem->getBehavior()->getCurrentActivity() == activities::Activity::Waiting ) )
                                        {
                                                // then remove an item from the set of switchers so that it may re~switch
                                                this->switchers.erase( s ++ );
                                        } else
                                                ++ s ; // to the next switcher
                                }
                        } else
                                this->switchers.clear ();

                        // is there anything above the switch?
                        if ( ! switchItem.canAdvanceTo( 0, 0, 1 ) ) {
                                while ( mediator->isThereAnyCollision() ) // there’s something
                                {
                                        ItemPtr itemAbove = mediator->findCollisionPop () ;

                                        // is it a free item
                                        if ( itemAbove != nilPointer &&
                                                ( itemAbove->whichItemClass() == "free item" || itemAbove->whichItemClass() == "avatar item" ) )
                                        {
                                                if ( itemAbove->getBehavior() != nilPointer &&
                                                        ! itemAbove->canAdvanceTo( 0, 0, -1 ) &&
                                                                // the switch doesn’t toggle when the character jumps
                                                                itemAbove->getBehavior()->getCurrentActivity() != activities::Activity::Jumping )
                                                {
                                                        // toggle the switch when it’s the only one item below the switcher
                                                        if ( ! this->switchedFromAbove && mediator->howManyCollisions() <= 1 )
                                                        {
                                                                this->switchedFromAbove = true ;

                                                                toggleIt () ;
                                                        }
                                                }
                                        }
                                }
                        } else
                                this->switchedFromAbove = false ;
                }
                        break;

                case activities::Activity::PushedNorth:
                case activities::Activity::PushedSouth:
                case activities::Activity::PushedEast:
                case activities::Activity::PushedWest:
                case activities::Activity::PushedNortheast:
                case activities::Activity::PushedSoutheast:
                case activities::Activity::PushedSouthwest:
                case activities::Activity::PushedNorthwest:
                {
                        const ItemPtr & pusher = getWhatAffectedThisBehavior() ;
                        if ( pusher != nilPointer )
                        {
                                bool isNewPusher = this->switchers.insert( pusher->getUniqueName () ).second ;

                                if ( isNewPusher ) toggleIt() ;
                        }

                        setCurrentActivity( activities::Activity::Waiting );
                }
                        break;

                default:
                        ;
        }

        return false ;
}

void Switch::toggleIt ()
{
        Item & switchItem = getItem() ;

        switchItem.animate ();
        switchItem.getMediator()->toggleSwitchInRoom () ;

        // play the sound of switching
        SoundManager::getInstance().play( switchItem.getKind(), "switch" );
}

void Switch::lookForItemsNearby ( std::set< std::string > & itemsNearby )
{
        Item & switchItem = getItem() ;
        Mediator* mediator = switchItem.getMediator();

        // is there an item at north
        if ( ! switchItem.canAdvanceTo( -1, 0, 0 ) )
                while ( mediator->isThereAnyCollision() )
                        itemsNearby.insert( mediator->findCollisionPop()->getUniqueName () );

        // is there an item at south
        if ( ! switchItem.canAdvanceTo( 1, 0, 0 ) )
                while ( mediator->isThereAnyCollision() )
                        itemsNearby.insert( mediator->findCollisionPop()->getUniqueName () );

        // is there an item at east
        if ( ! switchItem.canAdvanceTo( 0, -1, 0 ) )
                while ( mediator->isThereAnyCollision() )
                        itemsNearby.insert( mediator->findCollisionPop()->getUniqueName () );

        // is there an item at west
        if ( ! switchItem.canAdvanceTo( 0, 1, 0 ) )
                while ( mediator->isThereAnyCollision() )
                        itemsNearby.insert( mediator->findCollisionPop()->getUniqueName () );
}

}
