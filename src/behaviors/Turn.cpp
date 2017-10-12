
#include "Turn.hpp"
#include "Item.hpp"
#include "ItemData.hpp"
#include "FreeItem.hpp"
#include "MoveKindOfActivity.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"


namespace isomot
{

Turn::Turn( Item * item, const std::string & behavior ) :
        Behavior( item, behavior )
{
        speedTimer = new HPC();
        fallTimer = new HPC();
        speedTimer->start();
        fallTimer->start();
}

Turn::~Turn()
{
        delete speedTimer;
        delete fallTimer;
}

bool Turn::update ()
{
        bool freeze = false;
        FreeItem* freeItem = dynamic_cast< FreeItem * >( this->item );

        switch ( activity )
        {
                case Wait:
                        begin();
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
                                                turn();

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
                        // Emite el sonido de de desplazamiento
                        SoundManager::getInstance()->play( freeItem->getLabel(), activity );

                        // El elemento es deplazado por otro. Si el desplazamiento no se pudo realizar por
                        // colisión entonces el estado se propaga a los elementos con los que ha chocado
                        whatToDo->displace( this, &activity, true );

                        // Una vez se ha completado el desplazamiento el elemento vuelve a su comportamiento normal
                        activity = Wait;

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
                                // El elemento desaparece
                                freeze = true;
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

        return freeze;
}

void Turn::begin()
{
        switch ( this->item->getOrientation().getIntegerOfWay () )
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

void Turn::turn()
{
        switch ( this->item->getOrientation().getIntegerOfWay () )
        {
                case North:
                        activity = ( getNameOfBehavior() == "behavior of move then turn left and move" ? MoveWest : MoveEast );
                        this->item->changeOrientation( getNameOfBehavior() == "behavior of move then turn left and move" ? West : East );
                        break;

                case South:
                        activity = ( getNameOfBehavior() == "behavior of move then turn left and move" ? MoveEast : MoveWest );
                        this->item->changeOrientation( getNameOfBehavior() == "behavior of move then turn left and move" ? East : West );
                        break;

                case East:
                        activity = ( getNameOfBehavior() == "behavior of move then turn left and move" ? MoveNorth : MoveSouth );
                        this->item->changeOrientation( getNameOfBehavior() == "behavior of move then turn left and move" ? North : South );
                        break;

                case West:
                        activity = ( getNameOfBehavior() == "behavior of move then turn left and move" ? MoveSouth : MoveNorth );
                        this->item->changeOrientation( getNameOfBehavior() == "behavior of move then turn left and move" ? South : North );
                        break;

                default:
                        ;
        }
}

}
