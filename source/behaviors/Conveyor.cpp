
#include "Conveyor.hpp"

#include "FreeItem.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"
#include "Mediator.hpp"
#include "SoundManager.hpp"

#include <stack>


namespace behaviors
{

Conveyor::Conveyor( Item & item, const std::string & behavior )
        : Behavior( item, behavior )
        , speedTimer( new Timer() )
        , animationTimer( new Timer() )
{
        speedTimer->go();
        animationTimer->go();
}

bool Conveyor::update_returningdisappearance ()
{
        Item & conveyorItem = getItem() ;
        Mediator * mediator = conveyorItem.getMediator ();

        switch ( getCurrentActivity() )
        {
                case activities::Activity::Waiting:
                        if ( speedTimer->getValue() > conveyorItem.getSpeed() )
                        {
                                if ( ! conveyorItem.canAdvanceTo( 0, 0, 1 ) )
                                {
                                        // copy the stack of collisions
                                        std::stack< std::string > itemsAbove ;
                                        while ( mediator->isThereAnyCollision() )
                                                itemsAbove.push( mediator->popCollision() );

                                        while ( ! itemsAbove.empty () ) // for each such item
                                        {
                                                ItemPtr collision = mediator->findItemByUniqueName( itemsAbove.top() );
                                                itemsAbove.pop() ;

                                                // is it free item
                                                if ( collision != nilPointer &&
                                                        ( collision->whichItemClass() == "free item" || collision->whichItemClass() == "avatar item" ) )
                                                {
                                                        FreeItem & itemAbove = dynamic_cast< FreeItem & >( *collision );

                                                        if ( /* the carrier is this conveyor */ conveyorItem.getUniqueName() == itemAbove.getCarrier()
                                                                        || /* or any other carrying item */ ! itemAbove.getCarrier().empty() )
                                                        {
                                                                if ( itemAbove.getBehavior() != nilPointer )
                                                                {
                                                                        Activity activityOfItemAbove = itemAbove.getBehavior()->getCurrentActivity() ;
                                                                        bool outOfGravity = ( activityOfItemAbove == activities::Activity::Jumping
                                                                                                  || activityOfItemAbove == activities::Activity::Vanishing ) ;

                                                                        if ( ! outOfGravity ) {
                                                                                if ( conveyorItem.getHeading() == "south" )
                                                                                        itemAbove.getBehavior()->setCurrentActivity( activities::Activity::DraggedSouth );
                                                                                else if ( conveyorItem.getHeading() == "west" )
                                                                                        itemAbove.getBehavior()->setCurrentActivity( activities::Activity::DraggedWest );
                                                                                else if ( conveyorItem.getHeading() == "north" )
                                                                                        itemAbove.getBehavior()->setCurrentActivity( activities::Activity::DraggedNorth );
                                                                                else if ( conveyorItem.getHeading() == "east" )
                                                                                        itemAbove.getBehavior()->setCurrentActivity( activities::Activity::DraggedEast );
                                                                        }

                                                                        // play the sound of conveyor
                                                                        SoundManager::getInstance().play( conveyorItem.getKind (), "function" );
                                                                }
                                                        }
                                                }
                                        }
                                }

                                speedTimer->reset();
                        }

                        if ( animationTimer->getValue() > 2 * conveyorItem.getSpeed() )
                        {
                                conveyorItem.animate ();
                                animationTimer->reset();
                        }

                        break;

                default:
                        setCurrentActivity( activities::Activity::Waiting );
        }

        // conveyors are eternal
        return false ;
}

}
