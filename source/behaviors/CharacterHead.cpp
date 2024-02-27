
#include "CharacterHead.hpp"

#include "Item.hpp"
#include "AvatarItem.hpp"
#include "Mediator.hpp"
#include "Moving.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"


namespace behaviors
{

CharacterHead::CharacterHead( const ItemPtr & item, const std::string & behavior )
        : PlayerControlled( item, behavior )
{
        // salto
        const unsigned int jumpPhases = 20 ;
        for ( unsigned int i = 0; i < jumpPhases; i++ )
        {
                jumpVector.push_back( std::pair< int /* xy */, int /* z */ >( 1, 3 ) );
        }

        // salto largo
        const unsigned int highJumpPhases = 19 ;
        for ( unsigned int i = 0; i < highJumpPhases; i++ )
        {
                highJumpVector.push_back( std::pair< int /* xy */, int /* z */ >( 1, 4 ) );
        }

        // fotogramas de caída (falling)
        fallFrames[ "north" ] = 8;
        fallFrames[ "south" ] = 16;
        fallFrames[ "east" ] = 12;
        fallFrames[ "west" ] = 17;

        // fotogramas de parpadeo (blinking)
        blinkFrames[ "north" ] = 8;
        blinkFrames[ "south" ] = 19;
        blinkFrames[ "east" ] = 12;
        blinkFrames[ "west" ] = 20;

        // activate chronometers
        speedTimer->go ();
        fallTimer->go ();
        glideTimer->go ();
        timerForBlinking->go ();
}

CharacterHead::~CharacterHead( )
{
}

bool CharacterHead::update ()
{
        AvatarItem & characterItem = dynamic_cast< AvatarItem & >( * this->item );

        if ( characterItem.hasShield() )
        {
                characterItem.decrementShieldOverTime () ;
        }

        // change the height to climb bars easier
        characterItem.changeHeightTo( activity == activities::Activity::Falling || activity == activities::Activity::Gliding ? 23 : 24 );

        switch ( activity )
        {
                case activities::Activity::Waiting:
                        wait( characterItem );
                        break;

                case activities::Activity::AutomovingNorth:
                case activities::Activity::AutomovingSouth:
                case activities::Activity::AutomovingEast:
                case activities::Activity::AutomovingWest:
                        autoMove( characterItem );
                        break;

                case activities::Activity::MovingNorth:
                case activities::Activity::MovingSouth:
                case activities::Activity::MovingEast:
                case activities::Activity::MovingWest:
                        move( characterItem );
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
                        displace( characterItem );
                        break;

                case activities::Activity::CancelDragNorth:
                case activities::Activity::CancelDragSouth:
                case activities::Activity::CancelDragEast:
                case activities::Activity::CancelDragWest:
                        cancelDragging( characterItem );
                        break;

                case activities::Activity::Falling:
                        fall( characterItem );
                        break;

                case activities::Activity::Jumping :
                        jump( characterItem );
                        break;

                case activities::Activity::BeginTeletransportation:
                        enterTeletransport( characterItem );
                        break;
                case activities::Activity::EndTeletransportation:
                        exitTeletransport( characterItem );
                        break;

                case activities::Activity::MetLethalItem:
                case activities::Activity::Vanishing:
                        collideWithMortalItem( characterItem );
                        break;

                case activities::Activity::Gliding:
                        glide( characterItem );
                        break;

                case activities::Activity::Blinking:
                        blink( characterItem );
                        break;

                default:
                        ;
        }

        // play sound for the current activity
        SoundManager::getInstance().play( characterItem.getOriginalKind(), SoundManager::activityToString( activity ) );

        return false;
}

void CharacterHead::behave ()
{
        AvatarItem & characterItem = dynamic_cast< AvatarItem & >( * this->item );
        const InputManager & input = InputManager::getInstance ();

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
                                // jump or teleport
                                characterItem.canAdvanceTo( 0, 0, -1 );
                                activity =
                                        characterItem.getMediator()->collisionWithBehavingAs( "behavior of teletransport" ) != nilPointer ?
                                                activities::Activity::BeginTeletransportation : activities::Activity::Jumping ;
                        }
                        else if ( input.doughnutTyped() )
                        {
                                useHooter( characterItem );
                                input.releaseKeyFor( "doughnut" );
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
                // already moving
                else if ( activity == activities::Activity::MovingNorth || activity == activities::Activity::MovingSouth ||
                        activity == activities::Activity::MovingEast || activity == activities::Activity::MovingWest )
                {
                        if ( input.jumpTyped() )
                        {
                                // look for teletransport below
                                characterItem.canAdvanceTo( 0, 0, -1 );
                                activity =
                                        characterItem.getMediator()->collisionWithBehavingAs( "behavior of teletransport" ) != nilPointer ?
                                                activities::Activity::BeginTeletransportation : activities::Activity::Jumping ;
                        }
                        else if ( input.doughnutTyped() )
                        {
                                useHooter( characterItem );
                                input.releaseKeyFor( "doughnut" );
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
                                SoundManager::getInstance().stop( characterItem.getOriginalKind(), SoundManager::activityToString( activity ) );
                                activity = activities::Activity::Waiting ;
                        }
                }
                // if you are being displaced
                else if ( activity == activities::Activity::PushedNorth || activity == activities::Activity::PushedSouth ||
                        activity == activities::Activity::PushedEast || activity == activities::Activity::PushedWest )
                {
                        if ( input.jumpTyped() )
                        {
                                activity = activities::Activity::Jumping;
                        }
                        else if ( input.doughnutTyped() )
                        {
                                useHooter( characterItem );
                                input.releaseKeyFor( "doughnut" );
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
                // if the character is dragged by a conveyor
                else if ( activity == activities::Activity::DraggedNorth || activity == activities::Activity::DraggedSouth ||
                        activity == activities::Activity::DraggedEast || activity == activities::Activity::DraggedWest )
                {
                        if ( input.jumpTyped() )
                        {
                                activity = activities::Activity::Jumping ;
                        }
                        // the character moves while being dragged
                        // moving in the opposite way cancels dragging
                        else if ( input.movenorthTyped() )
                        {
                                activity = ( activity == activities::Activity::DraggedSouth ) ? activities::Activity::CancelDragSouth : activities::Activity::MovingNorth ;
                        }
                        else if ( input.movesouthTyped() )
                        {
                                activity = ( activity == activities::Activity::DraggedNorth ) ? activities::Activity::CancelDragNorth : activities::Activity::MovingSouth ;
                        }
                        else if ( input.moveeastTyped() )
                        {
                                activity = ( activity == activities::Activity::DraggedWest ) ? activities::Activity::CancelDragWest : activities::Activity::MovingEast ;
                        }
                        else if ( input.movewestTyped() )
                        {
                                activity = ( activity == activities::Activity::DraggedEast ) ? activities::Activity::CancelDragEast : activities::Activity::MovingWest ;
                        }
                }
                else if ( activity == activities::Activity::Jumping )
                {
                        if ( input.doughnutTyped() )
                        {
                                useHooter( characterItem );
                                input.releaseKeyFor( "doughnut" );
                        }
                        // Head may change orientation when jumping
                        else if ( input.movenorthTyped() )
                        {
                                characterItem.changeHeading( "north" );
                        }
                        else if ( input.movesouthTyped() )
                        {
                                characterItem.changeHeading( "south" );
                        }
                        else if ( input.moveeastTyped() )
                        {
                                characterItem.changeHeading( "east" );
                        }
                        else if ( input.movewestTyped() )
                        {
                                characterItem.changeHeading( "west" );
                        }
                }
                else if ( activity == activities::Activity::Falling )
                {
                        if ( input.doughnutTyped() )
                        {
                                useHooter( characterItem );
                                input.releaseKeyFor( "doughnut" );
                        }
                        // entonces Head planea
                        else if ( input.anyMoveTyped() )
                        {
                                activity = activities::Activity::Gliding;
                        }
                }

                // for gliding, don’t wait for next cycle because there’s possibility
                // that gliding comes from falling, and waiting for next cycle may prevent
                // to enter gap between two grid items
                if ( activity == activities::Activity::Gliding )
                {
                        if ( input.doughnutTyped() )
                        {
                                useHooter( characterItem );
                                input.releaseKeyFor( "doughnut" );
                        }
                        // Head may change orientation when gliding
                        else if ( input.movenorthTyped() )
                        {
                                characterItem.changeHeading( "north" );
                        }
                        else if ( input.movesouthTyped() )
                        {
                                characterItem.changeHeading( "south" );
                        }
                        else if ( input.moveeastTyped() )
                        {
                                characterItem.changeHeading( "east" );
                        }
                        else if ( input.movewestTyped() )
                        {
                                characterItem.changeHeading( "west" );
                        }
                        else if ( ! input.anyMoveTyped() )
                        {
                                activity = activities::Activity::Falling;
                        }
                }
        }
}

void CharacterHead::wait( AvatarItem & characterItem )
{
        characterItem.wait();

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

void CharacterHead::blink( AvatarItem & characterItem )
{
        double timeToBlink = timerForBlinking->getValue();

        if ( ( timeToBlink > 0.0 && timeToBlink < 0.050 ) || ( timeToBlink > 0.400 && timeToBlink < 0.450 ) )
        {
                characterItem.changeFrame( blinkFrames[ characterItem.getHeading() ] );
        }
        else if ( ( timeToBlink > 0.250 && timeToBlink < 0.300 ) || ( timeToBlink > 0.750 && timeToBlink < 0.800 ) )
        {
        }
        else if ( timeToBlink > 0.800 )
        {
                timerForBlinking->reset();
                activity = activities::Activity::Waiting;
        }
}

}
