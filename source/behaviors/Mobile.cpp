
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
                case activities::Activity::Waiting:
                        // see if the item falls yet
                        if ( activities::Falling::getInstance().fall( this ) )
                        {
                                fallTimer->reset();
                                activity = activities::Activity::Fall;
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
                        // is it time to move
                        if ( speedTimer->getValue() > freeItem.getSpeed() )
                        {
                                // if the item isn't dragged
                                if ( this->affectedBy == nilPointer || this->affectedBy != this->item )
                                {       // emit the sound of pushing
                                        SoundManager::getInstance().play( freeItem.getKind (), "push" );
                                }

                                this->setCurrentActivity( activity );
                                activities::Displacing::getInstance().displace( this, &activity, true );

                                activity = activities::Activity::Waiting;

                                speedTimer->reset();
                        }

                        freeItem.animate();
                        break;

                case activities::Activity::DraggedNorth:
                case activities::Activity::DraggedSouth:
                case activities::Activity::DraggedEast:
                case activities::Activity::DraggedWest:
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
                                this->setCurrentActivity( activity );
                                if ( ! activities::Falling::getInstance().fall( this ) )
                                {
                                        // play the sound of falling
                                        SoundManager::getInstance().play( freeItem.getKind (), "fall" );
                                        activity = activities::Activity::Waiting;
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
