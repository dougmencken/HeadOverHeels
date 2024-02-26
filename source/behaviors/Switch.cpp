
#include "Switch.hpp"

#include "Item.hpp"
#include "AvatarItem.hpp"
#include "Mediator.hpp"
#include "SoundManager.hpp"

#include <stack>

#include <algorithm> // std::find_if


namespace behaviors
{

Switch::Switch( const ItemPtr & item, const std::string & behavior )
        : Behavior( item, behavior )
        , isItemAbove( false )
{

}

Switch::~Switch( )
{

}

bool Switch::update ()
{
        Mediator * mediator = item->getMediator();
        std::vector< ItemPtr > itemsNearby ;

        switch ( activity )
        {
                case activities::Activity::Waiting:
                        // look if there’re items near the switch
                        if ( lookForItemsNearby( itemsNearby ) )
                        {
                                // when some item that made the switch is no longer near,
                                // or that item is near but it’s a character that is waiting,
                                // then remove such an item from the list of triggers so that it may re~switch
                                for ( size_t i = 0 ; i < triggers.size() ; i ++ )
                                {
                                        ItemPtr trigger = triggers[ i ];

                                        if ( std::find( itemsNearby.begin (), itemsNearby.end (), trigger ) == itemsNearby.end () ||
                                                ( trigger->whichItemClass() == "avatar item" && trigger->getBehavior()->getCurrentActivity() == activities::Activity::Waiting ) )
                                        {
                                                triggers.erase( std::remove( triggers.begin (), triggers.end (), trigger ), triggers.end () );
                                        }
                                }
                        }
                        else
                        {
                                triggers.clear ();
                        }

                        // look for items above
                        if ( ! item->canAdvanceTo( 0, 0, 1 ) )
                        {
                                // copy the stack of collisions
                                std::stack< std::string > aboveItems;
                                while ( ! mediator->isStackOfCollisionsEmpty() )
                                {
                                        aboveItems.push( mediator->popCollision() );
                                }

                                // as long as there’re elements above this switch
                                while ( ! aboveItems.empty() )
                                {
                                        ItemPtr itemAbove = mediator->findItemByUniqueName( aboveItems.top() );
                                        aboveItems.pop();

                                        // is it a free item
                                        if ( itemAbove != nilPointer &&
                                                ( itemAbove->whichItemClass() == "free item" || itemAbove->whichItemClass() == "avatar item" ) )
                                        {
                                                if ( itemAbove->getBehavior() != nilPointer &&
                                                        ! itemAbove->canAdvanceTo( 0, 0, -1 ) &&
                                                                // the switch doesn’t toggle when the character jumps
                                                                itemAbove->getBehavior()->getCurrentActivity() != activities::Activity::Jump )
                                                {
                                                        // toggle the switch when there’s only one item below the character
                                                        if ( ! isItemAbove && mediator->depthOfStackOfCollisions() <= 1 )
                                                        {
                                                                isItemAbove = true;

                                                                this->item->animate();

                                                                mediator->toggleSwitchInRoom();

                                                                // play the sound of switching
                                                                SoundManager::getInstance().play( item->getKind (), "switch" );
                                                        }
                                                }
                                        }
                                }
                        } else
                                isItemAbove = false;

                        break;

                case activities::Activity::PushedNorth:
                case activities::Activity::PushedSouth:
                case activities::Activity::PushedEast:
                case activities::Activity::PushedWest:
                case activities::Activity::PushedNortheast:
                case activities::Activity::PushedSoutheast:
                case activities::Activity::PushedSouthwest:
                case activities::Activity::PushedNorthwest:
                        if ( std::find( triggers.begin (), triggers.end (), affectedBy ) == /* not there */ triggers.end () )
                        {
                                triggers.push_back( affectedBy );
                                item->animate ();

                                mediator->toggleSwitchInRoom() ;

                                // play the sound of switching
                                SoundManager::getInstance().play( item->getKind (), "switch" );
                        }

                        activity = activities::Activity::Waiting;
                        break;

                default:
                        ;
        }

        return false;
}

bool Switch::lookForItemsNearby( std::vector< ItemPtr > & itemsNearby )
{
        Mediator* mediator = item->getMediator();

        // is there an item at north
        if ( ! item->canAdvanceTo( -1, 0, 0 ) )
        {
                while ( ! mediator->isStackOfCollisionsEmpty() )
                {
                        itemsNearby.push_back( mediator->findCollisionPop() );
                }
        }

        // is there an item at south
        if ( ! item->canAdvanceTo( 1, 0, 0 ) )
        {
                while ( ! mediator->isStackOfCollisionsEmpty() )
                {
                        itemsNearby.push_back( mediator->findCollisionPop() );
                }
        }

        // is there an item at east
        if ( ! item->canAdvanceTo( 0, -1, 0 ) )
        {
                while ( ! mediator->isStackOfCollisionsEmpty() )
                {
                        itemsNearby.push_back( mediator->findCollisionPop() );
                }
        }

        // is there an item at west
        if ( ! item->canAdvanceTo( 0, 1, 0 ) )
        {
                while ( ! mediator->isStackOfCollisionsEmpty() )
                {
                        itemsNearby.push_back( mediator->findCollisionPop() );
                }
        }

        return ! itemsNearby.empty() ;
}

}
