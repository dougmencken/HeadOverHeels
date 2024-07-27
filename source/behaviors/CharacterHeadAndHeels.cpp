
#include "CharacterHeadAndHeels.hpp"

#include "AvatarItem.hpp"
#include "Mediator.hpp"
#include "Falling.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"


namespace behaviors
{

CharacterHeadAndHeels::CharacterHeadAndHeels( AvatarItem & item )
        : PlayerControlled( item, "behavior of Head over Heels" )
{
        // salto
        const unsigned int jumpPhases = 28 ;
        for ( unsigned int i = 0; i < jumpPhases; i++ )
        {
                std::pair< int /* xy */, int /* z */ > jumpPhase( 1, ( i < 4 ? 4 : ( i < 8 ? 3 : 2 ) ) );
                jumpVector.push_back( jumpPhase );
        }

        // salto largo
        const unsigned int highJumpPhases = 28 ;
        for ( unsigned int i = 0; i < highJumpPhases; i++ )
        {
                std::pair< int /* xy */, int /* z */ > jumpPhase( 1, 3 );
                highJumpVector.push_back( jumpPhase );
        }

        // fotogramas de caída (falling)
        fallFrames[ "north" ] =  8 ;
        fallFrames[ "south" ] = 16 ;
        fallFrames[ "east" ]  = 12 ;
        fallFrames[ "west" ]  = 17 ;

        // fotogramas de parpadeo (blinking)
        blinkFrames[ "north" ] =  8 ;
        blinkFrames[ "south" ] = 18 ;
        blinkFrames[ "east" ]  = 12 ;
        blinkFrames[ "west" ]  = 19 ;

        // activate chronometers
        speedTimer->go();
        fallTimer->go();
        glideTimer->go();
        timerForBlinking->go();
}

bool CharacterHeadAndHeels::update ()
{
        switch ( getCurrentActivity () )
        {
                case activities::Activity::Waiting:
                        wait ();
                        break;

                case activities::Activity::Blinking:
                        blink ();
                        break;

                case activities::Activity::AutomovingNorth:
                case activities::Activity::AutomovingSouth:
                case activities::Activity::AutomovingEast:
                case activities::Activity::AutomovingWest:
                        automove ();
                        break;

                case activities::Activity::MovingNorth:
                case activities::Activity::MovingSouth:
                case activities::Activity::MovingEast:
                case activities::Activity::MovingWest:
                        move ();
                        break;

                case activities::Activity::PushedNorth:
                case activities::Activity::PushedSouth:
                case activities::Activity::PushedEast:
                case activities::Activity::PushedWest:
                case activities::Activity::PushedNortheast:
                case activities::Activity::PushedSoutheast:
                case activities::Activity::PushedSouthwest:
                case activities::Activity::PushedNorthwest:
                case activities::Activity::DraggedNorth:
                case activities::Activity::DraggedSouth:
                case activities::Activity::DraggedEast:
                case activities::Activity::DraggedWest:
                        displace ();
                        break;

                case activities::Activity::CancelDragging:
                        cancelDragging ();
                        break;

                case activities::Activity::Falling:
                        fall ();
                        break;

                case activities::Activity::Gliding:
                        glide ();
                        break;

                case activities::Activity::Jumping :
                        jump ();
                        break;

                case activities::Activity::BeginTeletransportation:
                        enterTeletransport ();
                        break;
                case activities::Activity::EndTeletransportation:
                        exitTeletransport ();
                        break;

                case activities::Activity::MetLethalItem:
                case activities::Activity::Vanishing:
                        collideWithALethalItem ();
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

void CharacterHeadAndHeels::behave ()
{
        AvatarItem & avatar = dynamic_cast< AvatarItem & >( getItem() );

        Activity whatDoing = getCurrentActivity() ;

        if ( whatDoing == activities::Activity::AutomovingNorth || whatDoing == activities::Activity::AutomovingSouth ||
                whatDoing == activities::Activity::AutomovingEast || whatDoing == activities::Activity::AutomovingWest ||
                        whatDoing == activities::Activity::BeginTeletransportation || whatDoing == activities::Activity::EndTeletransportation
                                || whatDoing == activities::Activity::MetLethalItem || whatDoing == activities::Activity::Vanishing )
                return ; // moving by inertia, teleporting, or vanishing is not controlled by the player

        const InputManager & input = InputManager::getInstance ();

        // when waiting or blinking
        if ( whatDoing == activities::Activity::Waiting || whatDoing == activities::Activity::Blinking )
        {
                if ( input.jumpTyped() ) {
                        toJumpOrTeleport ();
                }
                else if ( input.doughnutTyped() ) {
                        useHooter ();
                        input.releaseKeyFor( "doughnut" );
                }
                else if ( input.takeTyped() ) {
                        setCurrentActivity( avatar.getDescriptionOfTakenItem() == nilPointer
                                                ? activities::Activity::TakingItem
                                                : activities::Activity::DroppingItem );
                        input.releaseKeyFor( "take" );
                }
                else if ( input.takeAndJumpTyped() ) {
                        setCurrentActivity( avatar.getDescriptionOfTakenItem() == nilPointer
                                                ? activities::Activity::TakeAndJump
                                                : activities::Activity::DropAndJump );
                        input.releaseKeyFor( "take&jump" );
                }
                else {
                        moveKeySetsActivity () ;
                }
        }
        // already moving
        else if ( whatDoing == activities::Activity::MovingNorth || whatDoing == activities::Activity::MovingSouth
                        || whatDoing == activities::Activity::MovingEast || whatDoing == activities::Activity::MovingWest )
        {
                if ( input.jumpTyped() ) {
                        toJumpOrTeleport ();
                }
                else if ( input.doughnutTyped() ) {
                        useHooter ();
                        input.releaseKeyFor( "doughnut" );
                }
                else if ( input.takeTyped() ) {
                        setCurrentActivity( avatar.getDescriptionOfTakenItem() == nilPointer
                                                ? activities::Activity::TakingItem
                                                : activities::Activity::DroppingItem );
                        input.releaseKeyFor( "take" );
                }
                else if ( input.takeAndJumpTyped() ) {
                        setCurrentActivity( avatar.getDescriptionOfTakenItem() == nilPointer
                                                ? activities::Activity::TakeAndJump
                                                : activities::Activity::DropAndJump );
                        input.releaseKeyFor( "take&jump" );
                }
                else if ( ! moveKeySetsActivity () ) {
                        // not moving is waiting
                        SoundManager::getInstance().stop( avatar.getOriginalKind(), SoundManager::activityToNameOfSound( whatDoing ) );
                        setCurrentActivity( activities::Activity::Waiting );
                }
        }
        // being pushed
        else if ( whatDoing == activities::Activity::PushedNorth || whatDoing == activities::Activity::PushedSouth ||
                        whatDoing == activities::Activity::PushedEast || whatDoing == activities::Activity::PushedWest )
        {
                if ( input.jumpTyped() ) {
                        setCurrentActivity( activities::Activity::Jumping );
                }
                else if ( input.doughnutTyped() ) {
                        useHooter ();
                        input.releaseKeyFor( "doughnut" );
                }
                else if ( input.takeTyped() ) {
                        setCurrentActivity( avatar.getDescriptionOfTakenItem() == nilPointer
                                                ? activities::Activity::TakingItem
                                                : activities::Activity::DroppingItem );
                        input.releaseKeyFor( "take" );
                }
                else if ( input.takeAndJumpTyped() ) {
                        setCurrentActivity( avatar.getDescriptionOfTakenItem() == nilPointer
                                                ? activities::Activity::TakeAndJump
                                                : activities::Activity::DropAndJump );
                        input.releaseKeyFor( "take&jump" );
                }
                else {
                        moveKeySetsActivity () ;
                }
        }
        // dragged by a conveyor
        else if ( whatDoing == activities::Activity::DraggedNorth || whatDoing == activities::Activity::DraggedSouth ||
                        whatDoing == activities::Activity::DraggedEast || whatDoing == activities::Activity::DraggedWest )
        {
                if ( input.jumpTyped() ) {
                        setCurrentActivity( activities::Activity::Jumping );
                }
                else {
                        handleMoveKeyWhenDragged () ;
                }
        }
        else if ( whatDoing == activities::Activity::Jumping )
        {
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
                if ( input.doughnutTyped() ) {
                        useHooter ();
                        input.releaseKeyFor( "doughnut" );
                }
                // pick or drop an item when falling
                else if ( input.takeTyped() ) {
                        setCurrentActivity( avatar.getDescriptionOfTakenItem() == nilPointer
                                                ? activities::Activity::TakingItem
                                                : activities::Activity::DroppingItem );
                        input.releaseKeyFor( "take" );
                }
                // entonces Head y Heels planean
                else if ( moveKeyChangesHeading() ) {
                        setCurrentActivity( activities::Activity::Gliding );
                }
                else
                        resetHowLongFalls (); // don’t accelerate falling
        }

        // for gliding don’t wait for the next cycle because gliding can happen just after falling, and
        // waiting for the next cycle may take away the chance of entering the gap between two items
        if ( getCurrentActivity() == activities::Activity::Gliding )
        {
                if ( input.doughnutTyped() ) {
                        useHooter ();
                        input.releaseKeyFor( "doughnut" );
                }
                else if ( input.takeTyped() ) {
                        // pick or drop an item when gliding
                        setCurrentActivity( avatar.getDescriptionOfTakenItem() == nilPointer
                                                ? activities::Activity::TakingItem
                                                : activities::Activity::DroppingItem );
                        input.releaseKeyFor( "take" );
                }
                else if ( ! moveKeyChangesHeading () ) {
                        setCurrentActivity( activities::Activity::Falling );
                }
        }
}

void CharacterHeadAndHeels::wait ()
{
        PlayerControlled::wait() ;

        if ( getCurrentActivity() == activities::Activity::Waiting ) {
                if ( timerForBlinking->getValue() >= ( rand() % 4 ) + 5 )
                {
                        timerForBlinking->go() ;
                        setCurrentActivity( activities::Activity::Blinking );
                }
        }
}

void CharacterHeadAndHeels::blink ()
{
        double time = timerForBlinking->getValue() ;

        // eyes closed
        if ( ( time > 0.0 && time < 0.050 ) || ( time > 0.400 && time < 0.450 ) ) {
                AvatarItem & avatar = dynamic_cast< AvatarItem & >( getItem () );
                avatar.changeFrame( blinkFrames[ avatar.getHeading() ] );
        }
        // eyes open
        else if ( ( time > 0.250 && time < 0.300 ) || ( time > 0.750 && time < 0.800 ) ) {
        }
        // end of blinking
        else if ( time > 0.800 ) {
                timerForBlinking->go() ;
                setCurrentActivity( activities::Activity::Waiting );
        }
}

}
