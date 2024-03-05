
#include "Displacing.hpp"

#include "Falling.hpp"
#include "Behavior.hpp"
#include "Item.hpp"
#include "GridItem.hpp"
#include "FreeItem.hpp"
#include "PropagateActivity.hpp"


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
        bool displaced = false ;

        Item & item = behavior.getItem ();

        const Activity & currentActivity = behavior.getCurrentActivity ();
        Activity activityToPropagate = currentActivity ;

        switch ( currentActivity )
        {
                case activities::Activity::DraggedNorth:
                        activityToPropagate = activities::Activity::PushedNorth;
                        // fallthru
                case activities::Activity::PushedNorth:
                        displaced = item.addToX( -1 );
                        break;

                case activities::Activity::DraggedSouth:
                        activityToPropagate = activities::Activity::PushedSouth;
                        // fallthru
                case activities::Activity::PushedSouth:
                        displaced = item.addToX( 1 );
                        break;

                case activities::Activity::DraggedEast:
                        activityToPropagate = activities::Activity::PushedEast;
                        // fallthru
                case activities::Activity::PushedEast:
                        displaced = item.addToY( -1 );
                        break;

                case activities::Activity::DraggedWest:
                        activityToPropagate = activities::Activity::PushedWest;
                        // fallthru
                case activities::Activity::PushedWest:
                        displaced = item.addToY( 1 );
                        break;

                case activities::Activity::PushedNortheast:
                        displaced = item.addToPosition( -1, -1, 0 );
                        break;

                case activities::Activity::PushedNorthwest:
                        displaced = item.addToPosition( -1, 1, 0 );
                        break;

                case activities::Activity::PushedSoutheast:
                        displaced = item.addToPosition( 1, -1, 0 );
                        break;

                case activities::Activity::PushedSouthwest:
                        displaced = item.addToPosition( 1, 1, 0 );
                        break;

                case activities::Activity::PushedUp:
                        displaced = item.addToZ( 1 );
                        break;

                default:
                        ;
        }

        if ( item.whichItemClass() == "free item" || item.whichItemClass() == "avatar item" )
        {
                // when there’s a collision
                if ( ! displaced )
                {
                        // move involved items
                        PropagateActivity::toAdjacentItems( item, activityToPropagate );
                }
                else {
                        // maybe there’s something on top
                        PropagateActivity::toItemsAbove( item, activityToPropagate );
                }
        }

        // when item can fall
        if ( canFall )
        {
                if ( Falling::getInstance().fall( behavior ) )
                {
                        behavior.setCurrentActivity( activities::Activity::Falling );
                        displaced = true ;
                }
        }

        return displaced ;
}

}
