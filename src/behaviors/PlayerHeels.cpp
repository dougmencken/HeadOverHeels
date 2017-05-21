
#include "PlayerHeels.hpp"
#include "Item.hpp"
#include "ItemData.hpp"
#include "PlayerItem.hpp"
#include "Mediator.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"


namespace isomot
{

PlayerHeels::PlayerHeels( Item* item, const BehaviorId& id ) :
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
        shotDataLabel = 38;

        // Pasos automáticos
        automaticStepsCounter = 16;

        // Estado inicial
        stateId = StateWait;

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
        switch ( stateId )
        {
                case StateWait:
                        wait( playerItem );
                        break;

                case StateAutoMoveNorth:
                case StateAutoMoveSouth:
                case StateAutoMoveEast:
                case StateAutoMoveWest:
                        autoMove( playerItem );
                        break;

                case StateMoveNorth:
                case StateMoveSouth:
                case StateMoveEast:
                case StateMoveWest:
                        move( playerItem );
                        break;

                case StateDisplaceNorth:
                case StateDisplaceSouth:
                case StateDisplaceEast:
                case StateDisplaceWest:
                case StateDisplaceNortheast:
                case StateDisplaceSoutheast:
                case StateDisplaceSouthwest:
                case StateDisplaceNorthwest:
                case StateForceDisplaceNorth:
                case StateForceDisplaceSouth:
                case StateForceDisplaceEast:
                case StateForceDisplaceWest:
                        displace( playerItem );
                        break;

                case StateCancelDisplaceNorth:
                case StateCancelDisplaceSouth:
                case StateCancelDisplaceEast:
                case StateCancelDisplaceWest:
                        cancelDisplace( playerItem );
                        break;

                case StateFall:
                        fall( playerItem );
                        break;

                case StateJump:
                case StateRegularJump:
                case StateHighJump:
                        jump( playerItem );
                        break;

                case StateStartWayOutTeletransport:
                case StateWayOutTeletransport:
                        wayOutTeletransport( playerItem );
                        break;

                case StateStartWayInTeletransport:
                case StateWayInTeletransport:
                        wayInTeletransport( playerItem );
                        break;

                case StateStartDestroy:
                case StateDestroy:
                        destroy( playerItem );
                        break;

                case StateTakeItem:
                case StateTakeAndJump:
                        take( playerItem );
                        break;

                case StateTakenItem:
                        playerItem->addToZ( - LayerHeight );
                        stateId = StateWait;
                        break;

                case StateDropItem:
                case StateDropAndJump:
                        drop( playerItem );
                        break;

                default:
                        ;
        }

        // Reproduce el sonido asociado al estado actual
        SoundManager::getInstance()->play( playerItem->getLabel(), stateId );

        return false;
}

