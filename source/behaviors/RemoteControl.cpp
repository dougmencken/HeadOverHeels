
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
                case activities::Activity::Waiting :
                        break ;

                case activities::Activity::Moving :
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
                        break ;

                case activities::Activity::Pushed :
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
                                DescribedItemPtr controlledItem = getItem().getMediator()->findItemBehavingAs( "behavior of remotely controlled one" );
                                if ( controlledItem != nilPointer && controlledItem->whichItemClass() == "free item" )
                                {
                                        Motion2D pushVector = get2DVelocityVector() ;

                                        Activity toRemotelyControlled = pushVector.isRest() ? activities::Activity::Waiting : activities::Activity::Pushed ;

                                        // since ‘toRemotelyControlled’ is not Moving but Pushed, a displacement itself doesn’t change the heading
                                        FreeItem & puppet = dynamic_cast< FreeItem & >( *controlledItem );
                                        if ( pushVector.isMovingOnlySouth() )
                                                puppet.changeHeading( "south" );
                                        else if ( pushVector.isMovingOnlyNorth() )
                                                puppet.changeHeading( "north" );
                                        else if ( pushVector.isMovingOnlyWest() )
                                                puppet.changeHeading( "west" );
                                        else if ( pushVector.isMovingOnlyEast() )
                                                puppet.changeHeading( "east" );

                                        controlledItem->getBehavior()->setCurrentActivity( toRemotelyControlled, pushVector );
                                }

                                setCurrentActivity( activities::Activity::Waiting );
                        }

                        break ;

                case activities::Activity::Falling :
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
                        break ;

                default:
                        ;
        }

        return present ;
}

}
