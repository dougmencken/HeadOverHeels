
#include "PlayerHeels.hpp"
#include "Item.hpp"
#include "ItemData.hpp"
#include "PlayerItem.hpp"
#include "Mediator.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"


namespace isomot
{

PlayerHeels::PlayerHeels( Item* item, const std::string& behavior ) :
        UserControlled( item, behavior )
{
        jumpFrames = 20;
        highJumpFrames = 21;

        // salto normal
        for ( unsigned int i = 0; i < jumpFrames; i++ )
        {
                JumpMotion jumpMotion( i == 9 || i == 19 ? 2 : 1 , ( i < jumpFrames / 2 ) ? 3 : -3 );
                jumpVector.push_back( jumpMotion );
        }

        // salto largo
        for ( unsigned int i = 0; i < highJumpFrames; i++ )
        {
                JumpMotion jumpMotion( 2 , ( i < 17 ) ? 3 : -3 );
                highJumpVector.push_back( jumpMotion );
        }

        // la primera fase del salto
        jumpIndex = 0;

        // Fotogramas de caída
        fallFrames[ North ] = 6;
        fallFrames[ South ] = 0;
        fallFrames[  East ] = 9;
        fallFrames[  West ] = 3;

        // fotograma en blanco
        nullFrame = 12;

        labelOfTransitionViaTeleport = "bubbles";
        labelOfFireFromHooter = "bubbles";

        // pasos automáticos
        automaticStepsThruDoor = 16;

        // create and activate chronometers
        speedTimer = new Timer ();
        fallTimer = new Timer ();
        speedTimer->go ();
        fallTimer->go ();
}

PlayerHeels::~PlayerHeels( )
{
}

bool PlayerHeels::update()
{
        PlayerItem* player = dynamic_cast< PlayerItem* >( this->item );

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

                case TakeItem:
                case TakeAndJump:
                        takeItem( player );
                        break;

                case TakenItem:
                        player->addToZ( - LayerHeight );
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
        SoundManager::getInstance()->play( player->getOriginalLabel(), activity );

        return false;
}

void PlayerHeels::behave ()
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
                        if ( input->take() )
                        {
                                activity = ( playerItem->getTakenItemData() == nilPointer ? TakeItem : DropItem );
                                input->noRepeat( "take" );
                        }
                        else if ( input->takeAndJump() )
                        {
                                activity = ( playerItem->getTakenItemData() == nilPointer ? TakeAndJump : DropAndJump );
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
                        else if ( input->jump() )
                        {
                                // look for item below
                                playerItem->checkPosition( 0, 0, -1, Add );
                                // key to teleport is the same as for jump
                                activity = ( playerItem->getMediator()->collisionWithByBehavior( "behavior of teletransport" ) ? BeginWayOutTeletransport : Jump );
                        }
                }
                // already moving
                else if ( activity == MoveNorth || activity == MoveSouth || activity == MoveEast || activity == MoveWest )
                {
                        if( input->jump() )
                        {
                                // teleport when teletransport is below
                                playerItem->checkPosition( 0, 0, -1, Add );
                                activity = ( playerItem->getMediator()->collisionWithByBehavior( "behavior of teletransport" ) ? BeginWayOutTeletransport : Jump );
                        }
                        else if ( input->take() )
                        {
                                activity = ( playerItem->getTakenItemData() == nilPointer ? TakeItem : DropItem );
                                input->noRepeat( "take" );
                        }
                        else if ( input->takeAndJump() )
                        {
                                activity = ( playerItem->getTakenItemData() == nilPointer ? TakeAndJump : DropAndJump );
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
                // if you are being displaced
                else if ( activity == DisplaceNorth || activity == DisplaceSouth || activity == DisplaceEast || activity == DisplaceWest )
                {
                        if ( input->jump() )
                        {
                                activity = Jump;
                        }
                        else if ( input->take() )
                        {
                                activity = ( playerItem->getTakenItemData() == nilPointer ? TakeItem : DropItem );
                                input->noRepeat( "take" );
                        }
                        else if ( input->takeAndJump() )
                        {
                                activity = ( playerItem->getTakenItemData() == nilPointer ? TakeAndJump : DropAndJump );
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

                }
                else if ( activity == Fall )
                {
                        // pick or drop an item when falling
                        if ( input->take() )
                        {
                                activity = ( playerItem->getTakenItemData() == nilPointer ? TakeItem : DropItem );
                                input->noRepeat( "take" );
                        }
                }
        }
}

}
