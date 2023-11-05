
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
        std::vector< ItemPtr > sideItems;

        switch ( activity )
        {
                case activities::Activity::Wait:
                        // look if there’re items on sides
                        if ( checkSideItems( sideItems ) )
                        {
                                // when some item that made switch is no longer near,
                                // or that item is near but it’s a character which yet waits,
                                // then remove such item from list of triggers so that it may re~switch
                                for ( size_t i = 0; i < triggerItems.size(); i++ )
                                {
                                        ItemPtr trigger = triggerItems[ i ];

                                        if ( std::find( sideItems.begin (), sideItems.end (), trigger ) == sideItems.end () ||
                                                ( trigger->whichItemClass() == "avatar item" && trigger->getBehavior()->getActivityOfItem() == activities::Activity::Wait ) )
                                        {
                                                triggerItems.erase( std::remove( triggerItems.begin (), triggerItems.end (), trigger ), triggerItems.end () );
                                        }
                                }
                        }
                        else
                        {
                                triggerItems.clear();
                        }

                        // look for items above
                        if ( ! item->canAdvanceTo( 0, 0, 1 ) )
                        {
                                // copy stack of collisions
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

                                        // is it free item
                                        if ( itemAbove != nilPointer &&
                                                ( itemAbove->whichItemClass() == "free item" || itemAbove->whichItemClass() == "avatar item" ) )
                                        {
                                                if ( itemAbove->getBehavior() != nilPointer &&
                                                        ! itemAbove->canAdvanceTo( 0, 0, -1 ) &&
                                                                // yep, the switch doesn’t toggle when the character jumps
                                                                itemAbove->getBehavior()->getActivityOfItem() != activities::Activity::RegularJump &&
                                                                itemAbove->getBehavior()->getActivityOfItem() != activities::Activity::HighJump )
                                                {
                                                        // when there’s no more than one item below initiator of switch
                                                        if ( ! isItemAbove && mediator->depthOfStackOfCollisions() <= 1 )
                                                        {
                                                                isItemAbove = true;

                                                                this->item->animate();

                                                                mediator->toggleSwitchInRoom();

                                                                // play sound of switching
                                                                SoundManager::getInstance().play( item->getKind (), activities::Activity::SwitchIt );
                                                        }
                                                }
                                        }
                                }
                        } else
                                isItemAbove = false;

                        break;

                case activities::Activity::DisplaceNorth:
                case activities::Activity::DisplaceSouth:
                case activities::Activity::DisplaceEast:
                case activities::Activity::DisplaceWest:
                case activities::Activity::DisplaceNortheast:
                case activities::Activity::DisplaceSoutheast:
                case activities::Activity::DisplaceSouthwest:
                case activities::Activity::DisplaceNorthwest:
                        if ( std::find( triggerItems.begin (), triggerItems.end (), sender ) == triggerItems.end () )
                        {
                                triggerItems.push_back( sender );
                                item->animate();

                                mediator->toggleSwitchInRoom();

                                // play sound of switching
                                SoundManager::getInstance().play( item->getKind (), activities::Activity::SwitchIt );
                        }

                        activity = activities::Activity::Wait;
                        break;

                default:
                        ;
        }

        return false;
}

bool Switch::checkSideItems( std::vector< ItemPtr >& sideItems )
{
        Mediator* mediator = item->getMediator();

        // is there item at north
        if ( ! item->canAdvanceTo( -1, 0, 0 ) )
        {
                while ( ! mediator->isStackOfCollisionsEmpty() )
                {
                        sideItems.push_back( mediator->findCollisionPop() );
                }
        }

        // is there item at south
        if ( ! item->canAdvanceTo( 1, 0, 0 ) )
        {
                while ( ! mediator->isStackOfCollisionsEmpty() )
                {
                        sideItems.push_back( mediator->findCollisionPop() );
                }
        }

        // is there item at east
        if ( ! item->canAdvanceTo( 0, -1, 0 ) )
        {
                while ( ! mediator->isStackOfCollisionsEmpty() )
                {
                        sideItems.push_back( mediator->findCollisionPop() );
                }
        }

        // is there item at west
        if ( ! item->canAdvanceTo( 0, 1, 0 ) )
        {
                while ( ! mediator->isStackOfCollisionsEmpty() )
                {
                        sideItems.push_back( mediator->findCollisionPop() );
                }
        }

        return ! sideItems.empty();
}

}
