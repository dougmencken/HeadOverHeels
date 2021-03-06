
#include "PlayerHeadAndHeels.hpp"
#include "Item.hpp"
#include "DescriptionOfItem.hpp"
#include "PlayerItem.hpp"
#include "Mediator.hpp"
#include "FallKindOfActivity.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"


namespace iso
{

PlayerHeadAndHeels::PlayerHeadAndHeels( const ItemPtr & item, const std::string & behavior )
        : UserControlled( item, behavior )
{
        jumpPhases = 28;
        highJumpPhases = 28;

        // salto normal
        for ( unsigned int i = 0; i < jumpPhases; i++ )
        {
                std::pair< int /* xy */, int /* z */ > jumpPhase( 1, ( i < 4 ? 4 : ( i < 8 ? 3 : 2 ) ) );
                jumpVector.push_back( jumpPhase );
        }

        // salto largo
        for ( unsigned int i = 0; i < highJumpPhases; i++ )
        {
                std::pair< int /* xy */, int /* z */ > jumpPhase( 1, 3 );
                highJumpVector.push_back( jumpPhase );
        }

        // fotogramas de caída
        fallFrames[ "north" ] = 8;
        fallFrames[ "south" ] = 16;
        fallFrames[ "east" ] = 12;
        fallFrames[ "west" ] = 17;

        // fotogramas de parpadeo
        blinkFrames[ "north" ] = 8;
        blinkFrames[ "south" ] = 18;
        blinkFrames[ "east" ] = 12;
        blinkFrames[ "west" ] = 19;

        labelOfBubbles = "double-bubbles";

        // activate chronometers
        speedTimer->go();
        fallTimer->go();
        glideTimer->go();
        timerForBlinking->go();
}

PlayerHeadAndHeels::~PlayerHeadAndHeels( )
{
}

bool PlayerHeadAndHeels::update ()
{
        PlayerItem& playerItem = dynamic_cast< PlayerItem& >( * this->item );

        if ( playerItem.hasShield() )
        {
                playerItem.decreaseShield();
        }

        switch ( activity )
        {
                case Activity::Wait:
                        wait( playerItem );
                        break;

                case Activity::AutoMoveNorth:
                case Activity::AutoMoveSouth:
                case Activity::AutoMoveEast:
                case Activity::AutoMoveWest:
                        autoMove( playerItem );
                        break;

                case Activity::MoveNorth:
                case Activity::MoveSouth:
                case Activity::MoveEast:
                case Activity::MoveWest:
                        move( playerItem );
                        break;

                case Activity::DisplaceNorth:
                case Activity::DisplaceSouth:
                case Activity::DisplaceEast:
                case Activity::DisplaceWest:
                case Activity::DisplaceNortheast:
                case Activity::DisplaceSoutheast:
                case Activity::DisplaceSouthwest:
                case Activity::DisplaceNorthwest:
                case Activity::ForceDisplaceNorth:
                case Activity::ForceDisplaceSouth:
                case Activity::ForceDisplaceEast:
                case Activity::ForceDisplaceWest:
                        displace( playerItem );
                        break;

                case Activity::CancelDisplaceNorth:
                case Activity::CancelDisplaceSouth:
                case Activity::CancelDisplaceEast:
                case Activity::CancelDisplaceWest:
                        cancelDisplace( playerItem );
                        break;

                case Activity::Fall:
                        fall( playerItem );
                        break;

                case Activity::Jump:
                case Activity::RegularJump:
                case Activity::HighJump:
                        jump( playerItem );
                        break;

                case Activity::BeginWayOutTeletransport:
                case Activity::WayOutTeletransport:
                        wayOutTeletransport( playerItem );
                        break;

                case Activity::BeginWayInTeletransport:
                case Activity::WayInTeletransport:
                        wayInTeletransport( playerItem );
                        break;

                case Activity::MeetMortalItem:
                case Activity::Vanish:
                        collideWithMortalItem( playerItem );
                        break;

                case Activity::Glide:
                        glide( playerItem );
                        break;

                case Activity::Blink:
                        blink( playerItem );
                        break;

                case Activity::TakeItem:
                case Activity::TakeAndJump:
                        takeItem( playerItem );
                        break;

                case Activity::ItemTaken:
                        playerItem.addToZ( -LayerHeight );
                        activity = Activity::Wait;
                        break;

                case Activity::DropItem:
                case Activity::DropAndJump:
                        dropItem( playerItem );
                        break;

                default:
                        ;
        }

        // play sound for current activity
        SoundManager::getInstance().play( playerItem.getOriginalLabel(), activity );

        return false;
}

void PlayerHeadAndHeels::behave ()
{
        PlayerItem& playerItem = dynamic_cast< PlayerItem& >( * this->item );
        InputManager& input = InputManager::getInstance();

        // if it’s not a move by inertia or some other exotic activity
        if ( activity != Activity::AutoMoveNorth && activity != Activity::AutoMoveSouth &&
                activity != Activity::AutoMoveEast && activity != Activity::AutoMoveWest &&
                activity != Activity::BeginWayOutTeletransport && activity != Activity::WayOutTeletransport &&
                activity != Activity::BeginWayInTeletransport && activity != Activity::WayInTeletransport &&
                activity != Activity::MeetMortalItem && activity != Activity::Vanish )
        {
                // when waiting or blinking
                if ( activity == Activity::Wait || activity == Activity::Blink )
                {
                        if ( input.jumpTyped() )
                        {
                                // is there teleport below
                                playerItem.canAdvanceTo( 0, 0, -1 );
                                activity =
                                        playerItem.getMediator()->collisionWithByBehavior( "behavior of teletransport" ) != nilPointer ?
                                                Activity::BeginWayOutTeletransport : Activity::Jump ;
                        }
                        else if ( input.doughnutTyped() && ! donutFromHooterIsHere )
                        {
                                useHooter( playerItem );
                                input.releaseKeyFor( "doughnut" );
                        }
                        else if ( input.takeTyped() )
                        {
                                activity = ( playerItem.getDescriptionOfTakenItem() == nilPointer ? Activity::TakeItem : Activity::DropItem );
                                input.releaseKeyFor( "take" );
                        }
                        else if ( input.takeAndJumpTyped() )
                        {
                                activity = ( playerItem.getDescriptionOfTakenItem() == nilPointer ? Activity::TakeAndJump : Activity::DropAndJump );
                                input.releaseKeyFor( "take&jump" );
                        }
                        else if ( input.movenorthTyped() )
                        {
                                activity = Activity::MoveNorth;
                        }
                        else if ( input.movesouthTyped() )
                        {
                                activity = Activity::MoveSouth;
                        }
                        else if ( input.moveeastTyped() )
                        {
                                activity = Activity::MoveEast;
                        }
                        else if ( input.movewestTyped() )
                        {
                                activity = Activity::MoveWest;
                        }
                }
                // already moving
                else if ( activity == Activity::MoveNorth || activity == Activity::MoveSouth ||
                        activity == Activity::MoveEast || activity == Activity::MoveWest )
                {
                        if ( input.jumpTyped() )
                        {
                                // jump or teleport
                                playerItem.canAdvanceTo( 0, 0, -1 );
                                activity =
                                        playerItem.getMediator()->collisionWithByBehavior( "behavior of teletransport" ) != nilPointer ?
                                                Activity::BeginWayOutTeletransport : Activity::Jump ;
                        }
                        else if ( input.doughnutTyped() && ! donutFromHooterIsHere )
                        {
                                useHooter( playerItem );
                                input.releaseKeyFor( "doughnut" );
                        }
                        else if ( input.takeTyped() )
                        {
                                activity = ( playerItem.getDescriptionOfTakenItem() == nilPointer ? Activity::TakeItem : Activity::DropItem );
                                input.releaseKeyFor( "take" );
                        }
                        else if ( input.takeAndJumpTyped() )
                        {
                                activity = ( playerItem.getDescriptionOfTakenItem() == nilPointer ? Activity::TakeAndJump : Activity::DropAndJump );
                                input.releaseKeyFor( "take&jump" );
                        }
                        else if ( input.movenorthTyped() )
                        {
                                activity = Activity::MoveNorth;
                        }
                        else if ( input.movesouthTyped() )
                        {
                                activity = Activity::MoveSouth;
                        }
                        else if ( input.moveeastTyped() )
                        {
                                activity = Activity::MoveEast;
                        }
                        else if ( input.movewestTyped() )
                        {
                                activity = Activity::MoveWest;
                        }
                        else if ( ! input.anyMoveTyped() )
                        {
                                SoundManager::getInstance().stop( playerItem.getLabel(), activity );
                                activity = Activity::Wait;
                        }
                }
                // character is being displaced
                else if ( activity == Activity::DisplaceNorth || activity == Activity::DisplaceSouth ||
                        activity == Activity::DisplaceEast || activity == Activity::DisplaceWest )
                {
                        if ( input.jumpTyped() )
                        {
                                activity = Activity::Jump;
                        }
                        else if ( input.doughnutTyped() && ! donutFromHooterIsHere )
                        {
                                useHooter( playerItem );
                                input.releaseKeyFor( "doughnut" );
                        }
                        else if ( input.takeTyped() )
                        {
                                activity = ( playerItem.getDescriptionOfTakenItem() == nilPointer ? Activity::TakeItem : Activity::DropItem );
                                input.releaseKeyFor( "take" );
                        }
                        else if ( input.takeAndJumpTyped() )
                        {
                                activity = ( playerItem.getDescriptionOfTakenItem() == nilPointer ? Activity::TakeAndJump : Activity::DropAndJump );
                                input.releaseKeyFor( "take&jump" );
                        }
                        else if ( input.movenorthTyped() )
                        {
                                activity = Activity::MoveNorth;
                        }
                        else if ( input.movesouthTyped() )
                        {
                                activity = Activity::MoveSouth;
                        }
                        else if ( input.moveeastTyped() )
                        {
                                activity = Activity::MoveEast;
                        }
                        else if ( input.movewestTyped() )
                        {
                                activity = Activity::MoveWest;
                        }
                }
                // character is being displaced forcibly
                else if ( activity == Activity::ForceDisplaceNorth || activity == Activity::ForceDisplaceSouth ||
                        activity == Activity::ForceDisplaceEast || activity == Activity::ForceDisplaceWest )
                {
                        if ( input.jumpTyped() )
                        {
                                activity = Activity::Jump;
                        }
                        // cancel displace when moving in direction opposite to displacement
                        else if ( input.movenorthTyped() )
                        {
                                activity = ( activity == Activity::ForceDisplaceSouth ? Activity::CancelDisplaceSouth : Activity::MoveNorth );
                        }
                        else if ( input.movesouthTyped() )
                        {
                                activity = ( activity == Activity::ForceDisplaceNorth ? Activity::CancelDisplaceNorth : Activity::MoveSouth );
                        }
                        else if ( input.moveeastTyped() )
                        {
                                activity = ( activity == Activity::ForceDisplaceWest ? Activity::CancelDisplaceWest : Activity::MoveEast );
                        }
                        else if ( input.movewestTyped() )
                        {
                                activity = ( activity == Activity::ForceDisplaceEast ? Activity::CancelDisplaceEast : Activity::MoveWest );
                        }
                }
                else if ( activity == Activity::Jump || activity == Activity::RegularJump || activity == Activity::HighJump )
                {
                        if ( input.doughnutTyped() && ! donutFromHooterIsHere )
                        {
                                useHooter( playerItem );
                                input.releaseKeyFor( "doughnut" );
                        }
                        else if ( input.movenorthTyped() )
                        {
                                playerItem.changeOrientation( "north" );
                        }
                        else if ( input.movesouthTyped() )
                        {
                                playerItem.changeOrientation( "south" );
                        }
                        else if ( input.moveeastTyped() )
                        {
                                playerItem.changeOrientation( "east" );
                        }
                        else if ( input.movewestTyped() )
                        {
                                playerItem.changeOrientation( "west" );
                        }
                }
                else if ( activity == Activity::Fall )
                {
                        if ( input.doughnutTyped() && ! donutFromHooterIsHere )
                        {
                                useHooter( playerItem );
                                input.releaseKeyFor( "doughnut" );
                        }
                        // pick or drop an item when falling
                        else if ( input.takeTyped() )
                        {
                                activity = ( playerItem.getDescriptionOfTakenItem() == nilPointer ? Activity::TakeItem : Activity::DropItem );
                                input.releaseKeyFor( "take" );
                        }
                        // entonces Head y Heels planean
                        else if ( input.anyMoveTyped() )
                        {
                                activity = Activity::Glide;
                        }
                }

                // for gliding, don’t wait for next cycle because there’s possibility
                // that gliding comes from falling, and waiting for next cycle may prevent
                // to enter gap between grid items
                if ( activity == Activity::Glide )
                {
                        if ( input.doughnutTyped() && ! donutFromHooterIsHere )
                        {
                                useHooter( playerItem );
                                input.releaseKeyFor( "doughnut" );
                        }
                        // pick or drop an item when gliding
                        else if ( input.takeTyped() )
                        {
                                activity = ( playerItem.getDescriptionOfTakenItem() == nilPointer ? Activity::TakeItem : Activity::DropItem );
                                input.releaseKeyFor( "take" );
                        }
                        else if ( input.movenorthTyped() )
                        {
                                playerItem.changeOrientation( "north" );
                        }
                        else if ( input.movesouthTyped() )
                        {
                                playerItem.changeOrientation( "south" );
                        }
                        else if ( input.moveeastTyped() )
                        {
                                playerItem.changeOrientation( "east" );
                        }
                        else if ( input.movewestTyped() )
                        {
                                playerItem.changeOrientation( "west" );
                        }
                        else if ( ! input.anyMoveTyped() )
                        {
                                activity = Activity::Fall;
                        }
                }
        }
}

void PlayerHeadAndHeels::wait( PlayerItem & playerItem )
{
        playerItem.wait();

        if ( timerForBlinking->getValue() >= ( rand() % 4 ) + 5 )
        {
                timerForBlinking->reset();
                activity = Activity::Blink;
        }

        if ( FallKindOfActivity::getInstance().fall( this ) )
        {
                speedTimer->reset();
                activity = Activity::Fall;
        }
}

void PlayerHeadAndHeels::blink( PlayerItem & playerItem )
{
        double blinkTime = timerForBlinking->getValue();

        // close the eyes
        if ( ( blinkTime > 0.0 && blinkTime < 0.050 ) || ( blinkTime > 0.400 && blinkTime < 0.450 ) )
        {
                playerItem.changeFrame( blinkFrames[ playerItem.getOrientation() ] );
        }
        // open the eyes
        else if ( ( blinkTime > 0.250 && blinkTime < 0.300 ) || ( blinkTime > 0.750 && blinkTime < 0.800 ) )
        {
        }
        // end blinking
        else if ( blinkTime > 0.800 )
        {
                timerForBlinking->reset();
                activity = Activity::Wait;
        }
}

}
