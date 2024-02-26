
#include "Hunter.hpp"

#include "Item.hpp"
#include "DescriptionOfItem.hpp"
#include "ItemDescriptions.hpp"
#include "FreeItem.hpp"
#include "AvatarItem.hpp"
#include "Moving.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"
#include "GameManager.hpp"


namespace behaviors
{

Hunter::Hunter( const ItemPtr & item, const std::string & behavior )
        : Behavior( item, behavior )
        , speedTimer( new Timer() )
{
        speedTimer->go ();
}

Hunter::~Hunter()
{
}

bool Hunter::update ()
{
        Mediator* mediator = this->item->getMediator();
        AvatarItemPtr whoToHunt = mediator->getActiveCharacter();

        bool alive = true;

        switch ( this->activity )
        {
                case activities::Activity::Waiting:
                        // if the hunter is not a waiting one, activate it just now
                        if ( getNameOfBehavior() == "behavior of hunter in four directions" ||
                                        getNameOfBehavior() == "behavior of hunter in eight directions" )
                        {
                                SoundManager::getInstance().play( this->item->getKind (), "wait" );
                                activity = updateDirection( activity );
                        }
                        // otherwise check if the character is within the defined rectangle near the hunter
                        else
                        {
                                const unsigned int sizeOfRectangleInTiles = 3 ;
                                const int coverage = mediator->getRoom()->getSizeOfOneTile() * sizeOfRectangleInTiles ;

                                if ( whoToHunt != nilPointer  &&
                                        whoToHunt->getX() > this->item->getX() - coverage  &&
                                        whoToHunt->getX() < this->item->getX() + this->item->getWidthX() + coverage  &&
                                        whoToHunt->getY() > this->item->getY() - coverage  &&
                                        whoToHunt->getY() < this->item->getY() + this->item->getWidthY() + coverage )
                                {
                                        activity = updateDirection( activity );
                                }

                                // an eight-directional waiting hunter emits the sound when it waits
                                if ( getNameOfBehavior() == "behavior of waiting hunter in eight directions" )
                                {
                                        SoundManager::getInstance().play( this->item->getKind (), "wait" );
                                }

                                // animate item while it waits
                                this->item->animate ();
                        }
                break;

                case activities::Activity::MovingNorth:
                case activities::Activity::MovingSouth:
                case activities::Activity::MovingEast:
                case activities::Activity::MovingWest:
                        // bin original item when full-bodied guard is created
                        if ( getNameOfBehavior() == "behavior of waiting hunter in four directions" && createFullBody () )
                        {
                                alive = false;
                        }
                        else if ( ! dynamic_cast< FreeItem& >( * this->item ).isFrozen() )
                        {
                                if ( speedTimer->getValue() > this->item->getSpeed() )
                                {
                                        // move item
                                        activities::Moving::getInstance().move( this, &activity, false );

                                        // reset timer to next cycle
                                        speedTimer->reset();

                                        // see if direction changes
                                        activity = updateDirection( activity );

                                        // fall if you have to
                                        if ( this->item->getWeight() > 0 )
                                        {
                                                activities::Falling::getInstance().fall( this );
                                        }
                                }

                                this->item->animate();

                                SoundManager::getInstance().play( this->item->getKind (), "move" );
                        }
                        break;

                case activities::Activity::MovingNortheast:
                case activities::Activity::MovingNorthwest:
                case activities::Activity::MovingSoutheast:
                case activities::Activity::MovingSouthwest:
                        if ( ! dynamic_cast< FreeItem& >( * this->item ).isFrozen() )
                        {
                                if ( speedTimer->getValue() > this->item->getSpeed() )
                                {
                                        // move item
                                        if ( ! activities::Moving::getInstance().move( this, &activity, false ) )
                                        {
                                                if ( activity == activities::Activity::MovingNortheast || activity == activities::Activity::MovingNorthwest )
                                                {
                                                        Activity tempActivity = activities::Activity::MovingNorth;
                                                        if ( ! activities::Moving::getInstance().move( this, &tempActivity, false ) )
                                                        {
                                                                activity = ( activity == activities::Activity::MovingNortheast ? activities::Activity::MovingEast : activities::Activity::MovingWest );
                                                                if ( this->item->getWeight() > 0 )
                                                                {
                                                                        activities::Falling::getInstance().fall( this );
                                                                }
                                                        }
                                                }
                                                else
                                                {
                                                        Activity tempActivity = activities::Activity::MovingSouth;
                                                        if ( ! activities::Moving::getInstance().move( this, &tempActivity, false ) )
                                                        {
                                                                activity = ( activity == activities::Activity::MovingSoutheast ? activities::Activity::MovingEast : activities::Activity::MovingWest );
                                                                if ( this->item->getWeight() > 0 )
                                                                {
                                                                        activities::Falling::getInstance().fall( this );
                                                                }
                                                        }
                                                }
                                        }
                                        else
                                        {
                                                // see if direction changes
                                                activity = updateDirection( activity );
                                        }

                                        // reset timer to next cycle
                                        speedTimer->reset();
                                }

                                this->item->animate();

                                SoundManager::getInstance().play( this->item->getKind (), "move" );
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
                        // when item is active and it’s time to move
                        if ( speedTimer->getValue() > this->item->getSpeed() )
                        {
                                activities::Displacing::getInstance().displace( this, &activity, false );
                                activity = activities::Activity::Waiting;
                                speedTimer->reset();
                        }

                        // preserve inactivity for frozen item
                        if ( dynamic_cast< FreeItem& >( * this->item ).isFrozen() )
                        {
                                activity = activities::Activity::Freeze;
                        }
                        break;

                case activities::Activity::Freeze:
                        dynamic_cast< FreeItem& >( * this->item ).setFrozen( true );
                        break;

                case activities::Activity::WakeUp:
                        dynamic_cast< FreeItem& >( * this->item ).setFrozen( false );
                        activity = activities::Activity::Waiting;
                        break;

                default:
                        ;
        }

        return ! alive ;
}

Activity Hunter::updateDirection( const Activity & activity )
{
        AvatarItemPtr whoToHunt = this->item->getMediator()->getActiveCharacter() ;
        if ( whoToHunt != nilPointer ) {
                // a character above hunter is unseen
                if ( whoToHunt->getZ () < this->item->getZ () + this->item->getHeight () )
                {
                        if ( getNameOfBehavior() == "behavior of hunter in four directions"
                                        || getNameOfBehavior() == "behavior of waiting hunter in four directions" )
                                return updateDirection4( activity );
                        else if ( getNameOfBehavior() == "behavior of hunter in eight directions"
                                        || getNameOfBehavior() == "behavior of waiting hunter in eight directions" )
                                return updateDirection8( activity );
                }
        }

        return activities::Activity::Waiting ;
}

Activity Hunter::updateDirection4( const Activity & activity )
{
        AvatarItemPtr whoToHunt = this->item->getMediator()->getActiveCharacter() ;
        if ( whoToHunt != nilPointer )
        {
                int dx = this->item->getX() - whoToHunt->getX();
                int dy = this->item->getY() - whoToHunt->getY();

                if ( abs( dy ) > abs( dx ) )
                {
                        if ( dx > 0 )
                        {
                                setCurrentActivity( activities::Activity::MovingNorth );
                        }
                        else if ( dx < 0 )
                        {
                                setCurrentActivity( activities::Activity::MovingSouth );
                        }
                        else
                        {
                                if ( dy > 0 )
                                        setCurrentActivity( activities::Activity::MovingEast );
                                else if ( dy < 0 )
                                        setCurrentActivity( activities::Activity::MovingWest );
                        }
                }
                else if ( abs( dy ) < abs( dx ) )
                {
                        if ( dy > 0 )
                        {
                                setCurrentActivity( activities::Activity::MovingEast );
                        }
                        else if ( dy < 0 )
                        {
                                setCurrentActivity( activities::Activity::MovingWest );
                        }
                        else
                        {
                                if ( dx > 0 )
                                        setCurrentActivity( activities::Activity::MovingNorth );
                                else if ( dx < 0 )
                                        setCurrentActivity( activities::Activity::MovingSouth );
                        }
                }
        }

        return activity ;
}

Activity Hunter::updateDirection8( const Activity& activity )
{
        AvatarItemPtr whoToHunt = this->item->getMediator()->getActiveCharacter();

        if ( whoToHunt != nilPointer ) // if there’s the active character in the room
        {
                int dx = this->item->getX() - whoToHunt->getX();
                int dy = this->item->getY() - whoToHunt->getY();

                // change the direction to reach the character as quick as possible
                // look for the distances on X and Y between the hunter and the character

                if ( abs( dy ) > abs( dx ) )
                {
                        if ( dx > 1 )
                        {
                                if ( dy > 1 )
                                        setCurrentActivity( activities::Activity::MovingNortheast );
                                else if ( dy < -1 )
                                        setCurrentActivity( activities::Activity::MovingNorthwest );
                                else
                                        setCurrentActivity( activities::Activity::MovingNorth );
                        }
                        else if ( dx < -1 )
                        {
                                if ( dy > 1 )
                                        setCurrentActivity( activities::Activity::MovingSoutheast );
                                else if ( dy < -1 )
                                        setCurrentActivity( activities::Activity::MovingSouthwest );
                                else
                                        setCurrentActivity( activities::Activity::MovingSouth );
                        }
                        else
                        {
                                if ( dy > 0 )
                                        setCurrentActivity( activities::Activity::MovingEast );
                                else if ( dy < 0 )
                                        setCurrentActivity( activities::Activity::MovingWest );
                        }
                }
                else if ( abs( dy ) < abs( dx ) )
                {
                        if ( dy > 1 )
                        {
                                if ( dx > 1 )
                                        setCurrentActivity( activities::Activity::MovingNortheast );
                                else if ( dx < -1 )
                                        setCurrentActivity( activities::Activity::MovingSoutheast );
                                else
                                        setCurrentActivity( activities::Activity::MovingEast );
                        }
                        else if ( dy < -1 )
                        {
                                if ( dx > 1 )
                                        setCurrentActivity( activities::Activity::MovingNorthwest );
                                else if ( dx < -1 )
                                        setCurrentActivity( activities::Activity::MovingSouthwest );
                                else
                                        setCurrentActivity( activities::Activity::MovingWest );
                        }
                        else
                        {
                                if ( dx > 0 )
                                        setCurrentActivity( activities::Activity::MovingNorth );
                                else if ( dx < 0 )
                                        setCurrentActivity( activities::Activity::MovingSouth );
                        }
                }

                // the guardian of throne flees from the player with four crowns
                if ( item->getKind() == "throne-guard" && GameManager::getInstance().howManyFreePlanets() >= 4 )
                {
                        setCurrentActivity( activities::Activity::MovingSouthwest );
                }
        }

        return activity;
}

bool Hunter::createFullBody()
{
        FreeItem& thisItem = dynamic_cast< FreeItem& >( * this->item );
        bool created = false;

        if ( thisItem.getKind() == "imperial-guard-head" && thisItem.canAdvanceTo( 0, 0, - Room::LayerHeight ) )
        {
                created = true;

                // create the new item at the same location
                FreeItemPtr newItem( new FreeItem (
                        ItemDescriptions::descriptions().getDescriptionByKind( "imperial-guard" ),
                        thisItem.getX(), thisItem.getY(), thisItem.getZ() - Room::LayerHeight,
                        thisItem.getOrientation() ) );

                newItem->setBehaviorOf( "behavior of hunter in four directions" );

                // switch off collisions for this item
                // otherwise it would be impossible to create a full-bodied guard
                thisItem.setIgnoreCollisions( true );

                thisItem.getMediator()->getRoom()->addFreeItem( newItem );
        }

        return created;
}

}
