
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Behavior.hpp"
#include "Item.hpp"
#include "GridItem.hpp"
#include "FreeItem.hpp"


namespace iso
{

DisplaceKindOfActivity * DisplaceKindOfActivity::instance = nilPointer ;

DisplaceKindOfActivity& DisplaceKindOfActivity::getInstance()
{
        if ( instance == nilPointer )
        {
                instance = new DisplaceKindOfActivity();
        }

        return *instance;
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

        ItemPtr item = behavior->getItem ();
        if ( item == nilPointer ) return false ;

        ActivityOfItem activityToPropagate = *activity;

        switch ( *activity )
        {
                case Activity::ForceDisplaceNorth:
                        activityToPropagate = Activity::DisplaceNorth;
                        // fallthru
                case Activity::DisplaceNorth:
                        itemDisplaced = item->addToX( -1 );
                        break;

                case Activity::ForceDisplaceSouth:
                        activityToPropagate = Activity::DisplaceSouth;
                        // fallthru
                case Activity::DisplaceSouth:
                        itemDisplaced = item->addToX( 1 );
                        break;

                case Activity::ForceDisplaceEast:
                        activityToPropagate = Activity::DisplaceEast;
                        // fallthru
                case Activity::DisplaceEast:
                        itemDisplaced = item->addToY( -1 );
                        break;

                case Activity::ForceDisplaceWest:
                        activityToPropagate = Activity::DisplaceWest;
                        // fallthru
                case Activity::DisplaceWest:
                        itemDisplaced = item->addToY( 1 );
                        break;

                case Activity::DisplaceNortheast:
                        itemDisplaced = item->addToPosition( -1, -1, 0 );
                        break;

                case Activity::DisplaceNorthwest:
                        itemDisplaced = item->addToPosition( -1, 1, 0 );
                        break;

                case Activity::DisplaceSoutheast:
                        itemDisplaced = item->addToPosition( 1, -1, 0 );
                        break;

                case Activity::DisplaceSouthwest:
                        itemDisplaced = item->addToPosition( 1, 1, 0 );
                        break;

                case Activity::DisplaceUp:
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
                        this->propagateActivityToAdjacentItems( *item, activityToPropagate );
                }
                else
                {
                        // look if items on top of this item needs to move too
                        this->propagateActivityToItemsAbove( *item, *activity );
                }
        }

        // when item can fall
        if ( canFall )
        {
                // look if it falls yet
                if ( FallKindOfActivity::getInstance().fall( behavior ) )
                {
                        *activity = Activity::Fall;
                        itemDisplaced = true;
                }
        }

        return itemDisplaced ;
}

}
