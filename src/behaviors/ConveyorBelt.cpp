
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
        , active( false )
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
                case Wait:
                        if ( speedTimer->getValue() > item->getSpeed() )
                        {
                                if ( ! item->checkPosition( 0, 0, 1, Add ) )
                                {
                                        std::stack< int > topItems;
                                        while ( ! mediator->isStackOfCollisionsEmpty() )
                                        {
                                                topItems.push( mediator->popCollision() );
                                        }

                                        // check conditions as long as there are items on top
                                        while ( ! topItems.empty () )
                                        {
                                                int top = topItems.top();
                                                topItems.pop();

                                                // is it free item
                                                if ( top >= FirstFreeId && ( top & 1 ) )
                                                {
                                                        FreeItem* topItem = dynamic_cast< FreeItem * >( mediator->findItemById( top ) );

                                                        // is it item with behavior
                                                        if ( topItem != nilPointer && topItem->getBehavior() != nilPointer )
                                                        {
                                                                if ( topItem->getAnchor() == nilPointer || item->getId() == topItem->getAnchor()->getId() )
                                                                {
                                                                        if ( item->getOrientation().toString() == "south" ) {
                                                                                if ( topItem->getBehavior()->getActivityOfItem() != RegularJump && topItem->getBehavior()->getActivityOfItem() != HighJump ) {
                                                                                        topItem->getBehavior()->changeActivityOfItem( ForceDisplaceSouth );
                                                                                }
                                                                        } else if ( item->getOrientation().toString() == "west" ) {
                                                                                if ( topItem->getBehavior()->getActivityOfItem() != RegularJump && topItem->getBehavior()->getActivityOfItem() != HighJump ) {
                                                                                        topItem->getBehavior()->changeActivityOfItem( ForceDisplaceWest );
                                                                                }
                                                                        } else if ( item->getOrientation().toString() == "north" ) {
                                                                                if ( topItem->getBehavior()->getActivityOfItem() != RegularJump && topItem->getBehavior()->getActivityOfItem() != HighJump ) {
                                                                                        topItem->getBehavior()->changeActivityOfItem( ForceDisplaceNorth );
                                                                                }
                                                                        } else if ( item->getOrientation().toString() == "east" ) {
                                                                                if ( topItem->getBehavior()->getActivityOfItem() != RegularJump && topItem->getBehavior()->getActivityOfItem() != HighJump ) {
                                                                                        topItem->getBehavior()->changeActivityOfItem( ForceDisplaceEast );
                                                                                }
                                                                        }

                                                                        // play sound of conveyor belt
                                                                        SoundManager::getInstance()->play( item->getLabel(), IsActive );
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
                        activity = Wait;
        }

        return false;
}

}
