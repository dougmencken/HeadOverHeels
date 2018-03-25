
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
                JumpMotion jumpMotion( 1, 3 );
                jumpVector.push_back( jumpMotion );
        }

        // salto largo
        for ( unsigned int i = 0; i < highJumpFrames; i++ )
        {
                JumpMotion jumpMotion( 1, 4 );
                highJumpVector.push_back( jumpMotion );
        }

        // la primera fase del salto
        jumpIndex = 0;

        // fotogramas de caída
        fallFrames[ North ] = 6;
        fallFrames[ South ] = 12;
        fallFrames[ East ] = 9;
        fallFrames[ West ] = 13;

        // fotograma en blanco
        nullFrame = 14;

        // fotogramas de parpadeo
        blinkFrames[ North ] = 6;
        blinkFrames[ South ] = 15;
        blinkFrames[ East ] = 9;
        blinkFrames[ West ] = 16;

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

        fireFromHooterIsPresent = false;
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
        player->setHeight( activity == Fall || activity == Glide ? 23 : 24 );

        switch ( activity )
        {
                case Wait:
                        wait( player );
                        break;

                case AutoMoveNorth:
                case AutoMoveSouth:
                case AutoMoveEast:
                case AutoMoveWest:
                        autoMove( player );
                        break;

                case MoveNorth:
                case MoveSouth:
                case MoveEast:
                case MoveWest:
                        move( player );
                        break;

                case DisplaceNorth:
                case DisplaceSouth:
                case DisplaceEast:
                case DisplaceWest:
                case DisplaceNortheast:
                case DisplaceSoutheast:
                case DisplaceSouthwest:
                case DisplaceNorthwest:
                case ForceDisplaceNorth:
                case ForceDisplaceSouth:
                case ForceDisplaceEast:
                case ForceDisplaceWest:
                        displace( player );
                        break;

                case CancelDisplaceNorth:
                case CancelDisplaceSouth:
                case CancelDisplaceEast:
                case CancelDisplaceWest:
                        cancelDisplace( player );
                        break;

                case Fall:
                        fall( player );
                        break;

                case Jump:
                case RegularJump:
                case HighJump:
                        jump( player );
                        break;

                case BeginWayOutTeletransport:
                case WayOutTeletransport:
                        wayOutTeletransport( player );
                        break;

                case BeginWayInTeletransport:
                case WayInTeletransport:
                        wayInTeletransport( player );
                        break;

                case MeetMortalItem:
                case Vanish:
                        collideWithMortalItem( player );
                        break;

                case Glide:
                        glide( player );
                        break;

                case Blink:
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
        if ( activity != AutoMoveNorth && activity != AutoMoveSouth && activity != AutoMoveEast && activity != AutoMoveWest &&
                activity != BeginWayOutTeletransport && activity != WayOutTeletransport && activity != BeginWayInTeletransport && activity != WayInTeletransport &&
                activity != MeetMortalItem && activity != Vanish )
        {
                // when waiting or blinking
                if ( activity == Wait || activity == Blink )
                {
                        if ( input->jump() )
                        {
                                // jump or teleport
                                playerItem->checkPosition( 0, 0, -1, Add );
                                activity = ( playerItem->getMediator()->collisionWithByBehavior( "behavior of teletransport" ) ? BeginWayOutTeletransport : Jump );
                        }
                        else if ( input->doughnut() && ! fireFromHooterIsPresent )
                        {
                                useHooter( playerItem );
                                input->noRepeat( "doughnut" );
                        }
                        else if ( input->movenorth() && ! input->movesouth() && ! input->moveeast() && ! input->movewest() )
                        {
                                activity = MoveNorth;
                        }
                        else if ( ! input->movenorth() && input->movesouth() && ! input->moveeast() && ! input->movewest() )
                        {
                                activity = MoveSouth;
                        }
                        else if ( ! input->movenorth() && ! input->movesouth() && input->moveeast() && ! input->movewest() )
                        {
                                activity = MoveEast;
                        }
                        else if ( ! input->movenorth() && ! input->movesouth() && ! input->moveeast() && input->movewest() )
                        {
                                activity = MoveWest;
                        }
                }
                // already moving
                else if ( activity == MoveNorth || activity == MoveSouth || activity == MoveEast || activity == MoveWest )
                {
                        if ( input->jump() )
                        {
                                // look for teletransport below
                                playerItem->checkPosition( 0, 0, -1, Add );
                                activity = ( playerItem->getMediator()->collisionWithByBehavior( "behavior of teletransport" ) ? BeginWayOutTeletransport : Jump );
                        }
                        else if ( input->doughnut() && ! fireFromHooterIsPresent )
                        {
                                useHooter( playerItem );
                                input->noRepeat( "doughnut" );
                        }
                        else if ( input->movenorth() && ! input->movesouth() && ! input->moveeast() && ! input->movewest() )
                        {
                                activity = MoveNorth;
                        }
                        else if ( ! input->movenorth() && input->movesouth() && ! input->moveeast() && ! input->movewest() )
                        {
                                activity = MoveSouth;
                        }
                        else if ( ! input->movenorth() && ! input->movesouth() && input->moveeast() && ! input->movewest() )
                        {
                                activity = MoveEast;
                        }
                        else if ( ! input->movenorth() && ! input->movesouth() && ! input->moveeast() && input->movewest() )
                        {
                                activity = MoveWest;
                        }
                        else if ( ! input->movenorth() && ! input->movesouth() && ! input->moveeast() && ! input->movewest() )
                        {
                                SoundManager::getInstance()->stop( playerItem->getLabel(), activity );
                                activity = Wait;
                        }
                }
                // if you are being displaced
                else if ( activity == DisplaceNorth || activity == DisplaceSouth || activity == DisplaceEast || activity == DisplaceWest )
                {
                        if ( input->jump() )
                        {
                                activity = Jump;
                        }
                        else if ( input->doughnut() && ! fireFromHooterIsPresent )
                        {
                                useHooter( playerItem );
                                input->noRepeat( "doughnut" );
                        }
                        else if ( input->movenorth() && ! input->movesouth() && ! input->moveeast() && ! input->movewest() )
                        {
                                activity = MoveNorth;
                        }
                        else if ( ! input->movenorth() && input->movesouth() && ! input->moveeast() && ! input->movewest() )
                        {
                                activity = MoveSouth;
                        }
                        else if ( ! input->movenorth() && ! input->movesouth() && input->moveeast() && ! input->movewest() )
                        {
                                activity = MoveEast;
                        }
                        else if ( ! input->movenorth() && ! input->movesouth() && ! input->moveeast() && input->movewest() )
                        {
                                activity = MoveWest;
                        }
                }
                // if you are being forcibly displaced
                else if ( activity == ForceDisplaceNorth || activity == ForceDisplaceSouth || activity == ForceDisplaceEast || activity == ForceDisplaceWest )
                {
                        if ( input->jump() )
                        {
                                activity = Jump;
                        }
                        // user moves while displacing
                        // cancel displace when moving in direction opposite to displacement
                        else if ( input->movenorth() && ! input->movesouth() && ! input->moveeast() && ! input->movewest() )
                        {
                                activity = ( activity == ForceDisplaceSouth ? CancelDisplaceSouth : MoveNorth );
                        }
                        else if ( ! input->movenorth() && input->movesouth() && ! input->moveeast() && ! input->movewest() )
                        {
                                activity = ( activity == ForceDisplaceNorth ? CancelDisplaceNorth : MoveSouth );
                        }
                        else if ( ! input->movenorth() && ! input->movesouth() && input->moveeast() && ! input->movewest() )
                        {
                                activity = ( activity == ForceDisplaceWest ? CancelDisplaceWest : MoveEast );
                        }
                        else if ( ! input->movenorth() && ! input->movesouth() && ! input->moveeast() && input->movewest() )
                        {
                                activity = ( activity == ForceDisplaceEast ? CancelDisplaceEast : MoveWest );
                        }
                }
                else if ( activity == Jump || activity == RegularJump || activity == HighJump )
                {
                        if ( input->doughnut() && ! fireFromHooterIsPresent )
                        {
                                useHooter( playerItem );
                                input->noRepeat( "doughnut" );
                        }
                        // Head may change direction when jumping
                        else if ( input->movenorth() && ! input->movesouth() && ! input->moveeast() && ! input->movewest() )
                        {
                                playerItem->changeOrientation( North );
                        }
                        else if ( ! input->movenorth() && input->movesouth() && ! input->moveeast() && ! input->movewest() )
                        {
                                playerItem->changeOrientation( South );
                        }
                        else if ( ! input->movenorth() && ! input->movesouth() && input->moveeast() && ! input->movewest() )
                        {
                                playerItem->changeOrientation( East );
                        }
                        else if ( ! input->movenorth() && ! input->movesouth() && ! input->moveeast() && input->movewest() )
                        {
                                playerItem->changeOrientation( West );
                        }
                }
                else if ( activity == Fall )
                {
                        if ( input->doughnut() && ! fireFromHooterIsPresent )
                        {
                                useHooter( playerItem );
                                input->noRepeat( "doughnut" );
                        }
                        // entonces Head planea
                        else if ( input->movenorth() || input->movesouth() || input->moveeast() || input->movewest() )
                        {
                                activity = Glide;
                        }
                }

                // for gliding, don’t wait for next cycle because there’s possibility
                // that gliding comes from falling, and waiting for next cycle may prevent
                // to enter gap between two grid items
                if ( activity == Glide )
                {
                        if ( input->doughnut() && ! fireFromHooterIsPresent )
                        {
                                useHooter( playerItem );
                                input->noRepeat( "doughnut" );
                        }
                        // Head may change direction when gliding
                        else if ( input->movenorth() && ! input->movesouth() && ! input->moveeast() && ! input->movewest() )
                        {
                                playerItem->changeOrientation( North );
                        }
                        else if ( ! input->movenorth() && input->movesouth() && ! input->moveeast() && ! input->movewest() )
                        {
                                playerItem->changeOrientation( South );
                        }
                        else if ( ! input->movenorth() && ! input->movesouth() && input->moveeast() && ! input->movewest() )
                        {
                                playerItem->changeOrientation( East );
                        }
                        else if ( ! input->movenorth() && ! input->movesouth() && ! input->moveeast() && input->movewest() )
                        {
                                playerItem->changeOrientation( West );
                        }
                        else if ( ! input->movenorth() && ! input->movesouth() && ! input->moveeast() && ! input->movewest() )
                        {
                                activity = Fall;
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
                activity = Blink;
        }

        // Se comprueba si el jugador debe empezar a caer
        if( FallKindOfActivity::getInstance()->fall( this ) )
        {
                speedTimer->reset();
                activity = Fall;
        }
}

void PlayerHead::blink( PlayerItem* playerItem )
{
        double timeValue = blinkingTimer->getValue();

        if( ( timeValue > 0.0 && timeValue < 0.050 ) || ( timeValue > 0.400 && timeValue < 0.450 ) )
        {
                playerItem->changeFrame( blinkFrames[ playerItem->getOrientation().getIntegerOfWay () ] );
        }
        else if( ( timeValue > 0.250 && timeValue < 0.300 ) || ( timeValue > 0.750 && timeValue < 0.800 ) )
        {
        }
        else if( timeValue > 0.800 )
        {
                blinkingTimer->reset();
                activity = Wait;
        }
}

}
