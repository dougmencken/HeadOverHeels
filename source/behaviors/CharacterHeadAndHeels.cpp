
#include "CharacterHeadAndHeels.hpp"

#include "AvatarItem.hpp"
#include "Mediator.hpp"
#include "Falling.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"


namespace behaviors
{

CharacterHeadAndHeels::CharacterHeadAndHeels( const ItemPtr & item, const std::string & behavior )
        : PlayerControlled( item, behavior )
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
        fallFrames[ "north" ] = 8;
        fallFrames[ "south" ] = 16;
        fallFrames[ "east" ] = 12;
        fallFrames[ "west" ] = 17;

        // fotogramas de parpadeo (blinking)
        blinkFrames[ "north" ] = 8;
        blinkFrames[ "south" ] = 18;
        blinkFrames[ "east" ] = 12;
        blinkFrames[ "west" ] = 19;

        kindOfBubbles = "double-bubbles";

        // activate chronometers
        speedTimer->go();
        fallTimer->go();
        glideTimer->go();
        timerForBlinking->go();
}

CharacterHeadAndHeels::~CharacterHeadAndHeels( )
{
}

bool CharacterHeadAndHeels::update ()
{
        AvatarItem & avatar = dynamic_cast< AvatarItem & >( * this->item );

        if ( avatar.hasShield() ) avatar.decrementShieldOverTime () ;

        switch ( getCurrentActivity () )
        {
                case activities::Activity::Waiting:
                        wait( avatar );
                        break;

                case activities::Activity::AutomovingNorth:
                case activities::Activity::AutomovingSouth:
                case activities::Activity::AutomovingEast:
                case activities::Activity::AutomovingWest:
                        automove( avatar );
                        break;

                case activities::Activity::MovingNorth:
                case activities::Activity::MovingSouth:
                case activities::Activity::MovingEast:
                case activities::Activity::MovingWest:
                        move( avatar );
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
                        displace( avatar );
                        break;

                case activities::Activity::CancelDragging:
                        cancelDragging( avatar );
                        break;

                case activities::Activity::Falling:
                        fall( avatar );
                        break;

                case activities::Activity::Jumping :
                        jump( avatar );
                        break;

                case activities::Activity::BeginTeletransportation:
                        enterTeletransport( avatar );
                        break;
                case activities::Activity::EndTeletransportation:
                        exitTeletransport( avatar );
                        break;

                case activities::Activity::MetLethalItem:
                case activities::Activity::Vanishing:
                        collideWithALethalItem( avatar );
                        break;

                case activities::Activity::Gliding:
                        glide( avatar );
                        break;

                case activities::Activity::Blinking:
                        blink( avatar );
                        break;

                case activities::Activity::TakeItem:
                case activities::Activity::TakeAndJump:
                        takeItem( avatar );
                        break;

                case activities::Activity::ItemTaken:
                        avatar.addToZ( - Room::LayerHeight );
                        activity = activities::Activity::Waiting;
                        break;

                case activities::Activity::DropItem:
                case activities::Activity::DropAndJump:
                        dropItem( avatar );
                        break;

                default:
                        ;
        }

        // play sound for the current activity
        SoundManager::getInstance().play( avatar.getOriginalKind(), SoundManager::activityToNameOfSound ( getCurrentActivity() ) );

        return false ;
}

