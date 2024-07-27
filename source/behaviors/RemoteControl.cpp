
#include "RemoteControl.hpp"

#include "FreeItem.hpp"
#include "Moving.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"


namespace behaviors
{

RemoteControl::RemoteControl( FreeItem & item, const std::string & behavior )
        : Behavior( item, behavior )
        , speedTimer( new Timer() )
        , fallTimer( new Timer() )
{
        // move the controlled one but not the controller
        if ( getNameOfBehavior() == "behavior of remotely controlled one" )
        {
                speedTimer->go ();
                fallTimer->go ();
        }
}

bool RemoteControl::update ()
{
        FreeItem & thisItem = dynamic_cast< FreeItem & >( getItem() );

        bool present = true ;

        switch ( getCurrentActivity() )
        {
                case activities::Activity::Waiting:
                        break;

                case activities::Activity::MovingNorth:
                case activities::Activity::MovingSouth:
                case activities::Activity::MovingEast:
                case activities::Activity::MovingWest:
                        if ( getNameOfBehavior() == "behavior of remotely controlled one" )
                        {
                                if ( speedTimer->getValue() > thisItem.getSpeed() )
                                {
                                        activities::Moving::getInstance().move( *this, true );

                                        if ( getCurrentActivity() != activities::Activity::Falling )
                                                setCurrentActivity( activities::Activity::Waiting );

                                        speedTimer->go() ;
                                }

                                thisItem.animate() ;
                        }
                        break;

                case activities::Activity::PushedNorth:
                case activities::Activity::PushedSouth:
                case activities::Activity::PushedEast:
                case activities::Activity::PushedWest:
                case activities::Activity::PushedNortheast:
                case activities::Activity::PushedNorthwest:
                case activities::Activity::PushedSoutheast:
                case activities::Activity::PushedSouthwest:
                        if ( getNameOfBehavior() == "behavior of remotely controlled one" )
                        {
                                if ( speedTimer->getValue() > thisItem.getSpeed() )
                                {
                                        if ( getWhatAffectedThisBehavior() != nilPointer )
                                                SoundManager::getInstance().play( thisItem.getKind(), "push" );

                                        activities::Displacing::getInstance().displace( *this, true );

                                        if ( getCurrentActivity() != activities::Activity::Falling )
                                                setCurrentActivity( activities::Activity::Waiting );

                                        speedTimer->go() ;
                                }

                                thisItem.animate() ;
                        }
                        else
                        if ( getNameOfBehavior() == "behavior of remote control" )
                        {
                                // what to move by the remote control
                                ItemPtr controlledItem = thisItem.getMediator()->findItemBehavingAs( "behavior of remotely controlled one" );
                                if ( controlledItem != nilPointer )
                                {
                                        Activity toRemotelyControlled = activities::Activity::Waiting ;

                                        switch ( getCurrentActivity() )
                                        {
                                                case activities::Activity::PushedNorth:
                                                        controlledItem->changeHeading( "north" );
                                                        toRemotelyControlled = activities::Activity::PushedNorth ; /// previously MovingNorth ;
                                                        break;

                                                case activities::Activity::PushedSouth:
                                                        controlledItem->changeHeading( "south" );
                                                        toRemotelyControlled = activities::Activity::PushedSouth ; /// previously MovingSouth ;
                                                        break;

                                                case activities::Activity::PushedEast:
                                                        controlledItem->changeHeading( "east" );
                                                        toRemotelyControlled = activities::Activity::PushedEast ; /// previously MovingEast ;
                                                        break;

                                                case activities::Activity::PushedWest:
                                                        controlledItem->changeHeading( "west" );
                                                        toRemotelyControlled = activities::Activity::PushedWest ; /// previously MovingWest ;
                                                        break;

                                                default:
                                                        ;
                                        }

                                        controlledItem->getBehavior()->setCurrentActivity( toRemotelyControlled );
                                }

                                setCurrentActivity( activities::Activity::Waiting );
                        }

                        break;

                case activities::Activity::Falling:
                        if ( thisItem.getZ() == 0 && ! thisItem.getMediator()->getRoom()->hasFloor() ) {
                                // disappear when reached bottom of a room without floor
                                present = false ;
                        }
                        // is it time to fall for the controlled item
                        else if ( getNameOfBehavior() == "behavior of remotely controlled one" && fallTimer->getValue() > thisItem.getWeight() )
                        {
                                if ( ! activities::Falling::getInstance().fall( * this ) ) {
                                        // emit the sound for the end of falling down
                                        SoundManager::getInstance().play( thisItem.getKind (), "fall" );

                                        setCurrentActivity( activities::Activity::Waiting );
                                }

                                fallTimer->go() ;
                        }
                        break;

                default:
                        ;
        }

        return present ;
}

}
