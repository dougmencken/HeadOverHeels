
#include "Patrol.hpp"

#include "FreeItem.hpp"
#include "Moving.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"


namespace behaviors
{

Patrol::Patrol( FreeItem & item, const std::string & behavior )
        : Behavior( item, behavior )
        , speedTimer( new Timer() )
        , fallTimer( new Timer() )
        , patrolTimer( new Timer() )
{
        speedTimer->go ();
        fallTimer->go ();
        patrolTimer->go ();
}

bool Patrol::update ()
{
        FreeItem & patrolItem = dynamic_cast< FreeItem & >( getItem() );

        bool present = true ;

        switch ( getCurrentActivity() )
        {
                case activities::Activity::Waiting:
                        randomlyChangeOrientation ();
                        break;

                case activities::Activity::MovingNorth:
                case activities::Activity::MovingSouth:
                case activities::Activity::MovingEast:
                case activities::Activity::MovingWest:
                case activities::Activity::MovingNortheast:
                case activities::Activity::MovingNorthwest:
                case activities::Activity::MovingSoutheast:
                case activities::Activity::MovingSouthwest:
                        if ( ! patrolItem.isFrozen() )
                        {
                                if ( speedTimer->getValue () > patrolItem.getSpeed () )
                                {
                                        // ¿ cambio de dirección ?
                                        if ( patrolTimer->getValue() > ( double( rand() % 1000 ) + 400.0 ) / 1000.0 )
                                        {
                                                randomlyChangeOrientation ();
                                                patrolTimer->go() ;
                                        }

                                        if ( ! activities::Moving::getInstance().move( *this, true ) ) {
                                                // a collision with something
                                                SoundManager::getInstance().play( patrolItem.getKind (), "collision" );

                                                randomlyChangeOrientation ();
                                        }

                                        SoundManager::getInstance().play( patrolItem.getKind (), "move" );

                                        speedTimer->go() ;
                                }

                                patrolItem.animate() ;
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
                        SoundManager::getInstance().play( patrolItem.getKind (), "push" );

                        // displace this item when it’s pushed by some other one
                        activities::Displacing::getInstance().displace( *this, true );

                        if ( patrolItem.isFrozen() ) // frozen item remains frozen
                                setCurrentActivity( activities::Activity::Freeze );
                        else
                                setCurrentActivity( activities::Activity::Waiting );

                        break;

                case activities::Activity::Falling:
                        if ( patrolItem.getZ() == 0 && ! patrolItem.getMediator()->getRoom()->hasFloor() )
                        {       // disappear when reached floor in a room without floor
                                present = false ;
                        }
                        // is it time to fall
                        else if ( fallTimer->getValue() > patrolItem.getWeight() )
                        {
                                if ( ! activities::Falling::getInstance().fall( * this ) )
                                {
                                        SoundManager::getInstance().play( patrolItem.getKind (), "fall" );
                                        setCurrentActivity( activities::Activity::Waiting );
                                }

                                fallTimer->go() ;
                        }
                        break;

                case activities::Activity::Freeze:
                        patrolItem.setFrozen( true );
                        break;

                case activities::Activity::WakeUp:
                        patrolItem.setFrozen( false );
                        setCurrentActivity( activities::Activity::Waiting );
                        break;

                default:
                        ;
        }

        return present ;
}

void Patrol::randomlyChangeOrientation ()
{
        int randomOrientation = -1 ;

        if ( getNameOfBehavior() == "behavior of random patroling in four primary directions" )
                randomOrientation = ( rand() % 4 );
        else
        if ( getNameOfBehavior() == "behavior of random patroling in four secondary directions" )
                randomOrientation = ( rand() % 4 ) + 4 ;
        else
        if ( getNameOfBehavior() == "behavior of random patroling in eight directions" )
                randomOrientation = ( rand() % 8 );

        switch ( randomOrientation )
        {
                case 0:
                        setCurrentActivity( activities::Activity::MovingNorth );
                        break;

                case 1:
                        setCurrentActivity( activities::Activity::MovingSouth );
                        break;

                case 2:
                        setCurrentActivity( activities::Activity::MovingEast );
                        break;

                case 3:
                        setCurrentActivity( activities::Activity::MovingWest );
                        break;

                case 4:
                        setCurrentActivity( activities::Activity::MovingNortheast );
                        break;

                case 5:
                        setCurrentActivity( activities::Activity::MovingNorthwest );
                        break;

                case 6:
                        setCurrentActivity( activities::Activity::MovingSoutheast );
                        break;

                case 7:
                        setCurrentActivity( activities::Activity::MovingSouthwest );
                        break;

                default:
                        ;
        }
}

}
