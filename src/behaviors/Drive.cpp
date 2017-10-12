
#include "Drive.hpp"
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

Drive::Drive( Item* item, const std::string& behavior )
        : Behavior( item, behavior )
{
        running = false;
        speedTimer = new HPC();
        fallTimer = new HPC();
        speedTimer->start();
        fallTimer->start();
}

Drive::~Drive( )
{
        delete speedTimer;
        delete fallTimer;
}

bool Drive::update ()
{
        FreeItem* freeItem = dynamic_cast< FreeItem * >( this->item );
        Mediator* mediator = freeItem->getMediator();
        bool isGone = false;
        bool playerFound = false;

        switch ( activity )
        {
                case Wait:
                        if ( running )
                        {
                                switch ( freeItem->getOrientation().getIntegerOfWay() )
                                {
                                        case North:
                                                changeActivityOfItem( MoveNorth );
                                                break;

                                        case South:
                                                changeActivityOfItem( MoveSouth );
                                                break;

                                        case East:
                                                changeActivityOfItem( MoveEast );
                                                break;

                                        case West:
                                                changeActivityOfItem( MoveWest );
                                                break;

                                        default:
                                                ;
                                }
                        }
                        // Si está parado comprueba si hay un jugador encima y toma su orientación para empezar a avanzar
                        else
                        {
                                if ( ! freeItem->checkPosition( 0, 0, 1, Add ) )
                                {
                                        while ( ! mediator->isStackOfCollisionsEmpty() && ! playerFound )
                                        {
                                                Item * item = mediator->findCollisionPop ();

                                                if ( dynamic_cast< PlayerItem * >( item ) )
                                                {
                                                        playerFound = true;
                                                        running = true;

                                                        switch ( item->getOrientation().getIntegerOfWay () )
                                                        {
                                                                case North:
                                                                        changeActivityOfItem( MoveNorth );
                                                                        break;

                                                                case South:
                                                                        changeActivityOfItem( MoveSouth );
                                                                        break;

                                                                case East:
                                                                        changeActivityOfItem( MoveEast );
                                                                        break;

                                                                case West:
                                                                        changeActivityOfItem( MoveWest );
                                                                        break;

                                                                default:
                                                                        ;
                                                        }
                                                }
                                        }
                                }
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
                                                running = false;
                                                activity = Wait;

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

                        // inactive item will continue to be inactive
                        if ( freeItem->isFrozen() )
                        {
                                activity = Freeze;
                        }
                        break;

                case Fall:
                        // Se comprueba si ha topado con el suelo en una sala sin suelo
                        if ( freeItem->getZ() == 0 && freeItem->getMediator()->getRoom()->getKindOfFloor() == "none" )
                        {
                                // item disappears
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

        return isGone;
}

}
