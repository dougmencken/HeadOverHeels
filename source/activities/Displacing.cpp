
#include "Displacing.hpp"

#include "Falling.hpp"
#include "Behavior.hpp"
#include "Item.hpp"
#include "GridItem.hpp"
#include "FreeItem.hpp"


namespace activities
{

Displacing * Displacing::instance = nilPointer ;

Displacing& Displacing::getInstance()
{
        if ( instance == nilPointer )
        {
                instance = new Displacing();
        }

        return *instance;
}


bool Displacing::displace( behaviors::Behavior* behavior, ActivityOfItem* activity, bool canFall )
{
        bool itemDisplaced = false;

        ItemPtr item = behavior->getItem ();
        if ( item == nilPointer ) return false ;

        ActivityOfItem activityToPropagate = *activity;

        switch ( *activity )
        {
                case activities::Activity::ForcePushNorth:
                        activityToPropagate = activities::Activity::DisplaceNorth;
                        // fallthru
                case activities::Activity::DisplaceNorth:
                        itemDisplaced = item->addToX( -1 );
                        break;

                case activities::Activity::ForcePushSouth:
                        activityToPropagate = activities::Activity::DisplaceSouth;
                        // fallthru
                case activities::Activity::DisplaceSouth:
                        itemDisplaced = item->addToX( 1 );
                        break;

                case activities::Activity::ForcePushEast:
                        activityToPropagate = activities::Activity::DisplaceEast;
                        // fallthru
                case activities::Activity::DisplaceEast:
                        itemDisplaced = item->addToY( -1 );
                        break;

                case activities::Activity::ForcePushWest:
                        activityToPropagate = activities::Activity::DisplaceWest;
                        // fallthru
                case activities::Activity::DisplaceWest:
                        itemDisplaced = item->addToY( 1 );
                        break;

                case activities::Activity::DisplaceNortheast:
                        itemDisplaced = item->addToPosition( -1, -1, 0 );
                        break;

                case activities::Activity::DisplaceNorthwest:
                        itemDisplaced = item->addToPosition( -1, 1, 0 );
                        break;

                case activities::Activity::DisplaceSoutheast:
                        itemDisplaced = item->addToPosition( 1, -1, 0 );
                        break;

                case activities::Activity::DisplaceSouthwest:
                        itemDisplaced = item->addToPosition( 1, 1, 0 );
                        break;

                case activities::Activity::DisplaceUp:
                        itemDisplaced = item->addToZ( 1 );
                        break;

                default:
                        ;
        }

        if ( item->whichItemClass() == "free item" || item->whichItemClass() == "avatar item" )
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
                if ( Falling::getInstance().fall( behavior ) )
                {
                        *activity = activities::Activity::Fall;
                        itemDisplaced = true;
                }
        }

        return itemDisplaced ;
}

}
