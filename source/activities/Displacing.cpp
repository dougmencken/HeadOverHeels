
#include "Displacing.hpp"

#include "Activity.hpp"
#include "Falling.hpp"
#include "PropagateActivity.hpp"
#include "DescribedItem.hpp"
#include "Behavior.hpp"
#include "GridItem.hpp"
#include "FreeItem.hpp"


namespace activities
{

Displacing * Displacing::instance = nilPointer ;

Displacing& Displacing::getInstance()
{
        if ( instance == nilPointer )
                instance = new Displacing () ;

        return *instance;
}

bool Displacing::displace( behaviors::Behavior & behavior, bool canFall )
{
        DescribedItem & item = dynamic_cast< DescribedItem & >( behavior.getItem() );

        bool displaced = false ;

        const Activity & currentActivity = behavior.getCurrentActivity ();
        Motion2D vector = behavior.get2DVelocityVector() ;

        if ( currentActivity == activities::Activity::Pushed || currentActivity == activities::Activity::Dragged )
                displaced = item.addToPosition( vector.getMotionAlongX(), vector.getMotionAlongY(), 0 );
        else if ( currentActivity == activities::Activity::PushedUp )
                displaced = item.addToZ( 1 );

        if ( item.whichItemClass() == "free item" || item.whichItemClass() == "avatar item" )
        {
                Activity activityToPropagate = ( currentActivity == activities::Activity::Dragged )
                                                        ? Activity( activities::Activity::Pushed )
                                                        : currentActivity ;
                // when there’s a collision
                if ( ! displaced )
                        // move involved items
                        PropagateActivity::toAdjacentItems( item, activityToPropagate, vector );
                else
                        // maybe there’s something on top
                        PropagateActivity::toItemsAbove( item, activityToPropagate, vector );
        }

        if ( canFall ) {
                if ( Falling::getInstance().fall( behavior ) ) {
                        behavior.setCurrentActivity( activities::Activity::Falling );
                        displaced = true ;
                }
        }

        return displaced ;
}

}
