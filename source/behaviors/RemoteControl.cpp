
#include "RemoteControl.hpp"

#include "Moving.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"


namespace behaviors
{

RemoteControl::RemoteControl( const ItemPtr & item, const std::string & behavior )
        : Behavior( item, behavior )
        , controlledItem ()
        , speedTimer( new Timer() )
        , fallTimer( new Timer() )
{
        activity = activities::Activity::Wait;

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
                case activities::Activity::Wait:
                        break;

                case activities::Activity::MoveNorth:
                case activities::Activity::MoveSouth:
                case activities::Activity::MoveEast:
                case activities::Activity::MoveWest:
                        if ( getNameOfBehavior() == "behavior of remotely controlled one" )
                        {
                                if ( speedTimer->getValue() > freeItem.getSpeed() )
                                {
                                        // move item
                                        activities::Moving::getInstance().move( this, &activity, true );

                                        if ( activity != activities::Activity::Fall )
                                        {
                                                activity = activities::Activity::Wait;
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
                        if ( getNameOfBehavior() == "behavior of remotely controlled one" )
                        {
                                if ( speedTimer->getValue() > freeItem.getSpeed() )
                                {
                                        // emit the sound of pushing if the item isn't displaced by another item below it
                                        if ( this->sender == nilPointer || this->sender != this->item )
                                        {
                                                SoundManager::getInstance().play( freeItem.getKind (), "push" );
                                        }

                                        activities::Displacing::getInstance().displace( this, &activity, true );

                                        if ( activity != activities::Activity::Fall )
                                        {
                                                activity = activities::Activity::Wait;
                                        }

                                        speedTimer->reset();
                                }

                                freeItem.animate();
                        }

                        // controller changes movement of controlled item
                        if ( activity == activities::Activity::PushedNorth || activity == activities::Activity::PushedSouth ||
                                activity == activities::Activity::PushedEast || activity == activities::Activity::PushedWest )
                        {
                                if ( getNameOfBehavior() == "behavior of remote control" )
                                {
                                        ActivityOfItem motionActivity = activities::Activity::Wait;

                                        switch ( activity )
                                        {
                                                case activities::Activity::PushedNorth:
                                                        motionActivity = activities::Activity::MoveNorth;
                                                        break;

                                                case activities::Activity::PushedSouth:
                                                        motionActivity = activities::Activity::MoveSouth;
                                                        break;

                                                case activities::Activity::PushedEast:
                                                        motionActivity = activities::Activity::MoveEast;
                                                        break;

                                                case activities::Activity::PushedWest:
                                                        motionActivity = activities::Activity::MoveWest;
                                                        break;

                                                default:
                                                        ;
                                        }

                                        dynamic_cast< RemoteControl * >( controlledItem->getBehavior().get () )->changeActivityOfItem( motionActivity );
                                        activity = activities::Activity::Wait;
                                }
                        }
                        break;

                case activities::Activity::Fall:
                        // look for reaching floor in a room without floor
                        if ( freeItem.getZ() == 0 && ! freeItem.getMediator()->getRoom()->hasFloor() )
                        {
                                // item disappears
                                vanish = true;
                        }
                        // is it time to fall for controlled item
                        else if ( getNameOfBehavior() == "behavior of remotely controlled one" && fallTimer->getValue() > freeItem.getWeight() )
                        {
                                if ( ! activities::Falling::getInstance().fall( this ) )
                                {
                                        // play the sound of falling down
                                        SoundManager::getInstance().play( freeItem.getKind (), "fall" );
                                        activity = activities::Activity::Wait;
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
