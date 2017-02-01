#include "OneWay.hpp"
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

OneWay::OneWay( Item * item, const BehaviorId & id, bool isFlying ) :
        Behavior( item, id )
{
        stateId = StateWait;

        speedTimer = new HPC();
        speedTimer->start();

        if ( ! isFlying )
        {
                fallTimer = new HPC();
                fallTimer->start();
        }

        this->isFlying = isFlying;
}

OneWay::~OneWay()
{
        delete speedTimer;
        if ( this->isFlying )
        {
                delete fallTimer;
        }
}

bool OneWay::update ()
{
        bool destroy = false;
        FreeItem * freeItem = dynamic_cast< FreeItem * >( this->item );

        switch ( stateId )
        {
                case StateWait:
                        start();
                        break;

                case StateMoveNorth:
                case StateMoveSouth:
                case StateMoveEast:
                case StateMoveWest:
                        // Si el elemento está activo y ha llegado el momento de moverse, entonces:
                        if ( ! freeItem->isDead() )
                        {
                                if ( speedTimer->getValue() > freeItem->getSpeed() )
                                {
                                        // El elemento se mueve. Si el movimiento no se pudo realizar por colisión entonces
                                        // se desplaza a los elementos con los que pudiera haber chocado y el elemento da media
                                        // vuelta cambiando su estado a otro de movimiento
                                        if ( ! state->move( this, &stateId, true ) )
                                        {
                                                turnRound();
                                                // Emite el sonido de colisión
                                                this->soundManager->play( freeItem->getLabel(), StateCollision );
                                        }

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
                        if ( ! this->isFlying )
                        {
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
                        }
                        else
                        {
                                stateId = StateWait;
                        }
                        break;

                case StateFall:
                        if ( ! this->isFlying )
                        {
                                // Se comprueba si ha topado con el suelo en una sala sin suelo
                                if ( freeItem->getZ() == 0 && freeItem->getMediator()->getRoom()->getFloorType() == NoFloor )
                                {
                                        // El elemento desaparece
                                        destroy = true;
                                }
                                // Si ha llegado el momento de caer entonces el elemento desciende una unidad
                                else if ( fallTimer->getValue() > freeItem->getWeight() )
                                {
                                        if ( ! state->fall( this ) )
                                        {
                                                // Emite el sonido de caída
                                                this->soundManager->play( freeItem->getLabel(), stateId );
                                                stateId = StateWait;
                                        }

                                        // Se pone a cero el cronómetro para el siguiente ciclo
                                        fallTimer->reset();
                                }
                        }
                        else
                        {
                                stateId = StateWait;
                        }
                        break;

                case StateFreeze:
                        freeItem->setDead( true );
                        break;

                case StateWakeUp:
                        freeItem->setDead( false );
                        stateId = StateWait;
                        break;

                default:
                        ;
        }

        return destroy;
}

void OneWay::start()
{
        switch ( dynamic_cast< FreeItem * >( this->item )->getDirection() )
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

                default:
                        ;
        }

        state = MoveState::getInstance();
}

void OneWay::turnRound()
{
        FreeItem * freeItem = dynamic_cast< FreeItem * >( this->item );

        switch ( freeItem->getDirection() )
        {
                case North:
                        stateId = StateMoveSouth;
                        if ( freeItem->getDirectionFrames() > 1 )
                        {
                                freeItem->changeDirection( South );
                        }
                        break;

                case South:
                        stateId = StateMoveNorth;
                        if ( freeItem->getDirectionFrames() > 1 )
                        {
                                freeItem->changeDirection( North );
                        }
                        break;

                case East:
                        stateId = StateMoveWest;
                        if ( freeItem->getDirectionFrames() > 1 )
                        {
                                freeItem->changeDirection( West );
                        }
                        break;

                case West:
                        stateId = StateMoveEast;
                        if ( freeItem->getDirectionFrames() > 1 )
                        {
                                freeItem->changeDirection( East );
                        }
                        break;

                default:
                        ;
        }
}

}
