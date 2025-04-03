
#include "Conveyor.hpp"

#include "GridItem.hpp"
#include "FreeItem.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"
#include "Mediator.hpp"
#include "SoundManager.hpp"

#include "ActivityToString.hpp"

#include <stack>

#ifdef DEBUG
#  define DEBUG_CONVEYOR        1
#endif


namespace behaviors
{

Conveyor::Conveyor( GridItem & item, const std::string & behavior )
        : Behavior( item, behavior )
        , dragTimer( new Timer() )
        , animationTimer( new Timer() )
{
        dragTimer->go() ;
        animationTimer->go() ;
}

bool Conveyor::update ()
{
        GridItem & conveyorItem = dynamic_cast< GridItem & >( getItem() );
        Mediator * mediator = conveyorItem.getMediator ();

        if ( getCurrentActivity() != activities::Activity::Waiting ) setCurrentActivity( activities::Activity::Waiting );

        if ( dragTimer->getValue() >= conveyorItem.getSpeed() )
        {
                if ( ! conveyorItem.canAdvanceTo( 0, 0, 1 ) )
                {
                        // collect collisions
                        std::stack< std::string > itemsAbove ;
                        while ( mediator->isThereAnyCollision() )
                                itemsAbove.push( mediator->popCollision() );

                        while ( ! itemsAbove.empty () ) // for each such item
                        {
                                DescribedItemPtr collision = mediator->findItemByUniqueName( itemsAbove.top() );
                                itemsAbove.pop() ;

                                // is it free item
                                if ( collision != nilPointer &&
                                        ( collision->whichItemClass() == "free item" || collision->whichItemClass() == "avatar item" ) )
                                {
                                        FreeItem & itemAbove = dynamic_cast< FreeItem & >( *collision );

                                        ////////int conveyorNorthX = conveyorItem.getX ();
                                        ////////int conveyorWestY  = conveyorItem.getY ();

                                        /////////int aboveItemNorthX = itemAbove.getX ();
                                        /////////int aboveItemWestY  = itemAbove.getY ();

                                        if ( /* the carrier is this conveyor */ conveyorItem.getUniqueName() == itemAbove.getCarrier()
                                                        || /* or any other carrying item */ ! itemAbove.getCarrier().empty() )
                                        {
                                                if ( itemAbove.getBehavior() != nilPointer )
                                                {
                                                        Behavior & behaviorAbove = * itemAbove.getBehavior() ;
                                                        Activity activityOfItemAbove = behaviorAbove.getCurrentActivity() ;
                                                        Motion2D velocityOfItemAbove = behaviorAbove.get2DVelocityVector() ;

                                                        bool outOfGravity = ( activityOfItemAbove == activities::Activity::Jumping
                                                                                        || activityOfItemAbove == activities::Activity::Vanishing ) ;
                                                        if ( ! outOfGravity )
                                                        {
                                                                const std::string & conveyorHeading = conveyorItem.getOrientation() ;
                                                                Motion2D conveyingVelocity = Motion2D::rest() ;

                                                                if ( conveyorHeading == "south" )
                                                                        conveyingVelocity = Motion2D::movingSouth() ;
                                                                else if ( conveyorHeading == "north" )
                                                                        conveyingVelocity = Motion2D::movingNorth() ;
                                                                else if ( conveyorHeading == "west" )
                                                                        conveyingVelocity = Motion2D::movingWest() ;
                                                                else if ( conveyorHeading == "east" )
                                                                        conveyingVelocity = Motion2D::movingEast() ;

                                                        #if defined( DEBUG_CONVEYOR ) && DEBUG_CONVEYOR
                                                                std::cout << "Conveyor::update() sums conveying velocity " << conveyingVelocity.toString()
                                                                                << " with the velocity " << velocityOfItemAbove.toString()
                                                                                << " of above item \"" << itemAbove.getUniqueName() << "\"" << std::endl ;
                                                        #endif
                                                                Motion2D sumOfVelocities = velocityOfItemAbove.add( conveyingVelocity ) ;

                                                                activityOfItemAbove = activities::Activity::Dragged ;
                                                                behaviorAbove.setCurrentActivity( activityOfItemAbove, sumOfVelocities );
                                                        }

                                                        // play the sound of conveyor
                                                        SoundManager::getInstance().play( conveyorItem.getKind (), "function" );
                                                }
                                        }
                                }
                        }
                }

                dragTimer->go() ;
        }

        if ( animationTimer->getValue() > 2 * conveyorItem.getSpeed() )
        {
                conveyorItem.animate ();
                animationTimer->go() ;
        }

        // conveyors are eternal
        return true ;
}

}
