
#include "Detector.hpp"

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
        AvatarItemPtr activeCharacter = freeItem.getMediator()->getActiveCharacter();
        bool isGone = false;

        if ( activeCharacter != nilPointer )
        {
                switch ( activity )
                {
                        case activities::Activity::Wait:
                                // the character meets the detector on the X way
                                if ( freeItem.getX() >= activeCharacter->getX() - 1 && freeItem.getX() <= activeCharacter->getX() + 1 )
                                {
                                        if ( activeCharacter->getY() <= freeItem.getY() )
                                        {
                                                changeActivityOfItem( activities::Activity::MoveEast );
                                        }
                                        else if ( activeCharacter->getY() >= freeItem.getY() )
                                        {
                                                changeActivityOfItem( activities::Activity::MoveWest );
                                        }
                                }
                                // the character meets the detector on the Y way
                                else if ( freeItem.getY() >= activeCharacter->getY() - 1 && freeItem.getY() <= activeCharacter->getY() + 1 )
                                {
                                        if ( activeCharacter->getX() <= freeItem.getX() )
                                        {
                                                changeActivityOfItem( activities::Activity::MoveNorth );
                                        }
                                        else
                                        if ( activeCharacter->getX() >= freeItem.getX() )
                                        {
                                                changeActivityOfItem( activities::Activity::MoveSouth );
                                        }
                                }

                                // play sound on change of activity
                                if ( activity != activities::Activity::Wait )
                                {
                                        SoundManager::getInstance().play( freeItem.getLabel(), activity );
                                }
                                break;

                        case activities::Activity::MoveNorth:
                        case activities::Activity::MoveSouth:
                        case activities::Activity::MoveEast:
                        case activities::Activity::MoveWest:
                                // is item active
                                if ( ! freeItem.isFrozen() )
                                {
                                        // is it time to move
                                        if ( speedTimer->getValue() > freeItem.getSpeed() )
                                        {
                                                // move item, if there’s collision let’s wait
                                                if ( ! activities::Moving::getInstance().move( this, &activity, true ) )
                                                {
                                                        activity = activities::Activity::Wait;
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
                        case activities::Activity::DisplaceNorthwest:
                        case activities::Activity::DisplaceSoutheast:
                        case activities::Activity::DisplaceSouthwest:
                                // is it time to move
                                if ( speedTimer->getValue() > freeItem.getSpeed() )
                                {
                                        if ( ! activities::Displacing::getInstance().displace( this, &activity, true ) )
                                        {
                                                activity = activities::Activity::Wait;
                                        }

                                        speedTimer->reset();
                                }

                                // preserve inactivity for inactive item
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
                                // is it time to fall
                                else if ( fallTimer->getValue() > freeItem.getWeight() )
                                {
                                        if ( ! activities::Falling::getInstance().fall( this ) )
                                        {
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
        }

        return isGone;
}

}
