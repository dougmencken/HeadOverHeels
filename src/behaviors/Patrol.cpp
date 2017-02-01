
#include "Patrol.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "MoveState.hpp"
#include "DisplaceState.hpp"
#include "FallState.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"


namespace isomot
{

Patrol::Patrol( Item * item, const BehaviorId & id ) :
        Behavior( item, id )
{
        stateId = StateWait;
        speedTimer = new HPC();
        fallTimer = new HPC();
        changeTimer = new HPC();
        speedTimer->start();
        fallTimer->start();
        changeTimer->start();
}

Patrol::~Patrol()
{
        delete speedTimer;
        delete fallTimer;
        delete changeTimer;
}

bool Patrol::update ()
{
        FreeItem * freeItem = dynamic_cast< FreeItem * >( this->item );
        bool destroy = false;

        switch ( stateId )
        {
                case StateWait:
                        changeDirection();
                        break;

                case StateMoveNorth:
                case StateMoveSouth:
                case StateMoveEast:
                case StateMoveWest:
                case StateMoveNortheast:
                case StateMoveNorthwest:
                case StateMoveSoutheast:
                case StateMoveSouthwest:
                        // Si el elemento está activo y ha llegado el momento de moverse, entonces:
                        if ( ! freeItem->isDead() )
                        {
                                if ( speedTimer->getValue() > freeItem->getSpeed())
                                {
                                        // ¿Cambio de dirección?
                                        if ( changeTimer->getValue() > ( double( rand() % 1000 ) + 400.0 ) / 1000.0 )
                                        {
                                                changeDirection();
                                                changeTimer->reset();
                                        }

                                        // El elemento se mueve. Si el movimiento no se pudo realizar por colisión entonces
                                        // se desplaza a los elementos con los que pudiera haber chocado y el elemento da media
                                        // vuelta cambiando su estado a otro de movimiento
                                        if ( ! state->move( this, &stateId, true ) )
                                        {
                                                // Fuerza el cambio de dirección
                                                changeDirection();

                                                // Emite el sonido de colisión
                                                this->soundManager->play( freeItem->getLabel(), StateCollision );
                                        }

                                        // Emite el sonido de movimiento
                                        this->soundManager->play( freeItem->getLabel(), stateId );

                                        // Se pone a cero el cronómetro para el siguiente ciclo
                                        speedTimer->reset();
                                }

                                // Anima el elemento
                                freeItem->animate();
                        }
                        break;

                case StateDisplaceNorth:
                case StateDisplaceSouth:
                case StateDisplaceEast:
                case StateDisplaceWest:
                case StateDisplaceNortheast:
                case StateDisplaceSoutheast:
                case StateDisplaceSouthwest:
                case StateDisplaceNorthwest:
                        // Emite el sonido de de desplazamiento
                        this->soundManager->play( freeItem->getLabel(), stateId );

                        // El elemento es deplazado por otro. Si el desplazamiento no se pudo realizar por
                        // colisión entonces el estado se propaga a los elementos con los que ha chocado
                        state->displace( this, &stateId, true );

                        // Una vez se ha completado el desplazamiento el elemento vuelve a su comportamiento normal
                        stateId = StateWait;

                        // Si el elemento estaba inactivo, después del desplazamiento seguirá estando
                        if ( freeItem->isDead() )
                        {
                                stateId = StateFreeze;
                        }
                        break;

                case StateFall:
                        // Se comprueba si ha topado con el suelo en una sala sin suelo
                        if ( item->getZ() == 0 && item->getMediator()->getRoom()->getFloorType() == NoFloor )
                        {
                                // El elemento desaparece
                                destroy = true;
                        }
                        // Si ha llegado el momento de caer entonces el elemento desciende una unidad
                        else if ( fallTimer->getValue() > freeItem->getWeight() )
                        {
                                // Si termina de caer vuelve al estado inicial
                                if ( ! state->fall( this ) )
                                {
                                        // Emite el sonido de caída
                                        this->soundManager->play( freeItem->getLabel(), stateId );
                                        stateId = StateWait;
                                }

                                // Se pone a cero el cronómetro para el siguiente ciclo
                                fallTimer->reset();
                        }
                        break;

                case StateFreeze:
                        freeItem->setDead(true);
                        break;

                case StateWakeUp:
                        freeItem->setDead(false);
                        stateId = StateWait;
                        break;

                default:
                        ;
        }

        return destroy;
}

void Patrol::changeDirection()
{
        int direction = 0;

        // En función del tipo las orientaciones a barajar son distintas
        switch ( id )
        {
                case Patrol4cBehavior:
                        direction = ( rand() % 4 );
                        break;

                case Patrol4dBehavior:
                        direction = ( rand() % 4 ) + 4;
                        break;

                case Patrol8Behavior:
                        direction = ( rand() % 8 );
                        break;

                default:
                        ;
        }

        // Asigna el estado según el valor de la dirección aleatoria calculada
        switch ( static_cast< Direction >( direction ) )
        {
                case North:
                stateId = StateMoveNorth;
                break;

                case South:
                stateId = StateMoveSouth;
                break;

                case East:
                        stateId = StateMoveEast;
                        break;

                case West:
                        stateId = StateMoveWest;
                        break;

                case Northeast:
                        stateId = StateMoveNortheast;
                        break;

                case Northwest:
                        stateId = StateMoveNorthwest;
                        break;

                case Southeast:
                        stateId = StateMoveSoutheast;
                        break;

                case Southwest:
                        stateId = StateMoveSouthwest;
                        break;

                default:
                        ;
        }

        state = MoveState::getInstance();
}

}
