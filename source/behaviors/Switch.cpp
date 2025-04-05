
#include "Switch.hpp"

#include "FreeItem.hpp"
#include "Mediator.hpp"
#include "SoundManager.hpp"

#include <algorithm> // std::find


namespace behaviors
{

Switch::Switch( FreeItem & item, const std::string & behavior )
        : Behavior( item, behavior )
        , switchedFromAbove( false )
        , whenToggledTimer( new Timer() )
{
        whenToggledTimer->go ();
}

bool Switch::update ()
{
        FreeItem & theSwitch = dynamic_cast< FreeItem & >( getItem () );
        Mediator * mediator = theSwitch.getMediator();

        switch ( getCurrentActivity() )
        {
                case activities::Activity::Waiting :
                {
                        std::set< std::string > itemsNearbyNow ;
                        lookForItemsNearby( itemsNearbyNow );

                        if ( ! itemsNearbyNow.empty () ) // if there’re items near the switch
                        {
                                for ( std::set< std::string >::iterator s = this->switchers.begin (), e = this->switchers.end () ; s != e ; )
                                {
                                        const std::string & switcher = *s ;
                                        DescribedItemPtr switcherItem = mediator->findItemByUniqueName( switcher );

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
                        if ( ! theSwitch.canAdvanceTo( 0, 0, 1 ) ) {
                                while ( mediator->isThereAnyCollision() ) // there’s something
                                {
                                        DescribedItemPtr switcherAbove = mediator->findCollisionPop () ;
                                        if ( switcherAbove == nilPointer || switcherAbove->getBehavior() == nilPointer
                                                || switcherAbove->canAdvanceTo( 0, 0, -1 ) ) continue ;

                                        // is it a free item
                                        if ( switcherAbove->whichItemClass() == "free item" || switcherAbove->whichItemClass() == "avatar item" )
                                        {
                                                // the switch doesn’t toggle when the switcher is jumping
                                                if ( switcherAbove->getBehavior()->getCurrentActivity() != activities::Activity::Jumping )
                                                {
                                                        // toggle if the switch is the only thing below the switcher
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
                        break ;

                case activities::Activity::Pushed :
                        const AbstractItemPtr & pusher = getWhatAffectedThisBehavior() ;
                        if ( pusher != nilPointer ) {
                                bool isNewPusher = this->switchers.insert( pusher->getUniqueName () ).second ;

                                if ( isNewPusher ) toggleIt() ;
                        }

                        beWaiting() ;

                        break ;
        }

        // switches are eternal
        return true ;
}

void Switch::toggleIt ()
{
        // switching again only after a second
        if ( this->whenToggledTimer->getValue() < 1.0 ) return ;

        AbstractItem & theSwitch = getItem() ;

        theSwitch.animate ();
        theSwitch.getMediator()->toggleSwitchInRoom () ;

        this->whenToggledTimer->go() ;

        // play the sound of switching
        SoundManager::getInstance().play( dynamic_cast< DescribedItem & >( theSwitch ).getKind(), "switch" );
}

void Switch::lookForItemsNearby ( std::set< std::string > & itemsNearby )
{
        DescribedItem & theSwitch = dynamic_cast< DescribedItem & >( getItem() );
        Mediator* mediator = theSwitch.getMediator();

        // is there an item at north
        if ( ! theSwitch.canAdvanceTo( -1, 0, 0 ) )
                while ( mediator->isThereAnyCollision() )
                        itemsNearby.insert( mediator->findCollisionPop()->getUniqueName () );

        // is there an item at south
        if ( ! theSwitch.canAdvanceTo( 1, 0, 0 ) )
                while ( mediator->isThereAnyCollision() )
                        itemsNearby.insert( mediator->findCollisionPop()->getUniqueName () );

        // is there an item at east
        if ( ! theSwitch.canAdvanceTo( 0, -1, 0 ) )
                while ( mediator->isThereAnyCollision() )
                        itemsNearby.insert( mediator->findCollisionPop()->getUniqueName () );

        // is there an item at west
        if ( ! theSwitch.canAdvanceTo( 0, 1, 0 ) )
                while ( mediator->isThereAnyCollision() )
                        itemsNearby.insert( mediator->findCollisionPop()->getUniqueName () );
}

}
