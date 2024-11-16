
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

                case activities::Activity::Pushed :
                        SoundManager::getInstance().play( turningItem.getKind (), "push" );

                        activities::Displacing::getInstance().displace( *this, true );

                        setCurrentActivity( turningItem.isFrozen() // a frozen item remains to be frozen
                                                        ? activities::Activity::Freeze
                                                        : activities::Activity::Waiting );
                        break ;

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
        FreeItem & itemThatTurns = dynamic_cast< FreeItem & >( getItem() );
        const std::string & heading = itemThatTurns.getHeading ();
        bool turnLeft = ( getNameOfBehavior().find( "turn left" ) != std::string::npos );

             if ( heading == "north" ) itemThatTurns.changeHeading( turnLeft ? "west" : "east" );
        else if ( heading == "south" ) itemThatTurns.changeHeading( turnLeft ? "east" : "west" );
        else if ( heading ==  "east" ) itemThatTurns.changeHeading( turnLeft ? "north" : "south" );
        else if ( heading == "west"  ) itemThatTurns.changeHeading( turnLeft ? "south" : "north" );

        // will move where it is heading
        setCurrentActivity( activities::Activity::Moving );
}

}
