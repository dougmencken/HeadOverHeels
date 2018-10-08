
#include "PlayerHeadAndHeels.hpp"
#include "Item.hpp"
#include "ItemData.hpp"
#include "PlayerItem.hpp"
#include "Mediator.hpp"
#include "FallKindOfActivity.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"


namespace isomot
{

PlayerHeadAndHeels::PlayerHeadAndHeels( Item * item, const std::string & behavior )
        : UserControlled( item, behavior )
{
        jumpFrames = 28;
        highJumpFrames = 28;

        // salto normal
        for ( unsigned int i = 0; i < jumpFrames; i++ )
        {
                std::pair< int /* xy */, int /* z */ > jumpPhase( 1, ( i < 4 ? 4 : ( i < 8 ? 3 : 2 ) ) );
                jumpVector.push_back( jumpPhase );
        }

        // salto largo
        for ( unsigned int i = 0; i < highJumpFrames; i++ )
        {
                std::pair< int /* xy */, int /* z */ > jumpPhase( 1, 3 );
                highJumpVector.push_back( jumpPhase );
        }

        // la primera fase del salto
        jumpIndex = 0;

        // fotogramas de caída
        fallFrames[ "north" ] = 8;
        fallFrames[ "south" ] = 16;
        fallFrames[ "east" ] = 12;
        fallFrames[ "west" ] = 17;

        // fotograma en blanco
        nullFrame = 20;

        // fotogramas de parpadeo
        blinkFrames[ "north" ] = 8;
        blinkFrames[ "south" ] = 18;
        blinkFrames[ "east" ] = 12;
        blinkFrames[ "west" ] = 19;

        labelOfTransitionViaTeleport = "double-bubbles";
        labelOfFireFromHooter = "bubbles";

        // pasos automáticos
        automaticStepsThruDoor = 16;

        // create and activate chronometers
        speedTimer = new Timer();
        speedTimer->go();
        fallTimer = new Timer();
        fallTimer->go();
        glideTimer = new Timer();
        glideTimer->go();
        blinkingTimer = new Timer();
        blinkingTimer->go();
}

PlayerHeadAndHeels::~PlayerHeadAndHeels( )
{
}

bool PlayerHeadAndHeels::update ()
{
        PlayerItem* player = dynamic_cast< PlayerItem * >( this->item );

        if ( player->hasShield() )
        {
                player->decreaseShield();
        }

        switch ( activity )
        {
                case Activity::Wait:
                        wait( player );
                        break;

                case Activity::AutoMoveNorth:
                case Activity::AutoMoveSouth:
                case Activity::AutoMoveEast:
                case Activity::AutoMoveWest:
                        autoMove( player );
                        break;

                case Activity::MoveNorth:
                case Activity::MoveSouth:
                case Activity::MoveEast:
                case Activity::MoveWest:
                        move( player );
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
                        displace( player );
                        break;

                case Activity::CancelDisplaceNorth:
                case Activity::CancelDisplaceSouth:
                case Activity::CancelDisplaceEast:
                case Activity::CancelDisplaceWest:
                        cancelDisplace( player );
                        break;

                case Activity::Fall:
                        fall( player );
                        break;

                case Activity::Jump:
                case Activity::RegularJump:
                case Activity::HighJump:
                        jump( player );
                        break;

                case Activity::BeginWayOutTeletransport:
                case Activity::WayOutTeletransport:
                        wayOutTeletransport( player );
                        break;

                case Activity::BeginWayInTeletransport:
                case Activity::WayInTeletransport:
                        wayInTeletransport( player );
                        break;

                case Activity::MeetMortalItem:
                case Activity::Vanish:
                        collideWithMortalItem( player );
                        break;

                case Activity::Glide:
                        glide( player );
                        break;

                case Activity::Blink:
                        blink( player );
                        break;

                case Activity::TakeItem:
                case Activity::TakeAndJump:
                        takeItem( player );
                        break;

                case Activity::ItemTaken:
                        player->addToZ( -LayerHeight );
                        activity = Activity::Wait;
                        break;

                case Activity::DropItem:
                case Activity::DropAndJump:
                        dropItem( player );
                        break;

                default:
                        ;
        }

        // play sound for current activity
        SoundManager::getInstance()->play( player->getOriginalLabel(), activity );

        return false;
}

void PlayerHeadAndHeels::behave ()
{
        PlayerItem* playerItem = dynamic_cast< PlayerItem * >( this->item );
        InputManager* input = InputManager::getInstance();

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
                        if ( input->jumpTyped() )
                        {
                                // is there teleport below
                                playerItem->canAdvanceTo( 0, 0, -1 );
                                activity =
                                        playerItem->getMediator()->collisionWithByBehavior( "behavior of teletransport" ) ?
                                                Activity::BeginWayOutTeletransport : Activity::Jump ;
                        }
                        else if ( input->doughnutTyped() && ! donutFromHooterIsHere )
                        {
                                useHooter( playerItem );
                                input->releaseKeyFor( "doughnut" );
                        }
                        else if ( input->takeTyped() )
                        {
                                activity = ( playerItem->getTakenItemData() == nilPointer ? Activity::TakeItem : Activity::DropItem );
                                input->releaseKeyFor( "take" );
                        }
                        else if ( input->takeAndJumpTyped() )
                        {
                                activity = ( playerItem->getTakenItemData() == nilPointer ? Activity::TakeAndJump : Activity::DropAndJump );
                                input->releaseKeyFor( "take&jump" );
                        }
                        else if ( input->movenorthTyped() )
                        {
                                activity = Activity::MoveNorth;
                        }
                        else if ( input->movesouthTyped() )
                        {
                                activity = Activity::MoveSouth;
                        }
                        else if ( input->moveeastTyped() )
                        {
                                activity = Activity::MoveEast;
                        }
                        else if ( input->movewestTyped() )
                        {
                                activity = Activity::MoveWest;
                        }
                }
                // already moving
                else if ( activity == Activity::MoveNorth || activity == Activity::MoveSouth ||
                        activity == Activity::MoveEast || activity == Activity::MoveWest )
                {
                        if ( input->jumpTyped() )
                        {
                                // jump or teleport
                                playerItem->canAdvanceTo( 0, 0, -1 );
                                activity =
                                        playerItem->getMediator()->collisionWithByBehavior( "behavior of teletransport" ) ?
                                                Activity::BeginWayOutTeletransport : Activity::Jump ;
                        }
                        else if ( input->doughnutTyped() && ! donutFromHooterIsHere )
                        {
                                useHooter( playerItem );
                                input->releaseKeyFor( "doughnut" );
                        }
                        else if ( input->takeTyped() )
                        {
                                activity = ( playerItem->getTakenItemData() == nilPointer ? Activity::TakeItem : Activity::DropItem );
                                input->releaseKeyFor( "take" );
                        }
                        else if ( input->takeAndJumpTyped() )
                        {
                                activity = ( playerItem->getTakenItemData() == nilPointer ? Activity::TakeAndJump : Activity::DropAndJump );
                                input->releaseKeyFor( "take&jump" );
                        }
                        else if ( input->movenorthTyped() )
                        {
                                activity = Activity::MoveNorth;
                        }
                        else if ( input->movesouthTyped() )
                        {
                                activity = Activity::MoveSouth;
                        }
                        else if ( input->moveeastTyped() )
                        {
                                activity = Activity::MoveEast;
                        }
                        else if ( input->movewestTyped() )
                        {
                                activity = Activity::MoveWest;
                        }
                        else if ( ! input->anyMoveTyped() )
                        {
                                SoundManager::getInstance()->stop( playerItem->getLabel(), activity );
                                activity = Activity::Wait;
                        }
                }
                // character is being displaced
                else if ( activity == Activity::DisplaceNorth || activity == Activity::DisplaceSouth ||
                        activity == Activity::DisplaceEast || activity == Activity::DisplaceWest )
                {
                        if ( input->jumpTyped() )
                        {
                                activity = Activity::Jump;
                        }
                        else if ( input->doughnutTyped() && ! donutFromHooterIsHere )
                        {
                                useHooter( playerItem );
                                input->releaseKeyFor( "doughnut" );
                        }
                        else if ( input->takeTyped() )
                        {
                                activity = ( playerItem->getTakenItemData() == nilPointer ? Activity::TakeItem : Activity::DropItem );
                                input->releaseKeyFor( "take" );
                        }
                        else if ( input->takeAndJumpTyped() )
                        {
                                activity = ( playerItem->getTakenItemData() == nilPointer ? Activity::TakeAndJump : Activity::DropAndJump );
                                input->releaseKeyFor( "take&jump" );
                        }
                        else if ( input->movenorthTyped() )
                        {
                                activity = Activity::MoveNorth;
                        }
                        else if ( input->movesouthTyped() )
                        {
                                activity = Activity::MoveSouth;
                        }
                        else if ( input->moveeastTyped() )
                        {
                                activity = Activity::MoveEast;
                        }
                        else if ( input->movewestTyped() )
                        {
                                activity = Activity::MoveWest;
                        }
                }
                // character is being displaced forcibly
                else if ( activity == Activity::ForceDisplaceNorth || activity == Activity::ForceDisplaceSouth ||
                        activity == Activity::ForceDisplaceEast || activity == Activity::ForceDisplaceWest )
                {
                        if ( input->jumpTyped() )
                        {
                                activity = Activity::Jump;
                        }
                        // cancel displace when moving in direction opposite to displacement
                        else if ( input->movenorthTyped() )
                        {
                                activity = ( activity == Activity::ForceDisplaceSouth ? Activity::CancelDisplaceSouth : Activity::MoveNorth );
                        }
                        else if ( input->movesouthTyped() )
                        {
                                activity = ( activity == Activity::ForceDisplaceNorth ? Activity::CancelDisplaceNorth : Activity::MoveSouth );
                        }
                        else if ( input->moveeastTyped() )
                        {
                                activity = ( activity == Activity::ForceDisplaceWest ? Activity::CancelDisplaceWest : Activity::MoveEast );
                        }
                        else if ( input->movewestTyped() )
                        {
                                activity = ( activity == Activity::ForceDisplaceEast ? Activity::CancelDisplaceEast : Activity::MoveWest );
                        }
                }
                else if ( activity == Activity::Jump || activity == Activity::RegularJump || activity == Activity::HighJump )
                {
                        if ( input->doughnutTyped() && ! donutFromHooterIsHere )
                        {
                                useHooter( playerItem );
                                input->releaseKeyFor( "doughnut" );
                        }
                        else if ( input->movenorthTyped() )
                        {
                                playerItem->changeOrientation( Way( "north" ) );
                        }
                        else if ( input->movesouthTyped() )
                        {
                                playerItem->changeOrientation( Way( "south" ) );
                        }
                        else if ( input->moveeastTyped() )
                        {
                                playerItem->changeOrientation( Way( "east" ) );
                        }
                        else if ( input->movewestTyped() )
                        {
                                playerItem->changeOrientation( Way( "west" ) );
                        }
                }
                else if ( activity == Activity::Fall )
                {
                        if ( input->doughnutTyped() && ! donutFromHooterIsHere )
                        {
                                useHooter( playerItem );
                                input->releaseKeyFor( "doughnut" );
                        }
                        // pick or drop an item when falling
                        else if ( input->takeTyped() )
                        {
                                activity = ( playerItem->getTakenItemData() == nilPointer ? Activity::TakeItem : Activity::DropItem );
                                input->releaseKeyFor( "take" );
                        }
                        // entonces Head y Heels planean
                        else if ( input->anyMoveTyped() )
                        {
                                activity = Activity::Glide;
                        }
                }

