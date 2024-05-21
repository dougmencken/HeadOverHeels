
#include "FinalBall.hpp"

#include "FreeItem.hpp"
#include "Mediator.hpp"
#include "Moving.hpp"
#include "Mediator.hpp"
#include "Room.hpp"


namespace behaviors
{

FinalBall::FinalBall( FreeItem & item, const std::string & behavior )
        : Behavior( item, behavior )
        , speedTimer( new Timer() )
{
        speedTimer->go();
}

bool FinalBall::update ()
{
        Item & thisBall = getItem() ;

        switch ( getCurrentActivity () )
        {
                case activities::Activity::Waiting:
                        setCurrentActivity( activities::Activity::MovingNorth );
                        break;

                case activities::Activity::MovingNorth:
                        if ( speedTimer->getValue() > thisBall.getSpeed() )
                        {
                                speedTimer->reset();

                                thisBall.animate() ;

                                // look for collisions with items that are to the north
                                thisBall.canAdvanceTo( -1, 0, 0 );

                                if ( ! thisBall.getMediator()->isThereAnyCollision() ) {
                                        // move the ball
                                        activities::Moving::getInstance().move( *this, false );
                                }
                                else {
                                        thisBall.setIgnoreCollisions( true );

                                        thisBall.metamorphInto( "bubbles", "vanishing final ball" );
                                        thisBall.setBehaviorOf( "behavior of disappearance in time" );
                                }
                        }
                        break;

                default:
                        ;
        }

        return true ;
}

}
