
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
                case activities::Activity::Waiting:
                        begin();
                        break;

                case activities::Activity::MovingNorth:
                case activities::Activity::MovingSouth:
                case activities::Activity::MovingEast:
                case activities::Activity::MovingWest:
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

                case activities::Activity::PushedNorth:
                case activities::Activity::PushedSouth:
                case activities::Activity::PushedEast:
                case activities::Activity::PushedWest:
                case activities::Activity::PushedNortheast:
                case activities::Activity::PushedSoutheast:
                case activities::Activity::PushedSouthwest:
                case activities::Activity::PushedNorthwest:
                        SoundManager::getInstance().play( freeItem.getKind (), "push" );

                        activities::Displacing::getInstance().displace( this, &activity, true );

                        activity = activities::Activity::Waiting;

                        // inactive item continues to be inactive
                        if ( freeItem.isFrozen() )
                        {
                                activity = activities::Activity::Freeze;
                        }
                        break;

                case activities::Activity::Falling:
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
                                        activity = activities::Activity::Waiting ;
                                }

                                fallTimer->reset();
                        }
                        break;

                case activities::Activity::Freeze:
                        freeItem.setFrozen( true );
                        break;

                case activities::Activity::WakeUp:
                        freeItem.setFrozen( false );
                        activity = activities::Activity::Waiting;
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
                        activity = activities::Activity::MovingNorth;
                        break;

                case Way::South:
                        activity = activities::Activity::MovingSouth;
                        break;

                case Way::East:
                        activity = activities::Activity::MovingEast;
                        break;

                case Way::West:
                        activity = activities::Activity::MovingWest;
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
                                activity = activities::Activity::MovingWest ;
                                item->changeOrientation( "west" );
                        } else {
                                activity = activities::Activity::MovingEast ;
                                item->changeOrientation( "east" );
                        }
                        break;

                case Way::South:
                        if ( turnLeft ) {
                                activity = activities::Activity::MovingEast ;
                                item->changeOrientation( "east" );
                        } else {
                                activity = activities::Activity::MovingWest ;
                                item->changeOrientation( "west" );
                        }
                        break;

                case Way::East:
                        if ( turnLeft ) {
                                activity = activities::Activity::MovingNorth ;
                                item->changeOrientation( "north" );
                        } else {
                                activity = activities::Activity::MovingSouth ;
                                item->changeOrientation( "south" );
                        }
                        break;

                case Way::West:
                        if ( turnLeft ) {
                                activity = activities::Activity::MovingSouth ;
                                item->changeOrientation( "south" );
                        } else {
                                activity = activities::Activity::MovingNorth ;
                                item->changeOrientation( "north" );
                        }
                        break;

                default:
                        ;
        }
}

}
