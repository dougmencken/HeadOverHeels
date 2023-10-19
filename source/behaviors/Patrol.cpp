
#include "Patrol.hpp"

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

Patrol::Patrol( const ItemPtr & item, const std::string & behavior )
        : Behavior( item, behavior )
        , speedTimer( new Timer() )
        , fallTimer( new Timer() )
        , changeTimer( new Timer() )
{
        speedTimer->go ();
        fallTimer->go ();
        changeTimer->go ();
}

Patrol::~Patrol()
{
}

bool Patrol::update ()
{
        FreeItem& freeItem = dynamic_cast< FreeItem& >( * this->item );
        bool isGone = false;

        switch ( activity )
        {
                case activities::Activity::Wait:
                        changeOrientation();
                        break;

                case activities::Activity::MoveNorth:
                case activities::Activity::MoveSouth:
                case activities::Activity::MoveEast:
                case activities::Activity::MoveWest:
                case activities::Activity::MoveNortheast:
                case activities::Activity::MoveNorthwest:
                case activities::Activity::MoveSoutheast:
                case activities::Activity::MoveSouthwest:
                        if ( ! freeItem.isFrozen() )
                        {
                                if ( speedTimer->getValue() > freeItem.getSpeed())
                                {
                                        // ¿ cambio de dirección ?
                                        if ( changeTimer->getValue() > ( double( rand() % 1000 ) + 400.0 ) / 1000.0 )
                                        {
                                                changeOrientation();
                                                changeTimer->reset();
                                        }

                                        // move item
                                        if ( ! activities::Moving::getInstance().move( this, &activity, true ) )
                                        {
                                                changeOrientation();

                                                SoundManager::getInstance().play( freeItem.getLabel(), activities::Activity::Collision );
                                        }

                                        // play sound of moving
                                        SoundManager::getInstance().play( freeItem.getLabel(), activity );

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
                        // play sound of displacing
                        SoundManager::getInstance().play( freeItem.getLabel(), activity );

                        // displace this item by some other one
                        activities::Displacing::getInstance().displace( this, &activity, true );

                        activity = activities::Activity::Wait;

                        // preserve inactivity for frozen item
                        if ( freeItem.isFrozen() )
                        {
                                activity = activities::Activity::Freeze;
                        }
                        break;

                case activities::Activity::Fall:
                        // look for reaching floor in a room without floor
                        if ( item->getZ() == 0 && ! item->getMediator()->getRoom()->hasFloor() )
                        {
                                isGone = true;
                        }
                        // is it time to fall
                        else if ( fallTimer->getValue() > freeItem.getWeight() )
                        {
                                if ( ! activities::Falling::getInstance().fall( this ) )
                                {
                                        SoundManager::getInstance().play( freeItem.getLabel(), activity );
                                        activity = activities::Activity::Wait;
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

        return isGone ;
}

void Patrol::changeOrientation()
{
        unsigned int orientation = 0;

        if ( getNameOfBehavior() == "behavior of random patroling in four primary directions" )
        {
                orientation = ( rand() % 4 );
        }
        else if ( getNameOfBehavior() == "behavior of random patroling in four secondary directions" )
        {
                orientation = ( rand() % 4 ) + 4;
        }
        else if ( getNameOfBehavior() == "behavior of random patroling in eight directions" )
        {
                orientation = ( rand() % 8 );
        }

        // change activity depending on value of calculated direction
        switch ( orientation )
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

                case Way::Northeast:
                        activity = activities::Activity::MoveNortheast;
                        break;

                case Way::Northwest:
                        activity = activities::Activity::MoveNorthwest;
                        break;

                case Way::Southeast:
                        activity = activities::Activity::MoveSoutheast;
                        break;

                case Way::Southwest:
                        activity = activities::Activity::MoveSouthwest;
                        break;

                default:
                        ;
        }
}

}
