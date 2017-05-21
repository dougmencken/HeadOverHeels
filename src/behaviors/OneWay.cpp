
#include "OneWay.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "MoveKindOfActivity.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"


namespace isomot
{

OneWay::OneWay( Item * item, const BehaviorOfItem & id, bool isFlying ) :
        Behavior( item, id )
{
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

        switch ( activity )
        {
                case Wait:
                        start();
                        break;

                case MoveNorth:
                case MoveSouth:
                case MoveEast:
                case MoveWest:
                        if ( ! freeItem->isFrozen() )
                        {
                                if ( speedTimer->getValue() > freeItem->getSpeed() )
                                {
                                        // El elemento se mueve. Si el movimiento no se pudo realizar por colisión entonces
                                        // se desplaza a los elementos con los que pudiera haber chocado y el elemento da media
                                        // vuelta cambiando su estado a otro de movimiento
                                        if ( ! whatToDo->move( this, &activity, true ) )
                                        {
                                                turnRound();

                                                // Emite el sonido de colisión
                                                SoundManager::getInstance()->play( freeItem->getLabel(), Collision );
                                        }

                                        // Se pone a cero el cronómetro para el siguiente ciclo
                                        speedTimer->reset();
                                }

                                // Anima el elemento
                                freeItem->animate();
                        }
                        break;

                case DisplaceNorth:
                case DisplaceSouth:
                case DisplaceEast:
                case DisplaceWest:
                case DisplaceNortheast:
                case DisplaceSoutheast:
                case DisplaceSouthwest:
                case DisplaceNorthwest:
                        if ( ! this->isFlying )
                        {
                                // Emite el sonido de de desplazamiento
                                SoundManager::getInstance()->play( freeItem->getLabel(), activity );

                                // El elemento es deplazado por otro. Si el desplazamiento no se pudo realizar por
                                // colisión entonces el estado se propaga a los elementos con los que ha chocado
                                whatToDo->displace( this, &activity, true );

                                // Una vez se ha completado el desplazamiento el elemento vuelve a su comportamiento normal
                                activity = Wait;

                                // preserve inactivity for frozen item
                                if ( freeItem->isFrozen() )
                                        activity = Freeze;
                        }
                        else
                        {
                                activity = Wait;
                        }
                        break;

                case Fall:
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
                                        if ( ! whatToDo->fall( this ) )
                                        {
                                                // Emite el sonido de caída
                                                SoundManager::getInstance()->play( freeItem->getLabel(), activity );
                                                activity = Wait;
                                        }

                                        // Se pone a cero el cronómetro para el siguiente ciclo
                                        fallTimer->reset();
                                }
                        }
                        else
                        {
                                activity = Wait;
                        }
                        break;

                case Freeze:
                        freeItem->setFrozen( true );
                        break;

                case WakeUp:
                        freeItem->setFrozen( false );
                        activity = Wait;
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
                        activity = MoveNorth;
                        break;

                case South:
                        activity = MoveSouth;
                        break;

                case East:
                        activity = MoveEast;
                        break;

                case West:
                        activity = MoveWest;
                        break;

                default:
                        ;
        }

        whatToDo = MoveKindOfActivity::getInstance();
}

void OneWay::turnRound()
{
        FreeItem * freeItem = dynamic_cast< FreeItem * >( this->item );

        switch ( freeItem->getDirection() )
        {
                case North:
                        activity = MoveSouth;
                        if ( freeItem->getDirectionFrames() > 1 )
                        {
                                freeItem->changeDirection( South );
                        }
                        break;

                case South:
                        activity = MoveNorth;
                        if ( freeItem->getDirectionFrames() > 1 )
                        {
                                freeItem->changeDirection( North );
                        }
                        break;

                case East:
                        activity = MoveWest;
                        if ( freeItem->getDirectionFrames() > 1 )
                        {
                                freeItem->changeDirection( West );
                        }
                        break;

                case West:
                        activity = MoveEast;
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
