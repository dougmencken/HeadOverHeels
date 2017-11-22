
#include "OneWay.hpp"
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

OneWay::OneWay( Item * item, const std::string & behavior, bool flying ) :
        Behavior( item, behavior )
        , isFlying( flying )
        , speedTimer( 0 )
        , fallTimer( 0 )
{
        speedTimer = new HPC();
        speedTimer->start();

        if ( ! flying )
        {
                fallTimer = new HPC();
                fallTimer->start();
        }
}

OneWay::~OneWay()
{
        delete speedTimer ;
        delete fallTimer ;
}

bool OneWay::update ()
{
        bool vanish = false;
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

                                        speedTimer->reset();
                                }

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
                                if ( freeItem->getZ() == 0 && freeItem->getMediator()->getRoom()->getKindOfFloor() == "none" )
                                {
                                        // item disappears
                                        vanish = true;
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

        return vanish;
}

void OneWay::start()
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

void OneWay::turnRound()
{
        switch ( this->item->getOrientation().getIntegerOfWay () )
        {
                case North:
                        activity = MoveSouth;
                        this->item->changeOrientation( South );
                        break;

                case South:
                        activity = MoveNorth;
                        this->item->changeOrientation( North );
                        break;

                case East:
                        activity = MoveWest;
                        this->item->changeOrientation( West );
                        break;

                case West:
                        activity = MoveEast;
                        this->item->changeOrientation( East );
                        break;

                default:
                        ;
        }
}

}
