
#include "Special.hpp"
#include "Item.hpp"
#include "ItemData.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "BonusManager.hpp"
#include "SoundManager.hpp"


namespace isomot
{

Special::Special( Item * item, const BehaviorOfItem & id ) :
        Behavior( item, id )
{
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

        switch ( activity )
        {
                case Wait:
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
                                                activity = Destroy;
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
                        if ( activity != Destroy )
                        {
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
                case DisplaceUp:
                        // Si el elemento es desplazado por un jugador y éste puede tomarlo entonces se destruye
                        if ( dynamic_cast< PlayerItem * >( sender ) && checkDestruction( sender ) )
                        {
                                activity = Destroy;
                        }
                        // El elemento es deplazado por otro. Si el desplazamiento no se pudo realizar por
                        // colisión entonces el estado se propaga a los elementos con los que ha chocado
                        else if ( speedTimer->getValue() > item->getSpeed() )
                        {
                                whatToDo = DisplaceKindOfActivity::getInstance();
                                whatToDo->displace( this, &activity, true );

                                // Una vez se ha completado el desplazamiento el elemento vuelve a su comportamiento normal
                                activity = Fall;

                                // Se pone a cero el cronómetro para el siguiente ciclo
                                speedTimer->reset();
                        }
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
                        if ( item->getZ() == 0 && item->getMediator()->getRoom()->getFloorType() == NoFloor )
                        {
                                // El elemento desaparece
                                destroy = true;
                        }
                        // Si ha llegado el momento de caer entonces el elemento desciende una unidad
                        else if ( fallTimer->getValue() > item->getWeight() )
                        {
                                whatToDo = FallKindOfActivity::getInstance();
                                if ( ! whatToDo->fall( this ) )
                                {
                                        activity = Wait;
                                }

                                // Se pone a cero el cronómetro para el siguiente ciclo
                                fallTimer->reset();
                        }
                        break;

                case Destroy:
                        if ( destroyTimer->getValue() > 0.100 )
                        {
                                destroy = true;

                                // Emite el sonido de destrucción
                                SoundManager::getInstance()->play( item->getLabel(), activity );

                                // Los bonus deben desaparecer de las salas una vez se cojan. Al regresar ya no deben estar
                                BonusManager::getInstance()->markBonusAsAbsent( item->getMediator()->getRoom()->getIdentifier(), item->getLabel() );

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
                                                magicItemId == HighSpeedItem ||
                                                magicItemId == Shield ||
                                                magicItemId == Crown ||
                                                magicItemId == Horn ||
                                                magicItemId == ReincarnationFish ) )
                ||

                ( playerId == Heels     &&  ( magicItemId == ExtraLife ||
                                                magicItemId == HighJumpItem ||
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
        switch ( this->item->getLabel() )
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

                case HighSpeedItem:
                        who->activateHighSpeed();
                        break;

                case HighJumpItem:
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
