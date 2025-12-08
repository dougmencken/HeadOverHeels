
#include "SpringStool.hpp"

#include "FreeItem.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"


namespace behaviors
{

SpringStool::SpringStool( FreeItem & item, const std::string & behavior )
        : Behavior( item, behavior )
        , folded( false )
        , rebounding( false )
        , speedTimer( new Timer() )
        , fallTimer( new Timer() )
        , reboundTimer( new Timer() )
{
        speedTimer->go() ;
        fallTimer->go() ;
        reboundTimer->go() ;
}

bool SpringStool::update ()
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
                                springItem.changeFrameInTheCurrentSequence( folded_frame );
                        }
                        else
                        {
                                // the spring continues to bounce after unloading
                                if ( this->rebounding && reboundTimer->get() < 0.600 )
                                {
                                        springItem.animate() ;

                                        // play the sound of bouncing
                                        if ( reboundTimer->get() > 0.100 )
                                                SoundManager::getInstance().play( springItem.getKind(), "bounce" );
                                }
                                else {
                                        // begin bouncing when an item above moves away
                                        if ( this->folded ) {
                                                this->rebounding = true ;
                                                reboundTimer->go() ;
                                        }

                                        // folded no longer
                                        this->folded = false ;

                                        springItem.changeFrameInTheCurrentSequence( unstrained_frame );
                                }
                        }

                        if ( activities::Falling::getInstance().fall( *this ) ) {
                                // it falls down
                                fallTimer->go() ;
                                setCurrentActivity( activities::Activity::Falling, Motion2D::rest() );
                        }

                        break;

                case activities::Activity::Pushed :
                        // is it time to move
                        if ( speedTimer->get() > springItem.getSpeed() ) {
                                // play the sound of pushing
                                SoundManager::getInstance().play( springItem.getKind(), "push" );

                                activities::Displacing::getInstance().displace( *this, true );

                                if ( getCurrentActivity() != activities::Activity::Falling )
                                        beWaiting() ;

                                speedTimer->go() ;
                        }
                        break ;

                case activities::Activity::Falling:
                        if ( springItem.getZ() == 0 && ! springItem.getMediator()->getRoom()->hasFloor() ) {
                                // disappear when reached the bottom of a room without floor
                                present = false ;
                        }
                        // is it time to fall
                        else if ( fallTimer->get() > springItem.getWeight() )
                        {
                                if ( ! activities::Falling::getInstance().fall( *this ) ) {
                                        // play the end of falling sound
                                        SoundManager::getInstance().play( springItem.getKind (), "fall" );
                                        beWaiting() ;
                                }

                                fallTimer->go() ;
                        }
                        break;

                ////case activities::Activity::Vanishing:
                        ////present = false ;
                        ////break;

                default:
                        ;
        }

        return present ;
}

}
