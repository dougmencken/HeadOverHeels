
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
                case activities::Activity::Waiting :
                        // meet the character on the X way
                        if ( detectorItem.getX() >= activeCharacter->getX() - 1 && detectorItem.getX() <= activeCharacter->getX() + 1 )
                        {
                                if ( activeCharacter->getY() <= detectorItem.getY() )
                                        setCurrentActivity( activities::Activity::Moving, Motion2D::movingEast() );
                                else
                                if ( activeCharacter->getY() >= detectorItem.getY() )
                                        setCurrentActivity( activities::Activity::Moving, Motion2D::movingWest() );
                        }
                        // meet the character on the Y way
                        else if ( detectorItem.getY() >= activeCharacter->getY() - 1 && detectorItem.getY() <= activeCharacter->getY() + 1 )
                        {
                                if ( activeCharacter->getX() <= detectorItem.getX() )
                                        setCurrentActivity( activities::Activity::Moving, Motion2D::movingNorth() );
                                else
                                if ( activeCharacter->getX() >= detectorItem.getX() )
                                        setCurrentActivity( activities::Activity::Moving, Motion2D::movingSouth() );
                        }

                        // play the sound if moving
                        if ( getCurrentActivity() != activities::Activity::Waiting )
                                SoundManager::getInstance().play( detectorItem.getKind(), "move" );

                        break;

                case activities::Activity::Moving:
                        if ( ! detectorItem.isFrozen() )
                        {
                                if ( /* is it time to move */ speedTimer->getValue() > detectorItem.getSpeed() ) {
                                        if ( ! activities::Moving::getInstance().move( *this, true ) )
                                                // to waiting when can’t move
                                                beWaiting() ;

                                        speedTimer->go() ;
                                }

                                detectorItem.animate() ;
                        }
                        break;

                case activities::Activity::Pushed:
                        if ( /* is it time to move */ speedTimer->getValue() > detectorItem.getSpeed() )
                        {
                                if ( ! activities::Displacing::getInstance().displace( *this, true ) )
                                        beWaiting() ;

                                speedTimer->go() ;
                        }

                        // retain a frozen item’s inactivity
                        if ( detectorItem.isFrozen() )
                                setCurrentActivity( activities::Activity::Freeze, Motion2D::rest() );

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
                                        beWaiting() ;

                                fallTimer->go() ;
                        }
                        break;

                case activities::Activity::Freeze:
                        detectorItem.setFrozen( true );
                        break;

                case activities::Activity::WakeUp:
                        detectorItem.setFrozen( false );
                        beWaiting() ;
                        break;

                default:
                        ;
        }

        return present ;
}

}
