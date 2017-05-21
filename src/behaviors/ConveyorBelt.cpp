
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

ConveyorBelt::ConveyorBelt( Item* item, const BehaviorOfItem& behavior )
        : Behavior( item, behavior )
        , active( false )
{
        speedTimer = new HPC();
        speedTimer->start();
}

ConveyorBelt::~ConveyorBelt( )
{
        delete speedTimer;
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
                                        // Copia la pila de colisiones
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

                                                // El elemento tiene que ser un elemento libre
                                                if ( top >= FirstFreeId && ( top & 1 ) )
                                                {
                                                        FreeItem* topItem = dynamic_cast< FreeItem * >( mediator->findItemById( top ) );

                                                        // El elemento debe tener comportamiento
                                                        if ( topItem != 0 && topItem->getBehavior() != 0 )
                                                        {
                                                                // El ancla del elemento debe ser esta cinta transportadora para proceder a arrastrarlo
                                                                if ( topItem->getAnchor() == 0 || item->getId() == topItem->getAnchor()->getId() )
                                                                {
                                                                        if ( this->theBehavior == ConveyorBeltNortheast )
                                                                        {
                                                                                if ( item->getDirection() == South )
                                                                                {
                                                                                        if ( topItem->getBehavior()->getActivityOfItem() != RegularJump && topItem->getBehavior()->getActivityOfItem() != HighJump )
                                                                                        {
                                                                                                topItem->getBehavior()->changeActivityOfItem( ForceDisplaceNorth );
                                                                                        }
                                                                                }
                                                                                else if ( item->getDirection() == West )
                                                                                {
                                                                                        if ( topItem->getBehavior()->getActivityOfItem() != RegularJump && topItem->getBehavior()->getActivityOfItem() != HighJump )
                                                                                        {
                                                                                                topItem->getBehavior()->changeActivityOfItem( ForceDisplaceEast );
                                                                                        }
                                                                                }
                                                                        }
                                                                        if ( this->theBehavior == ConveyorBeltSouthwest )
                                                                        {
                                                                                if ( item->getDirection() == South )
                                                                                {
                                                                                        if ( topItem->getBehavior()->getActivityOfItem() != RegularJump && topItem->getBehavior()->getActivityOfItem() != HighJump )
                                                                                        {
                                                                                                topItem->getBehavior()->changeActivityOfItem( ForceDisplaceSouth );
                                                                                        }
                                                                                }
                                                                                else if ( item->getDirection() == West )
                                                                                {
                                                                                        if ( topItem->getBehavior()->getActivityOfItem() != RegularJump && topItem->getBehavior()->getActivityOfItem() != HighJump )
                                                                                        {
                                                                                                topItem->getBehavior()->changeActivityOfItem( ForceDisplaceWest );
                                                                                        }
                                                                                }
                                                                        }

                                                                        // play sound of conveyor belt
                                                                        SoundManager::getInstance()->play( item->getLabel(), IsActive );
                                                                }
                                                        }
                                                }
                                        }
                                }

                                // Reinicia el cronómetro para el siguiente ciclo
                                speedTimer->reset();
                        }
                        break;

                default:
                        activity = Wait;
        }

        return false;
}

}
