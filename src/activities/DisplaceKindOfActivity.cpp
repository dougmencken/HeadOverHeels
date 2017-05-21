
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Behavior.hpp"
#include "Item.hpp"
#include "GridItem.hpp"
#include "FreeItem.hpp"


namespace isomot
{

KindOfActivity * DisplaceKindOfActivity::instance = 0 ;


DisplaceKindOfActivity::DisplaceKindOfActivity() : KindOfActivity()
{

}

DisplaceKindOfActivity::~DisplaceKindOfActivity()
{
}

KindOfActivity* DisplaceKindOfActivity::getInstance()
{
        if ( instance == 0 )
        {
                instance = new DisplaceKindOfActivity();
        }

        return instance;
}

bool DisplaceKindOfActivity::displace( Behavior* behavior, ActivityOfItem* activity, bool canFall )
{
        bool changedData = false;
        FreeItem* freeItem = 0;
        ActivityOfItem displaceActivity = *activity;

        // Acceso al elemento que hay que mover si dicho elemento es libre
        if ( behavior->getItem()->getId() & 1 )
        {
                freeItem = dynamic_cast < FreeItem * > ( behavior->getItem () );
        }

        switch ( *activity )
        {
                case DisplaceNorth:
                case ForceDisplaceNorth:
                        if ( freeItem != 0 )
                        {
                                changedData = freeItem->addToX( -1 );
                        }
                        displaceActivity = DisplaceNorth;
                        break;

                case DisplaceSouth:
                case ForceDisplaceSouth:
                        if ( freeItem != 0 )
                        {
                                changedData = freeItem->addToX( 1 );
                        }
                        displaceActivity = DisplaceSouth;
                        break;

                case DisplaceEast:
                case ForceDisplaceEast:
                        if ( freeItem != 0 )
                        {
                                changedData = freeItem->addToY( -1 );
                        }
                        displaceActivity = DisplaceEast;
                        break;

                case DisplaceWest:
                case ForceDisplaceWest:
                        if ( freeItem != 0 )
                        {
                                changedData = freeItem->addToY( 1 );
                        }
                        displaceActivity = DisplaceWest;
                        break;

                case DisplaceNortheast:
                        if ( freeItem != 0 )
                        {
                                changedData = freeItem->addToPosition( -1, -1, 0 );
                        }
                        break;

                case DisplaceNorthwest:
                        if ( freeItem != 0 )
                        {
                                changedData = freeItem->addToPosition( -1, 1, 0 );
                        }
                        break;

                case DisplaceSoutheast:
                        if ( freeItem != 0 )
                        {
                                changedData = freeItem->addToPosition( 1, -1, 0 );
                        }
                        break;

                case DisplaceSouthwest:
                        if ( freeItem != 0 )
                        {
                                changedData = freeItem->addToPosition( 1, 1, 0 );
                        }
                        break;

                case DisplaceUp:
                        changedData = behavior->getItem()->addToZ( 1 );
                        break;

                default:
                        ;
        }

        if ( freeItem != 0 )
        {
                // En caso de colisión en los ejes X o Y se desplaza a los elementos implicados
                if ( ! changedData )
                {
                        this->propagateActivityToAdjacentItems( freeItem, displaceActivity );
                }
                // En caso de que el elemento se haya movido se comprueba si hay que desplazar los elementos
                // que pueda tener encima
                else
                {
                        this->propagateActivityToTopItems( freeItem, *activity );
                }
        }

        // Si el elemento puede caer entonces se comprueba si hay que cambiar el estado
        if ( canFall )
        {
                if ( FallKindOfActivity::getInstance()->fall( behavior ) )
                {
                        changeKindOfActivity( behavior, FallKindOfActivity::getInstance() );
                        *activity = Fall;
                        changedData = true;
                }
        }

        return changedData;
}

}