void CharacterHeadAndHeels::behave ()
{
        AvatarItem & avatar = dynamic_cast< AvatarItem & >( * this->item );
        const InputManager & input = InputManager::getInstance() ;

        // if it’s not a move by inertia or some other exotic activity
        if ( activity != activities::Activity::AutomovingNorth && activity != activities::Activity::AutomovingSouth
                        && activity != activities::Activity::AutomovingEast && activity != activities::Activity::AutomovingWest
                        && activity != activities::Activity::BeginTeletransportation && activity != activities::Activity::EndTeletransportation
                        && activity != activities::Activity::MetLethalItem && activity != activities::Activity::Vanishing )
        {
                // when waiting or blinking
                if ( activity == activities::Activity::Waiting || activity == activities::Activity::Blinking )
                {
                        if ( input.jumpTyped() )
                        {
                                // is there teleport below
                                avatar.canAdvanceTo( 0, 0, -1 );
                                activity =
                                        avatar.getMediator()->collisionWithBehavingAs( "behavior of teletransport" ) != nilPointer ?
                                                activities::Activity::BeginTeletransportation : activities::Activity::Jumping ;
                        }
                        else if ( input.doughnutTyped() )
                        {
                                useHooter( avatar );
                                input.releaseKeyFor( "doughnut" );
                        }
                        else if ( input.takeTyped() )
                        {
                                activity = ( avatar.getDescriptionOfTakenItem() == nilPointer ? activities::Activity::TakeItem : activities::Activity::DropItem );
                                input.releaseKeyFor( "take" );
                        }
                        else if ( input.takeAndJumpTyped() )
                        {
                                activity = ( avatar.getDescriptionOfTakenItem() == nilPointer ? activities::Activity::TakeAndJump : activities::Activity::DropAndJump );
                                input.releaseKeyFor( "take&jump" );
                        }
                        else if ( input.movenorthTyped() )
                        {
                                activity = activities::Activity::MovingNorth;
                        }
                        else if ( input.movesouthTyped() )
                        {
                                activity = activities::Activity::MovingSouth;
                        }
                        else if ( input.moveeastTyped() )
                        {
                                activity = activities::Activity::MovingEast;
                        }
                        else if ( input.movewestTyped() )
                        {
                                activity = activities::Activity::MovingWest;
                        }
                }
                // when already moving
                else if ( activity == activities::Activity::MovingNorth || activity == activities::Activity::MovingSouth ||
                        activity == activities::Activity::MovingEast || activity == activities::Activity::MovingWest )
                {
                        if ( input.jumpTyped() )
                        {
                                // jump or teleport
                                avatar.canAdvanceTo( 0, 0, -1 );
                                activity =
                                        avatar.getMediator()->collisionWithBehavingAs( "behavior of teletransport" ) != nilPointer ?
                                                activities::Activity::BeginTeletransportation : activities::Activity::Jumping ;
                        }
                        else if ( input.doughnutTyped() )
                        {
                                useHooter( avatar );
                                input.releaseKeyFor( "doughnut" );
                        }
                        else if ( input.takeTyped() )
                        {
                                activity = ( avatar.getDescriptionOfTakenItem() == nilPointer ? activities::Activity::TakeItem : activities::Activity::DropItem );
                                input.releaseKeyFor( "take" );
                        }
                        else if ( input.takeAndJumpTyped() )
                        {
                                activity = ( avatar.getDescriptionOfTakenItem() == nilPointer ? activities::Activity::TakeAndJump : activities::Activity::DropAndJump );
                                input.releaseKeyFor( "take&jump" );
                        }
                        else if ( input.movenorthTyped() )
                        {
                                activity = activities::Activity::MovingNorth;
                        }
                        else if ( input.movesouthTyped() )
                        {
                                activity = activities::Activity::MovingSouth;
                        }
                        else if ( input.moveeastTyped() )
                        {
                                activity = activities::Activity::MovingEast;
                        }
                        else if ( input.movewestTyped() )
                        {
                                activity = activities::Activity::MovingWest;
                        }
                        else if ( ! input.anyMoveTyped() )
                        {
                                SoundManager::getInstance().stop( avatar.getOriginalKind(), SoundManager::activityToNameOfSound( activity ) );
                                activity = activities::Activity::Waiting ;
                        }
                }
                // the character is being displaced
                else if ( activity == activities::Activity::PushedNorth || activity == activities::Activity::PushedSouth ||
                        activity == activities::Activity::PushedEast || activity == activities::Activity::PushedWest )
                {
                        if ( input.jumpTyped() )
                        {
                                activity = activities::Activity::Jumping ;
                        }
                        else if ( input.doughnutTyped() )
                        {
                                useHooter( avatar );
                                input.releaseKeyFor( "doughnut" );
                        }
                        else if ( input.takeTyped() )
                        {
                                activity = ( avatar.getDescriptionOfTakenItem() == nilPointer ? activities::Activity::TakeItem : activities::Activity::DropItem );
                                input.releaseKeyFor( "take" );
                        }
                        else if ( input.takeAndJumpTyped() )
                        {
                                activity = ( avatar.getDescriptionOfTakenItem() == nilPointer ? activities::Activity::TakeAndJump : activities::Activity::DropAndJump );
                                input.releaseKeyFor( "take&jump" );
                        }
                        else if ( input.movenorthTyped() )
                        {
                                activity = activities::Activity::MovingNorth;
                        }
                        else if ( input.movesouthTyped() )
                        {
                                activity = activities::Activity::MovingSouth;
                        }
                        else if ( input.moveeastTyped() )
                        {
                                activity = activities::Activity::MovingEast;
                        }
                        else if ( input.movewestTyped() )
                        {
                                activity = activities::Activity::MovingWest;
                        }
                }
                // dragged by a conveyor
                else if ( activity == activities::Activity::DraggedNorth || activity == activities::Activity::DraggedSouth ||
                                activity == activities::Activity::DraggedEast || activity == activities::Activity::DraggedWest )
                {
                        if ( input.jumpTyped() ) {
                                setCurrentActivity( activities::Activity::Jumping );
                        }
                        else {
                                handleMoveKeyWhenDragged () ;
                        }
                }
                else if ( activity == activities::Activity::Jumping )
                {
                        if ( input.doughnutTyped() )
                        {
                                useHooter( avatar );
                                input.releaseKeyFor( "doughnut" );
                        }
                        else if ( input.movenorthTyped() )
                        {
                                avatar.changeHeading( "north" );
                        }
                        else if ( input.movesouthTyped() )
                        {
                                avatar.changeHeading( "south" );
                        }
                        else if ( input.moveeastTyped() )
                        {
                                avatar.changeHeading( "east" );
                        }
                        else if ( input.movewestTyped() )
                        {
                                avatar.changeHeading( "west" );
                        }
                }
                else if ( activity == activities::Activity::Falling )
                {
                        if ( input.doughnutTyped() )
                        {
                                useHooter( avatar );
                                input.releaseKeyFor( "doughnut" );
                        }
                        // pick or drop an item when falling
                        else if ( input.takeTyped() )
                        {
                                activity = ( avatar.getDescriptionOfTakenItem() == nilPointer ? activities::Activity::TakeItem : activities::Activity::DropItem );
                                input.releaseKeyFor( "take" );
                        }
                        // entonces Head y Heels planean
                        else if ( input.anyMoveTyped() )
                        {
                                activity = activities::Activity::Gliding;
                        }
                }

                // for gliding, don’t wait for next cycle because there’s possibility
                // that gliding comes from falling, and waiting for next cycle may prevent
                // to enter gap between grid items
                if ( activity == activities::Activity::Gliding )
                {
                        if ( input.doughnutTyped() )
                        {
                                useHooter( avatar );
                                input.releaseKeyFor( "doughnut" );
                        }
                        // pick or drop an item when gliding
                        else if ( input.takeTyped() )
                        {
                                activity = ( avatar.getDescriptionOfTakenItem() == nilPointer ? activities::Activity::TakeItem : activities::Activity::DropItem );
                                input.releaseKeyFor( "take" );
                        }
                        else if ( input.movenorthTyped() )
                        {
                                avatar.changeHeading( "north" );
                        }
                        else if ( input.movesouthTyped() )
                        {
                                avatar.changeHeading( "south" );
                        }
                        else if ( input.moveeastTyped() )
                        {
                                avatar.changeHeading( "east" );
                        }
                        else if ( input.movewestTyped() )
                        {
                                avatar.changeHeading( "west" );
                        }
                        else if ( ! input.anyMoveTyped() )
                        {
                                activity = activities::Activity::Falling;
                        }
                }
        }
}

void CharacterHeadAndHeels::wait( AvatarItem & avatar )
{
        avatar.wait();

        if ( timerForBlinking->getValue() >= ( rand() % 4 ) + 5 )
        {
                timerForBlinking->reset();
                activity = activities::Activity::Blinking;
        }

        if ( activities::Falling::getInstance().fall( this ) )
        {
                speedTimer->reset();
                activity = activities::Activity::Falling;
        }
}

void CharacterHeadAndHeels::blink( AvatarItem & avatar )
{
        double blinkTime = timerForBlinking->getValue();

        // close the eyes
        if ( ( blinkTime > 0.0 && blinkTime < 0.050 ) || ( blinkTime > 0.400 && blinkTime < 0.450 ) )
        {
                avatar.changeFrame( blinkFrames[ avatar.getHeading() ] );
        }
        // open the eyes
        else if ( ( blinkTime > 0.250 && blinkTime < 0.300 ) || ( blinkTime > 0.750 && blinkTime < 0.800 ) )
        {
        }
        // end blinking
        else if ( blinkTime > 0.800 )
        {
                timerForBlinking->reset();
                activity = activities::Activity::Waiting;
        }
}

}
