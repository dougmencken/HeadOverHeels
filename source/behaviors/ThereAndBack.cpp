
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
                        setCurrentActivity( activities::Activity::Moving, thisItem.getHeading() );
                        break ;

                case activities::Activity::Moving:
                        if ( ! thisItem.isFrozen() )
                        {
                                if ( speedTimer->getValue() > thisItem.getSpeed() )
                                {
                                        if ( ! activities::Moving::getInstance().move( *this, true ) ) {
                                                // a collision with something
                                                SoundManager::getInstance().play( thisItem.getKind(), "collision" );

                                                turnBack() ;
                                        }

                                        speedTimer->go() ;
                                }

                                thisItem.animate() ;
                        }
                        break ;

                case activities::Activity::Pushed :
                {
                        if ( ! this->isFlying ) {
                                SoundManager::getInstance().play( thisItem.getKind(), "push" );

                                // this item is pushed by another one
                                activities::Displacing::getInstance().displace( *this, true );
                        }

                        Activity freezeOrWait = thisItem.isFrozen()
                                                        ? activities::Activity::Freeze // a frozen item remains frozen
                                                        : activities::Activity::Waiting ;
                        setCurrentActivity( freezeOrWait, Motion2D::rest() );
                }
                        break ;

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

                                                beWaiting() ;
                                        }

                                        fallTimer->go() ;
                                }
                        } else
                                beWaiting() ;

                        break;

                case activities::Activity::Freeze:
                        thisItem.setFrozen( true );
                        break;

                case activities::Activity::WakeUp:
                        thisItem.setFrozen( false );
                        beWaiting() ;
                        break;
        }

        return present ;
}

void ThereAndBack::turnBack ()
{
        setCurrentActivity( activities::Activity::Moving, get2DVelocityVector().reverse() );
        dynamic_cast< FreeItem & >( getItem() ).reverseHeading() ;
}

}
