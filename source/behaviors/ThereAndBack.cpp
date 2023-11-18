
#include "ThereAndBack.hpp"

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
                case activities::Activity::Wait:
                        letsMove();
                        break;

                case activities::Activity::MoveNorth:
                case activities::Activity::MoveSouth:
                case activities::Activity::MoveEast:
                case activities::Activity::MoveWest:
                        if ( ! freeItem.isFrozen() )
                        {
                                if ( speedTimer->getValue() > freeItem.getSpeed() )
                                {
                                        // move it
                                        if ( ! activities::Moving::getInstance().move( this, &activity, true ) )
                                        {
                                                turnRound();

                                                // play the sound of collision
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
                        if ( ! this->isFlying )
                        {
                                // emit the sound of displacing
                                SoundManager::getInstance().play( freeItem.getKind (), "push" );

                                // displace this item by other one
                                activities::Displacing::getInstance().displace( this, &activity, true );

                                activity = activities::Activity::Wait;

                                // preserve inactivity for frozen item
                                if ( freeItem.isFrozen() )
                                        activity = activities::Activity::Freeze;
                        }
                        else
                        {
                                activity = activities::Activity::Wait;
                        }
                        break;

                case activities::Activity::Fall:
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
                                        if ( ! activities::Falling::getInstance().fall( this ) )
                                        {
                                                // emit the sound of falling
                                                SoundManager::getInstance().play( freeItem.getKind (), "fall" );
                                                activity = activities::Activity::Wait;
                                        }

                                        fallTimer->reset();
                                }
                        }
                        else
                        {
                                activity = activities::Activity::Wait;
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

        return vanish;
}

void ThereAndBack::letsMove()
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

void ThereAndBack::turnRound()
{
        switch ( Way( item->getOrientation() ).getIntegerOfWay () )
        {
                case Way::North:
                        activity = activities::Activity::MoveSouth ;
                        item->changeOrientation( "south" );
                        break;

                case Way::South:
                        activity = activities::Activity::MoveNorth ;
                        item->changeOrientation( "north" );
                        break;

                case Way::East:
                        activity = activities::Activity::MoveWest ;
                        item->changeOrientation( "west" );
                        break;

                case Way::West:
                        activity = activities::Activity::MoveEast ;
                        item->changeOrientation( "east" );
                        break;

                default:
                        ;
        }
}

}
