
#include "Teleport.hpp"

#include "GridItem.hpp"
#include "Mediator.hpp"
#include "SoundManager.hpp"

#include <stack>


namespace behaviors
{

Teleport::Teleport( GridItem & item, const std::string & behavior ) :
        Behavior( item, behavior ),
        activated( false )
{
}

bool Teleport::update ()
{
        Item & teleportItem = getItem ();
        Mediator * mediator = teleportItem.getMediator() ;

        switch ( getCurrentActivity () )
        {
                case activities::Activity::Waiting:
                        // is there items above
                        if ( ! teleportItem.canAdvanceTo( 0, 0, 1 ) )
                        {
                                // copy the stack of collisions
                                std::stack< std::string > itemsAbove ;
                                while ( mediator->isThereAnyCollision() )
                                        itemsAbove.push( mediator->popCollision() );

                                // as long as there are items above the teletransport
                                while ( ! itemsAbove.empty() )
                                {
                                        ItemPtr aboveItem = mediator->findItemByUniqueName( itemsAbove.top() );
                                        itemsAbove.pop();

                                        // is it a free item with behavior
                                        if ( aboveItem != nilPointer &&
                                                ( aboveItem->whichItemClass() == "free item" || aboveItem->whichItemClass() == "avatar item" ) &&
                                                        aboveItem->getBehavior() != nilPointer )
                                        {
                                                // look for the items below
                                                if ( ! aboveItem->canAdvanceTo( 0, 0, -1 ) )
                                                {
                                                        bool characterIsAboveTeleport = false ;

                                                        while ( mediator->isThereAnyCollision() )
                                                        {
                                                                ItemPtr belowItem = mediator->findCollisionPop( );

                                                                if ( aboveItem->whichItemClass() == "avatar item"
                                                                        && belowItem->getUniqueName() == teleportItem.getUniqueName() )
                                                                {
                                                                        characterIsAboveTeleport = true ;
                                                                        break;
                                                                }
                                                        }

                                                        this->activated = characterIsAboveTeleport ;
                                                }
                                        }
                                }
                        } else
                                this->activated = false ;

                        if ( this->activated ) {
                                // animate activated teleport
                                teleportItem.animate ();
                                SoundManager::getInstance().play( teleportItem.getKind (), "function" );
                        }

                        break;

                default:
                        setCurrentActivity( activities::Activity::Waiting );
        }

        // teleports are eternal
        return true ;
}

}
