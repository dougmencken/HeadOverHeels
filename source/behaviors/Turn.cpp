
#include "Turn.hpp"

#include "FreeItem.hpp"
#include "Moving.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"


namespace behaviors
{

Turn::Turn( FreeItem & item, const std::string & behavior )
        : Behavior( item, behavior )
        , speedTimer( new Timer() )
        , fallTimer( new Timer() )
{
        speedTimer->go ();
        fallTimer->go ();
}

bool Turn::update ()
{
        FreeItem & turningItem = dynamic_cast< FreeItem & >( getItem() );

        bool present = true ;

        switch ( getCurrentActivity () )
        {
                case activities::Activity::Waiting:
                        beginMoving () ;
                        break;

                case activities::Activity::MovingNorth:
                case activities::Activity::MovingSouth:
                case activities::Activity::MovingEast:
                case activities::Activity::MovingWest:
                        if ( ! turningItem.isFrozen() )
                        {
                                if ( speedTimer->getValue() > turningItem.getSpeed() )
                                {
                                        if ( ! activities::Moving::getInstance().move( *this, true ) ) {
                                                // if can’t move on
                                                turn () ;

                                                SoundManager::getInstance().play( turningItem.getKind (), "collision" );
                                        }

                                        speedTimer->reset();
                                }

                                turningItem.animate() ;
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
                        SoundManager::getInstance().play( turningItem.getKind (), "push" );

                        activities::Displacing::getInstance().displace( *this, true );

                        if ( turningItem.isFrozen() ) // a frozen item remains to be frozen
                                setCurrentActivity( activities::Activity::Freeze );
                        else
                                setCurrentActivity( activities::Activity::Waiting );

                        break;

                case activities::Activity::Falling:
                        if ( turningItem.getZ() == 0 && ! turningItem.getMediator()->getRoom()->hasFloor() ) {
                                // reached the bottom of a room without floor
                                present = false ;
                        }
                        // is it time to fall
                        else if ( fallTimer->getValue() > turningItem.getWeight() )
                        {
                                if ( ! activities::Falling::getInstance().fall( * this ) ) {
                                        SoundManager::getInstance().play( turningItem.getKind (), "fall" );
                                        setCurrentActivity( activities::Activity::Waiting );
                                }

                                fallTimer->reset();
                        }
                        break;

                case activities::Activity::Freeze:
                        turningItem.setFrozen( true );
                        break;

                case activities::Activity::WakeUp:
                        turningItem.setFrozen( false );
                        setCurrentActivity( activities::Activity::Waiting );
                        break;

                default:
                        ;
        }

        return present ;
}

void Turn::beginMoving ()
{
        switch ( Way( getItem().getHeading () ).getIntegerOfWay () )
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

void Turn::turn ()
{
        bool turnLeft = ( getNameOfBehavior() == "behavior of move then turn left and move" );

        Item & item = getItem() ;
        const std::string & heading = item.getHeading ();

        if ( heading == "north" ) {
                if ( turnLeft ) {
                        setCurrentActivity( activities::Activity::MovingWest );
                        item.changeHeading( "west" );
                } else {
                        setCurrentActivity( activities::Activity::MovingEast );
                        item.changeHeading( "east" );
                }
        }
        else if ( heading == "south" ) {
                if ( turnLeft ) {
                        setCurrentActivity( activities::Activity::MovingEast );
                        item.changeHeading( "east" );
                } else {
                        setCurrentActivity( activities::Activity::MovingWest );
                        item.changeHeading( "west" );
                }
        }
        else if ( heading == "east" ) {
                if ( turnLeft ) {
                        setCurrentActivity( activities::Activity::MovingNorth );
                        item.changeHeading( "north" );
                } else {
                        setCurrentActivity( activities::Activity::MovingSouth );
                        item.changeHeading( "south" );
                }
        }
        else if ( heading == "west" ) {
                if ( turnLeft ) {
                        setCurrentActivity( activities::Activity::MovingSouth );
                        item.changeHeading( "south" );
                } else {
                        setCurrentActivity( activities::Activity::MovingNorth );
                        item.changeHeading( "north" );
                }
        }
}

}
