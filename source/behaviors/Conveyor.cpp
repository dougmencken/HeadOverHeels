
#include "Conveyor.hpp"

#include "GridItem.hpp"
#include "FreeItem.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"
#include "Mediator.hpp"
#include "SoundManager.hpp"

#include "ActivityToString.hpp"

#include <stack>


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
        Item & conveyorItem = getItem() ;
        Mediator * mediator = conveyorItem.getMediator ();

        switch ( getCurrentActivity() )
        {
                case activities::Activity::Waiting:
                        if ( dragTimer->getValue() < conveyorItem.getSpeed() ) break ;

                        if ( ! conveyorItem.canAdvanceTo( 0, 0, 1 ) )
                        {
                                // collect collisions
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
                                                                std::cout << "\"" << itemAbove.getUniqueName() << "\" is above a conveyor :" ;
                                                                std::cout << " activity before is \"" << activities::ActivityToString::toString( activityOfItemAbove ) << "\"" ;

                                                                bool outOfGravity = ( activityOfItemAbove == activities::Activity::Jumping
                                                                                                || activityOfItemAbove == activities::Activity::Vanishing ) ;
                                                                if ( ! outOfGravity )
                                                                {
                                                                        if ( conveyorItem.getHeading() == "south" ) {
                                                                                if ( activityOfItemAbove == activities::Activity::PushedEast )
                                                                                        behaviorAbove.setCurrentActivity( activities::Activity::PushedSoutheast );
                                                                                else
                                                                                if ( activityOfItemAbove == activities::Activity::PushedWest )
                                                                                        behaviorAbove.setCurrentActivity( activities::Activity::PushedSouthwest );
                                                                                else
                                                                                        behaviorAbove.setCurrentActivity( activities::Activity::DraggedSouth );
                                                                        } else if ( conveyorItem.getHeading() == "west" ) {
                                                                                if ( activityOfItemAbove == activities::Activity::PushedNorth )
                                                                                        behaviorAbove.setCurrentActivity( activities::Activity::PushedNorthwest );
                                                                                else
                                                                                if ( activityOfItemAbove == activities::Activity::PushedSouth )
                                                                                        behaviorAbove.setCurrentActivity( activities::Activity::PushedSouthwest );
                                                                                else
                                                                                        behaviorAbove.setCurrentActivity( activities::Activity::DraggedWest );
                                                                        } else if ( conveyorItem.getHeading() == "north" ) {
                                                                                if ( activityOfItemAbove == activities::Activity::PushedEast )
                                                                                        behaviorAbove.setCurrentActivity( activities::Activity::PushedNortheast );
                                                                                else
                                                                                if ( activityOfItemAbove == activities::Activity::PushedWest )
                                                                                        behaviorAbove.setCurrentActivity( activities::Activity::PushedNorthwest );
                                                                                else
                                                                                        behaviorAbove.setCurrentActivity( activities::Activity::DraggedNorth );
                                                                        } else if ( conveyorItem.getHeading() == "east" ) {
                                                                                if ( activityOfItemAbove == activities::Activity::PushedNorth )
                                                                                        behaviorAbove.setCurrentActivity( activities::Activity::PushedNortheast );
                                                                                else
                                                                                if ( activityOfItemAbove == activities::Activity::PushedSouth )
                                                                                        behaviorAbove.setCurrentActivity( activities::Activity::PushedSoutheast );
                                                                                else
                                                                                        behaviorAbove.setCurrentActivity( activities::Activity::DraggedEast );
                                                                        }
                                                                }

                                                                if ( activityOfItemAbove != behaviorAbove.getCurrentActivity() ) {
                                                                        std::cout << ", changed to \""
                                                                                        << activities::ActivityToString::toString( behaviorAbove.getCurrentActivity() ) << "\"" ;
                                                                }
                                                                std::cout << std::endl ;

                                                                // play the sound of conveyor
                                                                SoundManager::getInstance().play( conveyorItem.getKind (), "function" );
                                                        }
                                                }
                                        }
                                }
                        }

                        dragTimer->go() ;

                        if ( animationTimer->getValue() > 2 * conveyorItem.getSpeed() )
                        {
                                conveyorItem.animate ();
                                animationTimer->go() ;
                        }

                        break;

                default:
                        setCurrentActivity( activities::Activity::Waiting );
        }

        // conveyors are eternal
        return true ;
}

}
