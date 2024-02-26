
#include "Detector.hpp"

#include "Item.hpp"
#include "FreeItem.hpp"
#include "AvatarItem.hpp"
#include "Moving.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"


namespace behaviors
{

Detector::Detector( const ItemPtr & item, const std::string & behavior )
        : Behavior( item, behavior )
        , speedTimer( new Timer() )
        , fallTimer( new Timer() )
{
        speedTimer->go();
        fallTimer->go();
}

Detector::~Detector( )
{
}

bool Detector::update ()
{
        FreeItem& freeItem = dynamic_cast< FreeItem& >( * this->item );
        AvatarItemPtr activeCharacter = freeItem.getMediator()->getActiveCharacter();
        bool isGone = false;

        if ( activeCharacter != nilPointer )
        {
                switch ( activity )
                {
                        case activities::Activity::Waiting:
                                // the character meets the detector on the X way
                                if ( freeItem.getX() >= activeCharacter->getX() - 1 && freeItem.getX() <= activeCharacter->getX() + 1 )
                                {
                                        if ( activeCharacter->getY() <= freeItem.getY() )
                                        {
                                                setCurrentActivity( activities::Activity::MoveEast );
                                        }
                                        else if ( activeCharacter->getY() >= freeItem.getY() )
                                        {
                                                setCurrentActivity( activities::Activity::MoveWest );
                                        }
                                }
                                // the character meets the detector on the Y way
                                else if ( freeItem.getY() >= activeCharacter->getY() - 1 && freeItem.getY() <= activeCharacter->getY() + 1 )
                                {
                                        if ( activeCharacter->getX() <= freeItem.getX() )
                                        {
                                                setCurrentActivity( activities::Activity::MoveNorth );
                                        }
                                        else
                                        if ( activeCharacter->getX() >= freeItem.getX() )
                                        {
                                                setCurrentActivity( activities::Activity::MoveSouth );
                                        }
                                }

                                // if it changed activity, play the sound
                                if ( activity != activities::Activity::Waiting )
                                {
                                        SoundManager::getInstance().play( freeItem.getKind(), "move" );
                                }
                                break;

                        case activities::Activity::MoveNorth:
                        case activities::Activity::MoveSouth:
                        case activities::Activity::MoveEast:
                        case activities::Activity::MoveWest:
                                // is item active
                                if ( ! freeItem.isFrozen() )
                                {
                                        // is it time to move
                                        if ( speedTimer->getValue() > freeItem.getSpeed() )
                                        {
                                                // move item, if there’s collision let’s wait
                                                if ( ! activities::Moving::getInstance().move( this, &activity, true ) )
                                                {
                                                        activity = activities::Activity::Waiting;
                                                }

                                                speedTimer->reset();
                                        }

                                        freeItem.animate();
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
                                // is it time to move
                                if ( speedTimer->getValue() > freeItem.getSpeed() )
                                {
                                        if ( ! activities::Displacing::getInstance().displace( this, &activity, true ) )
                                        {
                                                activity = activities::Activity::Waiting;
                                        }

                                        speedTimer->reset();
                                }

                                // preserve inactivity for inactive item
                                if ( freeItem.isFrozen() )
                                {
                                        activity = activities::Activity::Freeze;
                                }
                                break;

                        case activities::Activity::Fall:
                                // look for reaching floor in a room without floor
                                if ( freeItem.getZ() == 0 && ! freeItem.getMediator()->getRoom()->hasFloor() )
                                {
                                        isGone = true;
                                }
                                // is it time to fall
                                else if ( fallTimer->getValue() > freeItem.getWeight() )
                                {
                                        if ( ! activities::Falling::getInstance().fall( this ) )
                                        {
                                                activity = activities::Activity::Waiting;
                                        }

                                        fallTimer->reset();
                                }
                                break;

                        case activities::Activity::Freeze:
                                freeItem.setFrozen( true );
                                break;

                        case activities::Activity::WakeUp:
                                freeItem.setFrozen( false );
                                activity = activities::Activity::Waiting;
                                break;

                        default:
                                ;
                }
        }

        return isGone;
}

}
