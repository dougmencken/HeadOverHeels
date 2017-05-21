
#include "Volatile.hpp"
#include "Item.hpp"
#include "ItemData.hpp"
#include "FreeItem.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "SoundManager.hpp"


namespace isomot
{

Volatile::Volatile( Item* item, const BehaviorOfItem& id )
        : Behavior( item, id )
        , solid( false )
{
        destroyTimer = new HPC();
        destroyTimer->start();
}

Volatile::~Volatile()
{
        delete destroyTimer;
}

bool Volatile::update ()
{
        bool destroy = false;
        Mediator* mediator = item->getMediator();

        switch ( activity )
        {
                case Wait:
                case WakeUp:
                        // En este estado tiene que ser forzosamente volátil
                        this->solid = false;

                        // Si es volátil por contacto o por peso y tiene un elemento encima entonces puede destruirse
                        if ( ( getBehaviorOfItem () == VolatileTouchBehavior ||
                                        getBehaviorOfItem () == VolatileWeightBehavior ||
                                                getBehaviorOfItem () == VolatileHeavyBehavior )
                                && ! item->checkPosition( 0, 0, 1, Add ) )
                        {
                                bool destroy = false;

                                // Copia la pila de colisiones
                                std::stack< Item * > topItems;

                                // Se inspeccionan todos los elementos que haya encima para ver
                                // si la destrucción se puede llevar a cabo
                                while ( ! mediator->isStackOfCollisionsEmpty() )
                                {
                                        // Identificador del primer elemento de la pila de colisiones
                                        int id = mediator->popCollision();

                                        // El elemento tiene que se un elemento libre
                                        if ( id >= FirstFreeId && ( id & 1 ) )
                                        {
                                                Item* item = mediator->findItemById( id );

                                                // Si el elemento existe, se mira si es volátil o especial porque
                                                // el elemento se destruirá a no ser que ese elemento esté apoyándose en otro
                                                if ( item != 0 && item->getBehavior() != 0 &&
                                                        item->getBehavior()->getBehaviorOfItem () != VolatileWeightBehavior &&
                                                        item->getBehavior()->getBehaviorOfItem () != VolatileHeavyBehavior &&
                                                        item->getBehavior()->getBehaviorOfItem () != VolatileTouchBehavior &&
                                                        item->getBehavior()->getBehaviorOfItem () != SpecialBehavior )
                                                {
                                                        destroy = true;
                                                        topItems.push( item );
                                                }
                                        }
                                }

                                if ( destroy )
                                {
                                        Item* topItem = topItems.top();
                                        topItems.pop();
                                        topItem->checkPosition( 0, 0, -1, Add );

                                        // Si el elemento que desencadena la destrucción está unicamente sobre el volátil
                                        // entonces la destrucción es segura. Si está sobre varios elementos, se tienen que dar
                                        // ciertas condiciones para poder destruilo
                                        if ( mediator->depthOfStackOfCollisions() > 1 )
                                        {
                                                while ( ! mediator->isStackOfCollisionsEmpty() )
                                                {
                                                        Item* bottomItem = mediator->findCollisionPop( );

                                                        if ( bottomItem != 0 )
                                                        {
                                                                // El volátil no se destruirá si está apoyándose:
                                                                // sobre un elemento sin comportamiento, o
                                                                // sobre un elemento que no sea volátil, o
                                                                // sobre un elemento que está destruyéndose
                                                                if ( ( bottomItem->getBehavior() == 0 ) ||
                                                                        ( bottomItem->getBehavior() != 0
                                                                                && bottomItem->getBehavior()->getBehaviorOfItem () != VolatileWeightBehavior
                                                                                && bottomItem->getBehavior()->getBehaviorOfItem () != VolatileTouchBehavior
                                                                                && bottomItem->getBehavior()->getBehaviorOfItem () != SpecialBehavior ) ||
                                                                        ( bottomItem->getBehavior() != 0
                                                                                && bottomItem->getBehavior()->getActivityOfItem() == Destroy ) )
                                                                {
                                                                        destroy = false;
                                                                }
                                                        }
                                                }
                                        }
                                }

                                // Cambio de estado si se han cumplido las condiciones
                                if ( destroy )
                                {
                                        activity = Destroy;
                                        destroyTimer->reset();
                                }
                        }
                        // Es un perrito del silencio. Desaparece si Head o el jugador compuesto están en la sala
                        else if ( getBehaviorOfItem () == VolatilePuppyBehavior )
                        {
                                if ( mediator->findItemByLabel( short( Head ) ) != 0 || mediator->findItemByLabel( short( HeadAndHeels ) ) != 0 )
                                {
                                        activity = Destroy;
                                        destroyTimer->reset();
                                }
                        }
                        // Es volátil por tiempo, es decir, el elemento creado al destruirse el volátil
                        else if ( getBehaviorOfItem () == VolatileTimeBehavior )
                        {
                                destroy = ( destroyTimer->getValue() > item->getFramesDelay() * double( item->countFrames() ) );
                                item->animate();
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
                        // Si un elemento volátil por contacto es desplazado entonces se destruye
                        if ( ! solid )
                        {
                                switch ( getBehaviorOfItem () )
                                {
                                        case VolatileTouchBehavior:
                                                activity = Destroy;
                                                break;

                                        case VolatilePuppyBehavior:
                                                if ( mediator->findItemByLabel( short( Head ) ) != 0 || mediator->findItemByLabel( short( HeadAndHeels ) ) != 0 )
                                                {
                                                        activity = Destroy;
                                                }
                                                break;

                                        default:
                                                activity = Wait;
                                }
                        }
                        else
                        {
                                activity = Freeze;
                        }
                        break;

                case Destroy:
                        if ( ( getBehaviorOfItem () != VolatileWeightBehavior && getBehaviorOfItem () != VolatileHeavyBehavior && getBehaviorOfItem () != VolatilePuppyBehavior ) ||
                                ( getBehaviorOfItem () == VolatileWeightBehavior && destroyTimer->getValue() > 0.030 ) ||
                                ( getBehaviorOfItem () == VolatileHeavyBehavior && destroyTimer->getValue() > 0.600 ) ||
                                ( getBehaviorOfItem () == VolatilePuppyBehavior && destroyTimer->getValue() > 0.500 ) )
                        {
                                destroy = true;

                                // Emite el sonido de destrucción
                                SoundManager::getInstance()->play( item->getLabel(), activity );

                                // Crea el elemento en la misma posición que el volátil y a su misma altura
                                FreeItem* freeItem = new FreeItem(
                                        bubblesData,
                                        item->getX(), item->getY(), item->getZ(),
                                        NoDirection );

                                freeItem->assignBehavior( VolatileTimeBehavior, 0 );
                                freeItem->setCollisionDetector( false );

                                // Se añade a la sala actual
                                mediator->getRoom()->addItem( freeItem );
                        }
                        break;

                case Freeze:
                        this->solid = true;
                        break;

                default:
                        ;
        }

        return destroy;
}

void Volatile::setMoreData( void* data )
{
        this->bubblesData = reinterpret_cast< ItemData * >( data );
}

}
