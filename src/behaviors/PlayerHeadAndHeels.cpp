
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

PlayerHeadAndHeels::PlayerHeadAndHeels( Item * item, const BehaviorOfItem & id ) : UserControlled( item, id )
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

        // Elemento de transición entre telepuertos
        transitionDataLabel = 82;
        // Elemento empleado como disparo
        labelOfFireFromHooter = 38;

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
        PlayerItem* playerItem = dynamic_cast< PlayerItem * >( this->item );

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

                case Glide:
                        glide( playerItem );
                        break;

                case Blink:
                        blink( playerItem );
                        break;

                case TakeItem:
                case TakeAndJump:
                        take( playerItem );
                        break;

                case TakenItem:
                        playerItem->addToZ( -LayerHeight );
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

void PlayerHeadAndHeels::behave ()
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
                        // ...y se ha pulsado la tecla de salto entonces salta
                        if ( input->jump() )
                        {
                                // Almacena en la pila de colisiones los elementos que tiene debajo
                                playerItem->checkPosition( 0, 0, -1, Add );
                                // Si está sobre un telepuerto y salta entonces el jugador será teletransportado, sino saltará
                                activity = ( playerItem->getMediator()->collisionWithByBehavior( TeleportBehavior ) ? StartWayOutTeletransport : Jump );
                        }
                        // ...y ha pulsado la tecla de disparo entonces dispara
                        else if ( input->shoot() && ! fireFromHooterIsPresent )
                        {
                                useHooter( dynamic_cast< PlayerItem * >( this->item ) );
                                // Las repeticiones de esta tecla no deben procesarse
                                input->noRepeat( "shoot" );
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
                        // ...y se ha pulsado alguna tecla de movimiento entonces se mueve
                        else if ( input->movenorth() && !input->movesouth() && !input->moveeast() && !input->movewest() )
                        {
                                activity = MoveNorth;
                        }
                        else if ( ! input->movenorth() && input->movesouth() && !input->moveeast() && !input->movewest() )
                        {
                                activity = MoveSouth;
                        }
                        else if ( ! input->movenorth() && !input->movesouth() && input->moveeast() && !input->movewest() )
                        {
                                activity = MoveEast;
                        }
                        else if ( ! input->movenorth() && !input->movesouth() && !input->moveeast() && input->movewest() )
                        {
                                activity = MoveWest;
                        }
                }
                // Si está moviéndose
                else if ( activity == MoveNorth || activity == MoveSouth || activity == MoveEast || activity == MoveWest )
                {
                        // ...y se ha pulsado la tecla de salto entonces salta
                        if ( input->jump() )
                        {
                                // Almacena en la pila de colisiones los elementos que tiene debajo
                                playerItem->checkPosition( 0, 0, -1, Add );
                                // Si está sobre un telepuerto y salta entonces el jugador será teletransportado, sino saltará
                                activity = ( playerItem->getMediator()->collisionWithByBehavior( TeleportBehavior ) ? StartWayOutTeletransport : Jump );
                        }
                        // ...y ha pulsado la tecla de disparo entonces dispara
                        else if ( input->shoot() && ! fireFromHooterIsPresent )
                        {
                                useHooter( dynamic_cast< PlayerItem * >( this->item ) );
                                // Las repeticiones de esta tecla no deben procesarse
                                input->noRepeat( "shoot" );
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
                        // Si por el contrario se han soltado las teclas de movimiento entonces se pone en espera
                        else if ( ! input->movenorth() && ! input->movesouth() && ! input->moveeast() && ! input->movewest() )
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
                        // ...y se ha pulsado la tecla de disparo entonces dispara
                        else if ( input->shoot() && ! fireFromHooterIsPresent )
                        {
                                useHooter( dynamic_cast< PlayerItem * >( this->item ) );
                                // Las repeticiones de esta tecla no deben procesarse
                                input->noRepeat( "shoot" );
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
                // Si está saltando
                else if ( activity == Jump || activity == RegularJump || activity == HighJump )
                {
                        // ...y ha pulsado la tecla de disparo entonces dispara
                        if ( input->shoot() && ! fireFromHooterIsPresent )
                        {
                                useHooter( dynamic_cast< PlayerItem * >( this->item ) );
                                // Las repeticiones de esta tecla no deben procesarse
                                input->noRepeat( "shoot" );
                        }
                        // ...y se ha pulsado alguna tecla de movimiento entonces el salto cambia de orientación
                        else if ( input->movenorth() && ! input->movesouth() && ! input->moveeast() && ! input->movewest() && playerItem->getDirection() != North )
                        {
                                playerItem->changeDirection( North );
                        }
                        else if ( ! input->movenorth() && input->movesouth() && ! input->moveeast() && ! input->movewest() && playerItem->getDirection() != South )
                        {
                                playerItem->changeDirection( South );
                        }
                        else if ( ! input->movenorth() && ! input->movesouth() && input->moveeast() && ! input->movewest() && playerItem->getDirection() != East )
                        {
                                playerItem->changeDirection( East );
                        }
                        else if ( ! input->movenorth() && ! input->movesouth() && ! input->moveeast() && input->movewest() && playerItem->getDirection() != West )
                        {
                                playerItem->changeDirection( West );
                        }
                }
                // Si está cayendo
                else if ( activity == Fall )
                {
                        // ...y ha pulsado la tecla de disparo entonces dispara
                        if ( input->shoot() && ! fireFromHooterIsPresent )
                        {
                                useHooter( dynamic_cast< PlayerItem * >( this->item ) );
                                // Las repeticiones de esta tecla no deben procesarse
                                input->noRepeat( "shoot" );
                        }
                        // ...y se intenta mover entonces Head y Heels planean
                        else if ( input->movenorth() || input->movesouth() || input->moveeast() || input->movewest() )
                        {
                                activity = Glide;
                        }
                }

                // Si está planeando se procesa sin necesidad de esperar al siguiente ciclo pues existe la posibilidad
                // de que el estado provenga del estado de caída. En este caso un procesamiento posterior provocará
                // que el personaje no sea capaz de entrar en los huecos existentes entre dos elementos rejilla
                if ( activity == Glide )
                {
                        // ...y ha pulsado la tecla de disparo entonces dispara
                        if ( input->shoot() && ! fireFromHooterIsPresent )
                        {
                                useHooter( dynamic_cast< PlayerItem * >( this->item ) );
                                // Las repeticiones de esta tecla no deben procesarse
                                input->noRepeat( "shoot" );
                        }
                        // ...y se ha pulsado alguna tecla de movimiento entonces cambia la orientación
                        else if ( input->movenorth() && !input->movesouth() && ! input->moveeast() && ! input->movewest() && playerItem->getDirection() != North)
                        {
                                playerItem->changeDirection( North );
                        }
                        else if ( ! input->movenorth() && input->movesouth() && ! input->moveeast() && ! input->movewest() && playerItem->getDirection() != South)
                        {
                                playerItem->changeDirection( South );
                        }
                        else if ( ! input->movenorth() && ! input->movesouth() && input->moveeast() && ! input->movewest() && playerItem->getDirection() != East)
                        {
                                playerItem->changeDirection( East );
                        }
                        else if ( ! input->movenorth() && ! input->movesouth() && ! input->moveeast() && input->movewest() && playerItem->getDirection() != West)
                        {
                                playerItem->changeDirection( West );
                        }
                        // ...y deja de moverse con las teclas de dirección entonces simplemente cae
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

        // Si está detenido entre 4 y 9 segundos entonces parpadea
        if ( blinkingTimer->getValue() >= ( rand() % 4 ) + 5 )
        {
                blinkingTimer->reset();
                activity = Blink;
        }

        // Se comprueba si el jugador debe empezar a caer
        if ( FallKindOfActivity::getInstance()->fall( this ) )
        {
                speedTimer->reset();
                activity = Fall;
        }
}

void PlayerHeadAndHeels::blink( PlayerItem * playerItem )
{
        double timeValue = blinkingTimer->getValue();

        // Si el crono se encuentra entre los tiempos especificados, Head cierra los ojos
        if ( ( timeValue > 0.0 && timeValue < 0.050 ) || ( timeValue > 0.400 && timeValue < 0.450 ) )
        {
                playerItem->changeFrame( blinkFrames[ playerItem->getDirection() ] );
        }
        // Si el crono se encuentra entre los tiempos especificados, Head abre los ojos
        else if ( ( timeValue > 0.250 && timeValue < 0.300 ) || ( timeValue > 0.750 && timeValue < 0.800 ) )
        {
                playerItem->changeDirection( playerItem->getDirection() );
        }
        // Después de 0,8 segundos el parpadeo termina
        else if ( timeValue > 0.800 )
        {
                blinkingTimer->reset();
                activity = Wait;
        }
}

}
