
#include "Trampoline.hpp"

#include "FreeItem.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"


namespace behaviors
{

Trampoline::Trampoline( FreeItem & item, const std::string & behavior )
        : Behavior( item, behavior )
        , folded( false )
        , rebounding( false )
        , unstrainedFrame( 0 )
        , foldedFrame( 1 )
        , speedTimer( new Timer() )
        , fallTimer( new Timer() )
        , reboundTimer( new Timer() )
{
        speedTimer->go() ;
        fallTimer->go() ;
        reboundTimer->go() ;
}

bool Trampoline::update ()
{
        FreeItem & springItem = dynamic_cast< FreeItem & >( getItem () );

        bool present = true ;

        switch ( getCurrentActivity() )
        {
                case activities::Activity::Waiting:
                        // is there anything above
                        if ( ! springItem.canAdvanceTo( 0, 0, 1 ) )
                        {
                                this->folded = true ;
                                this->rebounding = false ;
                                springItem.changeFrame( this->foldedFrame );
                        }
                        else
                        {
                                // the spring continues to bounce after unloading
                                if ( this->rebounding && reboundTimer->getValue() < 0.600 )
                                {
                                        springItem.animate() ;

                                        // play the sound of bouncing
                                        if ( reboundTimer->getValue() > 0.100 )
                                                SoundManager::getInstance().play( springItem.getKind(), "function" );
                                }
                                else
                                {
                                        // begin bouncing when an item above moves away
                                        if ( this->folded ) {
                                                this->rebounding = true ;
                                                reboundTimer->go() ;
                                        }

                                        // folded no longer
                                        this->folded = false ;

                                        springItem.changeFrame( this->unstrainedFrame );
                                }
                        }

                        if ( activities::Falling::getInstance().fall( *this ) ) {
                                // it falls down
                                fallTimer->go() ;
                                setCurrentActivity( activities::Activity::Falling );
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
                        // is it time to move
                        if ( speedTimer->getValue() > springItem.getSpeed() ) {
                                // play the sound of pushing
                                SoundManager::getInstance().play( springItem.getKind(), "push" );

                                activities::Displacing::getInstance().displace( *this, true );

                                if ( getCurrentActivity() != activities::Activity::Falling )
                                        setCurrentActivity( activities::Activity::Waiting );

                                speedTimer->go() ;
                        }
                        break;

                case activities::Activity::Falling:
                        if ( springItem.getZ() == 0 && ! springItem.getMediator()->getRoom()->hasFloor() ) {
                                // disappear when reached the bottom of a room without floor
                                present = false ;
                        }
                        // is it time to fall
                        else if ( fallTimer->getValue() > springItem.getWeight() )
                        {
                                if ( ! activities::Falling::getInstance().fall( *this ) ) {
                                        // play the end of falling sound
                                        SoundManager::getInstance().play( springItem.getKind (), "fall" );
                                        setCurrentActivity( activities::Activity::Waiting );
                                }

                                fallTimer->go() ;
                        }
                        break;

                case activities::Activity::Vanishing:
                        present = false ;
                        break;

                default:
                        ;
        }

        return present ;
}

}
