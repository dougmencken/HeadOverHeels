
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
                        setCurrentActivity( activities::Activity::Moving );
                        break ;

                case activities::Activity::Moving:
                        if ( ! turningItem.isFrozen() )
                        {
                                if ( speedTimer->getValue() > turningItem.getSpeed() )
                                {
                                        if ( ! activities::Moving::getInstance().move( *this, true ) ) {
                                                // if can’t move on
                                                turn () ;

                                                SoundManager::getInstance().play( turningItem.getKind (), "collision" );
                                        }

                                        speedTimer->go() ;
                                }

                                turningItem.animate() ;
                        }
                        break ;

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

                                fallTimer->go() ;
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

void Turn::turn ()
{
        Item & item = getItem() ;
        const std::string & heading = item.getHeading ();
        bool turnLeft = ( getNameOfBehavior().find( "turn left" ) != std::string::npos );

             if ( heading == "north" ) item.changeHeading( turnLeft ? "west" : "east" );
        else if ( heading == "south" ) item.changeHeading( turnLeft ? "east" : "west" );
        else if ( heading ==  "east" ) item.changeHeading( turnLeft ? "north" : "south" );
        else if ( heading == "west"  ) item.changeHeading( turnLeft ? "south" : "north" );

        // will move where it is heading
        setCurrentActivity( activities::Activity::Moving );
}

}
