
#include "Patrol.hpp"
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

Patrol::Patrol( Item * item, const std::string & behavior ) :
        Behavior( item, behavior )
{
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
        bool alive = true;

        switch ( activity )
        {
                case Wait:
                        changeDirection();
                        break;

                case MoveNorth:
                case MoveSouth:
                case MoveEast:
                case MoveWest:
                case MoveNortheast:
                case MoveNorthwest:
                case MoveSoutheast:
                case MoveSouthwest:
                        if ( ! freeItem->isFrozen() )
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
                                        if ( ! whatToDo->move( this, &activity, true ) )
                                        {
                                                // Fuerza el cambio de dirección
                                                changeDirection();

                                                // Emite el sonido de colisión
                                                SoundManager::getInstance()->play( freeItem->getLabel(), Collision );
                                        }

                                        // Emite el sonido de movimiento
                                        SoundManager::getInstance()->play( freeItem->getLabel(), activity );

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
                        // Emite el sonido de de desplazamiento
                        SoundManager::getInstance()->play( freeItem->getLabel(), activity );

                        // El elemento es deplazado por otro. Si el desplazamiento no se pudo realizar por
                        // colisión entonces el estado se propaga a los elementos con los que ha chocado
                        whatToDo->displace( this, &activity, true );

                        // Una vez se ha completado el desplazamiento el elemento vuelve a su comportamiento normal
                        activity = Wait;

                        // preserve inactivity for frozen item
                        if ( freeItem->isFrozen() )
                        {
                                activity = Freeze;
                        }
                        break;

                case Fall:
                        // Se comprueba si ha topado con el suelo en una sala sin suelo
                        if ( item->getZ() == 0 && item->getMediator()->getRoom()->getFloorType() == NoFloor )
                        {
                                // El elemento desaparece
                                alive = false;
                        }
                        // Si ha llegado el momento de caer entonces el elemento desciende una unidad
                        else if ( fallTimer->getValue() > freeItem->getWeight() )
                        {
                                // Si termina de caer vuelve al estado inicial
                                if ( ! whatToDo->fall( this ) )
                                {
                                        // Emite el sonido de caída
                                        SoundManager::getInstance()->play( freeItem->getLabel(), activity );
                                        activity = Wait;
                                }

                                // Se pone a cero el cronómetro para el siguiente ciclo
                                fallTimer->reset();
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

        return !alive ;
}

void Patrol::changeDirection()
{
        int direction = 0;

        if ( theBehavior == "behavior of random patroling in four primary directions" )
        {
                direction = ( rand() % 4 );
        }
        else if ( theBehavior == "behavior of random patroling in four secondary directions" )
        {
                direction = ( rand() % 4 ) + 4;
        }
        else if ( theBehavior == "behavior of random patroling in eight directions" )
        {
                direction = ( rand() % 8 );
        }

        // change activity depending on value of calculated direction
        switch ( static_cast< Direction >( direction ) )
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

                case Northeast:
                        activity = MoveNortheast;
                        break;

                case Northwest:
                        activity = MoveNorthwest;
                        break;

                case Southeast:
                        activity = MoveSoutheast;
                        break;

                case Southwest:
                        activity = MoveSouthwest;
                        break;

                default:
                        ;
        }

        whatToDo = MoveKindOfActivity::getInstance();
}

}
