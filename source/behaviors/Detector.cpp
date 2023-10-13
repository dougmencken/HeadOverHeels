
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


namespace iso
{

Detector::Detector( const ItemPtr & item, const std::string & behavior )
        : Behavior( item, behavior )
        , speedTimer( new Timer() )
        , fallTimer( new Timer() )
{
        speedTimer->go();
        fallTimer->go();
}

Detector::~Detector( )
{
}

bool Detector::update ()
{
        FreeItem& freeItem = dynamic_cast< FreeItem& >( * this->item );
        PlayerItemPtr activeCharacter = freeItem.getMediator()->getActiveCharacter();
        bool isGone = false;

        if ( activeCharacter != nilPointer )
        {
                switch ( activity )
                {
                        case Activity::Wait:
                                // player meets detector on X way
                                if ( freeItem.getX() >= activeCharacter->getX() - 1 && freeItem.getX() <= activeCharacter->getX() + 1 )
                                {
                                        if ( activeCharacter->getY() <= freeItem.getY() )
                                        {
                                                changeActivityOfItem( Activity::MoveEast );
                                        }
                                        else if ( activeCharacter->getY() >= freeItem.getY() )
                                        {
                                                changeActivityOfItem( Activity::MoveWest );
                                        }
                                }
                                // player meets detector on Y way
                                else if ( freeItem.getY() >= activeCharacter->getY() - 1 && freeItem.getY() <= activeCharacter->getY() + 1 )
                                {
                                        if ( activeCharacter->getX() <= freeItem.getX() )
                                        {
                                                changeActivityOfItem( Activity::MoveNorth );
                                        }
                                        else
                                        if ( activeCharacter->getX() >= freeItem.getX() )
                                        {
                                                changeActivityOfItem( Activity::MoveSouth );
                                        }
                                }

                                // play sound on change of activity
                                if ( activity != Activity::Wait )
                                {
                                        SoundManager::getInstance().play( freeItem.getLabel(), activity );
                                }
                                break;

                        case Activity::MoveNorth:
                        case Activity::MoveSouth:
                        case Activity::MoveEast:
                        case Activity::MoveWest:
                                // is item active
                                if ( ! freeItem.isFrozen() )
                                {
                                        // is it time to move
                                        if ( speedTimer->getValue() > freeItem.getSpeed() )
                                        {
                                                // move item, if there’s collision let’s wait
                                                if ( ! MoveKindOfActivity::getInstance().move( this, &activity, true ) )
                                                {
                                                        activity = Activity::Wait;
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
                        case Activity::DisplaceNorthwest:
                        case Activity::DisplaceSoutheast:
                        case Activity::DisplaceSouthwest:
                                // is it time to move
                                if ( speedTimer->getValue() > freeItem.getSpeed() )
                                {
                                        if ( ! DisplaceKindOfActivity::getInstance().displace( this, &activity, true ) )
                                        {
                                                activity = Activity::Wait;
                                        }

                                        speedTimer->reset();
                                }

                                // preserve inactivity for inactive item
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
                                // is it time to fall
                                else if ( fallTimer->getValue() > freeItem.getWeight() )
                                {
                                        if ( ! FallKindOfActivity::getInstance().fall( this ) )
                                        {
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
        }

        return isGone;
}

}
