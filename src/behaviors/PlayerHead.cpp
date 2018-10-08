
#include "PlayerHead.hpp"
#include "Item.hpp"
#include "ItemData.hpp"
#include "PlayerItem.hpp"
#include "Mediator.hpp"
#include "MoveKindOfActivity.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"


namespace isomot
{

PlayerHead::PlayerHead( Item* item, const std::string& behavior ) :
        UserControlled( item, behavior )
{
        jumpFrames = 20;
        highJumpFrames = 19;

        // salto normal
        for ( unsigned int i = 0; i < jumpFrames; i++ )
        {
                jumpVector.push_back( std::pair< int /* xy */, int /* z */ >( 1, 3 ) );
        }

        // salto largo
        for ( unsigned int i = 0; i < highJumpFrames; i++ )
        {
                highJumpVector.push_back( std::pair< int /* xy */, int /* z */ >( 1, 4 ) );
        }

        // la primera fase del salto
        jumpIndex = 0;

        // fotogramas de caída
        fallFrames[ "north" ] = 6;
        fallFrames[ "south" ] = 12;
        fallFrames[ "east" ] = 9;
        fallFrames[ "west" ] = 13;

        // fotograma en blanco
        nullFrame = 14;

        // fotogramas de parpadeo
        blinkFrames[ "north" ] = 6;
        blinkFrames[ "south" ] = 15;
        blinkFrames[ "east" ] = 9;
        blinkFrames[ "west" ] = 16;

        labelOfTransitionViaTeleport = "bubbles";
        labelOfFireFromHooter = "bubbles";

        // pasos automáticos
        automaticStepsThruDoor = 16;

        // create and activate chronometers
        speedTimer = new Timer ();
        fallTimer = new Timer ();
        glideTimer = new Timer ();
        blinkingTimer = new Timer ();
        speedTimer->go ();
        fallTimer->go ();
        glideTimer->go ();
        blinkingTimer->go ();
}

PlayerHead::~PlayerHead( )
{
}

bool PlayerHead::update ()
{
        PlayerItem* player = dynamic_cast< PlayerItem* >( this->item );

        if ( player->hasShield() )
        {
                player->decreaseShield();
        }

        // change height for climbing bars easily
        player->setHeight( activity == Activity::Fall || activity == Activity::Glide ? 23 : 24 );

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

                default:
                        ;
        }

        // play sound for current activity
        SoundManager::getInstance()->play( player->getOriginalLabel(), activity );

        return false;
}

void PlayerHead::behave ()
{
        PlayerItem* playerItem = dynamic_cast< PlayerItem* >( this->item );
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
                                // look for teletransport below
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
                // if you are being displaced
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
                // if you are being forcibly displaced
                else if ( activity == Activity::ForceDisplaceNorth || activity == Activity::ForceDisplaceSouth ||
                        activity == Activity::ForceDisplaceEast || activity == Activity::ForceDisplaceWest )
                {
                        if ( input->jumpTyped() )
                        {
                                activity = Activity::Jump;
                        }
                        // user moves while displacing
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
                        // Head may change direction when jumping
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
                        // entonces Head planea
                        else if ( input->anyMoveTyped() )
                        {
                                activity = Activity::Glide;
                        }
                }

                // for gliding, don’t wait for next cycle because there’s possibility
                // that gliding comes from falling, and waiting for next cycle may prevent
                // to enter gap between two grid items
                if ( activity == Activity::Glide )
                {
                        if ( input->doughnutTyped() && ! donutFromHooterIsHere )
                        {
                                useHooter( playerItem );
                                input->releaseKeyFor( "doughnut" );
                        }
                        // Head may change direction when gliding
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

void PlayerHead::wait( PlayerItem* playerItem )
{
        playerItem->wait();

        // Si está detenido entre 4 y 9 segundos entonces parpadea
        if( blinkingTimer->getValue() >= ( rand() % 4 ) + 5 )
        {
                blinkingTimer->reset();
                activity = Activity::Blink;
        }

        // Se comprueba si el jugador debe empezar a caer
        if( FallKindOfActivity::getInstance()->fall( this ) )
        {
                speedTimer->reset();
                activity = Activity::Fall;
        }
}

void PlayerHead::blink( PlayerItem* playerItem )
{
        double timeValue = blinkingTimer->getValue();

        if( ( timeValue > 0.0 && timeValue < 0.050 ) || ( timeValue > 0.400 && timeValue < 0.450 ) )
        {
                playerItem->changeFrame( blinkFrames[ playerItem->getOrientation().toString () ] );
        }
        else if( ( timeValue > 0.250 && timeValue < 0.300 ) || ( timeValue > 0.750 && timeValue < 0.800 ) )
        {
        }
        else if( timeValue > 0.800 )
        {
                blinkingTimer->reset();
                activity = Activity::Wait;
        }
}

}
