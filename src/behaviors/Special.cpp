
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

Special::Special( Item * item, const std::string & behavior ) :
        Behavior( item, behavior )
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
                                if ( dynamic_cast< PlayerItem * >( topItem ) && mayTake( topItem ) )
                                {
                                        topItem->checkPosition( 0, 0, -1, Add );

                                        // Si el elemento que desencadena la destrucción está unicamente sobre el especial
                                        // entonces la destrucción es segura. Si está sobre varios elementos hay que averiguar
                                        // (para poder destruirlo) si los elementos que rodean al especial son también especiales
                                        // y/o volátiles porque, de lo contrario, el elemento no se destruirá
                                        if ( mediator->depthOfStackOfCollisions() > 1 )
                                        {
                                                destroy = ! mediator->collisionWithByBehavior( "behavior of something special" ) &&
                                                                ! mediator->collisionWithByBehavior( "behavior of disappearance on jump into" ) &&
                                                                        ! mediator->collisionWithByBehavior( "behavior of disappearance on touch" );
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
                        if ( dynamic_cast< PlayerItem * >( sender ) && mayTake( sender ) )
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

                                // play sound of taking
                                SoundManager::getInstance()->play( item->getLabel(), activity );

                                // bonus item disappears from room once it is got
                                BonusManager::getInstance()->markBonusAsAbsent( item->getMediator()->getRoom()->getIdentifier(), item->getLabel() );

                                takeMagicItem( static_cast< PlayerItem* >( sender ) );

                                // create item "bubbles" in the place of magic item
                                freeItem = new FreeItem( bubblesData, item->getX(), item->getY(), item->getZ(), NoDirection );

                                freeItem->assignBehavior( "behavior of disappearance in time", 0 );
                                freeItem->setCollisionDetector( false );

                                // add to current room
                                mediator->getRoom()->addFreeItem( freeItem );
                        }
                        break;

                default:
                        ;
        }

        return destroy;
}

bool Special::mayTake( Item* sender )
{
        std::string player = sender->getLabel();
        std::string magicItem = this->item->getLabel();

        return  ( player == "head"      &&  ( magicItem == "donuts" ||
                                                magicItem == "extra-life" ||
                                                magicItem == "high-speed" ||
                                                magicItem == "shield" ||
                                                magicItem == "crown" ||
                                                magicItem == "horn" ||
                                                magicItem == "reincarnation-fish" ) )
                ||

                ( player == "heels"     &&  ( magicItem == "extra-life" ||
                                                magicItem == "high-jumps" ||
                                                magicItem == "shield" ||
                                                magicItem == "crown" ||
                                                magicItem == "handbag" ||
                                                magicItem == "reincarnation-fish" ) )
                ||

                ( player == "headoverheels"  &&  ( magicItem == "donuts" ||
                                                magicItem == "extra-life" ||
                                                magicItem == "shield" ||
                                                magicItem == "crown" ||
                                                magicItem == "horn" ||
                                                magicItem == "handbag" ||
                                                magicItem == "reincarnation-fish" ) ) ;
}

void Special::takeMagicItem( PlayerItem* who )
{
        std::string magicItem = this->item->getLabel();

        if ( magicItem == "donuts" )
        {
                const unsigned short DonutsPerBox = 6 ;
                who->addAmmo( DonutsPerBox );
        }
        else if ( magicItem == "extra-life" )
        {
                who->addLives( 2 );
        }
        else if ( magicItem == "high-speed" )
        {
                who->activateHighSpeed();
        }
        else if ( magicItem == "high-jumps" )
        {
                who->addHighJumps( 10 );
        }
        else if ( magicItem == "shield" )
        {
                who->activateShield();
        }
        else if ( magicItem == "crown" )
        {
                who->liberatePlanet();
        }
        else if ( magicItem == "horn" || magicItem == "handbag" )
        {
                who->takeTool( magicItem );
        }
        else if ( magicItem == "reincarnation-fish" )
        {
                who->saveAt( this->item->getX (), this->item->getY (), this->item->getZ () );
        }
}

void Special::setMoreData( void* data )
{
        this->bubblesData = reinterpret_cast< ItemData* >( data );
}

}
