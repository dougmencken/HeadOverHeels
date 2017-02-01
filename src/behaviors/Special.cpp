
#include "Special.hpp"
#include "Item.hpp"
#include "ItemData.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "DisplaceState.hpp"
#include "FallState.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "BonusManager.hpp"
#include "SoundManager.hpp"


namespace isomot
{

Special::Special( Item * item, const BehaviorId & id ) :
	Behavior( item, id )
{
        stateId = StateWait;
        destroyTimer = new HPC();
        speedTimer = new HPC();
        fallTimer = new HPC();
        destroyTimer->start();
        speedTimer->start();
        fallTimer->start();
}

Special::~Special( )
{
        delete destroyTimer;
}

bool Special::update ()
{
        bool destroy = false;
        FreeItem* freeItem = 0;
        Mediator* mediator = item->getMediator();

        switch ( stateId )
        {
                case StateWait:
                        // Si tiene un elemento encima entonces el elemento puede desaparecer
                        if ( ! item->checkPosition( 0, 0, 1, Add ) )
                        {
                                bool destroy = true;
                                Item* topItem = mediator->findCollisionPop( );

                                // El elemento situado encima debe ser un jugador y debe poder tomar el elemento
                                if ( dynamic_cast< PlayerItem * >( topItem ) && checkDestruction( topItem ) )
                                {
                                        topItem->checkPosition( 0, 0, -1, Add );

                                        // Si el elemento que desencadena la destrucción está unicamente sobre el especial
                                        // entonces la destrucción es segura. Si está sobre varios elementos hay que averiguar
                                        // (para poder destruirlo) si los elementos que rodean al especial son también especiales
                                        // y/o volátiles porque, de lo contrario, el elemento no se destruirá
                                        if ( mediator->depthOfStackOfCollisions() > 1 )
                                        {
                                                destroy = ! mediator->collisionWithByBehavior( SpecialBehavior ) &&
                                                                ! mediator->collisionWithByBehavior( VolatileWeightBehavior ) &&
                                                                        ! mediator->collisionWithByBehavior( VolatileTouchBehavior );
                                        }

                                        // Cambio de estado si se han cumplido las condiciones
                                        if ( destroy )
                                        {
                                                stateId = StateDestroy;
                                                // Almacena el elemento que ha provocado la destrucción
                                                this->sender = topItem;
                                                // Los elementos especiales si se cogen al posarse encima, da tiempo a saltar;
                                                // es decir, en este caso se comportan como un volátil por peso
                                                destroyTimer->reset();
                                        }
                                }
                        }

                        // Se anima el elemento
                        item->animate();

                        // Se comprueba si el elemento debe caer
                        if ( stateId != StateDestroy )
                        {
                                stateId = StateFall;
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
                case StateDisplaceUp:
                        // Si el elemento es desplazado por un jugador y éste puede tomarlo entonces se destruye
                        if ( dynamic_cast< PlayerItem * >( sender ) && checkDestruction( sender ) )
                        {
                                stateId = StateDestroy;
                        }
                        // El elemento es deplazado por otro. Si el desplazamiento no se pudo realizar por
                        // colisión entonces el estado se propaga a los elementos con los que ha chocado
                        else if ( speedTimer->getValue() > item->getSpeed() )
                        {
                                state = DisplaceState::getInstance();
                                state->displace( this, &stateId, true );

                                // Una vez se ha completado el desplazamiento el elemento vuelve a su comportamiento normal
                                stateId = StateFall;

                                // Se pone a cero el cronómetro para el siguiente ciclo
                                speedTimer->reset();
                        }
                        break;

                case StateForceDisplaceNorth:
                case StateForceDisplaceSouth:
                case StateForceDisplaceEast:
                case StateForceDisplaceWest:
                        // El elemento es arrastrado porque está encima de una cinta transportadora
                        if ( speedTimer->getValue() > item->getSpeed() )
                        {
                                state = DisplaceState::getInstance();
                                state->displace( this, &stateId, true );

                                // Una vez se ha completado el desplazamiento el elemento vuelve a su comportamiento normal
                                stateId = StateFall;

                                // Se pone a cero el cronómetro para el siguiente ciclo
                                speedTimer->reset();
                        }
                        break;

                case StateFall:
                        // Se comprueba si ha topado con el suelo en una sala sin suelo
                        if ( item->getZ() == 0 && item->getMediator()->getRoom()->getFloorType() == NoFloor )
                        {
                                // El elemento desaparece
                                destroy = true;
                        }
                        // Si ha llegado el momento de caer entonces el elemento desciende una unidad
                        else if ( fallTimer->getValue() > item->getWeight() )
                        {
                                state = FallState::getInstance();
                                if ( ! state->fall( this ) )
                                {
                                        stateId = StateWait;
                                }

                                // Se pone a cero el cronómetro para el siguiente ciclo
                                fallTimer->reset();
                        }
                        break;

                case StateDestroy:
                        if ( destroyTimer->getValue() > 0.100 )
                        {
                                destroy = true;

                                // Emite el sonido de destrucción
                                this->soundManager->play( item->getLabel(), stateId );

                                // Los bonus deben desaparecer de las salas una vez se cojan. Al regresar ya no deben estar
                                BonusManager::getInstance()->destroyBonus( item->getMediator()->getRoom()->getIdentifier(), item->getLabel() );

                                // Ejecuta la acción asociada a la toma del elemento especial
                                takeSpecial( static_cast< PlayerItem* >( sender ) );

                                // Crea el elemento "burbujas" en la misma posición que el especial y a su misma altura
                                freeItem = new FreeItem( bubblesData,
                                item->getX(), item->getY(), item->getZ(),
                                NoDirection );

                                freeItem->assignBehavior( VolatileTimeBehavior, 0 );
                                freeItem->setCollisionDetector( false );

                                // Se añade a la sala actual
                                mediator->getRoom()->addItem( freeItem );
                        }
                        break;

                default:
                        ;
        }

        return destroy;
}

bool Special::checkDestruction( Item* sender )
{
        short playerId = sender->getLabel();
        short magicItemId = this->item->getLabel();

        return  ( playerId == Head      &&  ( magicItemId == Donuts ||
                                                magicItemId == ExtraLife ||
                                                magicItemId == HighSpeed ||
                                                magicItemId == Shield ||
                                                magicItemId == Crown ||
                                                magicItemId == Horn ||
                                                magicItemId == ReincarnationFish ) )
                ||

                ( playerId == Heels     &&  ( magicItemId == ExtraLife ||
                                                magicItemId == HighJump ||
                                                magicItemId == Shield ||
                                                magicItemId == Crown ||
                                                magicItemId == Handbag ||
                                                magicItemId == ReincarnationFish ) )
                ||

                ( playerId == HeadAndHeels  &&  ( magicItemId == Donuts ||
                                                magicItemId == ExtraLife ||
                                                magicItemId == Shield ||
                                                magicItemId == Crown ||
                                                magicItemId == Horn ||
                                                magicItemId == Handbag ||
                                                magicItemId == ReincarnationFish ) ) ;
}

void Special::takeSpecial( PlayerItem* who )
{
        switch( this->item->getLabel() )
        {
                case Donuts:
                {
                        const unsigned short DonutsPerBox = 6 ;
                        who->addAmmo( DonutsPerBox );
                }
                        break;

                case ExtraLife:
                        who->addLives( 2 );
                        break;

                case HighSpeed:
                        who->activateHighSpeed();
                        break;

                case HighJump:
                        who->addHighJumps( 10 );
                        break;

                case Shield:
                        who->activateShield();
                        break;

                case Crown:
                        who->liberatePlanet();
                        break;

                case Horn:
                case Handbag:
                        who->takeTool( this->item->getLabel () );
                        break;

                case ReincarnationFish:
                        who->saveAt( this->item->getX (), this->item->getY (), this->item->getZ () );
                        break;
        }
}

void Special::setMoreData( void* data )
{
        this->bubblesData = reinterpret_cast< ItemData* >( data );
}

}
