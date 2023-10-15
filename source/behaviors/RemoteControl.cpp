
#include "RemoteControl.hpp"

#include "MoveKindOfActivity.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"


namespace iso
{

RemoteControl::RemoteControl( const ItemPtr & item, const std::string & behavior )
        : Behavior( item, behavior )
        , controlledItem ()
        , speedTimer( new Timer() )
        , fallTimer( new Timer() )
{
        activity = Activity::Wait;

        // move controlled one but not controller
        if ( getNameOfBehavior() == "behavior of remotely controlled one" )
        {
                speedTimer->go();
                fallTimer->go();
        }
}

RemoteControl::~RemoteControl()
{
}

bool RemoteControl::update ()
{
        FreeItem& freeItem = dynamic_cast< FreeItem& >( * this->item );
        bool vanish = false;

        // get controlled item
        if ( getNameOfBehavior() == "behavior of remote control" && controlledItem == nilPointer )
        {
                controlledItem = freeItem.getMediator()->findItemByBehavior( "behavior of remotely controlled one" );
        }

        switch ( activity )
        {
                case Activity::Wait:
                        break;

                case Activity::MoveNorth:
                case Activity::MoveSouth:
                case Activity::MoveEast:
                case Activity::MoveWest:
                        if ( getNameOfBehavior() == "behavior of remotely controlled one" )
                        {
                                if ( speedTimer->getValue() > freeItem.getSpeed() )
                                {
                                        // move item
                                        MoveKindOfActivity::getInstance().move( this, &activity, true );

                                        if ( activity != Activity::Fall )
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
                        if ( getNameOfBehavior() == "behavior of remotely controlled one" )
                        {
                                if ( speedTimer->getValue() > freeItem.getSpeed() )
                                {
                                        // emit sound of displacement if item is pushed but not displaced by item below it
                                        if ( this->sender == nilPointer || this->sender != this->item )
                                        {
                                                SoundManager::getInstance().play( freeItem.getLabel(), activity );
                                        }

                                        DisplaceKindOfActivity::getInstance().displace( this, &activity, true );

                                        if ( activity != Activity::Fall )
                                        {
                                                activity = Activity::Wait;
                                        }

                                        speedTimer->reset();
                                }

                                freeItem.animate();
                        }

                        // controller changes movement of controlled item
                        if ( activity == Activity::DisplaceNorth || activity == Activity::DisplaceSouth ||
                                activity == Activity::DisplaceEast || activity == Activity::DisplaceWest )
                        {
                                if ( getNameOfBehavior() == "behavior of remote control" )
                                {
                                        ActivityOfItem motionActivity = Activity::Wait;

                                        switch ( activity )
                                        {
                                                case Activity::DisplaceNorth:
                                                        motionActivity = Activity::MoveNorth;
                                                        break;

                                                case Activity::DisplaceSouth:
                                                        motionActivity = Activity::MoveSouth;
                                                        break;

                                                case Activity::DisplaceEast:
                                                        motionActivity = Activity::MoveEast;
                                                        break;

                                                case Activity::DisplaceWest:
                                                        motionActivity = Activity::MoveWest;
                                                        break;

                                                default:
                                                        ;
                                        }

                                        dynamic_cast< RemoteControl * >( controlledItem->getBehavior().get () )->changeActivityOfItem( motionActivity );
                                        activity = Activity::Wait;
                                }
                        }
                        break;

                case Activity::Fall:
                        // look for reaching floor in a room without floor
                        if ( freeItem.getZ() == 0 && ! freeItem.getMediator()->getRoom()->hasFloor() )
                        {
                                // item disappears
                                vanish = true;
                        }
                        // is it time to fall for controlled item
                        else if ( getNameOfBehavior() == "behavior of remotely controlled one" && fallTimer->getValue() > freeItem.getWeight() )
                        {
                                if ( ! FallKindOfActivity::getInstance().fall( this ) )
                                {
                                        // play sound of falling down
                                        SoundManager::getInstance().play( freeItem.getLabel(), activity );
                                        activity = Activity::Wait;
                                }

                                fallTimer->reset();
                        }
                        break;

                default:
                        ;
        }

        return vanish;
}

}
