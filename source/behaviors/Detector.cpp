
#include "Detector.hpp"

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

Detector::Detector( FreeItem & item, const std::string & behavior )
        : Behavior( item, behavior )
        , speedTimer( new Timer() )
        , fallTimer( new Timer() )
{
        speedTimer->go();
        fallTimer->go();
}

bool Detector::update ()
{
        FreeItem & detectorItem = dynamic_cast< FreeItem & >( getItem () );
        AvatarItemPtr activeCharacter = detectorItem.getMediator()->getActiveCharacter();

        if ( activeCharacter == nilPointer ) return false ;

        bool present = true ;

        switch ( getCurrentActivity() )
        {
                case activities::Activity::Waiting:
                        // meet the character on the X way
                        if ( detectorItem.getX() >= activeCharacter->getX() - 1 && detectorItem.getX() <= activeCharacter->getX() + 1 )
                        {
                                if ( activeCharacter->getY() <= detectorItem.getY() )
                                        setCurrentActivity( activities::Activity::MovingEast );
                                else
                                if ( activeCharacter->getY() >= detectorItem.getY() )
                                        setCurrentActivity( activities::Activity::MovingWest );
                        }
                        // meet the character on the Y way
                        else if ( detectorItem.getY() >= activeCharacter->getY() - 1 && detectorItem.getY() <= activeCharacter->getY() + 1 )
                        {
                                if ( activeCharacter->getX() <= detectorItem.getX() )
                                        setCurrentActivity( activities::Activity::MovingNorth );
                                else
                                if ( activeCharacter->getX() >= detectorItem.getX() )
                                        setCurrentActivity( activities::Activity::MovingSouth );
                        }

                        // play the sound if moving
                        if ( getCurrentActivity() != activities::Activity::Waiting )
                                SoundManager::getInstance().play( detectorItem.getKind(), "move" );

                        break;

                case activities::Activity::MovingNorth:
                case activities::Activity::MovingSouth:
                case activities::Activity::MovingEast:
                case activities::Activity::MovingWest:
                        if ( ! detectorItem.isFrozen() )
                        {
                                if ( /* is it time to move */ speedTimer->getValue() > detectorItem.getSpeed() )
                                {
                                        if ( ! activities::Moving::getInstance().move( *this, true ) )
                                                // to waiting when can’t move
                                                setCurrentActivity( activities::Activity::Waiting );

                                        speedTimer->reset();
                                }

                                detectorItem.animate() ;
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
                        if ( /* is it time to move */ speedTimer->getValue() > detectorItem.getSpeed() )
                        {
                                if ( ! activities::Displacing::getInstance().displace( *this, true ) )
                                        setCurrentActivity( activities::Activity::Waiting );

                                speedTimer->reset();
                        }

                        // retain a frozen item’s inactivity
                        if ( detectorItem.isFrozen() )
                                setCurrentActivity( activities::Activity::Freeze );

                        break;

                case activities::Activity::Falling:
                        if ( detectorItem.getZ() == 0 && ! detectorItem.getMediator()->getRoom()->hasFloor() ) {
                                // disappear when at the bottom of a room without floor
                                present = false ;
                        }
                        // is it time to fall
                        else if ( fallTimer->getValue() > detectorItem.getWeight() )
                        {
                                if ( ! activities::Falling::getInstance().fall( * this ) )
                                        setCurrentActivity( activities::Activity::Waiting );

                                fallTimer->reset();
                        }
                        break;

                case activities::Activity::Freeze:
                        detectorItem.setFrozen( true );
                        break;

                case activities::Activity::WakeUp:
                        detectorItem.setFrozen( false );
                        setCurrentActivity( activities::Activity::Waiting );
                        break;

                default:
                        ;
        }

        return present ;
}

}
