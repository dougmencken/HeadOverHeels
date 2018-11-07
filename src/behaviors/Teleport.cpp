
#include "Teleport.hpp"
#include "Item.hpp"
#include "GridItem.hpp"
#include "Mediator.hpp"
#include "SoundManager.hpp"

#include <stack>


namespace iso
{

Teleport::Teleport( const ItemPtr & item, const std::string & behavior ) :
        Behavior( item, behavior ),
        activated( false )
{

}

Teleport::~Teleport()
{

}

bool Teleport::update ()
{
        Mediator * mediator = item->getMediator();

        switch ( activity )
        {
                case Activity::Wait:
                        // is there items above
                        if ( ! item->canAdvanceTo( 0, 0, 1 ) )
                        {
                                // copy stack of collisions
                                std::stack< std::string > topItems;
                                while ( ! mediator->isStackOfCollisionsEmpty() )
                                {
                                        topItems.push( mediator->popCollision() );
                                }

                                // as long as there are items above teleport
                                while ( ! topItems.empty() )
                                {
                                        ItemPtr aboveItem = mediator->findItemByUniqueName( topItems.top() );
                                        topItems.pop();

                                        // is it free item with behavior
                                        if ( aboveItem != nilPointer &&
                                                ( aboveItem->whichKindOfItem() == "free item" || aboveItem->whichKindOfItem() == "player item" ) &&
                                                        aboveItem->getBehavior() != nilPointer )
                                        {
                                                // look for items below
                                                if ( ! aboveItem->canAdvanceTo( 0, 0, -1 ) )
                                                {
                                                        bool playerAboveTeleport = false;

                                                        while ( ! mediator->isStackOfCollisionsEmpty() )
                                                        {
                                                                ItemPtr belowItem = mediator->findCollisionPop( );

                                                                if ( aboveItem->whichKindOfItem() == "player item" && belowItem == this->item )
                                                                {
                                                                        playerAboveTeleport = true;
                                                                        break;
                                                                }
                                                        }

                                                        activated = playerAboveTeleport;
                                                }
                                        }
                                }
                        }
                        else
                        {
                                activated = false;
                        }

                        // animate activated teleport
                        if ( activated )
                        {
                                item->animate();
                                SoundManager::getInstance().play( item->getLabel(), Activity::IsActive );
                        }
                        break;

                default:
                        activity = Activity::Wait;
        }

        return false;
}

}
