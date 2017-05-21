
#include "PlayerHeels.hpp"
#include "Item.hpp"
#include "ItemData.hpp"
#include "PlayerItem.hpp"
#include "Mediator.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"


namespace isomot
{

PlayerHeels::PlayerHeels( Item* item, const BehaviorOfItem& id ) :
        UserControlled( item, id )
{
        // Fotogramas del salto
        jumpFrames = 20;
        highJumpFrames = 21;

        // Definición del salto normal
        for( int i = 0; i < jumpFrames; i++ )
        {
                // Unidades que se desplazará en el eje X o Y y en el eje Z, respectivamente
                JumpMotion jumpMotion( i == 9 || i == 19 ? 2 : 1 , ( i < jumpFrames / 2 ) ? 3 : -3 );
                jumpMatrix.push_back( jumpMotion );
        }

        // Definición del salto largo
        for( int i = 0; i < highJumpFrames; i++ )
        {
                // Unidades que se desplazará en el eje X o Y y en el eje Z, respectivamente
                JumpMotion jumpMotion( 2 , ( i < 17 ) ? 3 : -3 );
                highJumpMatrix.push_back( jumpMotion );
        }

        // La primera fase del salto
        jumpIndex = 0;

        // Fotogramas de caída
        fallFrames[ North ] = 6;
        fallFrames[ South ] = 0;
        fallFrames[  East ] = 9;
        fallFrames[  West ] = 3;

        // Fotograma en blanco
        nullFrame = 12;

        // Elemento de transición entre telepuertos
        transitionDataLabel = 38;
        // Elemento empleado como disparo
        labelOfFireFromHooter = 38;

        // Pasos automáticos
        automaticStepsCounter = 16;

        // Creación y puesta en marcha de los cronómetros
        speedTimer = new HPC ();
        fallTimer = new HPC ();
        speedTimer->start ();
        fallTimer->start ();

        // En principio no se cogido ningún elemento
        takenItemData = 0;
}

PlayerHeels::~PlayerHeels( )
{
        delete speedTimer;
        delete fallTimer;
}

bool PlayerHeels::update()
{
        PlayerItem* playerItem = dynamic_cast< PlayerItem* >( this->item );

        // Comprueba si tiene inmunidad y, en tal caso, si ha finalizado
        if ( playerItem->getShieldTime() > 0 )
        {
                playerItem->decreaseShield();
        }

        // Acciones a realizar en función del estado actual del jugador
        switch ( activity )
        {
                case Wait:
                        wait( playerItem );
                        break;

                case AutoMoveNorth:
                case AutoMoveSouth:
                case AutoMoveEast:
                case AutoMoveWest:
                        autoMove( playerItem );
                        break;

                case MoveNorth:
                case MoveSouth:
                case MoveEast:
                case MoveWest:
                        move( playerItem );
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
                        displace( playerItem );
                        break;

                case CancelDisplaceNorth:
                case CancelDisplaceSouth:
                case CancelDisplaceEast:
                case CancelDisplaceWest:
                        cancelDisplace( playerItem );
                        break;

                case Fall:
                        fall( playerItem );
                        break;

                case Jump:
                case RegularJump:
                case HighJump:
                        jump( playerItem );
                        break;

                case StartWayOutTeletransport:
                case WayOutTeletransport:
                        wayOutTeletransport( playerItem );
                        break;

                case StartWayInTeletransport:
                case WayInTeletransport:
                        wayInTeletransport( playerItem );
                        break;

                case StartDestroy:
                case Destroy:
                        destroy( playerItem );
                        break;

                case TakeItem:
                case TakeAndJump:
                        take( playerItem );
                        break;

                case TakenItem:
                        playerItem->addToZ( - LayerHeight );
                        activity = Wait;
                        break;

                case DropItem:
                case DropAndJump:
                        drop( playerItem );
                        break;

                default:
                        ;
        }

        // Reproduce el sonido asociado al estado actual
        SoundManager::getInstance()->play( playerItem->getLabel(), activity );

        return false;
}

void PlayerHeels::behave ()
{
        PlayerItem* playerItem = dynamic_cast< PlayerItem * >( this->item );
        InputManager* input = InputManager::getInstance();

        // Si no está en modo automático
        if ( activity != AutoMoveNorth && activity != AutoMoveSouth && activity != AutoMoveEast && activity != AutoMoveWest &&
                activity != StartWayOutTeletransport && activity != WayOutTeletransport && activity != StartWayInTeletransport && activity != WayInTeletransport &&
                activity != StartDestroy && activity != Destroy )
        {
                // Si está esperando o parpadeando
                if ( activity == Wait || activity == Blink )
                {
                        // ...y ha pulsado la tecla para coger un elemento entonces intenta cogerlo / dejarlo
                        if ( input->take() )
                        {
                                activity = ( takenItemData == 0 ? TakeItem : DropItem );
                                // Las repeticiones de esta tecla no deben procesarse
                                input->noRepeat( "take" );
                        }
                        // ...y ha pulsado la tecla para coger un elemento y luego saltar entonces
                        // intenta cogerlo / dejarlo y luego salta
                        else if ( input->takeAndJump() )
                        {
                                activity = ( takenItemData == 0 ? TakeAndJump : DropAndJump );
                                // Las repeticiones de esta tecla no deben procesarse
                                input->noRepeat( "take-jump" );
                        }
                        // ...y se ha pulsado alguna tecla de movimiento entonces se mueve
                        else if ( input->movenorth() && !input->movesouth() && !input->moveeast() && !input->movewest() )
                        {
                                activity = MoveNorth;
                        }
                        else if ( !input->movenorth() && input->movesouth() && !input->moveeast() && !input->movewest() )
                        {
                                activity = MoveSouth;
                        }
                        else if ( !input->movenorth() && !input->movesouth() && input->moveeast() && !input->movewest() )
                        {
                                activity = MoveEast;
                        }
                        else if ( !input->movenorth() && !input->movesouth() && !input->moveeast() && input->movewest() )
                        {
                                activity = MoveWest;
                        }
                        // ...y se ha pulsado la tecla de salto entonces salta
                        else if ( input->jump() )
                        {
                                // Almacena en la pila de colisiones los elementos que tiene debajo
                                playerItem->checkPosition( 0, 0, -1, Add );
                                // Si está sobre un telepuerto y salta entonces el jugador será teletransportado, sino saltará
                                activity = ( playerItem->getMediator()->collisionWithByBehavior( TeleportBehavior ) ? StartWayOutTeletransport : Jump );
                        }
                }
                // Si está moviéndose
                else if ( activity == MoveNorth || activity == MoveSouth || activity == MoveEast || activity == MoveWest )
                {
                        // ...y se ha pulsado la tecla de salto entonces salta
                        if( input->jump() )
                        {
                                // Almacena en la pila de colisiones los elementos que tiene debajo
                                playerItem->checkPosition( 0, 0, -1, Add );
                                // Si está sobre un telepuerto y salta entonces el jugador será teletransportado, sino saltará
                                activity = ( playerItem->getMediator()->collisionWithByBehavior( TeleportBehavior ) ? StartWayOutTeletransport : Jump );
                        }
                        // ...y ha pulsado la tecla para coger un elemento entonces intenta cogerlo / dejarlo
                        else if ( input->take() )
                        {
                                activity = ( takenItemData == 0 ? TakeItem : DropItem );
                                // Las repeticiones de esta tecla no deben procesarse
                                input->noRepeat( "take" );
                        }
                        // ...y ha pulsado la tecla para coger un elemento y luego saltar entonces
                        // intenta cogerlo / dejarlo y luego salta
                        else if ( input->takeAndJump() )
                        {
                                activity = ( takenItemData == 0 ? TakeAndJump : DropAndJump );
                                // Las repeticiones de esta tecla no deben procesarse
                                input->noRepeat( "take-jump" );
                        }
                        // ...y se ha pulsado alguna tecla de movimiento entonces sigue moviéndose
                        else if ( input->movenorth() && !input->movesouth() && !input->moveeast() && !input->movewest() )
                        {
                                activity = MoveNorth;
                        }
                        else if ( !input->movenorth() && input->movesouth() && !input->moveeast() && !input->movewest() )
                        {
                                activity = MoveSouth;
                        }
                        else if ( !input->movenorth() && !input->movesouth() && input->moveeast() && !input->movewest() )
                        {
                                activity = MoveEast;
                        }
                        else if ( !input->movenorth() && !input->movesouth() && !input->moveeast() && input->movewest() )
                        {
                                activity = MoveWest;
                        }
                        // Si por el contrario se han soltado las teclas de movimiento entonces se pone en espera
                        else if ( !input->movenorth() && !input->movesouth() && !input->moveeast() && !input->movewest() )
                        {
                                SoundManager::getInstance()->stop( playerItem->getLabel(), activity );
                                activity = Wait;
                        }
                }
                // Si está siendo desplazado
                else if ( activity == DisplaceNorth || activity == DisplaceSouth || activity == DisplaceEast || activity == DisplaceWest )
                {
                        // ...y se ha pulsado la tecla de salto entonces salta
                        if ( input->jump() )
                        {
                                activity = Jump;
                        }
                        // ...y ha pulsado la tecla para coger un elemento entonces intenta cogerlo
                        else if ( input->take() )
                        {
                                activity = ( takenItemData == 0 ? TakeItem : DropItem );
                                // Las repeticiones de esta tecla no deben procesarse
                                input->noRepeat( "take" );
                        }
                        // ...y ha pulsado la tecla para coger un elemento y luego saltar entonces
                        // intenta cogerlo / dejarlo y luego salta
                        else if ( input->takeAndJump() )
                        {
                                activity = ( takenItemData == 0 ? TakeAndJump : DropAndJump );
                                // Las repeticiones de esta tecla no deben procesarse
                                input->noRepeat( "take-jump" );
                        }
                        // ...y se ha pulsado alguna tecla de movimiento entonces sigue moviéndose
                        else if ( input->movenorth() && !input->movesouth() && !input->moveeast() && !input->movewest() )
                        {
                                activity = MoveNorth;
                        }
                        else if ( !input->movenorth() && input->movesouth() && !input->moveeast() && !input->movewest() )
                        {
                                activity = MoveSouth;
                        }
                        else if ( !input->movenorth() && !input->movesouth() && input->moveeast() && !input->movewest() )
                        {
                                activity = MoveEast;
                        }
                        else if ( !input->movenorth() && !input->movesouth() && !input->moveeast() && input->movewest() )
                        {
                                activity = MoveWest;
                        }
                }
                // Si está siendo desplazado forzosamente
                else if ( activity == ForceDisplaceNorth || activity == ForceDisplaceSouth || activity == ForceDisplaceEast || activity == ForceDisplaceWest )
                {
                        // ...y se ha pulsado la tecla de salto entonces salta
                        if ( input->jump() )
                        {
                                activity = Jump;
                        }
                        // ...y se ha pulsado alguna tecla de movimiento entonces: si pretende avanzar en la dirección
                        // contraria a la que se está deplazando entonces se anula el desplazamiento; en caso contrario,
                        // avanza en la dirección que se esté ordenando
                        else if ( input->movenorth() && !input->movesouth() && !input->moveeast() && !input->movewest() )
                        {
                                activity = ( activity == ForceDisplaceSouth ? CancelDisplaceSouth : MoveNorth );
                        }
                        else if ( !input->movenorth() && input->movesouth() && !input->moveeast() && !input->movewest() )
                        {
                                activity = ( activity == ForceDisplaceNorth ? CancelDisplaceNorth : MoveSouth );
                        }
                        else if ( !input->movenorth() && !input->movesouth() && input->moveeast() && !input->movewest() )
                        {
                                activity = ( activity == ForceDisplaceWest ? CancelDisplaceWest : MoveEast );
                        }
                        else if ( !input->movenorth() && !input->movesouth() && !input->moveeast() && input->movewest() )
                        {
                                activity = ( activity == ForceDisplaceEast ? CancelDisplaceEast : MoveWest );
                        }
                }
                // Si está saltando
                else if ( activity == Jump || activity == RegularJump || activity == HighJump )
                {

                }
                // Si está cayendo
                else if ( activity == Fall )
                {
                        // ...y ha pulsado la tecla para coger un elemento entonces intenta cogerlo / dejarlo
                        if ( input->take() )
                        {
                                activity = ( takenItemData == 0 ? TakeItem : DropItem );
                                // Las repeticiones de esta tecla no deben procesarse
                                input->noRepeat( "take" );
                        }
                }
        }
}

}
