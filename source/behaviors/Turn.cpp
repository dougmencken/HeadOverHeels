
#include "Turn.hpp"

#include "Item.hpp"
#include "FreeItem.hpp"
#include "Moving.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"


namespace behaviors
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
                case activities::Activity::Wait:
                        begin();
                        break;

                case activities::Activity::MoveNorth:
                case activities::Activity::MoveSouth:
                case activities::Activity::MoveEast:
                case activities::Activity::MoveWest:
                        if ( ! freeItem.isFrozen() )
                        {
                                if ( speedTimer->getValue() > freeItem.getSpeed() )
                                {
                                        if ( ! activities::Moving::getInstance().move( this, &activity, true ) )
                                        {
                                                turn();

                                                SoundManager::getInstance().play( freeItem.getKind (), "collision" );
                                        }

                                        speedTimer->reset();
                                }

                                freeItem.animate();
                        }
                        break;

                case activities::Activity::DisplaceNorth:
                case activities::Activity::DisplaceSouth:
                case activities::Activity::DisplaceEast:
                case activities::Activity::DisplaceWest:
                case activities::Activity::DisplaceNortheast:
                case activities::Activity::DisplaceSoutheast:
                case activities::Activity::DisplaceSouthwest:
                case activities::Activity::DisplaceNorthwest:
                        SoundManager::getInstance().play( freeItem.getKind (), "push" );

                        activities::Displacing::getInstance().displace( this, &activity, true );

                        activity = activities::Activity::Wait;

                        // inactive item continues to be inactive
                        if ( freeItem.isFrozen() )
                        {
                                activity = activities::Activity::Freeze;
                        }
                        break;

                case activities::Activity::Fall:
                        // look for reaching floor in a room without floor
                        if ( freeItem.getZ() == 0 && ! freeItem.getMediator()->getRoom()->hasFloor() )
                        {
                                isGone = true;
                        }
                        // is it time to lower by one unit
                        else if ( fallTimer->getValue() > freeItem.getWeight() )
                        {
                                if ( ! activities::Falling::getInstance().fall( this ) )
                                {
                                        SoundManager::getInstance().play( freeItem.getKind (), "fall" );
                                        activity = activities::Activity::Wait ;
                                }

                                fallTimer->reset();
                        }
                        break;

                case activities::Activity::Freeze:
                        freeItem.setFrozen( true );
                        break;

                case activities::Activity::WakeUp:
                        freeItem.setFrozen( false );
                        activity = activities::Activity::Wait;
                        break;

                default:
                        ;
        }

        return isGone;
}

void Turn::begin()
{
        switch ( Way( item->getOrientation() ).getIntegerOfWay () )
        {
                case Way::North:
                        activity = activities::Activity::MoveNorth;
                        break;

                case Way::South:
                        activity = activities::Activity::MoveSouth;
                        break;

                case Way::East:
                        activity = activities::Activity::MoveEast;
                        break;

                case Way::West:
                        activity = activities::Activity::MoveWest;
                        break;

                default:
                        ;
        }
}

void Turn::turn()
{
        bool turnLeft = ( getNameOfBehavior() == "behavior of move then turn left and move" );

        switch ( Way( item->getOrientation() ).getIntegerOfWay () )
        {
                case Way::North:
                        if ( turnLeft ) {
                                activity = activities::Activity::MoveWest ;
                                item->changeOrientation( "west" );
                        } else {
                                activity = activities::Activity::MoveEast ;
                                item->changeOrientation( "east" );
                        }
                        break;

                case Way::South:
                        if ( turnLeft ) {
                                activity = activities::Activity::MoveEast ;
                                item->changeOrientation( "east" );
                        } else {
                                activity = activities::Activity::MoveWest ;
                                item->changeOrientation( "west" );
                        }
                        break;

                case Way::East:
                        if ( turnLeft ) {
                                activity = activities::Activity::MoveNorth ;
                                item->changeOrientation( "north" );
                        } else {
                                activity = activities::Activity::MoveSouth ;
                                item->changeOrientation( "south" );
                        }
                        break;

                case Way::West:
                        if ( turnLeft ) {
                                activity = activities::Activity::MoveSouth ;
                                item->changeOrientation( "south" );
                        } else {
                                activity = activities::Activity::MoveNorth ;
                                item->changeOrientation( "north" );
                        }
                        break;

                default:
                        ;
        }
}

}
