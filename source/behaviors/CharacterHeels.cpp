
#include "CharacterHeels.hpp"

#include "AvatarItem.hpp"
#include "Mediator.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"


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
        const unsigned int highJumpPhases = 21 ;
        for ( unsigned int i = 0; i < highJumpPhases; i++ )
        {
                std::pair< int /* xy */, int /* z */ > jumpPhase( 2 , ( i < 17 ) ? 3 : -3 );
                highJumpVector.push_back( jumpPhase );
        }

        // fotogramas de caída (falling)
        fallFrames[ "north" ] =  8 ;
        fallFrames[ "south" ] =  0 ;
        fallFrames[ "east" ]  = 12 ;
        fallFrames[ "west" ]  =  4 ;

        // activate chronometers
        speedTimer->go ();
        fallTimer->go ();
}

bool CharacterHeels::update ()
{
        switch ( getCurrentActivity () )
        {
                case activities::Activity::Waiting:
                        wait ();
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

void CharacterHeels::behave ()
{
        AvatarItem & avatar = dynamic_cast< AvatarItem & >( getItem() );

        Activity whatDoing = getCurrentActivity() ;

        if ( whatDoing == activities::Activity::AutomovingNorth || whatDoing == activities::Activity::AutomovingSouth ||
                whatDoing == activities::Activity::AutomovingEast || whatDoing == activities::Activity::AutomovingWest ||
                        whatDoing == activities::Activity::BeginTeletransportation || whatDoing == activities::Activity::EndTeletransportation
                                || whatDoing == activities::Activity::MetLethalItem || whatDoing == activities::Activity::Vanishing )
                return ; // moving by inertia, teleporting, or vanishing is not controlled by the player

        const InputManager & input = InputManager::getInstance ();

        // when waiting
        if ( whatDoing == activities::Activity::Waiting /* || whatDoing == activities::Activity::Blinking */ )
        {
                if ( input.takeTyped() ) {
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
                else if ( input.jumpTyped() ) {
                        toJumpOrTeleport ();
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
        else if ( whatDoing == activities::Activity::Jumping || whatDoing == activities::Activity::Falling )
        {
                // take or drop an item when jumping or falling
                if ( input.takeTyped() ) {
                        setCurrentActivity( avatar.getDescriptionOfTakenItem() == nilPointer
                                                ? activities::Activity::TakingItem
                                                : activities::Activity::DroppingItem );
                        input.releaseKeyFor( "take" );
                }
        }
}

}
