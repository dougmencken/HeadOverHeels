
#include "Mobile.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "SoundManager.hpp"


namespace isomot
{

Mobile::Mobile( Item * item, const std::string & behavior ) :
        Behavior( item, behavior )
{
        speedTimer = new HPC();
        fallTimer = new HPC();
        speedTimer->start();
        fallTimer->start();
}

Mobile::~Mobile()
{
        delete speedTimer;
        delete fallTimer;
}

bool Mobile::update ()
{
        FreeItem* freeItem = dynamic_cast< FreeItem * >( this->item );
        bool freeze = false;

        switch ( activity )
        {
                case Wait:
                        // Se comprueba si el elemento debe empezar a caer
                        if ( FallKindOfActivity::getInstance()->fall( this ) )
                        {
                                fallTimer->reset();
                                activity = Fall;
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
                        // El elemento es deplazado por otro. Si el desplazamiento no se pudo realizar por
                        // colisión entonces el estado se propaga a los elementos con los que ha chocado
                        if ( speedTimer->getValue() > freeItem->getSpeed() )
                        {
                                // Emite el sonido de de desplazamiento si está siendo empujado, no desplazado
                                // por un elemento que haya debajo
                                // TODO Hecho para portátiles y para Carlos ¿aplica a otros elementos?
                                if ( this->sender == 0 || ( this->sender != 0 && this->sender != this->item ) )
                                {
                                        SoundManager::getInstance()->play( freeItem->getLabel(), activity );
                                }

                                // Actualiza el estado
                                this->changeActivityOfItem( activity );
                                whatToDo->displace( this, &activity, true );

                                // Una vez se ha completado el desplazamiento el elemento vuelve a su comportamiento normal
                                activity = Wait;

                                // Se pone a cero el cronómetro para el siguiente ciclo
                                speedTimer->reset();
                        }

                        // Anima el elemento
                        freeItem->animate();
                        break;

                case ForceDisplaceNorth:
                case ForceDisplaceSouth:
                case ForceDisplaceEast:
                case ForceDisplaceWest:
                        // El elemento es arrastrado porque está encima de una cinta transportadora
                        if ( speedTimer->getValue() > item->getSpeed() )
                        {
                                whatToDo = DisplaceKindOfActivity::getInstance();
                                whatToDo->displace( this, &activity, true );

                                // Una vez se ha completado el desplazamiento el elemento vuelve a su comportamiento normal
                                activity = Fall;

                                // Se pone a cero el cronómetro para el siguiente ciclo
                                speedTimer->reset();
                        }
                        break;

                case Fall:
                        // Se comprueba si ha topado con el suelo en una sala sin suelo
                        if ( freeItem->getZ() == 0 && freeItem->getMediator()->getRoom()->getFloorType() == NoFloor )
                        {
                                // El elemento desaparece
                                freeze = true;
                        }
                        // Si ha llegado el momento de caer entonces el elemento desciende una unidad
                        else if ( fallTimer->getValue() > freeItem->getWeight() )
                        {
                                // Actualiza el estado
                                this->changeActivityOfItem( activity );
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

                case Destroy:
                        // Se destruye cuando el elemento se coge
                        freeze = true;
                        break;

                default:
                        ;
        }

        return freeze;
}

}
