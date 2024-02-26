
#include "CharacterHeels.hpp"

#include "Item.hpp"
#include "DescriptionOfItem.hpp"
#include "AvatarItem.hpp"
#include "Mediator.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"


namespace behaviors
{

CharacterHeels::CharacterHeels( const ItemPtr & item, const std::string & behavior )
        : PlayerControlled( item, behavior )
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
        fallFrames[ "north" ] = 8;
        fallFrames[ "south" ] = 0;
        fallFrames[ "east" ] = 12;
        fallFrames[ "west" ] = 4;

        // activate chronometers
        speedTimer->go ();
        fallTimer->go ();
}

CharacterHeels::~CharacterHeels( )
{
}

bool CharacterHeels::update()
{
        AvatarItem & characterItem = dynamic_cast< AvatarItem & >( * this->item );

        if ( characterItem.hasShield() )
        {
                characterItem.decrementShieldOverTime () ;
        }

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

                case activities::Activity::TakeItem:
                case activities::Activity::TakeAndJump:
                        takeItem( characterItem );
                        break;

                case activities::Activity::ItemTaken:
                        characterItem.addToZ( - Room::LayerHeight );
                        activity = activities::Activity::Waiting;
                        break;

                case activities::Activity::DropItem:
                case activities::Activity::DropAndJump:
                        dropItem( characterItem );
                        break;

                default:
                        ;
        }

        // play sound for the current activity
        SoundManager::getInstance().play( characterItem.getOriginalKind(), SoundManager::activityToString( activity ) );

        return false;
}

void CharacterHeels::behave ()
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
                        if ( input.takeTyped() )
                        {
                                activity = ( characterItem.getDescriptionOfTakenItem() == nilPointer ? activities::Activity::TakeItem : activities::Activity::DropItem );
                                input.releaseKeyFor( "take" );
                        }
                        else if ( input.takeAndJumpTyped() )
                        {
                                activity = ( characterItem.getDescriptionOfTakenItem() == nilPointer ? activities::Activity::TakeAndJump : activities::Activity::DropAndJump );
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
                        else if ( input.jumpTyped() )
                        {
                                // look for item below
                                characterItem.canAdvanceTo( 0, 0, -1 );
                                // key to teleport is the same as for jump
                                activity =
                                        characterItem.getMediator()->collisionWithBehavingAs( "behavior of teletransport" ) != nilPointer ?
                                                activities::Activity::BeginTeletransportation : activities::Activity::Jumping ;
                        }
                }
                // already moving
                else if ( activity == activities::Activity::MovingNorth || activity == activities::Activity::MovingSouth ||
                        activity == activities::Activity::MovingEast || activity == activities::Activity::MovingWest )
                {
                        if( input.jumpTyped() )
                        {
                                // teleport when teletransport is below
                                characterItem.canAdvanceTo( 0, 0, -1 );
                                activity =
                                        characterItem.getMediator()->collisionWithBehavingAs( "behavior of teletransport" ) != nilPointer ?
                                                activities::Activity::BeginTeletransportation : activities::Activity::Jumping ;
                        }
                        else if ( input.takeTyped() )
                        {
                                activity = ( characterItem.getDescriptionOfTakenItem() == nilPointer ? activities::Activity::TakeItem : activities::Activity::DropItem );
                                input.releaseKeyFor( "take" );
                        }
                        else if ( input.takeAndJumpTyped() )
                        {
                                activity = ( characterItem.getDescriptionOfTakenItem() == nilPointer ? activities::Activity::TakeAndJump : activities::Activity::DropAndJump );
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
                        else if ( input.takeTyped() )
                        {
                                activity = ( characterItem.getDescriptionOfTakenItem() == nilPointer ? activities::Activity::TakeItem : activities::Activity::DropItem );
                                input.releaseKeyFor( "take" );
                        }
                        else if ( input.takeAndJumpTyped() )
                        {
                                activity = ( characterItem.getDescriptionOfTakenItem() == nilPointer ? activities::Activity::TakeAndJump : activities::Activity::DropAndJump );
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
                // if the character is dragged by a conveyor
                else if ( activity == activities::Activity::DraggedNorth || activity == activities::Activity::DraggedSouth ||
                        activity == activities::Activity::DraggedEast || activity == activities::Activity::DraggedWest )
                {
                        if ( input.jumpTyped() )
                        {
                                activity = activities::Activity::Jumping;
                        }
                        // the character moves while being dragged
                        // moving in the opposite way cancels dragging
                        else if ( input.movenorthTyped() )
                        {
                                activity = ( activity == activities::Activity::DraggedSouth ? activities::Activity::CancelDragSouth : activities::Activity::MovingNorth );
                        }
                        else if ( input.movesouthTyped() )
                        {
                                activity = ( activity == activities::Activity::DraggedNorth ? activities::Activity::CancelDragNorth : activities::Activity::MovingSouth );
                        }
                        else if ( input.moveeastTyped() )
                        {
                                activity = ( activity == activities::Activity::DraggedWest ? activities::Activity::CancelDragWest : activities::Activity::MovingEast );
                        }
                        else if ( input.movewestTyped() )
                        {
                                activity = ( activity == activities::Activity::DraggedEast ? activities::Activity::CancelDragEast : activities::Activity::MovingWest );
                        }
                }
                else if ( activity == activities::Activity::Jumping )
                {
                        // nothing here
                }
                else if ( activity == activities::Activity::Falling )
                {
                        // pick or drop an item when falling
                        if ( input.takeTyped() )
                        {
                                activity = ( characterItem.getDescriptionOfTakenItem() == nilPointer ? activities::Activity::TakeItem : activities::Activity::DropItem );
                                input.releaseKeyFor( "take" );
                        }
                }
        }
}

}
