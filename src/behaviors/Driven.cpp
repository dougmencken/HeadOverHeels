
#include "Driven.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "MoveKindOfActivity.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"


namespace iso
{

Driven::Driven( const ItemPtr & item, const std::string& behavior )
        : Behavior( item, behavior )
        , moving( false )
        , speedTimer( new Timer() )
        , fallTimer( new Timer() )
{
        speedTimer->go();
        fallTimer->go();
}

Driven::~Driven( )
{
}

bool Driven::update ()
{
        FreeItem& freeItem = dynamic_cast< FreeItem& >( * this->item );
        Mediator* mediator = freeItem.getMediator();
        bool isGone = false;
        bool playerFound = false;

        switch ( activity )
        {
                case Activity::Wait:
                        if ( moving )
                        {
                                switch ( freeItem.getOrientation().getIntegerOfWay() )
                                {
                                        case Way::North:
                                                changeActivityOfItem( Activity::MoveNorth );
                                                break;

                                        case Way::South:
                                                changeActivityOfItem( Activity::MoveSouth );
                                                break;

                                        case Way::East:
                                                changeActivityOfItem( Activity::MoveEast );
                                                break;

                                        case Way::West:
                                                changeActivityOfItem( Activity::MoveWest );
                                                break;

                                        default:
                                                ;
                                }
                        }
                        // when stopped, see if there is a character on it and use its orientation to begin moving
                        else
                        {
                                if ( ! freeItem.canAdvanceTo( 0, 0, 1 ) )
                                {
                                        while ( ! mediator->isStackOfCollisionsEmpty() && ! playerFound )
                                        {
                                                ItemPtr item = mediator->findCollisionPop ();

                                                if ( item->whichKindOfItem() == "player item" )
                                                {
                                                        playerFound = true;
                                                        moving = true;

                                                        switch ( item->getOrientation().getIntegerOfWay () )
                                                        {
                                                                case Way::North:
                                                                        changeActivityOfItem( Activity::MoveNorth );
                                                                        break;

                                                                case Way::South:
                                                                        changeActivityOfItem( Activity::MoveSouth );
                                                                        break;

                                                                case Way::East:
                                                                        changeActivityOfItem( Activity::MoveEast );
                                                                        break;

                                                                case Way::West:
                                                                        changeActivityOfItem( Activity::MoveWest );
                                                                        break;

                                                                default:
                                                                        ;
                                                        }
                                                }
                                        }
                                }
                        }
                        break;

                case Activity::MoveNorth:
                case Activity::MoveSouth:
                case Activity::MoveEast:
                case Activity::MoveWest:
                        // item is active and it is time to move
                        if ( ! freeItem.isFrozen() )
                        {
                                if ( speedTimer->getValue() > freeItem.getSpeed() )
                                {
                                        if ( ! MoveKindOfActivity::getInstance().move( this, &activity, true ) )
                                        {
                                                moving = false;
                                                activity = Activity::Wait;

                                                // emit sound of collision
                                                SoundManager::getInstance().play( freeItem.getLabel(), Activity::Collision );
                                        }

                                        speedTimer->reset();
                                }

                                freeItem.animate();
                        }
                        break;

                case Activity::DisplaceNorth:
                case Activity::DisplaceSouth:
                case Activity::DisplaceEast:
                case Activity::DisplaceWest:
                case Activity::DisplaceNortheast:
                case Activity::DisplaceNorthwest:
                case Activity::DisplaceSoutheast:
                case Activity::DisplaceSouthwest:
                        // is it time to move
                        if ( speedTimer->getValue() > freeItem.getSpeed() )
                        {
                                if ( ! DisplaceKindOfActivity::getInstance().displace( this, &activity, true ) )
                                {
                                        activity = Activity::Wait;
                                }

                                speedTimer->reset();
                        }

                        // inactive item continues to be inactive
                        if ( freeItem.isFrozen() )
                        {
                                activity = Activity::Freeze;
                        }
                        break;

                case Activity::Fall:
                        // look for reaching floor in a room without floor
                        if ( freeItem.getZ() == 0 && ! freeItem.getMediator()->getRoom()->hasFloor() )
                        {
                                // item disappears
                                isGone = true;
                        }
                        // is it time to fall
                        else if ( fallTimer->getValue() > freeItem.getWeight() )
                        {
                                if ( ! FallKindOfActivity::getInstance().fall( this ) )
                                {
                                        activity = Activity::Wait;
                                }

                                fallTimer->reset();
                        }
                        break;

                case Activity::Freeze:
                        freeItem.setFrozen( true );
                        break;

                case Activity::WakeUp:
                        freeItem.setFrozen( false );
                        activity = Activity::Wait;
                        break;

                default:
                        ;
        }

        return isGone;
}

}
