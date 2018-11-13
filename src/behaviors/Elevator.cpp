
#include "Elevator.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "MoveKindOfActivity.hpp"
#include "SoundManager.hpp"


namespace iso
{

Elevator::Elevator( const ItemPtr & item, const std::string & behavior )
        : Behavior( item, behavior )
        , top( 10 )
        , bottom( 0 )
        , ascent( true )
        , lastActivity( Activity::Wait )
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
                case Activity::Wait:
                        changeActivityOfItem ( ascent ? Activity::MoveUp : Activity::MoveDown );
                        lastActivity = activity;
                        break;

                case Activity::MoveUp:
                        if ( speedTimer->getValue() > freeItem.getSpeed() )
                        {
                                MoveKindOfActivity::getInstance().move( this, &activity, false );

                                speedTimer->reset();

                                // elevator reached its top
                                if ( freeItem.getZ() > top * LayerHeight )
                                {
                                        activity = Activity::StopAtTop;
                                        lastActivity = activity;
                                        stopTimer->reset();
                                }
                        }

                        freeItem.animate();
                        break;

                case Activity::MoveDown:
                        if ( speedTimer->getValue() > freeItem.getSpeed() )
                        {
                                MoveKindOfActivity::getInstance().move( this, &activity, false );

                                speedTimer->reset();

                                // elevator reached its bottom
                                if ( freeItem.getZ() <= bottom * LayerHeight )
                                {
                                        activity = Activity::StopAtBottom;
                                        lastActivity = activity;
                                        stopTimer->reset();
                                }
                        }

                        freeItem.animate();
                        break;

                // stop elevator for a moment when it reaches minimum height
                case Activity::StopAtBottom:
                        if ( stopTimer->getValue() >= 0.250 )
                        {
                                changeActivityOfItem( Activity::MoveUp );
                                lastActivity = activity;
                        }

                        freeItem.animate();
                        break;

                // stop elevator for a moment when it reaches maximum height
                case Activity::StopAtTop:
                        if ( stopTimer->getValue() >= 0.250 )
                        {
                                changeActivityOfItem( Activity::MoveDown );
                                lastActivity = activity;
                        }

                        freeItem.animate();
                        break;

                default:
                        activity = lastActivity;
                        break;
        }

        SoundManager::getInstance().play( freeItem.getLabel(), activity );

        return false;
}

}
