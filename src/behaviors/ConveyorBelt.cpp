
#include "ConveyorBelt.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Mediator.hpp"
#include "SoundManager.hpp"


namespace isomot
{

ConveyorBelt::ConveyorBelt( Item* item, const std::string& behavior )
        : Behavior( item, behavior )
{
        speedTimer = new Timer();
        speedTimer->go();

        animationTimer = new Timer();
        animationTimer->go();
}

ConveyorBelt::~ConveyorBelt( )
{
        delete speedTimer;
        delete animationTimer;
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
                                                Item* collision = mediator->findItemByUniqueName( topItems.top() );
                                                topItems.pop();

                                                // is it free item
                                                if ( collision != nilPointer &&
                                                        ( collision->whichKindOfItem() == "free item" || collision->whichKindOfItem() == "player item" ) )
                                                {
                                                        FreeItem* itemAbove = dynamic_cast< FreeItem * >( collision );

                                                        // is it item with behavior
                                                        if ( itemAbove->getBehavior() != nilPointer )
                                                        {
                                                                if ( itemAbove->getAnchor() == nilPointer || this->item == itemAbove->getAnchor() )
                                                                {
                                                                        if ( item->getOrientation().toString() == "south" ) {
                                                                                if ( itemAbove->getBehavior()->getActivityOfItem() != Activity::RegularJump &&
                                                                                                itemAbove->getBehavior()->getActivityOfItem() != Activity::HighJump )
                                                                                {
                                                                                        itemAbove->getBehavior()->changeActivityOfItem( Activity::ForceDisplaceSouth );
                                                                                }
                                                                        } else if ( item->getOrientation().toString() == "west" ) {
                                                                                if ( itemAbove->getBehavior()->getActivityOfItem() != Activity::RegularJump &&
                                                                                                itemAbove->getBehavior()->getActivityOfItem() != Activity::HighJump )
                                                                                {
                                                                                        itemAbove->getBehavior()->changeActivityOfItem( Activity::ForceDisplaceWest );
                                                                                }
                                                                        } else if ( item->getOrientation().toString() == "north" ) {
                                                                                if ( itemAbove->getBehavior()->getActivityOfItem() != Activity::RegularJump &&
                                                                                                itemAbove->getBehavior()->getActivityOfItem() != Activity::HighJump )
                                                                                {
                                                                                        itemAbove->getBehavior()->changeActivityOfItem( Activity::ForceDisplaceNorth );
                                                                                }
                                                                        } else if ( item->getOrientation().toString() == "east" ) {
                                                                                if ( itemAbove->getBehavior()->getActivityOfItem() != Activity::RegularJump &&
                                                                                                itemAbove->getBehavior()->getActivityOfItem() != Activity::HighJump )
                                                                                {
                                                                                        itemAbove->getBehavior()->changeActivityOfItem( Activity::ForceDisplaceEast );
                                                                                }
                                                                        }

                                                                        // play sound of conveyor belt
                                                                        SoundManager::getInstance()->play( item->getLabel(), Activity::IsActive );
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
