
#include "Switch.hpp"
#include "Item.hpp"
#include "PlayerItem.hpp"
#include "Mediator.hpp"
#include "SoundManager.hpp"


namespace isomot
{

Switch::Switch( Item * item, const std::string & behavior )
        : Behavior( item, behavior )
        , isItemAbove( false )
{

}

Switch::~Switch( )
{

}

bool Switch::update ()
{
        Mediator * mediator = item->getMediator();
        std::vector< Item * > sideItems;

        switch ( activity )
        {
                case Wait:
                        // Comprueba si hay elementos a los lados
                        if ( checkSideItems( sideItems ) )
                        {
                                // Si algún elemento que cambió el estado del interruptor ya no está junto a él, o bien
                                // ese elemento está junto a él pero es un jugador y está detenido, entonces se elimina de
                                // la lista de activadores para que dicho elemento pueda volver a activar el interruptor
                                for ( size_t i = 0; i < triggerItems.size(); i++ )
                                {
                                        Item* tempItem = triggerItems[ i ];

                                        if ( std::find_if( sideItems.begin (), sideItems.end (),
                                                std::bind2nd( EqualItemId(), tempItem->getId() ) ) == sideItems.end() ||
                                                ( dynamic_cast< PlayerItem * >( tempItem ) && tempItem->getBehavior()->getActivityOfItem() == Wait ) )
                                        {
                                                triggerItems.erase( std::remove_if(
                                                        triggerItems.begin (), triggerItems.end (),
                                                        std::bind2nd( EqualItemId(), tempItem->getId() )
                                                ), triggerItems.end () );
                                        }
                                }
                        }
                        else
                        {
                                triggerItems.clear();
                        }

                        // look for items above
                        if ( ! item->checkPosition( 0, 0, 1, Add ) )
                        {
                                // copy stack of collisions
                                std::stack< int > aboveItems;
                                while ( ! mediator->isStackOfCollisionsEmpty() )
                                {
                                        aboveItems.push( mediator->popCollision() );
                                }

                                // as long as there’re elements above this switch
                                while ( ! aboveItems.empty() )
                                {
                                        int id = aboveItems.top();
                                        aboveItems.pop();

                                        // is it free item
                                        if ( id >= FirstFreeId && ( id & 1 ) )
                                        {
                                                Item* itemAbove = mediator->findItemById( id );

                                                // yep, switch doesn’t toggle when player jumps
                                                if ( itemAbove != nilPointer && itemAbove->getBehavior() != nilPointer &&
                                                        ! itemAbove->checkPosition( 0, 0, -1, Add ) &&
                                                                itemAbove->getBehavior()->getActivityOfItem() != RegularJump &&
                                                                itemAbove->getBehavior()->getActivityOfItem() != HighJump )
                                                {
                                                        // when there’s no more than one item below initiator of switch
                                                        if ( ! isItemAbove && mediator->depthOfStackOfCollisions() <= 1 )
                                                        {
                                                                isItemAbove = true;
                                                                item->animate();

                                                                mediator->toggleSwitchInRoom();
                                                                SoundManager::getInstance()->play( item->getLabel(), SwitchIt );
                                                        }
                                                }
                                        }
                                }
                        }
                        else
                        {
                                isItemAbove = false;
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
                        // Si el elemento no estaba junto al interruptor entonces ahora lo activa y ya no lo podrá
                        // volver a activar a no ser que se cumplan las condiciones fijadas en el estado inicial
                        if ( std::find_if( triggerItems.begin (), triggerItems.end (), std::bind2nd( EqualItemId(), sender->getId() ) ) == triggerItems.end () )
                        {
                                triggerItems.push_back( sender );
                                item->animate();
                                // Comunica a la sala el cambio de estado del interruptor
                                mediator->toggleSwitchInRoom();
                                // Activa el sonido de conmutación
                                SoundManager::getInstance()->play( item->getLabel(), SwitchIt );
                        }

                        // Vuelta al estado inicial
                        activity = Wait;
                        break;

                default:
                        ;
        }

        return false;
}

bool Switch::checkSideItems( std::vector< Item * >& sideItems )
{
        Mediator* mediator = item->getMediator();

        // Comprueba si hay elementos al norte
        if ( ! item->checkPosition( -1, 0, 0, Add ) )
        {
                while ( ! mediator->isStackOfCollisionsEmpty() )
                {
                        sideItems.push_back( mediator->findCollisionPop() );
                }
        }

        // Comprueba si hay elementos al sur
        if ( ! item->checkPosition( 1, 0, 0, Add ) )
        {
                while ( ! mediator->isStackOfCollisionsEmpty() )
                {
                        sideItems.push_back( mediator->findCollisionPop() );
                }
        }

        // Comprueba si hay elementos al este
        if ( ! item->checkPosition( 0, -1, 0, Add ) )
        {
                while ( ! mediator->isStackOfCollisionsEmpty() )
                {
                        sideItems.push_back( mediator->findCollisionPop() );
                }
        }

        // Comprueba si hay elementos al oeste
        if ( ! item->checkPosition( 0, 1, 0, Add ) )
        {
                while ( ! mediator->isStackOfCollisionsEmpty() )
                {
                        sideItems.push_back( mediator->findCollisionPop() );
                }
        }

        return ! sideItems.empty();
}

}
