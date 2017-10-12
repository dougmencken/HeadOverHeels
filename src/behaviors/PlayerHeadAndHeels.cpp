
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
        // Fotogramas del salto
        jumpFrames = 28;
        highJumpFrames = 28;

        // Definición del salto normal
        for ( int i = 0; i < jumpFrames; i++ )
        {
                // Unidades que se desplazará en el eje X o Y y en el eje Z, respectivamente
                JumpMotion jumpMotion( 1, ( i < 4 ? 4 : ( i < 8 ? 3 : 2 ) ) );
                jumpMatrix.push_back( jumpMotion );
        }

        // Definición del salto largo
        for ( int i = 0; i < highJumpFrames; i++ )
        {
                // Unidades que se desplazará en el eje X o Y y en el eje Z, respectivamente
                JumpMotion jumpMotion( 1, 3 );
                highJumpMatrix.push_back( jumpMotion );
        }

        // La primera fase del salto
        jumpIndex = 0;

        // Fotogramas de caída
        fallFrames[ North ] = 8;
        fallFrames[ South ] = 16;
        fallFrames[ East ] = 12;
        fallFrames[ West ] = 17;

        // Fotograma en blanco
        nullFrame = 20;

        // Fotogramas de parpadeo
        blinkFrames[ North ] = 8;
        blinkFrames[ South ] = 18;
        blinkFrames[ East ] = 12;
        blinkFrames[ West ] = 19;

        labelOfTransitionViaTeleport = "double-bubbles";
        labelOfFireFromHooter = "bubbles";

        // Pasos automáticos
        automaticStepsCounter = 16;

        // Creación y puesta en marcha de los cronómetros
        speedTimer = new HPC();
        fallTimer = new HPC();
        glideTimer = new HPC();
        blinkingTimer = new HPC();
        speedTimer->start();
        fallTimer->start();
        glideTimer->start();
        blinkingTimer->start();

        // En principio no hay ningún disparo en la sala
        fireFromHooterIsPresent = false;
}

PlayerHeadAndHeels::~PlayerHeadAndHeels( )
{
        delete speedTimer;
        delete fallTimer;
        delete glideTimer;
        delete blinkingTimer;
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

                case TakeItem:
                case TakeAndJump:
                        takeItem( player );
                        break;

                case TakenItem:
                        player->addToZ( -LayerHeight );
                        activity = Wait;
                        break;

                case DropItem:
                case DropAndJump:
                        dropItem( player );
                        break;

                default:
                        ;
        }

        // play sound for current activity
        SoundManager::getInstance()->play( player->getLabel(), activity );

        return false;
}

void PlayerHeadAndHeels::behave ()
{
        PlayerItem* playerItem = dynamic_cast< PlayerItem * >( this->item );
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
                                // is there teleport below
                                playerItem->checkPosition( 0, 0, -1, Add );
                                activity = ( playerItem->getMediator()->collisionWithByBehavior( "behavior of teletransport" ) ? BeginWayOutTeletransport : Jump );
                        }
                        else if ( input->doughnut() && ! fireFromHooterIsPresent )
                        {
                                useHooter( playerItem );
                                input->noRepeat( "doughnut" );
                        }
                        else if ( input->take() )
                        {
                                activity = ( playerItem->getTakenItemData() == 0 ? TakeItem : DropItem );
                                input->noRepeat( "take" );
                        }
                        else if ( input->takeAndJump() )
                        {
                                activity = ( playerItem->getTakenItemData() == 0 ? TakeAndJump : DropAndJump );
                                input->noRepeat( "take&jump" );
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
                                // jump or teleport
                                playerItem->checkPosition( 0, 0, -1, Add );
                                activity = ( playerItem->getMediator()->collisionWithByBehavior( "behavior of teletransport" ) ? BeginWayOutTeletransport : Jump );
                        }
                        else if ( input->doughnut() && ! fireFromHooterIsPresent )
                        {
                                useHooter( playerItem );
                                input->noRepeat( "doughnut" );
                        }
                        else if ( input->take() )
                        {
                                activity = ( playerItem->getTakenItemData() == 0 ? TakeItem : DropItem );
                                input->noRepeat( "take" );
                        }
                        else if ( input->takeAndJump() )
                        {
                                activity = ( playerItem->getTakenItemData() == 0 ? TakeAndJump : DropAndJump );
                                input->noRepeat( "take&jump" );
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
                // character is being displaced
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
                        else if ( input->take() )
                        {
                                activity = ( playerItem->getTakenItemData() == 0 ? TakeItem : DropItem );
                                input->noRepeat( "take" );
                        }
                        else if ( input->takeAndJump() )
                        {
                                activity = ( playerItem->getTakenItemData() == 0 ? TakeAndJump : DropAndJump );
                                input->noRepeat( "take&jump" );
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
                // character is being displaced forcibly
                else if ( activity == ForceDisplaceNorth || activity == ForceDisplaceSouth || activity == ForceDisplaceEast || activity == ForceDisplaceWest )
                {
                        if ( input->jump() )
                        {
                                activity = Jump;
                        }
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
                        // pick or drop an item when falling
                        else if ( input->take() )
                        {
                                activity = ( playerItem->getTakenItemData() == 0 ? TakeItem : DropItem );
                                input->noRepeat( "take" );
                        }
                        // entonces Head y Heels planean
                        else if ( input->movenorth() || input->movesouth() || input->moveeast() || input->movewest() )
                        {
                                activity = Glide;
                        }
                }

                // for gliding, don’t wait for next cycle because there’s possibility
                // that gliding comes from falling, and waiting for next cycle may prevent
                // to enter gap between grid items
                if ( activity == Glide )
                {
                        if ( input->doughnut() && ! fireFromHooterIsPresent )
                        {
                                useHooter( playerItem );
                                input->noRepeat( "doughnut" );
                        }
                        // pick or drop an item when gliding
                        else if ( input->take() )
                        {
                                activity = ( playerItem->getTakenItemData() == 0 ? TakeItem : DropItem );
                                input->noRepeat( "take" );
                        }
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

void PlayerHeadAndHeels::wait( PlayerItem * playerItem )
{
        playerItem->wait();

        if ( blinkingTimer->getValue() >= ( rand() % 4 ) + 5 )
        {
                blinkingTimer->reset();
                activity = Blink;
        }

        if ( FallKindOfActivity::getInstance()->fall( this ) )
        {
                speedTimer->reset();
                activity = Fall;
        }
}

void PlayerHeadAndHeels::blink( PlayerItem * playerItem )
{
        double timeValue = blinkingTimer->getValue();

        // close the eyes
        if ( ( timeValue > 0.0 && timeValue < 0.050 ) || ( timeValue > 0.400 && timeValue < 0.450 ) )
        {
                playerItem->changeFrame( blinkFrames[ playerItem->getOrientation().getIntegerOfWay () ] );
        }
        // open the eyes
        else if ( ( timeValue > 0.250 && timeValue < 0.300 ) || ( timeValue > 0.750 && timeValue < 0.800 ) )
        {
        }
        // end blinking
        else if ( timeValue > 0.800 )
        {
                blinkingTimer->reset();
                activity = Wait;
        }
}

}
