
#include "Driven.hpp"

#include "Item.hpp"
#include "FreeItem.hpp"
#include "AvatarItem.hpp"
#include "Moving.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"


namespace behaviors
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
        bool characterFound = false;

        switch ( activity )
        {
                case activities::Activity::Waiting:
                        if ( moving )
                        {
                                switch ( Way( freeItem.getOrientation() ).getIntegerOfWay() )
                                {
                                        case Way::North:
                                                setCurrentActivity( activities::Activity::MoveNorth );
                                                break;

                                        case Way::South:
                                                setCurrentActivity( activities::Activity::MoveSouth );
                                                break;

                                        case Way::East:
                                                setCurrentActivity( activities::Activity::MoveEast );
                                                break;

                                        case Way::West:
                                                setCurrentActivity( activities::Activity::MoveWest );
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
                                        while ( ! mediator->isStackOfCollisionsEmpty() && ! characterFound )
                                        {
                                                ItemPtr item = mediator->findCollisionPop ();

                                                if ( item->whichItemClass() == "avatar item" )
                                                {
                                                        characterFound = true ;
                                                        moving = true ;

                                                        switch ( Way( item->getOrientation() ).getIntegerOfWay () )
                                                        {
                                                                case Way::North:
                                                                        setCurrentActivity( activities::Activity::MoveNorth );
                                                                        break;

                                                                case Way::South:
                                                                        setCurrentActivity( activities::Activity::MoveSouth );
                                                                        break;

                                                                case Way::East:
                                                                        setCurrentActivity( activities::Activity::MoveEast );
                                                                        break;

                                                                case Way::West:
                                                                        setCurrentActivity( activities::Activity::MoveWest );
                                                                        break;

                                                                default:
                                                                        ;
                                                        }
                                                }
                                        }
                                }
                        }
                        break;

                case activities::Activity::MoveNorth:
                case activities::Activity::MoveSouth:
                case activities::Activity::MoveEast:
                case activities::Activity::MoveWest:
                        // item is active and it is time to move
                        if ( ! freeItem.isFrozen() )
                        {
                                if ( speedTimer->getValue() > freeItem.getSpeed() )
                                {
                                        if ( ! activities::Moving::getInstance().move( this, &activity, true ) )
                                        {
                                                moving = false;
                                                activity = activities::Activity::Waiting;

                                                SoundManager::getInstance().play( freeItem.getKind(), "collision" );
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
                case activities::Activity::PushedNorthwest:
                case activities::Activity::PushedSoutheast:
                case activities::Activity::PushedSouthwest:
                        // is it time to move
                        if ( speedTimer->getValue() > freeItem.getSpeed() )
                        {
                                if ( ! activities::Displacing::getInstance().displace( this, &activity, true ) )
                                {
                                        activity = activities::Activity::Waiting;
                                }

                                speedTimer->reset();
                        }

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
                                // item disappears
                                isGone = true;
                        }
                        // is it time to fall
                        else if ( fallTimer->getValue() > freeItem.getWeight() )
                        {
                                if ( ! activities::Falling::getInstance().fall( this ) )
                                {
                                        activity = activities::Activity::Waiting;
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

}
