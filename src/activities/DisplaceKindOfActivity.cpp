
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Behavior.hpp"
#include "Item.hpp"
#include "GridItem.hpp"
#include "FreeItem.hpp"


namespace isomot
{

KindOfActivity * DisplaceKindOfActivity::instance = nilPointer ;


DisplaceKindOfActivity::DisplaceKindOfActivity() : KindOfActivity()
{

}

DisplaceKindOfActivity::~DisplaceKindOfActivity()
{
}

KindOfActivity* DisplaceKindOfActivity::getInstance()
{
        if ( instance == nilPointer )
        {
                instance = new DisplaceKindOfActivity();
        }

        return instance;
}

bool DisplaceKindOfActivity::displace( Behavior* behavior, ActivityOfItem* activity, bool canFall )
{
        bool itemDisplaced = false;

        // get free item to move from its behavior
        FreeItem* freeItem = dynamic_cast < FreeItem * > ( behavior->getItem () );

        ActivityOfItem displaceActivity = *activity;

        switch ( *activity )
        {
                case DisplaceNorth:
                case ForceDisplaceNorth:
                        if ( freeItem != nilPointer )
                        {
                                itemDisplaced = freeItem->addToX( -1 );
                        }
                        displaceActivity = DisplaceNorth;
                        break;

                case DisplaceSouth:
                case ForceDisplaceSouth:
                        if ( freeItem != nilPointer )
                        {
                                itemDisplaced = freeItem->addToX( 1 );
                        }
                        displaceActivity = DisplaceSouth;
                        break;

                case DisplaceEast:
                case ForceDisplaceEast:
                        if ( freeItem != nilPointer )
                        {
                                itemDisplaced = freeItem->addToY( -1 );
                        }
                        displaceActivity = DisplaceEast;
                        break;

                case DisplaceWest:
                case ForceDisplaceWest:
                        if ( freeItem != nilPointer )
                        {
                                itemDisplaced = freeItem->addToY( 1 );
                        }
                        displaceActivity = DisplaceWest;
                        break;

                case DisplaceNortheast:
                        if ( freeItem != nilPointer )
                        {
                                itemDisplaced = freeItem->addToPosition( -1, -1, 0 );
                        }
                        break;

                case DisplaceNorthwest:
                        if ( freeItem != nilPointer )
                        {
                                itemDisplaced = freeItem->addToPosition( -1, 1, 0 );
                        }
                        break;

                case DisplaceSoutheast:
                        if ( freeItem != nilPointer )
                        {
                                itemDisplaced = freeItem->addToPosition( 1, -1, 0 );
                        }
                        break;

                case DisplaceSouthwest:
                        if ( freeItem != nilPointer )
                        {
                                itemDisplaced = freeItem->addToPosition( 1, 1, 0 );
                        }
                        break;

                case DisplaceUp:
                        itemDisplaced = behavior->getItem()->addToZ( 1 );
                        break;

                default:
                        ;
        }

        if ( freeItem != nilPointer )
        {
                // when there’s collision
                if ( ! itemDisplaced )
                {
                        // move involved items
                        this->propagateActivityToAdjacentItems( freeItem, displaceActivity );
                }
                else
                {
                        // look if items on top of this item needs to move too
                        this->propagateActivityToItemsAbove( freeItem, *activity );
                }
        }

        // when item can fall
        if ( canFall )
        {
                // look if it falls yet
                if ( FallKindOfActivity::getInstance()->fall( behavior ) )
                {
                        behavior->changeActivityTo( FallKindOfActivity::getInstance() );
                        *activity = Fall;
                        itemDisplaced = true;
                }
        }

        return itemDisplaced ;
}

}