void PlayerHeels::behave ()
{
        PlayerItem* playerItem = dynamic_cast< PlayerItem * >( this->item );
        InputManager* input = InputManager::getInstance();

        // Si no está en modo automático
        if ( stateId != StateAutoMoveNorth && stateId != StateAutoMoveSouth && stateId != StateAutoMoveEast && stateId != StateAutoMoveWest &&
                stateId != StateStartWayOutTeletransport && stateId != StateWayOutTeletransport && stateId != StateStartWayInTeletransport && stateId != StateWayInTeletransport &&
                stateId != StateStartDestroy && stateId != StateDestroy )
        {
                // Si está esperando o parpadeando
                if ( stateId == StateWait || stateId == StateBlink )
                {
                        // ...y ha pulsado la tecla para coger un elemento entonces intenta cogerlo / dejarlo
                        if ( input->take() )
                        {
                                stateId = ( takenItemData == 0 ? StateTakeItem : StateDropItem );
                                // Las repeticiones de esta tecla no deben procesarse
                                input->noRepeat( "take" );
                        }
                        // ...y ha pulsado la tecla para coger un elemento y luego saltar entonces
                        // intenta cogerlo / dejarlo y luego salta
                        else if ( input->takeAndJump() )
                        {
                                stateId = ( takenItemData == 0 ? StateTakeAndJump : StateDropAndJump );
                                // Las repeticiones de esta tecla no deben procesarse
                                input->noRepeat( "take-jump" );
                        }
                        // ...y se ha pulsado alguna tecla de movimiento entonces se mueve
                        else if ( input->movenorth() && !input->movesouth() && !input->moveeast() && !input->movewest() )
                        {
                                stateId = StateMoveNorth;
                        }
                        else if ( !input->movenorth() && input->movesouth() && !input->moveeast() && !input->movewest() )
                        {
                                stateId = StateMoveSouth;
                        }
                        else if ( !input->movenorth() && !input->movesouth() && input->moveeast() && !input->movewest() )
                        {
                                stateId = StateMoveEast;
                        }
                        else if ( !input->movenorth() && !input->movesouth() && !input->moveeast() && input->movewest() )
                        {
                                stateId = StateMoveWest;
                        }
                        // ...y se ha pulsado la tecla de salto entonces salta
                        else if ( input->jump() )
                        {
                                // Almacena en la pila de colisiones los elementos que tiene debajo
                                playerItem->checkPosition( 0, 0, -1, Add );
                                // Si está sobre un telepuerto y salta entonces el jugador será teletransportado, sino saltará
                                stateId = ( playerItem->getMediator()->collisionWithByBehavior( TeleportBehavior ) ? StateStartWayOutTeletransport : StateJump );
                        }
                }
                // Si está moviéndose
                else if ( stateId == StateMoveNorth || stateId == StateMoveSouth || stateId == StateMoveEast || stateId == StateMoveWest )
                {
                        // ...y se ha pulsado la tecla de salto entonces salta
                        if( input->jump() )
                        {
                                // Almacena en la pila de colisiones los elementos que tiene debajo
                                playerItem->checkPosition( 0, 0, -1, Add );
                                // Si está sobre un telepuerto y salta entonces el jugador será teletransportado, sino saltará
                                stateId = ( playerItem->getMediator()->collisionWithByBehavior( TeleportBehavior ) ? StateStartWayOutTeletransport : StateJump );
                        }
                        // ...y ha pulsado la tecla para coger un elemento entonces intenta cogerlo / dejarlo
                        else if ( input->take() )
                        {
                                stateId = ( takenItemData == 0 ? StateTakeItem : StateDropItem );
                                // Las repeticiones de esta tecla no deben procesarse
                                input->noRepeat( "take" );
                        }
                        // ...y ha pulsado la tecla para coger un elemento y luego saltar entonces
                        // intenta cogerlo / dejarlo y luego salta
                        else if ( input->takeAndJump() )
                        {
                                stateId = ( takenItemData == 0 ? StateTakeAndJump : StateDropAndJump );
                                // Las repeticiones de esta tecla no deben procesarse
                                input->noRepeat( "take-jump" );
                        }
                        // ...y se ha pulsado alguna tecla de movimiento entonces sigue moviéndose
                        else if ( input->movenorth() && !input->movesouth() && !input->moveeast() && !input->movewest() )
                        {
                                stateId = StateMoveNorth;
                        }
                        else if ( !input->movenorth() && input->movesouth() && !input->moveeast() && !input->movewest() )
                        {
                                stateId = StateMoveSouth;
                        }
                        else if ( !input->movenorth() && !input->movesouth() && input->moveeast() && !input->movewest() )
                        {
                                stateId = StateMoveEast;
                        }
                        else if ( !input->movenorth() && !input->movesouth() && !input->moveeast() && input->movewest() )
                        {
                                stateId = StateMoveWest;
                        }
                        // Si por el contrario se han soltado las teclas de movimiento entonces se pone en espera
                        else if ( !input->movenorth() && !input->movesouth() && !input->moveeast() && !input->movewest() )
                        {
                                SoundManager::getInstance()->stop( playerItem->getLabel(), stateId );
                                stateId = StateWait;
                        }
                }
                // Si está siendo desplazado
                else if ( stateId == StateDisplaceNorth || stateId == StateDisplaceSouth || stateId == StateDisplaceEast || stateId == StateDisplaceWest )
                {
                        // ...y se ha pulsado la tecla de salto entonces salta
                        if ( input->jump() )
                        {
                                stateId = StateJump;
                        }
                        // ...y ha pulsado la tecla para coger un elemento entonces intenta cogerlo
                        else if ( input->take() )
                        {
                                stateId = ( takenItemData == 0 ? StateTakeItem : StateDropItem );
                                // Las repeticiones de esta tecla no deben procesarse
                                input->noRepeat( "take" );
                        }
                        // ...y ha pulsado la tecla para coger un elemento y luego saltar entonces
                        // intenta cogerlo / dejarlo y luego salta
                        else if ( input->takeAndJump() )
                        {
                                stateId = ( takenItemData == 0 ? StateTakeAndJump : StateDropAndJump );
                                // Las repeticiones de esta tecla no deben procesarse
                                input->noRepeat( "take-jump" );
                        }
                        // ...y se ha pulsado alguna tecla de movimiento entonces sigue moviéndose
                        else if ( input->movenorth() && !input->movesouth() && !input->moveeast() && !input->movewest() )
                        {
                                stateId = StateMoveNorth;
                        }
                        else if ( !input->movenorth() && input->movesouth() && !input->moveeast() && !input->movewest() )
                        {
                                stateId = StateMoveSouth;
                        }
                        else if ( !input->movenorth() && !input->movesouth() && input->moveeast() && !input->movewest() )
                        {
                                stateId = StateMoveEast;
                        }
                        else if ( !input->movenorth() && !input->movesouth() && !input->moveeast() && input->movewest() )
                        {
                                stateId = StateMoveWest;
                        }
                }
                // Si está siendo desplazado forzosamente
                else if ( stateId == StateForceDisplaceNorth || stateId == StateForceDisplaceSouth || stateId == StateForceDisplaceEast || stateId == StateForceDisplaceWest )
                {
                        // ...y se ha pulsado la tecla de salto entonces salta
                        if ( input->jump() )
                        {
                                stateId = StateJump;
                        }
                        // ...y se ha pulsado alguna tecla de movimiento entonces: si pretende avanzar en la dirección
                        // contraria a la que se está deplazando entonces se anula el desplazamiento; en caso contrario,
                        // avanza en la dirección que se esté ordenando
                        else if ( input->movenorth() && !input->movesouth() && !input->moveeast() && !input->movewest() )
                        {
                                stateId = ( stateId == StateForceDisplaceSouth ? StateCancelDisplaceSouth : StateMoveNorth );
                        }
                        else if ( !input->movenorth() && input->movesouth() && !input->moveeast() && !input->movewest() )
                        {
                                stateId = ( stateId == StateForceDisplaceNorth ? StateCancelDisplaceNorth : StateMoveSouth );
                        }
                        else if ( !input->movenorth() && !input->movesouth() && input->moveeast() && !input->movewest() )
                        {
                                stateId = ( stateId == StateForceDisplaceWest ? StateCancelDisplaceWest : StateMoveEast );
                        }
                        else if ( !input->movenorth() && !input->movesouth() && !input->moveeast() && input->movewest() )
                        {
                                stateId = ( stateId == StateForceDisplaceEast ? StateCancelDisplaceEast : StateMoveWest );
                        }
                }
                // Si está saltando
                else if ( stateId == StateJump || stateId == StateRegularJump || stateId == StateHighJump )
                {

                }
                // Si está cayendo
                else if ( stateId == StateFall )
                {
                        // ...y ha pulsado la tecla para coger un elemento entonces intenta cogerlo / dejarlo
                        if ( input->take() )
                        {
                                stateId = ( takenItemData == 0 ? StateTakeItem : StateDropItem );
                                // Las repeticiones de esta tecla no deben procesarse
                                input->noRepeat( "take" );
                        }
                }
        }
}

}
