
#include "Trampoline.hpp"
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

Trampoline::Trampoline( Item * item, const std::string & behavior ) :
        Behavior( item, behavior )
{
        folded = rebounding = false;
        regularFrame = 0;
        foldedFrame = 1;
        speedTimer = new HPC();
        fallTimer = new HPC();
        reboundTimer = new HPC();
        speedTimer->start();
        fallTimer->start();
        reboundTimer->start();
}

Trampoline::~Trampoline()
{
        delete speedTimer;
        delete fallTimer;
        delete reboundTimer;
}

bool Trampoline::update ()
{
        FreeItem * freeItem = dynamic_cast< FreeItem * >( this->item );
        bool destroy = false;

        switch ( activity )
        {
                case Wait:
                        // Si hay elementos encima el trampolín se pliega
                        if ( ! freeItem->checkPosition( 0, 0, 1, Add ) )
                        {
                                folded = true;
                                rebounding = false;
                                freeItem->changeFrame( foldedFrame );
                        }
                        else
                        {
                                // Si el trampolín está rebotando se anima hasta que finalice el tiempo
                                if ( rebounding && reboundTimer->getValue() < 0.600 )
                                {
                                        freeItem->animate();

                                        // Emite el sonido de rebote
                                        if ( reboundTimer->getValue() > 0.100 )
                                        {
                                                SoundManager::getInstance()->play( freeItem->getLabel(), IsActive );
                                        }
                                }
                                else
                                {
                                        // Si no hay elementos encima pero los había entonces el trampolín rebota
                                        if ( folded )
                                        {
                                                rebounding = true;
                                                reboundTimer->reset();
                                        }

                                        // Ya no está pleglado
                                        folded = false;

                                        freeItem->changeFrame(regularFrame);
                                }
                        }

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
                case DisplaceNorthwest:
                case DisplaceSoutheast:
                case DisplaceSouthwest:
                        // Si el elemento está activo y ha llegado el momento de moverse, entonces:
                        if ( speedTimer->getValue() > freeItem->getSpeed() )
                        {
                                // Emite el sonido de de desplazamiento
                                SoundManager::getInstance()->play( freeItem->getLabel(), activity );

                                // Actualiza el estado
                                this->changeActivityOfItem( activity );
                                whatToDo->displace( this, &activity, true );

                                // Si no está cayendo entonces vuelve al estado inicial
                                if ( activity != Fall )
                                {
                                        activity = Wait;
                                }

                                // Se pone a cero el cronómetro para el siguiente ciclo
                                speedTimer->reset();
                        }
                        break;

                case Fall:
                        // Se comprueba si ha topado con el suelo en una sala sin suelo
                        if ( item->getZ() == 0 && item->getMediator()->getRoom()->getFloorType() == NoFloor )
                        {
                                // El elemento desaparece
                                destroy = true;
                        }
                        // Si ha llegado el momento de caer entonces el elemento desciende una unidad
                        else if ( fallTimer->getValue() > freeItem->getWeight() )
                        {
                                // El elemento cae
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
                        destroy = true;
                        break;

                default:
                        ;
        }

        return destroy;
}

}
