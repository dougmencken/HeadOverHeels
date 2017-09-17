
#include "Teleport.hpp"
#include "Item.hpp"
#include "GridItem.hpp"
#include "Mediator.hpp"
#include "SoundManager.hpp"


namespace isomot
{

Teleport::Teleport( Item * item, const std::string & behavior ) :
        Behavior( item, behavior ),
        activated( false )
{

}

Teleport::~Teleport()
{

}

bool Teleport::update ()
{
        Mediator * mediator = item->getMediator();

        switch ( activity )
        {
                case Wait:
                // Comprueba si hay elementos encima
                if ( ! item->checkPosition( 0, 0, 1, Add ) )
                {
                        // Copia la pila de colisiones
                        std::stack< int > topItems;
                        while ( ! mediator->isStackOfCollisionsEmpty() )
                        {
                                topItems.push( mediator->popCollision() );
                        }

                        // Mientras haya elementos encima de este elemento se comprobarán
                        // las condiciones para ver si se puede activar
                        while ( ! topItems.empty() )
                        {
                                // Identificador del primer elemento de la pila de colisiones
                                int id = topItems.top();
                                topItems.pop();

                                // El elemento tiene que ser un elemento libre
                                if ( id >= FirstFreeId && ( id & 1 ) )
                                {
                                        Item * topItem = mediator->findItemById( id );

                                        // El elemento debe tener comportamiento
                                        if ( topItem != 0 && topItem->getBehavior() != 0 )
                                        {
                                                // Si debajo del elemento que está encima del elemento superior hay más elementos,
                                                // se busca cuál de estos últimos tiene unas coordenadas espaciales mayores
                                                if ( ! topItem->checkPosition( 0, 0, -1, Add ) )
                                                {
                                                        int stackSize = 0;
                                                        bool closerItem = true;

                                                        while ( ! mediator->isStackOfCollisionsEmpty() )
                                                        {
                                                                Item * downItem = mediator->findCollisionPop( );

                                                                if ( downItem != 0 && item != downItem && item->getX() + item->getY() < item->getX() + item->getY() )
                                                                {
                                                                        closerItem = false;
                                                                }

                                                                stackSize++;
                                                        }

                                                        // Si sólo hay un elemento debajo o se encontró el más cercano a la cámara entonces se activa
                                                        activated = ( stackSize == 1 || closerItem );
                                                }
                                        }
                                }
                        }
                }
                else
                {
                        activated = false;
                }

                // Si está activo se pone en funcionamiento
                if ( activated )
                {
                        item->animate();
                        SoundManager::getInstance()->play( item->getLabel(), IsActive );
                }
                break;

                default:
                        activity = Wait;
        }

        return false;
}

}
