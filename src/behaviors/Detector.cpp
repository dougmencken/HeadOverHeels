
#include "Detector.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "MoveKindOfActivity.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"


namespace isomot
{

Detector::Detector( Item * item, const std::string & behavior ) :
        Behavior( item, behavior )
{
        speedTimer = new Timer();
        speedTimer->go();

        fallTimer = new Timer();
        fallTimer->go();
}

Detector::~Detector( )
{
        delete speedTimer;
        delete fallTimer;
}

bool Detector::update ()
{
        FreeItem* freeItem = dynamic_cast< FreeItem * >( this->item );
        PlayerItem* playerItem = freeItem->getMediator()->getActivePlayer();
        bool isGone = false;

        if ( playerItem != nilPointer )
        {
                switch ( activity )
                {
                        case Wait:
                                // player meets detector on X way
                                if ( freeItem->getX() >= playerItem->getX() - 1 && freeItem->getX() <= playerItem->getX() + 1 )
                                {
                                        if ( playerItem->getY() <= freeItem->getY() )
                                        {
                                                changeActivityOfItem( MoveEast );
                                        }
                                        else if ( playerItem->getY() >= freeItem->getY() )
                                        {
                                                changeActivityOfItem( MoveWest );
                                        }
                                }
                                // player meets detector on Y way
                                else if ( freeItem->getY() >= playerItem->getY() - 1 && freeItem->getY() <= playerItem->getY() + 1 )
                                {
                                        if ( playerItem->getX() <= freeItem->getX() )
                                        {
                                                changeActivityOfItem( MoveNorth );
                                        }
                                        else
                                        if ( playerItem->getX() >= freeItem->getX() )
                                        {
                                                changeActivityOfItem( MoveSouth );
                                        }
                                }

                                // play sound on change of activity
                                if ( activity != Wait )
                                {
                                        SoundManager::getInstance()->play( freeItem->getLabel(), activity );
                                }
                                break;

                        case MoveNorth:
                        case MoveSouth:
                        case MoveEast:
                        case MoveWest:
                                // is item active
                                if ( ! freeItem->isFrozen() )
                                {
                                        // is it time to move
                                        if ( speedTimer->getValue() > freeItem->getSpeed() )
                                        {
                                                // move item, if there’s collision let’s wait
                                                if ( ! MoveKindOfActivity::getInstance()->move( this, &activity, true ) )
                                                {
                                                        activity = Wait;
                                                }

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
                        case DisplaceNorthwest:
                        case DisplaceSoutheast:
                        case DisplaceSouthwest:
                                // is it time to move
                                if ( speedTimer->getValue() > freeItem->getSpeed() )
                                {
                                        if ( ! DisplaceKindOfActivity::getInstance()->displace( this, &activity, true ) )
                                        {
                                                activity = Wait;
                                        }

                                        speedTimer->reset();
                                }

                                // preserve inactivity for inactive item
                                if ( freeItem->isFrozen() )
                                {
                                        activity = Freeze;
                                }
                                break;

                        case Fall:
                                // look for reaching floor in a room without floor
                                if ( freeItem->getZ() == 0 && freeItem->getMediator()->getRoom()->getKindOfFloor() == "none" )
                                {
                                        isGone = true;
                                }
                                // is it time to fall
                                else if ( fallTimer->getValue() > freeItem->getWeight() )
                                {
                                        if ( ! FallKindOfActivity::getInstance()->fall( this ) )
                                        {
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
        }

        return isGone;
}

}
