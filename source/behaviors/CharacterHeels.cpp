
#include "CharacterHeels.hpp"

#include "AvatarItem.hpp"
#include "Mediator.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"
#include "ActivityToString.hpp"


namespace behaviors
{

CharacterHeels::CharacterHeels( AvatarItem & item )
        : PlayerControlled( item, "behavior of Heels" )
{
        // salto
        const unsigned int jumpPhases = 20 ;
        for ( unsigned int i = 0; i < jumpPhases; i++ )
        {
                std::pair< int /* xy */, int /* z */ > jumpPhase( i == 9 || i == 19 ? 2 : 1 , ( i < jumpPhases / 2 ) ? 3 : -3 );
                jumpVector.push_back( jumpPhase );
        }

        // salto largo
        const unsigned int bigJumpPhases = 21 ;
        for ( unsigned int i = 0; i < bigJumpPhases; i++ )
        {
                std::pair< int /* xy */, int /* z */ > jumpPhase( 2 , ( i < 17 ) ? 3 : -3 );
                bigJumpVector.push_back( jumpPhase );
        }

        // fotogramas de caída (falling)
        fallFrames[ "south" ] = std::pair< std::string, unsigned int >( "south", 0 );
        fallFrames[ "west" ]  = std::pair< std::string, unsigned int >( "west", 0 );
        fallFrames[ "north" ] = std::pair< std::string, unsigned int >( "north", 0 );
        fallFrames[ "east" ]  = std::pair< std::string, unsigned int >( "east", 0 );

        // activate chronometers
        speedTimer->go ();
        fallTimer->go ();
}

bool CharacterHeels::update ()
{
#if defined( DEBUG_ACTIVITIES ) || defined( DEBUG_WAITING )
        std::string aboutActivity = "Heels is " + activities::ActivityToString::toString( getCurrentActivity() ) + " on update()" ;
        std::string aboutActivityAndMotionVector = aboutActivity + " and the motion vector is " + get2DVelocityVector().toString() ;
#endif

        switch ( getCurrentActivity () )
        {
                case activities::Activity::Waiting:
        #if defined( DEBUG_WAITING ) && DEBUG_WAITING
                        std::cout << aboutActivity << std::endl ;
        #endif
                        wait ();
                        break;

                case activities::Activity::Automoving:
        #if defined( DEBUG_ACTIVITIES ) && DEBUG_ACTIVITIES
                        std::cout << aboutActivityAndMotionVector << std::endl ;
        #endif
                        automove ();
                        break;

                case activities::Activity::Moving:
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

                case activities::Activity::Falling:
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

                case activities::Activity::TakingItem:
                case activities::Activity::TakeAndJump:
                        takeItem ();
                        break;

                case activities::Activity::DroppingItem:
                case activities::Activity::DropAndJump:
                        dropItem ();
                        break;

                default:
                        ;
        }

        return PlayerControlled::update() ;
}

void CharacterHeels::behave ()
{
        AvatarItem & avatar = dynamic_cast< AvatarItem & >( getItem() );

        Activity whatDoing = getCurrentActivity() ;

        if ( whatDoing == activities::Activity::Automoving ||
                        whatDoing == activities::Activity::BeginTeletransportation || whatDoing == activities::Activity::EndTeletransportation
                                || whatDoing == activities::Activity::MetLethalItem || whatDoing == activities::Activity::Vanishing )
                return ; // moving by inertia, teleporting, or vanishing is not controlled by the player

#if defined( DEBUG_ACTIVITIES ) || defined( DEBUG_WAITING )
        std::string aboutActivity = "Heels is " + activities::ActivityToString::toString( getCurrentActivity() ) + " on behave()" ;
        std::string aboutActivityAndMotionVector = aboutActivity + " with the motion vector " + get2DVelocityVector().toString() ;
#endif

        const InputManager & input = InputManager::getInstance ();
        bool noItemTaken = ( avatar.getDescriptionOfTakenItem() == nilPointer );

        // when waiting
        if ( whatDoing == activities::Activity::Waiting /* || whatDoing == activities::Activity::Blinking */ )
        {
        #if defined( DEBUG_WAITING ) && DEBUG_WAITING
                std::cout << aboutActivity << std::endl ;
        #endif
                if ( input.takeTyped() ) {
                        setCurrentActivity( noItemTaken ? activities::Activity::TakingItem : activities::Activity::DroppingItem, Motion2D::rest() );
                        input.releaseKeyFor( "take" );
                }
                else if ( input.takeAndJumpTyped() ) {
                        setCurrentActivity( noItemTaken ? activities::Activity::TakeAndJump : activities::Activity::DropAndJump, Motion2D::rest() );
                        input.releaseKeyFor( "take&jump" );
                }
                else if ( input.jumpTyped() ) {
                        toJumpOrTeleport ();
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
                else if ( input.takeTyped() ) {
                        setCurrentActivity( noItemTaken ? activities::Activity::TakingItem : activities::Activity::DroppingItem, Motion2D::rest() );
                        input.releaseKeyFor( "take" );
                }
                else if ( input.takeAndJumpTyped() ) {
                        setCurrentActivity( noItemTaken ? activities::Activity::TakeAndJump : activities::Activity::DropAndJump, Motion2D::rest() );
                        input.releaseKeyFor( "take&jump" );
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
                else if ( input.takeTyped() ) {
                        setCurrentActivity( noItemTaken ? activities::Activity::TakingItem : activities::Activity::DroppingItem, Motion2D::rest() );
                        input.releaseKeyFor( "take" );
                }
                else if ( input.takeAndJumpTyped() ) {
                        setCurrentActivity( noItemTaken ? activities::Activity::TakeAndJump : activities::Activity::DropAndJump, Motion2D::rest() );
                        input.releaseKeyFor( "take&jump" );
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
                if ( input.jumpTyped() )
                        setCurrentActivity( activities::Activity::Jumping, Motion2D::rest() );
                else
                        handleMoveKeyWhenDragged () ;
        }
        else if ( whatDoing == activities::Activity::Jumping || whatDoing == activities::Activity::Falling )
        {
        #if defined( DEBUG_ACTIVITIES ) && DEBUG_ACTIVITIES
                std::cout << aboutActivity << std::endl ;
        #endif
                // take or drop an item when jumping or falling
                if ( input.takeTyped() ) {
                        setCurrentActivity( noItemTaken ? activities::Activity::TakingItem : activities::Activity::DroppingItem, Motion2D::rest() );
                        input.releaseKeyFor( "take" );
                }
        }
}

}
