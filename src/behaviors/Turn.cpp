
#include "Turn.hpp"
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

Turn::Turn( const ItemPtr & item, const std::string & behavior )
        : Behavior( item, behavior )
        , speedTimer( new Timer() )
        , fallTimer( new Timer() )
{
        speedTimer->go();
        fallTimer->go();
}

Turn::~Turn()
{
}

bool Turn::update ()
{
        bool isGone = false;
        FreeItem& freeItem = dynamic_cast< FreeItem& >( * this->item );

        switch ( activity )
        {
                case Activity::Wait:
                        begin();
                        break;

                case Activity::MoveNorth:
                case Activity::MoveSouth:
                case Activity::MoveEast:
                case Activity::MoveWest:
                        if ( ! freeItem.isFrozen() )
                        {
                                if ( speedTimer->getValue() > freeItem.getSpeed() )
                                {
                                        if ( ! MoveKindOfActivity::getInstance().move( this, &activity, true ) )
                                        {
                                                turn();

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
                        SoundManager::getInstance().play( freeItem.getLabel(), activity );

                        DisplaceKindOfActivity::getInstance().displace( this, &activity, true );

                        activity = Activity::Wait;

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
                                isGone = true;
                        }
                        // is it time to lower by one unit
                        else if ( fallTimer->getValue() > freeItem.getWeight() )
                        {
                                if ( ! FallKindOfActivity::getInstance().fall( this ) )
                                {
                                        SoundManager::getInstance().play( freeItem.getLabel(), activity );
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

void Turn::begin()
{
        switch ( this->item->getOrientation().getIntegerOfWay () )
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

void Turn::turn()
{
        switch ( item->getOrientation().getIntegerOfWay () )
        {
                case Way::North:
                        if ( getNameOfBehavior() == "behavior of move then turn left and move" ) {
                                activity = Activity::MoveWest ;
                                item->changeOrientation( Way( "west" ) );
                        } else {
                                activity = Activity::MoveEast ;
                                item->changeOrientation( Way( "east" ) );
                        }
                        break;

                case Way::South:
                        if ( getNameOfBehavior() == "behavior of move then turn left and move" ) {
                                activity = Activity::MoveEast ;
                                item->changeOrientation( Way( "east" ) );
                        } else {
                                activity = Activity::MoveWest ;
                                item->changeOrientation( Way( "west" ) );
                        }
                        break;

                case Way::East:
                        if ( getNameOfBehavior() == "behavior of move then turn left and move" ) {
                                activity = Activity::MoveNorth ;
                                item->changeOrientation( Way( "north" ) );
                        } else {
                                activity = Activity::MoveSouth ;
                                item->changeOrientation( Way( "south" ) );
                        }
                        break;

                case Way::West:
                        if ( getNameOfBehavior() == "behavior of move then turn left and move" ) {
                                activity = Activity::MoveSouth ;
                                item->changeOrientation( Way( "south" ) );
                        } else {
                                activity = Activity::MoveNorth ;
                                item->changeOrientation( Way( "north" ) );
                        }
                        break;

                default:
                        ;
        }
}

}
