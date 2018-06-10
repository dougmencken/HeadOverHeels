
#include "Switch.hpp"
#include "Item.hpp"
#include "PlayerItem.hpp"
#include "Mediator.hpp"
#include "SoundManager.hpp"

#include <algorithm>  // std::find_if


namespace isomot
{

Switch::Switch( Item * item, const std::string & behavior )
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
        std::vector< Item * > sideItems;

        switch ( activity )
        {
                case Wait:
                        // look if there’re items on sides
                        if ( checkSideItems( sideItems ) )
                        {
                                // when some item that made switch is no longer near,
                                // or that item is near but it’s a character which yet waits,
                                // then remove such item from list of triggers so that it may re~switch
                                for ( size_t i = 0; i < triggerItems.size(); i++ )
                                {
                                        Item* trigger = triggerItems[ i ];

                                        if ( std::find( sideItems.begin (), sideItems.end (), trigger ) == sideItems.end () ||
                                                ( trigger->whichKindOfItem() == "player item" && trigger->getBehavior()->getActivityOfItem() == Wait ) )
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
                                        Item* itemAbove = mediator->findItemByUniqueName( aboveItems.top() );
                                        aboveItems.pop();

                                        // is it free item
                                        if ( itemAbove != nilPointer &&
                                                ( itemAbove->whichKindOfItem() == "free item" || itemAbove->whichKindOfItem() == "player item" ) )
                                        {
                                                if ( itemAbove->getBehavior() != nilPointer &&
                                                        ! itemAbove->canAdvanceTo( 0, 0, -1 ) &&
                                                                // yep, switch doesn’t toggle when player jumps
                                                                itemAbove->getBehavior()->getActivityOfItem() != RegularJump &&
                                                                itemAbove->getBehavior()->getActivityOfItem() != HighJump )
                                                {
                                                        // when there’s no more than one item below initiator of switch
                                                        if ( ! isItemAbove && mediator->depthOfStackOfCollisions() <= 1 )
                                                        {
                                                                isItemAbove = true;

                                                                this->item->animate();

                                                                mediator->toggleSwitchInRoom();

                                                                // play sound of switching
                                                                SoundManager::getInstance()->play( item->getLabel(), SwitchIt );
                                                        }
                                                }
                                        }
                                }
                        }
                        else
                        {
                                isItemAbove = false;
                        }
                        break;

                case DisplaceNorth:
                case DisplaceSouth:
                case DisplaceEast:
                case DisplaceWest:
                case DisplaceNortheast:
                case DisplaceSoutheast:
                case DisplaceSouthwest:
                case DisplaceNorthwest:
                        if ( std::find( triggerItems.begin (), triggerItems.end (), sender ) == triggerItems.end () )
                        {
                                triggerItems.push_back( sender );
                                item->animate();

                                mediator->toggleSwitchInRoom();

                                // play sound of switching
                                SoundManager::getInstance()->play( item->getLabel(), SwitchIt );
                        }

                        activity = Wait;
                        break;

                default:
                        ;
        }

        return false;
}

bool Switch::checkSideItems( std::vector< Item * >& sideItems )
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
