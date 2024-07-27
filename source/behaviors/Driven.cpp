
#include "Driven.hpp"

#include "FreeItem.hpp"
#include "Moving.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"


namespace behaviors
{

Driven::Driven( FreeItem & item, const std::string & behavior )
        : Behavior( item, behavior )
        , moving( false )
        , speedTimer( new Timer() )
        , fallTimer( new Timer() )
{
        speedTimer->go() ;
        fallTimer->go() ;
}

bool Driven::update ()
{
        FreeItem & freeItem = dynamic_cast< FreeItem & >( getItem () );
        Mediator * mediator = freeItem.getMediator() ;

        bool present = true ;

        switch ( getCurrentActivity() )
        {
                case activities::Activity::Waiting:
                        if ( this->moving )
                        {
                                switch ( Way( freeItem.getHeading() ).getIntegerOfWay() )
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
                        else // when stopped, see if there is a character on it and use its orientation to begin moving
                        {
                                if ( ! freeItem.canAdvanceTo( 0, 0, 1 ) )
                                {
                                        bool foundCharacter = false ;

                                        while ( mediator->isThereAnyCollision() && ! foundCharacter )
                                        {
                                                ItemPtr item = mediator->findCollisionPop ();

                                                if ( item->whichItemClass() == "avatar item" )
                                                {
                                                        foundCharacter = true ;
                                                        this->moving = true ;

                                                        switch ( Way( item->getHeading() ).getIntegerOfWay () )
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
                                        }
                                }
                        }
                        break;

                case activities::Activity::MovingNorth:
                case activities::Activity::MovingSouth:
                case activities::Activity::MovingEast:
                case activities::Activity::MovingWest:
                        // not frozen and it’s time to move
                        if ( ! freeItem.isFrozen() ) {
                                if ( speedTimer->getValue() > freeItem.getSpeed() )
                                {
                                        if ( ! activities::Moving::getInstance().move( *this, true ) ) {
                                                // when can’t move
                                                this->moving = false ;
                                                setCurrentActivity( activities::Activity::Waiting );

                                                SoundManager::getInstance().play( freeItem.getKind(), "collision" );
                                        }

                                        speedTimer->go() ;
                                }

                                freeItem.animate() ;
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
                        if ( speedTimer->getValue() > freeItem.getSpeed() ) // is it time to move
                        {
                                if ( ! activities::Displacing::getInstance().displace( *this, true ) )
                                        setCurrentActivity( activities::Activity::Waiting );

                                speedTimer->go() ;
                        }

                        // frozen item continues to be frozen
                        if ( freeItem.isFrozen() )
                                setCurrentActivity( activities::Activity::Freeze );

                        break;

                case activities::Activity::Falling:
                        if ( freeItem.getZ() == 0 && ! freeItem.getMediator()->getRoom()->hasFloor() ) {
                                // disappear when reached the bottom of a room without floor
                                present = false ;
                        }
                        // is it time to fall
                        else if ( fallTimer->getValue() > freeItem.getWeight() )
                        {
                                if ( ! activities::Falling::getInstance().fall( *this ) )
                                        setCurrentActivity( activities::Activity::Waiting );

                                fallTimer->go() ;
                        }
                        break;

                case activities::Activity::Freeze:
                        freeItem.setFrozen( true );
                        break;

                case activities::Activity::WakeUp:
                        freeItem.setFrozen( false );
                        setCurrentActivity( activities::Activity::Waiting );
                        break;

                default:
                        ;
        }

        return present ;
}

}
