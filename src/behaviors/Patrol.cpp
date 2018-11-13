
#include "Patrol.hpp"
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
                case Activity::Wait:
                        changeOrientation();
                        break;

                case Activity::MoveNorth:
                case Activity::MoveSouth:
                case Activity::MoveEast:
                case Activity::MoveWest:
                case Activity::MoveNortheast:
                case Activity::MoveNorthwest:
                case Activity::MoveSoutheast:
                case Activity::MoveSouthwest:
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
                                        if ( ! MoveKindOfActivity::getInstance().move( this, &activity, true ) )
                                        {
                                                changeOrientation();

                                                SoundManager::getInstance().play( freeItem.getLabel(), Activity::Collision );
                                        }

                                        // play sound of moving
                                        SoundManager::getInstance().play( freeItem.getLabel(), activity );

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
                        // play sound of displacing
                        SoundManager::getInstance().play( freeItem.getLabel(), activity );

                        // displace this item by some other one
                        DisplaceKindOfActivity::getInstance().displace( this, &activity, true );

                        activity = Activity::Wait;

                        // preserve inactivity for frozen item
                        if ( freeItem.isFrozen() )
                        {
                                activity = Activity::Freeze;
                        }
                        break;

                case Activity::Fall:
                        // look for reaching floor in a room without floor
                        if ( item->getZ() == 0 && ! item->getMediator()->getRoom()->hasFloor() )
                        {
                                isGone = true;
                        }
                        // is it time to fall
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

                case Way::Northeast:
                        activity = Activity::MoveNortheast;
                        break;

                case Way::Northwest:
                        activity = Activity::MoveNorthwest;
                        break;

                case Way::Southeast:
                        activity = Activity::MoveSoutheast;
                        break;

                case Way::Southwest:
                        activity = Activity::MoveSouthwest;
                        break;

                default:
                        ;
        }
}

}
