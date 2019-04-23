
#include "PlayerHead.hpp"
#include "Item.hpp"
#include "PlayerItem.hpp"
#include "Mediator.hpp"
#include "MoveKindOfActivity.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"


namespace iso
{

PlayerHead::PlayerHead( const ItemPtr & item, const std::string& behavior ) :
        UserControlled( item, behavior )
{
        jumpPhases = 20;
        highJumpPhases = 19;

        // salto normal
        for ( unsigned int i = 0; i < jumpPhases; i++ )
        {
                jumpVector.push_back( std::pair< int /* xy */, int /* z */ >( 1, 3 ) );
        }

        // salto largo
        for ( unsigned int i = 0; i < highJumpPhases; i++ )
        {
                highJumpVector.push_back( std::pair< int /* xy */, int /* z */ >( 1, 4 ) );
        }

        // fotogramas de caída
        fallFrames[ "north" ] = 8;
        fallFrames[ "south" ] = 16;
        fallFrames[ "east" ] = 12;
        fallFrames[ "west" ] = 17;

        // fotogramas de parpadeo
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

PlayerHead::~PlayerHead( )
{
}

bool PlayerHead::update ()
{
        PlayerItem& playerItem = dynamic_cast< PlayerItem& >( * this->item );

        if ( playerItem.hasShield() )
        {
                playerItem.decreaseShield();
        }

        // change height for climbing bars easily
        playerItem.setHeight( activity == Activity::Fall || activity == Activity::Glide ? 23 : 24 );

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

                default:
                        ;
        }

        // play sound for current activity
        SoundManager::getInstance().play( playerItem.getOriginalLabel(), activity );

        return false;
}

void PlayerHead::behave ()
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
                                // look for teletransport below
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
                // if you are being displaced
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
                // if you are being forcibly displaced
                else if ( activity == Activity::ForceDisplaceNorth || activity == Activity::ForceDisplaceSouth ||
                        activity == Activity::ForceDisplaceEast || activity == Activity::ForceDisplaceWest )
                {
                        if ( input.jumpTyped() )
                        {
                                activity = Activity::Jump;
                        }
                        // user moves while displacing
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
                        // Head may change orientation when jumping
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
                        // entonces Head planea
                        else if ( input.anyMoveTyped() )
                        {
                                activity = Activity::Glide;
                        }
                }

                // for gliding, don’t wait for next cycle because there’s possibility
                // that gliding comes from falling, and waiting for next cycle may prevent
                // to enter gap between two grid items
                if ( activity == Activity::Glide )
                {
                        if ( input.doughnutTyped() && ! donutFromHooterIsHere )
                        {
                                useHooter( playerItem );
                                input.releaseKeyFor( "doughnut" );
                        }
                        // Head may change orientation when gliding
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

void PlayerHead::wait( PlayerItem& playerItem )
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

void PlayerHead::blink( PlayerItem& playerItem )
{
        double timeToBlink = timerForBlinking->getValue();

        if ( ( timeToBlink > 0.0 && timeToBlink < 0.050 ) || ( timeToBlink > 0.400 && timeToBlink < 0.450 ) )
        {
                playerItem.changeFrame( blinkFrames[ playerItem.getOrientation() ] );
        }
        else if ( ( timeToBlink > 0.250 && timeToBlink < 0.300 ) || ( timeToBlink > 0.750 && timeToBlink < 0.800 ) )
        {
        }
        else if ( timeToBlink > 0.800 )
        {
                timerForBlinking->reset();
                activity = Activity::Wait;
        }
}

}
