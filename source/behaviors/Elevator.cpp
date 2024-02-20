
#include "Elevator.hpp"

#include "Item.hpp"
#include "FreeItem.hpp"
#include "Room.hpp"
#include "Moving.hpp"
#include "SoundManager.hpp"


namespace behaviors
{

Elevator::Elevator( const ItemPtr & item, const std::string & behavior )
        : Behavior( item, behavior )
        , top( 10 )
        , bottom( 0 )
        , ascent( true )
        , lastActivity( activities::Activity::Waiting )
        , speedTimer( new Timer() )
        , stopTimer( new Timer() )
{
        speedTimer->go ();
        stopTimer->go ();
}

Elevator::~Elevator( )
{
}

bool Elevator::update ()
{
        FreeItem& freeItem = dynamic_cast< FreeItem& >( * this->item );

        switch ( activity )
        {
                case activities::Activity::Waiting:
                        setActivityOfItem ( ascent ? activities::Activity::MoveUp : activities::Activity::MoveDown );
                        lastActivity = activity;
                        break;

                case activities::Activity::MoveUp:
                        if ( speedTimer->getValue() > freeItem.getSpeed() )
                        {
                                activities::Moving::getInstance().move( this, &activity, false );

                                speedTimer->reset();

                                // elevator reached the top
                                if ( freeItem.getZ() > top * Room::LayerHeight )
                                {
                                        activity = activities::Activity::StopAtTop;
                                        lastActivity = activity;
                                        stopTimer->reset();
                                }
                        }

                        freeItem.animate();
                        break;

                case activities::Activity::MoveDown:
                        if ( speedTimer->getValue() > freeItem.getSpeed() )
                        {
                                activities::Moving::getInstance().move( this, &activity, false );

                                speedTimer->reset();

                                // elevator reached its bottom
                                if ( freeItem.getZ() <= bottom * Room::LayerHeight )
                                {
                                        activity = activities::Activity::StopAtBottom;
                                        lastActivity = activity;
                                        stopTimer->reset();
                                }
                        }

                        freeItem.animate();
                        break;

                // stop elevator for a moment when it reaches minimum height
                case activities::Activity::StopAtBottom:
                        if ( stopTimer->getValue() >= 0.250 )
                        {
                                setActivityOfItem( activities::Activity::MoveUp );
                                lastActivity = activity;
                        }

                        freeItem.animate();
                        break;

                // stop elevator for a moment when it reaches maximum height
                case activities::Activity::StopAtTop:
                        if ( stopTimer->getValue() >= 0.250 )
                        {
                                setActivityOfItem( activities::Activity::MoveDown );
                                lastActivity = activity;
                        }

                        freeItem.animate();
                        break;

                default:
                        activity = lastActivity;
                        break;
        }

        SoundManager::getInstance().play( freeItem.getKind (), SoundManager::activityToString( activity ) );

        return false;
}

}
