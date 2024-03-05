
#include "Elevator.hpp"

#include "FreeItem.hpp"
#include "Room.hpp"
#include "Moving.hpp"
#include "SoundManager.hpp"


namespace behaviors
{

Elevator::Elevator( Item & item, const std::string & behavior )
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

bool Elevator::update_returningdisappearance ()
{
        FreeItem & freeItem = dynamic_cast< FreeItem & >( getItem() );

        switch ( getCurrentActivity() )
        {
                case activities::Activity::Waiting:
                        setCurrentActivity ( ascent ? activities::Activity::GoingUp : activities::Activity::GoingDown );
                        this->lastActivity = getCurrentActivity ();
                        break;

                case activities::Activity::GoingUp:
                        if ( speedTimer->getValue() > freeItem.getSpeed() )
                        {
                                activities::Moving::getInstance().move( *this, false );

                                speedTimer->reset() ;

                                // elevator reached the top
                                if ( freeItem.getZ() > top * Room::LayerHeight )
                                {
                                        setCurrentActivity( activities::Activity::StopAtTop );
                                        this->lastActivity = getCurrentActivity ();
                                        waitingTimer->reset() ;
                                }
                        }

                        freeItem.animate();
                        break;

                case activities::Activity::GoingDown:
                        if ( speedTimer->getValue() > freeItem.getSpeed() )
                        {
                                activities::Moving::getInstance().move( *this, false );

                                speedTimer->reset() ;

                                // elevator reached the bottom
                                if ( freeItem.getZ() <= bottom * Room::LayerHeight )
                                {
                                        setCurrentActivity( activities::Activity::StopAtBottom );
                                        this->lastActivity = getCurrentActivity ();
                                        waitingTimer->reset() ;
                                }
                        }

                        freeItem.animate();
                        break;

                // stop elevator for a moment when it reaches the lowest point
                case activities::Activity::StopAtBottom:
                        if ( waitingTimer->getValue() >= 0.250 )
                        {
                                setCurrentActivity( activities::Activity::GoingUp );
                                this->lastActivity = getCurrentActivity ();
                        }

                        freeItem.animate();
                        break;

                // stop elevator for a moment when it reaches the highest point
                case activities::Activity::StopAtTop:
                        if ( waitingTimer->getValue() >= 0.250 )
                        {
                                setCurrentActivity( activities::Activity::GoingDown );
                                this->lastActivity = getCurrentActivity ();
                        }

                        freeItem.animate();
                        break;

                default:
                        setCurrentActivity( this->lastActivity );
                        break;
        }

        SoundManager::getInstance().play( freeItem.getKind(), SoundManager::activityToNameOfSound( getCurrentActivity() ) );

        return false ;
}

}