                // for gliding, don’t wait for next cycle because there’s possibility
                // that gliding comes from falling, and waiting for next cycle may prevent
                // to enter gap between grid items
                if ( activity == Activity::Glide )
                {
                        if ( input->doughnutTyped() && ! donutFromHooterIsHere )
                        {
                                useHooter( playerItem );
                                input->releaseKeyFor( "doughnut" );
                        }
                        // pick or drop an item when gliding
                        else if ( input->takeTyped() )
                        {
                                activity = ( playerItem->getTakenItemData() == nilPointer ? Activity::TakeItem : Activity::DropItem );
                                input->releaseKeyFor( "take" );
                        }
                        else if ( input->movenorthTyped() )
                        {
                                playerItem->changeOrientation( Way( "north" ) );
                        }
                        else if ( input->movesouthTyped() )
                        {
                                playerItem->changeOrientation( Way( "south" ) );
                        }
                        else if ( input->moveeastTyped() )
                        {
                                playerItem->changeOrientation( Way( "east" ) );
                        }
                        else if ( input->movewestTyped() )
                        {
                                playerItem->changeOrientation( Way( "west" ) );
                        }
                        else if ( ! input->anyMoveTyped() )
                        {
                                activity = Activity::Fall;
                        }
                }
        }
}

void PlayerHeadAndHeels::wait( PlayerItem * playerItem )
{
        playerItem->wait();

        if ( blinkingTimer->getValue() >= ( rand() % 4 ) + 5 )
        {
                blinkingTimer->reset();
                activity = Activity::Blink;
        }

        if ( FallKindOfActivity::getInstance()->fall( this ) )
        {
                speedTimer->reset();
                activity = Activity::Fall;
        }
}

void PlayerHeadAndHeels::blink( PlayerItem * playerItem )
{
        double timeValue = blinkingTimer->getValue();

        // close the eyes
        if ( ( timeValue > 0.0 && timeValue < 0.050 ) || ( timeValue > 0.400 && timeValue < 0.450 ) )
        {
                playerItem->changeFrame( blinkFrames[ playerItem->getOrientation().toString () ] );
        }
        // open the eyes
        else if ( ( timeValue > 0.250 && timeValue < 0.300 ) || ( timeValue > 0.750 && timeValue < 0.800 ) )
        {
        }
        // end blinking
        else if ( timeValue > 0.800 )
        {
                blinkingTimer->reset();
                activity = Activity::Wait;
        }
}

}
