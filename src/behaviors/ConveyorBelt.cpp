
#include "ConveyorBelt.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Mediator.hpp"
#include "SoundManager.hpp"


namespace iso
{

ConveyorBelt::ConveyorBelt( const ItemPtr & item, const std::string& behavior )
        : Behavior( item, behavior )
        , speedTimer( new Timer() )
        , animationTimer( new Timer() )
{
        speedTimer->go();
        animationTimer->go();
}

ConveyorBelt::~ConveyorBelt( )
{
}

bool ConveyorBelt::update ()
{
        Mediator* mediator = item->getMediator();

        switch ( activity )
        {
                case Activity::Wait:
                        if ( speedTimer->getValue() > item->getSpeed() )
                        {
                                if ( ! item->canAdvanceTo( 0, 0, 1 ) )
                                {
                                        std::stack< std::string > topItems;
                                        while ( ! mediator->isStackOfCollisionsEmpty() )
                                        {
                                                topItems.push( mediator->popCollision() );
                                        }

                                        // check conditions as long as there are items on top
                                        while ( ! topItems.empty () )
                                        {
                                                ItemPtr collision = mediator->findItemByUniqueName( topItems.top() );
                                                topItems.pop();

                                                // is it free item
                                                if ( collision != nilPointer &&
                                                        ( collision->whichKindOfItem() == "free item" || collision->whichKindOfItem() == "player item" ) )
                                                {
                                                        FreeItem& itemAbove = dynamic_cast< FreeItem& >( *collision );

                                                        // is it item with behavior
                                                        if ( itemAbove.getBehavior() != nilPointer )
                                                        {
                                                                if ( ! itemAbove.getAnchor().empty() || this->item->getUniqueName() == itemAbove.getAnchor() )
                                                                {
                                                                        if ( item->getOrientation() == "south" ) {
                                                                                if ( itemAbove.getBehavior()->getActivityOfItem() != Activity::RegularJump &&
                                                                                                itemAbove.getBehavior()->getActivityOfItem() != Activity::HighJump &&
                                                                                                itemAbove.getBehavior()->getActivityOfItem() != Activity::Vanish )
                                                                                {
                                                                                        itemAbove.getBehavior()->changeActivityOfItem( Activity::ForceDisplaceSouth );
                                                                                }
                                                                        } else if ( item->getOrientation() == "west" ) {
                                                                                if ( itemAbove.getBehavior()->getActivityOfItem() != Activity::RegularJump &&
                                                                                                itemAbove.getBehavior()->getActivityOfItem() != Activity::HighJump &&
                                                                                                itemAbove.getBehavior()->getActivityOfItem() != Activity::Vanish )
                                                                                {
                                                                                        itemAbove.getBehavior()->changeActivityOfItem( Activity::ForceDisplaceWest );
                                                                                }
                                                                        } else if ( item->getOrientation() == "north" ) {
                                                                                if ( itemAbove.getBehavior()->getActivityOfItem() != Activity::RegularJump &&
                                                                                                itemAbove.getBehavior()->getActivityOfItem() != Activity::HighJump &&
                                                                                                itemAbove.getBehavior()->getActivityOfItem() != Activity::Vanish )
                                                                                {
                                                                                        itemAbove.getBehavior()->changeActivityOfItem( Activity::ForceDisplaceNorth );
                                                                                }
                                                                        } else if ( item->getOrientation() == "east" ) {
                                                                                if ( itemAbove.getBehavior()->getActivityOfItem() != Activity::RegularJump &&
                                                                                                itemAbove.getBehavior()->getActivityOfItem() != Activity::HighJump &&
                                                                                                itemAbove.getBehavior()->getActivityOfItem() != Activity::Vanish )
                                                                                {
                                                                                        itemAbove.getBehavior()->changeActivityOfItem( Activity::ForceDisplaceEast );
                                                                                }
                                                                        }

                                                                        // play sound of conveyor belt
                                                                        SoundManager::getInstance().play( item->getLabel(), Activity::IsActive );
                                                                }
                                                        }
                                                }
                                        }
                                }

                                speedTimer->reset();
                        }

                        if ( animationTimer->getValue() > 2 * item->getSpeed() )
                        {
                                item->animate ();
                                animationTimer->reset();
                        }

                        break;

                default:
                        activity = Activity::Wait;
        }

        return false;
}

}
