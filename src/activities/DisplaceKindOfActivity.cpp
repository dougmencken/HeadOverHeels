
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Behavior.hpp"
#include "Item.hpp"
#include "GridItem.hpp"
#include "FreeItem.hpp"


namespace isomot
{

DisplaceKindOfActivity * DisplaceKindOfActivity::instance = nilPointer ;

DisplaceKindOfActivity* DisplaceKindOfActivity::getInstance()
{
        if ( instance == nilPointer )
        {
                instance = new DisplaceKindOfActivity();
        }

        return instance;
}


DisplaceKindOfActivity::DisplaceKindOfActivity() : KindOfActivity()
{

}

DisplaceKindOfActivity::~DisplaceKindOfActivity()
{
}

bool DisplaceKindOfActivity::displace( Behavior* behavior, ActivityOfItem* activity, bool canFall )
{
        bool itemDisplaced = false;

        Item* item = behavior->getItem ();
        if ( item == nilPointer ) return false ;

        ActivityOfItem activityToPropagate = *activity;

        switch ( *activity )
        {
                case ForceDisplaceNorth:
                        activityToPropagate = DisplaceNorth;
                        // fallthru
                case DisplaceNorth:
                        itemDisplaced = item->addToX( -1 );
                        break;

                case ForceDisplaceSouth:
                        activityToPropagate = DisplaceSouth;
                        // fallthru
                case DisplaceSouth:
                        itemDisplaced = item->addToX( 1 );
                        break;

                case ForceDisplaceEast:
                        activityToPropagate = DisplaceEast;
                        // fallthru
                case DisplaceEast:
                        itemDisplaced = item->addToY( -1 );
                        break;

                case ForceDisplaceWest:
                        activityToPropagate = DisplaceWest;
                        // fallthru
                case DisplaceWest:
                        itemDisplaced = item->addToY( 1 );
                        break;

                case DisplaceNortheast:
                        itemDisplaced = item->addToPosition( -1, -1, 0 );
                        break;

                case DisplaceNorthwest:
                        itemDisplaced = item->addToPosition( -1, 1, 0 );
                        break;

                case DisplaceSoutheast:
                        itemDisplaced = item->addToPosition( 1, -1, 0 );
                        break;

                case DisplaceSouthwest:
                        itemDisplaced = item->addToPosition( 1, 1, 0 );
                        break;

                case DisplaceUp:
                        itemDisplaced = item->addToZ( 1 );
                        break;

                default:
                        ;
        }

        if ( item->whichKindOfItem() == "free item" || item->whichKindOfItem() == "player item" )
        {
                // when there’s collision
                if ( ! itemDisplaced )
                {
                        // move involved items
                        this->propagateActivityToAdjacentItems( item, activityToPropagate );
                }
                else
                {
                        // look if items on top of this item needs to move too
                        this->propagateActivityToItemsAbove( item, *activity );
                }
        }

        // when item can fall
        if ( canFall )
        {
                // look if it falls yet
                if ( FallKindOfActivity::getInstance()->fall( behavior ) )
                {
                        *activity = Fall;
                        itemDisplaced = true;
                }
        }

        return itemDisplaced ;
}

}
