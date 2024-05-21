
#include "ThereAndBack.hpp"

#include "FreeItem.hpp"
#include "Moving.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"


namespace behaviors
{

ThereAndBack::ThereAndBack( FreeItem & item, const std::string & behavior, bool flying )
        : Behavior( item, behavior )
        , isFlying( flying )
        , speedTimer( new Timer() )
        , fallTimer( new Timer() )
{
        speedTimer->go ();
        if ( ! flying ) fallTimer->go ();
}

bool ThereAndBack::update ()
{
        FreeItem & thisItem = dynamic_cast< FreeItem & >( getItem () );

        bool present = true ;

        switch ( getCurrentActivity () )
        {
                case activities::Activity::Waiting:
                        moveIt() ;
                        break;

                case activities::Activity::MovingNorth:
                case activities::Activity::MovingSouth:
                case activities::Activity::MovingEast:
                case activities::Activity::MovingWest:
                        if ( ! thisItem.isFrozen() )
                        {
                                if ( speedTimer->getValue() > thisItem.getSpeed() )
                                {
                                        // move it
                                        if ( ! activities::Moving::getInstance().move( *this, true ) )
                                        {
                                                turnBack() ;

                                                SoundManager::getInstance().play( thisItem.getKind(), "collision" );
                                        }

                                        speedTimer->reset();
                                }

                                thisItem.animate() ;
                        }
                        break;

                case activities::Activity::PushedNorth:
                case activities::Activity::PushedSouth:
                case activities::Activity::PushedEast:
                case activities::Activity::PushedWest:
                case activities::Activity::PushedNortheast:
                case activities::Activity::PushedSoutheast:
                case activities::Activity::PushedSouthwest:
                case activities::Activity::PushedNorthwest:
                        if ( ! this->isFlying ) {
                                SoundManager::getInstance().play( thisItem.getKind(), "push" );

                                // this item is pushed by another one
                                activities::Displacing::getInstance().displace( *this, true );
                        }

                        if ( thisItem.isFrozen() ) // a frozen item remains frozen
                                setCurrentActivity( activities::Activity::Freeze );
                        else
                                setCurrentActivity( activities::Activity::Waiting );

                        break;

                case activities::Activity::Falling:
                        if ( ! this->isFlying ) {
                                if ( thisItem.getZ() == 0 && ! thisItem.getMediator()->getRoom()->hasFloor() ) {
                                        // disappear if on the bottom of a room with no floor
                                        present = false ;
                                }
                                // is it time to fall
                                else if ( fallTimer->getValue() > thisItem.getWeight() )
                                {
                                        if ( ! activities::Falling::getInstance().fall( *this ) ) {
                                                // emit the sound of falling
                                                SoundManager::getInstance().play( thisItem.getKind (), "fall" );
                                                setCurrentActivity( activities::Activity::Waiting );
                                        }

                                        fallTimer->reset();
                                }
                        } else
                                setCurrentActivity( activities::Activity::Waiting );

                        break;

                case activities::Activity::Freeze:
                        thisItem.setFrozen( true );
                        break;

                case activities::Activity::WakeUp:
                        thisItem.setFrozen( false );
                        setCurrentActivity( activities::Activity::Waiting );
                        break;

                default:
                        ;
        }

        return present ;
}

void ThereAndBack::moveIt ()
{
        switch ( Way( getItem().getHeading() ).getIntegerOfWay () )
        {
                case Way::North:
                        setCurrentActivity( activities::Activity::MovingNorth );
                        break;

                case Way::South:
                        setCurrentActivity( activities::Activity::MovingSouth );
                        break;

                case Way::East:
                        setCurrentActivity( activities::Activity::MovingEast );
                        break;

                case Way::West:
                        setCurrentActivity( activities::Activity::MovingWest );
                        break;

                default:
                        ;
        }
}

void ThereAndBack::turnBack ()
{
        Item & thisItem = getItem () ;

        switch ( Way( thisItem.getHeading() ).getIntegerOfWay () )
        {
                case Way::North:
                        setCurrentActivity( activities::Activity::MovingSouth );
                        thisItem.changeHeading( "south" );
                        break;

                case Way::South:
                        setCurrentActivity( activities::Activity::MovingNorth );
                        thisItem.changeHeading( "north" );
                        break;

                case Way::East:
                        setCurrentActivity( activities::Activity::MovingWest );
                        thisItem.changeHeading( "west" );
                        break;

                case Way::West:
                        setCurrentActivity( activities::Activity::MovingEast );
                        thisItem.changeHeading( "east" );
                        break;

                default:
                        ;
        }
}

}
