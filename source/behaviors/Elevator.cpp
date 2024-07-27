
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
        , ascent( true )
        , lastActivity( activities::Activity::Waiting )
        , speedTimer( new Timer() )
        , waitingTimer( new Timer() )
{
        speedTimer->go ();
        waitingTimer->go ();
}

bool Elevator::update ()
{
        FreeItem & freeItem = dynamic_cast< FreeItem & >( getItem() );

        switch ( getCurrentActivity() )
        {
                case activities::Activity::Waiting :
                        setCurrentActivity ( this->ascent ? activities::ActivityOfElevator::GoingUp : activities::ActivityOfElevator::GoingDown );
                        this->lastActivity = getCurrentActivity ();
                        break;

                case activities::ActivityOfElevator::GoingUp :
                        if ( speedTimer->getValue() > freeItem.getSpeed() )
                        {
                                activities::Moving::getInstance().move( *this, false );

                                speedTimer->go() ;

                                // elevator reached the top
                                if ( freeItem.getZ() > this->top * Room::LayerHeight )
                                {
                                        setCurrentActivity( activities::ActivityOfElevator::ReachedTop );
                                        this->lastActivity = getCurrentActivity ();
                                        waitingTimer->go() ;
                                }
                        }

                        freeItem.animate();
                        break;

                case activities::ActivityOfElevator::GoingDown :
                        if ( speedTimer->getValue() > freeItem.getSpeed() )
                        {
                                activities::Moving::getInstance().move( *this, false );

                                speedTimer->go() ;

                                // elevator reached the bottom
                                if ( freeItem.getZ() <= this->bottom * Room::LayerHeight )
                                {
                                        setCurrentActivity( activities::ActivityOfElevator::ReachedBottom );
                                        this->lastActivity = getCurrentActivity ();
                                        waitingTimer->go() ;
                                }
                        }

                        freeItem.animate();
                        break;

                // stop elevator for a moment when it reaches the lowest point
                case activities::ActivityOfElevator::ReachedBottom :
                        if ( waitingTimer->getValue() >= 0.250 )
                        {
                                setCurrentActivity( activities::ActivityOfElevator::GoingUp );
                                this->lastActivity = getCurrentActivity ();
                        }

                        freeItem.animate();
                        break;

                // stop elevator for a moment when it reaches the highest point
                case activities::ActivityOfElevator::ReachedTop :
                        if ( waitingTimer->getValue() >= 0.250 )
                        {
                                setCurrentActivity( activities::ActivityOfElevator::GoingDown );
                                this->lastActivity = getCurrentActivity ();
                        }

                        freeItem.animate();
                        break;

                default:
                        setCurrentActivity( this->lastActivity );
                        break;
        }

        SoundManager::getInstance().play( freeItem.getKind(), SoundManager::activityToNameOfSound( getCurrentActivity() ) );

        // elevators are eternal
        return true ;
}

}
