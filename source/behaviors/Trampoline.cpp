
#include "Trampoline.hpp"

#include "Item.hpp"
#include "FreeItem.hpp"
#include "AvatarItem.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"


namespace behaviors
{

Trampoline::Trampoline( const ItemPtr & item, const std::string & behavior )
        : Behavior( item, behavior )
        , folded( false )
        , rebounding( false )
        , plainFrame( 0 )
        , foldedFrame( 1 )
        , speedTimer( new Timer() )
        , fallTimer( new Timer() )
        , reboundTimer( new Timer() )
{
        speedTimer->go();
        fallTimer->go();
        reboundTimer->go();
}

Trampoline::~Trampoline()
{
}

bool Trampoline::update ()
{
        FreeItem& freeItem = dynamic_cast< FreeItem& >( * this->item );
        bool vanish = false;

        switch ( activity )
        {
                case activities::Activity::Waiting:
                        // fold trampoline when there are items on top of it
                        if ( ! freeItem.canAdvanceTo( 0, 0, 1 ) )
                        {
                                folded = true;
                                rebounding = false;
                                freeItem.changeFrame( foldedFrame );
                        }
                        else
                        {
                                // the spring continues to bounce after unloading
                                if ( rebounding && reboundTimer->getValue() < 0.600 )
                                {
                                        freeItem.animate();

                                        // play the sound of bouncing
                                        if ( reboundTimer->getValue() > 0.100 )
                                        {
                                                SoundManager::getInstance().play( freeItem.getKind (), "function" );
                                        }
                                }
                                else
                                {
                                        // begin bouncing when item on top moves away
                                        if ( folded )
                                        {
                                                rebounding = true;
                                                reboundTimer->reset();
                                        }

                                        // it is no longer folded
                                        folded = false;

                                        freeItem.changeFrame( plainFrame );
                                }
                        }

                        // look if it falls down
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
                case activities::Activity::PushedNorthwest:
                case activities::Activity::PushedSoutheast:
                case activities::Activity::PushedSouthwest:
                        // is it time to move
                        if ( speedTimer->getValue() > freeItem.getSpeed() )
                        {
                                // play the sound of displacing
                                SoundManager::getInstance().play( freeItem.getKind (), "push" );

                                this->setActivityOfItem( activity );
                                activities::Displacing::getInstance().displace( this, &activity, true );

                                if ( activity != activities::Activity::Fall )
                                {
                                        activity = activities::Activity::Waiting;
                                }

                                speedTimer->reset();
                        }
                        break;

                case activities::Activity::Fall:
                        // look for reaching floor in a room without floor
                        if ( item->getZ() == 0 && ! item->getMediator()->getRoom()->hasFloor() )
                        {
                                // item disappears
                                vanish = true;
                        }
                        // is it time to lower by one unit
                        else if ( fallTimer->getValue() > freeItem.getWeight() )
                        {
                                // item falls
                                this->setActivityOfItem( activity );
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
                        vanish = true;
                        break;

                default:
                        ;
        }

        return vanish;
}

}
