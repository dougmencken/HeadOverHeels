
#include "ThereAndBack.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "MoveKindOfActivity.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"


namespace iso
{

ThereAndBack::ThereAndBack( const ItemPtr & item, const std::string & behavior, bool flying ) :
        Behavior( item, behavior )
        , isFlying( flying )
        , speedTimer( new Timer() )
        , fallTimer( new Timer() )
{
        speedTimer->go();
        if ( ! flying ) fallTimer->go();
}

ThereAndBack::~ThereAndBack()
{
}

bool ThereAndBack::update ()
{
        bool vanish = false;
        FreeItem& freeItem = dynamic_cast< FreeItem& >( * this->item );

        switch ( activity )
        {
                case Activity::Wait:
                        letsMove();
                        break;

                case Activity::MoveNorth:
                case Activity::MoveSouth:
                case Activity::MoveEast:
                case Activity::MoveWest:
                        if ( ! freeItem.isFrozen() )
                        {
                                if ( speedTimer->getValue() > freeItem.getSpeed() )
                                {
                                        // move it
                                        if ( ! MoveKindOfActivity::getInstance().move( this, &activity, true ) )
                                        {
                                                turnRound();

                                                // play sound of colliding
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
                case Activity::DisplaceSoutheast:
                case Activity::DisplaceSouthwest:
                case Activity::DisplaceNorthwest:
                        if ( ! this->isFlying )
                        {
                                // emit sound of displacing
                                SoundManager::getInstance().play( freeItem.getLabel(), activity );

                                // displace this item by other one
                                DisplaceKindOfActivity::getInstance().displace( this, &activity, true );

                                activity = Activity::Wait;

                                // preserve inactivity for frozen item
                                if ( freeItem.isFrozen() )
                                        activity = Activity::Freeze;
                        }
                        else
                        {
                                activity = Activity::Wait;
                        }
                        break;

                case Activity::Fall:
                        if ( ! this->isFlying )
                        {
                                // look for reaching floor in a room without floor
                                if ( freeItem.getZ() == 0 && ! freeItem.getMediator()->getRoom()->hasFloor() )
                                {
                                        // item disappears
                                        vanish = true;
                                }
                                // is it time to fall
                                else if ( fallTimer->getValue() > freeItem.getWeight() )
                                {
                                        if ( ! FallKindOfActivity::getInstance().fall( this ) )
                                        {
                                                // emit sound of falling down
                                                SoundManager::getInstance().play( freeItem.getLabel(), activity );
                                                activity = Activity::Wait;
                                        }

                                        fallTimer->reset();
                                }
                        }
                        else
                        {
                                activity = Activity::Wait;
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

        return vanish;
}

void ThereAndBack::letsMove()
{
        switch ( Way( item->getOrientation() ).getIntegerOfWay () )
        {
                case Way::North:
                        activity = Activity::MoveNorth;
                        break;

                case Way::South:
                        activity = Activity::MoveSouth;
                        break;

                case Way::East:
                        activity = Activity::MoveEast;
                        break;

                case Way::West:
                        activity = Activity::MoveWest;
                        break;

                default:
                        ;
        }
}

void ThereAndBack::turnRound()
{
        switch ( Way( item->getOrientation() ).getIntegerOfWay () )
        {
                case Way::North:
                        activity = Activity::MoveSouth ;
                        item->changeOrientation( "south" );
                        break;

                case Way::South:
                        activity = Activity::MoveNorth ;
                        item->changeOrientation( "north" );
                        break;

                case Way::East:
                        activity = Activity::MoveWest ;
                        item->changeOrientation( "west" );
                        break;

                case Way::West:
                        activity = Activity::MoveEast ;
                        item->changeOrientation( "east" );
                        break;

                default:
                        ;
        }
}

}
