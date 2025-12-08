
#include "CharacterHead.hpp"

#include "AvatarItem.hpp"
#include "Mediator.hpp"
#include "Falling.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"
#include "ActivityToString.hpp"


namespace behaviors
{

CharacterHead::CharacterHead( AvatarItem & item )
        : PlayerControlled( item, "behavior of Head" )
{
        // salto
        const unsigned int jumpPhases = 20 ;
        for ( unsigned int i = 0; i < jumpPhases; i++ )
        {
                jumpVector.push_back( std::pair< int /* xy */, int /* z */ >( 1, 3 ) );
        }

        // salto largo
        const unsigned int bigJumpPhases = 19 ;
        for ( unsigned int i = 0; i < bigJumpPhases; i++ )
        {
                bigJumpVector.push_back( std::pair< int /* xy */, int /* z */ >( 1, 4 ) );
        }

        // fotogramas de caída (falling)
        fallFrames[ "south" ] = std::pair< std::string, unsigned int >( "extra", 0 );
        fallFrames[ "west" ]  = std::pair< std::string, unsigned int >( "extra", 1 );
        fallFrames[ "north" ] = std::pair< std::string, unsigned int >( "north", 0 );
        fallFrames[ "east" ]  = std::pair< std::string, unsigned int >( "east", 0 );

        // fotogramas de parpadeo (blinking)
        blinkFrames[ "south" ] = std::pair< std::string, unsigned int >( "extra", 3 );
        blinkFrames[ "west" ]  = std::pair< std::string, unsigned int >( "extra", 4 );
        blinkFrames[ "north" ] = std::pair< std::string, unsigned int >( "north", 0 );
        blinkFrames[ "east" ]  = std::pair< std::string, unsigned int >( "east", 0 );

        // activate chronometers
        speedTimer->go ();
        fallTimer->go ();
        glideTimer->go ();
        timerForBlinking->go ();
}

bool CharacterHead::update ()
{
        // change the height for climbing bars easier
        dynamic_cast< ::DescribedItem & >( getItem() ).changeHeightTo (
                        ( getCurrentActivity() == activities::Activity::Gliding ||
                                        getCurrentActivity() == activities::Activity::Falling )
                        ? 23 : 24 );

#if defined( DEBUG_ACTIVITIES ) || defined( DEBUG_WAITING )
        std::string aboutActivity = "Head is " + activities::ActivityToString::toString( getCurrentActivity() ) + " on update()" ;
        std::string aboutActivityAndMotionVector = aboutActivity + " and the motion vector is " + get2DVelocityVector().toString() ;
#endif

        switch ( getCurrentActivity () )
        {
                case activities::Activity::Waiting :
        #if defined( DEBUG_WAITING ) && DEBUG_WAITING
                        std::cout << aboutActivity << std::endl ;
        #endif
                        wait ();
                        break;

                case activities::Activity::Automoving :
        #if defined( DEBUG_ACTIVITIES ) && DEBUG_ACTIVITIES
                        std::cout << aboutActivityAndMotionVector << std::endl ;
        #endif
                        automove ();
                        break;

                case activities::Activity::Moving :
        #if defined( DEBUG_ACTIVITIES ) && DEBUG_ACTIVITIES
                        std::cout << aboutActivityAndMotionVector << std::endl ;
        #endif
                        move ();
                        break;

                case activities::Activity::Pushed :
                case activities::Activity::Dragged :
        #if defined( DEBUG_ACTIVITIES ) && DEBUG_ACTIVITIES
                        std::cout << aboutActivityAndMotionVector << std::endl ;
        #endif
                        displace ();
                        break ;

                case activities::Activity::Falling :
        #if defined( DEBUG_ACTIVITIES ) && DEBUG_ACTIVITIES
                        std::cout << aboutActivity << std::endl ;
        #endif
                        fall ();
                        break;

                case activities::Activity::Jumping :
        #if defined( DEBUG_ACTIVITIES ) && DEBUG_ACTIVITIES
                        std::cout << aboutActivity << std::endl ;
        #endif
                        jump ();
                        break;

                case activities::Activity::Gliding :
        #if defined( DEBUG_ACTIVITIES ) && DEBUG_ACTIVITIES
                        std::cout << aboutActivity << std::endl ;
        #endif
                        glide ();
                        break;

                case activities::Activity::Blinking :
        #if defined( DEBUG_WAITING ) && DEBUG_WAITING
                        std::cout << aboutActivity << std::endl ;
        #endif
                        blink ();
                        break;

                default:
                        ;
        }

        return PlayerControlled::update() ;
}

void CharacterHead::behave ()
{
        AvatarItem & avatar = dynamic_cast< AvatarItem & >( getItem() );

        Activity whatDoing = getCurrentActivity() ;

        if ( whatDoing == activities::Activity::Automoving ||
                        whatDoing == activities::Activity::BeginTeletransportation || whatDoing == activities::Activity::EndTeletransportation
                                || whatDoing == activities::Activity::MetLethalItem || whatDoing == activities::Activity::Vanishing )
                return ; // moving by inertia, teleporting, or vanishing is not controlled by the player

#if defined( DEBUG_ACTIVITIES ) || defined( DEBUG_WAITING )
        std::string aboutActivity = "Head is " + activities::ActivityToString::toString( getCurrentActivity() ) + " on behave()" ;
        std::string aboutActivityAndMotionVector = aboutActivity + " with the motion vector " + get2DVelocityVector().toString() ;
#endif

        const InputManager & input = InputManager::getInstance ();

        // when waiting or blinking
        if ( whatDoing == activities::Activity::Waiting || whatDoing == activities::Activity::Blinking )
        {
        #if defined( DEBUG_WAITING ) && DEBUG_WAITING
                std::cout << aboutActivity << std::endl ;
        #endif
                if ( input.jumpTyped() ) {
                        toJumpOrTeleport ();
                }
                else if ( input.doughnutTyped() ) {
                        useHooter ();
                        input.releaseKeyFor( "doughnut" );
                }
                else {
                        moveKeySetsActivity () ;
                }
        }
        // already moving
        else if ( whatDoing == activities::Activity::Moving )
        {
        #if defined( DEBUG_ACTIVITIES ) && DEBUG_ACTIVITIES
                std::cout << aboutActivityAndMotionVector << std::endl ;
        #endif
                if ( input.jumpTyped() ) {
                        toJumpOrTeleport ();
                }
                else if ( input.doughnutTyped() ) {
                        useHooter ();
                        input.releaseKeyFor( "doughnut" );
                }
                else if ( ! moveKeySetsActivity () ) {
                        // not moving is waiting
                        SoundManager::getInstance().stop( avatar.getOriginalKind(), SoundManager::activityToNameOfSound( whatDoing ) );
                        beWaiting() ;
                }
        }
        // being pushed
        else if ( whatDoing == activities::Activity::Pushed )
        {
        #if defined( DEBUG_ACTIVITIES ) && DEBUG_ACTIVITIES
                std::cout << aboutActivityAndMotionVector << std::endl ;
        #endif
                if ( input.jumpTyped() ) {
                        setCurrentActivity( activities::Activity::Jumping, Motion2D::rest() );
                }
                else if ( input.doughnutTyped() ) {
                        useHooter ();
                        input.releaseKeyFor( "doughnut" );
                }
                else {
                        moveKeySetsActivity () ;
                }
        }
        // dragged by a conveyor
        else if ( whatDoing == activities::Activity::Dragged )
        {
        #if defined( DEBUG_ACTIVITIES ) && DEBUG_ACTIVITIES
                std::cout << aboutActivityAndMotionVector << std::endl ;
        #endif
                if ( input.jumpTyped() ) {
                        setCurrentActivity( activities::Activity::Jumping, Motion2D::rest() );
                }
                else {
                        handleMoveKeyWhenDragged () ;
                }
        }
        else if ( whatDoing == activities::Activity::Jumping )
        {
        #if defined( DEBUG_ACTIVITIES ) && DEBUG_ACTIVITIES
                std::cout << aboutActivity << std::endl ;
        #endif
                if ( input.doughnutTyped() ) {
                        useHooter ();
                        input.releaseKeyFor( "doughnut" );
                }
                else {
                        moveKeyChangesHeading () ;
                }
        }
        else if ( whatDoing == activities::Activity::Falling )
        {
        #if defined( DEBUG_ACTIVITIES ) && DEBUG_ACTIVITIES
                std::cout << aboutActivity << std::endl ;
        #endif
                if ( input.doughnutTyped() ) {
                        useHooter ();
                        input.releaseKeyFor( "doughnut" );
                }
                // entonces Head planea
                else if ( moveKeyChangesHeading() ) {
                        setCurrentActivity( activities::Activity::Gliding, Motion2D::rest() );
                }
                else
                        resetHowLongFalls (); // don’t accelerate falling
        }

        // for gliding don’t wait for the next cycle because gliding can happen just after falling, and
        // waiting for the next cycle may take away the chance of entering the gap between two items
        if ( getCurrentActivity() == activities::Activity::Gliding )
        {
        #if defined( DEBUG_ACTIVITIES ) && DEBUG_ACTIVITIES
                std::cout << aboutActivity << std::endl ;
        #endif
                if ( input.doughnutTyped() ) {
                        useHooter ();
                        input.releaseKeyFor( "doughnut" );
                }
                else if ( ! moveKeyChangesHeading () ) {
                        setCurrentActivity( activities::Activity::Falling, Motion2D::rest() );
                }
        }
}

void CharacterHead::wait ()
{
        PlayerControlled::wait() ;

        if ( getCurrentActivity() == activities::Activity::Waiting ) {
                if ( timerForBlinking->get() >= ( rand() % 4 ) + 5 )
                {
                        timerForBlinking->go() ;
                        setCurrentActivity( activities::Activity::Blinking, Motion2D::rest() );
                }
        }
}

void CharacterHead::blink ()
{
        double time = timerForBlinking->get() ;

        // eyes closed
        if ( ( time > 0.0 && time < 0.050 ) || ( time > 0.400 && time < 0.450 ) ) {
                AvatarItem & avatar = dynamic_cast< AvatarItem & >( getItem () );
                avatar.setSequenceAndFrame( blinkFrames[ avatar.getHeading() ].first,
                                                blinkFrames[ avatar.getHeading() ].second );
        }
        // eyes open
        else if ( ( time > 0.250 && time < 0.300 ) || ( time > 0.750 && time < 0.800 ) ) {
                FreeItem & item = dynamic_cast< FreeItem & >( getItem () );
                item.toTheHeadingFrameSequence() ;
        }
        // end of blinking
        else if ( time > 0.800 ) {
                timerForBlinking->go() ;
                beWaiting() ;
        }
}

}
