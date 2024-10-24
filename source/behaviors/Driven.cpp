
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
        FreeItem & drivenItem = dynamic_cast< FreeItem & >( getItem () );
        Mediator * mediator = drivenItem.getMediator() ;

        bool present = true ;

        switch ( getCurrentActivity() )
        {
                case activities::Activity::Waiting:
                        if ( this->moving )
                                setCurrentActivity( activities::Activity::Moving, drivenItem.getHeading() );
                        else // when stopped, see if there is a character on it and use its orientation to begin moving
                        {
                                if ( ! drivenItem.canAdvanceTo( 0, 0, 1 ) )
                                {
                                        bool foundCharacter = false ;

                                        while ( mediator->isThereAnyCollision() && ! foundCharacter )
                                        {
                                                ItemPtr item = mediator->findCollisionPop ();

                                                if ( item->whichItemClass() == "avatar item" )
                                                {
                                                        foundCharacter = true ;
                                                        this->moving = true ;

                                                        setCurrentActivity( activities::Activity::Moving, item->getHeading() );
                                                }
                                        }
                                }
                        }
                        break ;

                case activities::Activity::Moving:
                        // not frozen and it’s time to move
                        if ( ! drivenItem.isFrozen() ) {
                                if ( speedTimer->getValue() > drivenItem.getSpeed() )
                                {
                                        if ( ! activities::Moving::getInstance().move( *this, true ) ) {
                                                // when can’t move
                                                this->moving = false ;
                                                setCurrentActivity( activities::Activity::Waiting );

                                                SoundManager::getInstance().play( drivenItem.getKind(), "collision" );
                                        }

                                        speedTimer->go() ;
                                }

                                drivenItem.animate() ;
                        }
                        break ;

                case activities::Activity::PushedNorth:
                case activities::Activity::PushedSouth:
                case activities::Activity::PushedEast:
                case activities::Activity::PushedWest:
                case activities::Activity::PushedNortheast:
                case activities::Activity::PushedNorthwest:
                case activities::Activity::PushedSoutheast:
                case activities::Activity::PushedSouthwest:
                        if ( speedTimer->getValue() > drivenItem.getSpeed() ) // is it time to move
                        {
                                if ( ! activities::Displacing::getInstance().displace( *this, true ) )
                                        setCurrentActivity( activities::Activity::Waiting );

                                speedTimer->go() ;
                        }

                        // frozen item continues to be frozen
                        if ( drivenItem.isFrozen() )
                                setCurrentActivity( activities::Activity::Freeze );

                        break;

                case activities::Activity::Falling:
                        if ( drivenItem.getZ() == 0 && ! drivenItem.getMediator()->getRoom()->hasFloor() ) {
                                // disappear when reached the bottom of a room without floor
                                present = false ;
                        }
                        // is it time to fall
                        else if ( fallTimer->getValue() > drivenItem.getWeight() )
                        {
                                if ( ! activities::Falling::getInstance().fall( *this ) )
                                        setCurrentActivity( activities::Activity::Waiting );

                                fallTimer->go() ;
                        }
                        break;

                case activities::Activity::Freeze:
                        drivenItem.setFrozen( true );
                        break;

                case activities::Activity::WakeUp:
                        drivenItem.setFrozen( false );
                        setCurrentActivity( activities::Activity::Waiting );
                        break;

                default:
                        ;
        }

        return present ;
}

}
