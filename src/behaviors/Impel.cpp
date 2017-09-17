
#include "Impel.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Mediator.hpp"
#include "Room.hpp"


namespace isomot
{

Impel::Impel( Item * item, const std::string & behavior ) :
        Behavior( item, behavior )
{
        activity = Wait;
        speedTimer = new HPC();
        fallTimer = new HPC();
        speedTimer->start();
        fallTimer->start();
}

Impel::~Impel()
{
        delete speedTimer;
        delete fallTimer;
}

bool Impel::update ()
{
        FreeItem * freeItem = dynamic_cast< FreeItem * >( this->item );
        bool destroy = false;

        switch ( activity )
        {
                case Wait:
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

                        // Anima el elemento
                        freeItem->animate();
                        break;

                case Fall:
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
                                        activity = Wait;
                                }

                                // Se pone a cero el cronómetro para el siguiente ciclo
                                fallTimer->reset();
                        }
                        break;

                default:
                        ;
        }

        return destroy;
}

}
