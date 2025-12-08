
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
        , whereGoing( "" )
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
                case activities::Activity::Waiting :
                        if ( ! this->whereGoing.empty() )
                                setCurrentActivity( activities::Activity::Moving, this->whereGoing );
                        // when it doesn’t move see if there’s a character over it and move where that character is heading
                        else {
                                if ( ! drivenItem.canAdvanceTo( 0, 0, 1 ) )
                                {
                                        bool foundCharacter = false ;

                                        while ( mediator->isThereAnyCollision() && ! foundCharacter )
                                        {
                                                DescribedItemPtr itemAbove = mediator->findCollisionPop ();

                                                if ( itemAbove->whichItemClass() == "avatar item" ) {
                                                        foundCharacter = true ;

                                                        this->whereGoing = dynamic_cast< FreeItem & >( *itemAbove ).getHeading() ;
                                                        /////setCurrentActivity( activities::Activity::Moving, this->whereGoing );
                                                }
                                        }
                                }
                        }
                        break ;

                case activities::Activity::Moving:
                        // not frozen and it’s time to move
                        if ( ! drivenItem.isFrozen() ) {
                                if ( speedTimer->get() > drivenItem.getSpeed() )
                                {
                                        if ( ! activities::Moving::getInstance().move( *this, true ) ) {
                                                // when can’t move
                                                this->whereGoing = "" ;
                                                beWaiting() ;

                                                SoundManager::getInstance().play( drivenItem.getKind(), "collision" );
                                        }

                                        speedTimer->go() ;
                                }

                                drivenItem.animate() ;
                        }
                        break ;

                case activities::Activity::Pushed:
                        if ( speedTimer->get() > drivenItem.getSpeed() ) // is it time to move
                        {
                                if ( ! activities::Displacing::getInstance().displace( *this, true ) )
                                        beWaiting() ;

                                speedTimer->go() ;
                        }

                        // frozen item continues to be frozen
                        if ( drivenItem.isFrozen() )
                                setCurrentActivity( activities::Activity::Freeze, Motion2D::rest() );

                        break;

                case activities::Activity::Falling:
                        if ( drivenItem.getZ() == 0 && ! drivenItem.getMediator()->getRoom()->hasFloor() ) {
                                // disappear when reached the bottom of a room without floor
                                present = false ;
                        }
                        // is it time to fall
                        else if ( fallTimer->get() > drivenItem.getWeight() )
                        {
                                if ( ! activities::Falling::getInstance().fall( *this ) )
                                        beWaiting() ;

                                fallTimer->go() ;
                        }
                        break;

                case activities::Activity::Freeze:
                        drivenItem.setFrozen( true );
                        break;

                case activities::Activity::WakeUp:
                        drivenItem.setFrozen( false );
                        beWaiting() ;
                        break;

                default:
                        ;
        }

        return present ;
}

}
