
#include "FallKindOfActivity.hpp"
#include "Behavior.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "Mediator.hpp"
#include "Room.hpp"


namespace isomot
{

KindOfActivity * FallKindOfActivity::instance = 0;

FallKindOfActivity::FallKindOfActivity( ) : KindOfActivity()
{

}

FallKindOfActivity::~FallKindOfActivity( )
{

}

KindOfActivity* FallKindOfActivity::getInstance()
{
        if ( instance == 0 )
        {
                instance = new FallKindOfActivity();
        }

        return instance;
}

bool FallKindOfActivity::fall( Behavior * behavior )
{
        bool isFalling = behavior->getItem()->addToZ( -1 );
        /////behavior->getItem()->setAnchor( 0 );

        // Si ha dejado de caer se comprueba si elemento es un jugador y
        // puede haber chocado con un elemento mortal
        if ( ! isFalling )
        {
                Item* sender = behavior->getItem();
                Mediator* mediator = sender->getMediator();

                // Copia la pila de colisiones
                std::stack< int > bottomItems;
                while ( ! mediator->isStackOfCollisionsEmpty() )
                {
                        bottomItems.push( mediator->popCollision() );
                }

                // Asigna el elemento al que está anclado
                this->assignAnchor( dynamic_cast< FreeItem * >( sender ), bottomItems );

                // Mientras haya elementos que hayan chocado con el emisor
                while ( ! bottomItems.empty() )
                {
                        // Identificador del primer elemento de la pila de colisiones
                        int id = bottomItems.top();
                        bottomItems.pop();

                        // El elemento tiene que ser un elemento libre o uno rejilla
                        if ( ( id >= FirstFreeId && ( id & 1 )) || ( id >= FirstGridId && ! ( id & 1 ) ) )
                        {
                                Item* item = mediator->findItemById( id );

                                // Si el elemento se ha encontrado entonces:
                                if ( item != 0 )
                                {
                                        // Si el elemento es un jugador y el emisor es mortal entonces el jugador muere
                                        // siempre y cuando el emisor no esté sobre algún otro elemento
                                        if ( dynamic_cast< PlayerItem * >( item ) && sender->isMortal() )
                                        {
                                                if ( sender->checkPosition( 0, 0, -1, Add ) )
                                                {
                                                        item->getBehavior()->changeActivityOfItem( MeetMortalItem );
                                                }
                                        }
                                        // Si el elemento es mortal y el emisor es un jugador entonces el jugador muere
                                        // siempre y cuando el jugador esté únicamente sobre el elemento mortal y si está
                                        // sobre más elementos que todos sean mortales
                                        else if ( dynamic_cast< PlayerItem * >( sender ) && item->isMortal() )
                                        {
                                                if ( sender->checkPosition( 0, 0, -1, Add ) )
                                                {
                                                        sender->getBehavior()->changeActivityOfItem( MeetMortalItem );
                                                }
                                                else
                                                {
                                                        bool allMortal = true;

                                                        // Comprueba para todos los elementos que haya debajo del jugador
                                                        // si alguno no es mortal
                                                        while ( ! mediator->isStackOfCollisionsEmpty() )
                                                        {
                                                                if ( ! mediator->findCollisionPop()->isMortal() )
                                                                {
                                                                        allMortal = false;
                                                                }
                                                        }

                                                        // Si todos son mortales entonces el jugador muere
                                                        if ( allMortal )
                                                        {
                                                                sender->getBehavior()->changeActivityOfItem( MeetMortalItem );
                                                        }
                                                }
                                        }
                                }
                        }
                        // Si el emisor es un jugador y ha chocado con el suelo:
                        else if ( dynamic_cast< PlayerItem * >( sender ) && id == Floor )
                        {
                                PlayerItem* playerItem = dynamic_cast< PlayerItem * >( sender );

                                switch ( mediator->getRoom()->getFloorType() )
                                {
                                        // Se comprueba si la sala carece de suelo, porque en este caso el jugador cambiará de sala
                                        case NoFloor:
                                                playerItem->setExit( Down );
                                                playerItem->setOrientation( playerItem->getDirection() );
                                                break;

                                        // Si el suelo es normal no se hace nada
                                        case RegularFloor:
                                                break;

                                        // Se comprueba si el suelo de la sala es mortal, porque en este caso el jugador perderá una vida
                                        case MortalFloor:
                                                playerItem->getBehavior()->changeActivityOfItem( MeetMortalItem );
                                                break;
                                }
                        }
                }
        }

        return isFalling;
}

void FallKindOfActivity::assignAnchor( FreeItem* freeItem, std::stack< int > bottomItems )
{
        if ( freeItem != 0 )
        {
                Mediator* mediator = freeItem->getMediator();
                // El ancla se establecerá cuando un elemento caiga y se pose sobre algún otro (su ancla)
                Item* anchor = 0;
                Item* oldAnchor = freeItem->getAnchor();
                // En el caso de que caiga sobre varios elementos tendrá prioridad:
                int count = 0;
                // 1. El elemento rejilla frente al libre
                // 2. El elemento inofensivo frente al mortal
                // 3. El elemento con unas coordenadas espaciales menores

                // Búsqueda del ancla para este elemento:
                while ( ! bottomItems.empty() )
                {
                        int id = bottomItems.top();
                        bottomItems.pop();
                        Item* item = mediator->findItemById( id );
                        count++;

                        // Si ha colisionado con el elemento que ya era ancla entonces éste
                        // tiene prioridad sobre el resto
                        if ( oldAnchor != 0 && item != 0 && oldAnchor->getId() == item->getId() )
                        {
                                anchor = oldAnchor;
                                break;
                        }

                        if ( count == 1 )
                        {
                                anchor = item;
                        }
                        else
                        {
                                // Si es rejilla
                                if ( id >= FirstGridId && ! ( id & 1 ) )
                                {
                                        // ...y el ancla no es rejilla entonces se ha encontrado nuevo ancla
                                        if ( ! ( anchor->getId() >= FirstGridId && ! ( anchor->getId() & 1 ) ) )
                                        {
                                                anchor = item;
                                        }
                                }

                                // Si el ancla no ha cambiado
                                if ( anchor != item )
                                {
                                        // Si no es mortal
                                        if ( ! item->isMortal() )
                                        {
                                                // ...y el ancla sí es mortal entonces se ha encontrado nuevo ancla
                                                if ( anchor->isMortal() )
                                                {
                                                        anchor = item;
                                                }
                                                // ...y el ancla también es mortal entonces se ve su posición
                                                else
                                                {
                                                        // Si tiene unas coordenadas menores entonces se ha encontrado nuevo ancla
                                                        if ( anchor->getX() + anchor->getY() < item->getX() + item->getY() )
                                                        {
                                                                anchor = item;
                                                        }
                                                }
                                        }
                                }
                        }
                }

                // Se establece el ancla
                freeItem->setAnchor( anchor );
        }
}

}
