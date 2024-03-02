
#include "Teleport.hpp"

#include "Item.hpp"
#include "GridItem.hpp"
#include "Mediator.hpp"
#include "SoundManager.hpp"

#include <stack>


namespace behaviors
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
                case activities::Activity::Waiting:
                        // is there items above
                        if ( ! item->canAdvanceTo( 0, 0, 1 ) )
                        {
                                // copy stack of collisions
                                std::stack< std::string > topItems;
                                while ( mediator->isThereAnyCollision() )
                                {
                                        topItems.push( mediator->popCollision() );
                                }

                                // as long as there are items above teleport
                                while ( ! topItems.empty() )
                                {
                                        ItemPtr aboveItem = mediator->findItemByUniqueName( topItems.top() );
                                        topItems.pop();

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

                                                                if ( aboveItem->whichItemClass() == "avatar item" && belowItem == this->item )
                                                                {
                                                                        characterIsAboveTeleport = true ;
                                                                        break;
                                                                }
                                                        }

                                                        activated = characterIsAboveTeleport ;
                                                }
                                        }
                                }
                        }
                        else
                        {
                                activated = false;
                        }

                        if ( activated ) // animate activated teleport
                        {
                                item->animate ();
                                SoundManager::getInstance().play( item->getKind (), "function" );
                        }
                        break;

                default:
                        activity = activities::Activity::Waiting;
        }

        return false;
}

}
