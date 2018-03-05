
#include "Patrol.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "MoveKindOfActivity.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"


namespace isomot
{

Patrol::Patrol( Item * item, const std::string & behavior ) :
        Behavior( item, behavior )
{
        speedTimer = new Timer();
        fallTimer = new Timer();
        changeTimer = new Timer();

        speedTimer->go ();
        fallTimer->go ();
        changeTimer->go ();
}

Patrol::~Patrol()
{
        delete speedTimer;
        delete fallTimer;
        delete changeTimer;
}

bool Patrol::update ()
{
        FreeItem * freeItem = dynamic_cast< FreeItem * >( this->item );
        bool isGone = false;

        switch ( activity )
        {
                case Wait:
                        changeOrientation();
                        break;

                case MoveNorth:
                case MoveSouth:
                case MoveEast:
                case MoveWest:
                case MoveNortheast:
                case MoveNorthwest:
                case MoveSoutheast:
                case MoveSouthwest:
                        if ( ! freeItem->isFrozen() )
                        {
                                if ( speedTimer->getValue() > freeItem->getSpeed())
                                {
                                        // ¿ cambio de dirección ?
                                        if ( changeTimer->getValue() > ( double( rand() % 1000 ) + 400.0 ) / 1000.0 )
                                        {
                                                changeOrientation();
                                                changeTimer->reset();
                                        }

                                        // move item
                                        if ( ! MoveKindOfActivity::getInstance()->move( this, &activity, true ) )
                                        {
                                                changeOrientation();

                                                SoundManager::getInstance()->play( freeItem->getLabel(), Collision );
                                        }

                                        // play sound of moving
                                        SoundManager::getInstance()->play( freeItem->getLabel(), activity );

                                        speedTimer->reset();
                                }

                                freeItem->animate();
                        }
                        break;

                case DisplaceNorth:
                case DisplaceSouth:
                case DisplaceEast:
                case DisplaceWest:
                case DisplaceNortheast:
                case DisplaceSoutheast:
                case DisplaceSouthwest:
                case DisplaceNorthwest:
                        // play sound of displacing
                        SoundManager::getInstance()->play( freeItem->getLabel(), activity );

                        // displace this item by some other one
                        DisplaceKindOfActivity::getInstance()->displace( this, &activity, true );

                        activity = Wait;

                        // preserve inactivity for frozen item
                        if ( freeItem->isFrozen() )
                        {
                                activity = Freeze;
                        }
                        break;

                case Fall:
                        // look for reaching floor in a room without floor
                        if ( item->getZ() == 0 && item->getMediator()->getRoom()->getKindOfFloor() == "none" )
                        {
                                isGone = true;
                        }
                        // is it time to fall
                        else if ( fallTimer->getValue() > freeItem->getWeight() )
                        {
                                if ( ! FallKindOfActivity::getInstance()->fall( this ) )
                                {
                                        SoundManager::getInstance()->play( freeItem->getLabel(), activity );
                                        activity = Wait;
                                }

                                fallTimer->reset();
                        }
                        break;

                case Freeze:
                        freeItem->setFrozen( true );
                        break;

                case WakeUp:
                        freeItem->setFrozen( false );
                        activity = Wait;
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
                case North:
                        activity = MoveNorth;
                        break;

                case South:
                        activity = MoveSouth;
                        break;

                case East:
                        activity = MoveEast;
                        break;

                case West:
                        activity = MoveWest;
                        break;

                case Northeast:
                        activity = MoveNortheast;
                        break;

                case Northwest:
                        activity = MoveNorthwest;
                        break;

                case Southeast:
                        activity = MoveSoutheast;
                        break;

                case Southwest:
                        activity = MoveSouthwest;
                        break;

                default:
                        ;
        }
}

}
