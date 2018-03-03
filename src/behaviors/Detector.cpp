
#include "Detector.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"


namespace isomot
{

Detector::Detector( Item * item, const std::string & behavior ) :
        Behavior( item, behavior )
{
        speedTimer = new Timer();
        speedTimer->go();

        fallTimer = new Timer();
        fallTimer->go();
}

Detector::~Detector( )
{
        delete speedTimer;
        delete fallTimer;
}

bool Detector::update ()
{
        FreeItem* freeItem = dynamic_cast< FreeItem * >( this->item );
        PlayerItem* playerItem = freeItem->getMediator()->getActivePlayer();
        bool isGone = false;

        if ( playerItem != nilPointer )
        {
                switch ( activity )
                {
                        // Si está parado intenta detectar a un jugador
                        case Wait:
                                // Un jugador se cruza en el eje X del "perseguidor"
                                if ( freeItem->getX() >= playerItem->getX() - 1 && freeItem->getX() <= playerItem->getX() + 1 )
                                {
                                        // ...a la derecha de él
                                        if ( playerItem->getY() <= freeItem->getY() )
                                        {
                                                changeActivityOfItem( MoveEast );
                                        }
                                        // ...a la izquierda de él
                                        else if ( playerItem->getY() >= freeItem->getY() )
                                        {
                                                changeActivityOfItem( MoveWest );
                                        }
                                }
                                // Un jugador se cruza en el eje Y del "perseguidor"
                                else if ( freeItem->getY() >= playerItem->getY()-1 && freeItem->getY() <= playerItem->getY() + 1 )
                                {
                                        // ...a la derecha de él
                                        if ( playerItem->getX() <= freeItem->getX() )
                                        {
                                                changeActivityOfItem( MoveNorth );
                                        }
                                        else
                                        // ...a la izquierda de él
                                        if ( playerItem->getX() >= freeItem->getX() )
                                        {
                                                changeActivityOfItem( MoveSouth );
                                        }
                                }

                                // Emite el sonido de activación del movimiento
                                if ( activity != Wait )
                                {
                                        SoundManager::getInstance()->play( freeItem->getLabel(), activity );
                                }
                                break;

                        case MoveNorth:
                        case MoveSouth:
                        case MoveEast:
                        case MoveWest:
                                // item is active and it is time to move
                                if ( ! freeItem->isFrozen() )
                                {
                                        if ( speedTimer->getValue() > freeItem->getSpeed() )
                                        {
                                                // El elemento se mueve. Si colisiona vuelve al estado inicial para tomar una nueva dirección
                                                if ( ! whatToDo->move( this, &activity, true ) )
                                                {
                                                        activity = Wait;
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
                        case DisplaceNorthwest:
                        case DisplaceSoutheast:
                        case DisplaceSouthwest:
                                // Si el elemento está activo y ha llegado el momento de moverse, entonces:
                                if ( speedTimer->getValue() > freeItem->getSpeed() )
                                {
                                        // El elemento se mueve hasta detectar un colisión
                                        if ( ! whatToDo->displace( this, &activity, true ) )
                                        {
                                                activity = Wait;
                                        }

                                        // Se pone a cero el cronómetro para el siguiente ciclo
                                        speedTimer->reset();
                                }

                                // preserve inactivity for inactive item
                                if ( freeItem->isFrozen() )
                                {
                                        activity = Freeze;
                                }
                                break;

                        case Fall:
                                // Se comprueba si ha topado con el suelo en una sala sin suelo
                                if ( freeItem->getZ() == 0 && freeItem->getMediator()->getRoom()->getKindOfFloor() == "none" )
                                {
                                        isGone = true;
                                }
                                // Si ha llegado el momento de caer entonces el elemento desciende una unidad
                                else if ( fallTimer->getValue() > freeItem->getWeight() )
                                {
                                        if ( ! whatToDo->fall( this ) )
                                        {
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
        }

        return isGone;
}

}
