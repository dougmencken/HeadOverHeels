
#include "Mobile.hpp"

#include "Item.hpp"
#include "FreeItem.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"
#include "SoundManager.hpp"


namespace behaviors
{

Mobile::Mobile( const ItemPtr & item, const std::string & behavior )
        : Behavior( item, behavior )
        , speedTimer( new Timer() )
        , fallTimer( new Timer() )
{
        speedTimer->go ();
        fallTimer->go ();
}

Mobile::~Mobile()
{
}

bool Mobile::update ()
{
        FreeItem& freeItem = dynamic_cast< FreeItem& >( * this->item );
        bool isGone = false;

        switch ( activity )
        {
                case activities::Activity::Wait:
                        // see if the item falls yet
                        if ( activities::Falling::getInstance().fall( this ) )
                        {
                                fallTimer->reset();
                                activity = activities::Activity::Fall;
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
                        // is it time to move
                        if ( speedTimer->getValue() > freeItem.getSpeed() )
                        {
                                // emit sound of displacement if item is pushed but not displaced by item below it
                                if ( this->sender == nilPointer || this->sender != this->item )
                                {
                                        SoundManager::getInstance().play( freeItem.getLabel(), activity );
                                }

                                this->changeActivityOfItem( activity );
                                activities::Displacing::getInstance().displace( this, &activity, true );

                                activity = activities::Activity::Wait;

                                speedTimer->reset();
                        }

                        freeItem.animate();
                        break;

                case activities::Activity::ForceDisplaceNorth:
                case activities::Activity::ForceDisplaceSouth:
                case activities::Activity::ForceDisplaceEast:
                case activities::Activity::ForceDisplaceWest:
                        // item is on conveyor
                        if ( speedTimer->getValue() > item->getSpeed() )
                        {
                                activities::Displacing::getInstance().displace( this, &activity, true );

                                activity = activities::Activity::Fall;

                                speedTimer->reset();
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
                                this->changeActivityOfItem( activity );
                                if ( ! activities::Falling::getInstance().fall( this ) )
                                {
                                        // play sound of falling
                                        SoundManager::getInstance().play( freeItem.getLabel(), activity );
                                        activity = activities::Activity::Wait;
                                }

                                fallTimer->reset();
                        }
                        break;

                case activities::Activity::Vanish:
                        // disappear when this item is caught
                        isGone = true;
                        break;

                default:
                        ;
        }

        return isGone;
}

}
