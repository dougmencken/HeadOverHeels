
#include "Elevator.hpp"

#include "FreeItem.hpp"
#include "Room.hpp"
#include "Moving.hpp"
#include "SoundManager.hpp"


namespace behaviors
{

Elevator::Elevator( FreeItem & item, const std::string & behavior )
        : Behavior( item, behavior )
        , top( Room::MaxLayers )
        , bottom( 0 )
        , ascending( true )
        , lastActivity( activities::Activity::Waiting )
        , speedTimer( new Timer() )
        , waitingTimer( new Timer() )
{
        speedTimer->go ();
        waitingTimer->go ();
}

bool Elevator::update ()
{
        FreeItem & elevatorItem = dynamic_cast< FreeItem & >( getItem() );

        switch ( getCurrentActivity() )
        {
                case activities::Activity::Waiting :
                        this->lastActivity = ( this->ascending ? activities::ActivityOfElevator::GoingUp : activities::ActivityOfElevator::GoingDown );
                        setActivityOfElevator( this->lastActivity );
                        break;

                case activities::ActivityOfElevator::GoingUp :
                        if ( speedTimer->getValue() > elevatorItem.getSpeed() )
                        {
                                activities::Moving::getInstance().move( *this, false );

                                speedTimer->go() ;

                                // elevator reached the top
                                if ( elevatorItem.getZ() > this->top * Room::LayerHeight ) {
                                        this->lastActivity = activities::ActivityOfElevator::ReachedTop ;
                                        setActivityOfElevator( this->lastActivity );
                                        waitingTimer->go() ;
                                }
                        }

                        elevatorItem.animate();
                        break;

                case activities::ActivityOfElevator::GoingDown :
                        if ( speedTimer->getValue() > elevatorItem.getSpeed() )
                        {
                                activities::Moving::getInstance().move( *this, false );

                                speedTimer->go() ;

                                // elevator reached the bottom
                                if ( elevatorItem.getZ() <= this->bottom * Room::LayerHeight ) {
                                        this->lastActivity = activities::ActivityOfElevator::ReachedBottom ;
                                        setActivityOfElevator( this->lastActivity );
                                        waitingTimer->go() ;
                                }
                        }

                        elevatorItem.animate();
                        break;

                // stop elevator for a moment when it reaches the lowest point
                case activities::ActivityOfElevator::ReachedBottom :
                        if ( waitingTimer->getValue() >= Delay_Before_Reversing ) {
                                this->lastActivity = activities::ActivityOfElevator::GoingUp ;
                                setActivityOfElevator( this->lastActivity );
                        }

                        elevatorItem.animate();
                        break;

                // stop elevator for a moment when it reaches the highest point
                case activities::ActivityOfElevator::ReachedTop :
                        if ( waitingTimer->getValue() >= Delay_Before_Reversing ) {
                                this->lastActivity = activities::ActivityOfElevator::GoingDown ;
                                setActivityOfElevator( this->lastActivity );
                        }

                        elevatorItem.animate();
                        break;

                default:
                        setActivityOfElevator( this->lastActivity );
                        break;
        }

        SoundManager::getInstance().play( elevatorItem.getKind(), SoundManager::activityToNameOfSound( getCurrentActivity() ) );

        // elevators are eternal
        return true ;
}

}
