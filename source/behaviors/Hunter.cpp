
#include "Hunter.hpp"

#include "FreeItem.hpp"
#include "AvatarItem.hpp"
#include "ItemDescriptions.hpp"
#include "Moving.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"
#include "GameManager.hpp"


namespace behaviors
{

Hunter::Hunter( FreeItem & item, const std::string & behavior )
        : Behavior( item, behavior )
        , speedTimer( new Timer() )
{
        speedTimer->go ();
}

bool Hunter::update ()
{
        FreeItem & hunterItem = dynamic_cast< FreeItem & >( getItem () );
        AvatarItemPtr whoToHunt = hunterItem.getMediator()->getActiveCharacter() ;

        bool present = true ;

        switch ( getCurrentActivity() )
        {
                case activities::Activity::Waiting:
                        // if the hunter is not a waiting one, activate it just now
                        if ( getNameOfBehavior() == "behavior of hunter in four directions" ||
                                        getNameOfBehavior() == "behavior of hunter in eight directions" )
                        {
                                SoundManager::getInstance().play( hunterItem.getKind (), "wait" );
                                updateDirection() ;
                        }
                        // otherwise wait until the character is within the 3-tile rectangle around the hunter
                        else
                        {
                                const unsigned int sizeOfRectangleInTiles = 3 ;
                                const int coverage = hunterItem.getMediator()->getRoom()->getSizeOfOneTile() * sizeOfRectangleInTiles ;

                                if ( whoToHunt != nilPointer  &&
                                        whoToHunt->getX() > hunterItem.getX() - coverage  &&
                                        whoToHunt->getX() < hunterItem.getX() + hunterItem.getWidthX() + coverage  &&
                                        whoToHunt->getY() > hunterItem.getY() - coverage  &&
                                        whoToHunt->getY() < hunterItem.getY() + hunterItem.getWidthY() + coverage )
                                {
                                        updateDirection() ;
                                }

                                // the eight-directional waiting hunter emits the sound while it waits
                                if ( getNameOfBehavior() == "behavior of waiting hunter in eight directions" )
                                        SoundManager::getInstance().play( hunterItem.getKind (), "wait" );
                        }

                        // animate the waiting item
                        hunterItem.animate ();

                        break ;

                case activities::Activity::MovingNorth:
                case activities::Activity::MovingSouth:
                case activities::Activity::MovingEast:
                case activities::Activity::MovingWest:
                        // bin original item when full-bodied guard is created
                        if ( getNameOfBehavior() == "behavior of waiting hunter in four directions" && createFullBody () )
                        {
                                present = false ;
                        }
                        else if ( ! hunterItem.isFrozen() )
                        {
                                if ( speedTimer->getValue() > hunterItem.getSpeed() )
                                {
                                        // move item
                                        activities::Moving::getInstance().move( *this, false );

                                        // reset timer to next cycle
                                        speedTimer->reset();

                                        updateDirection () ;

                                        // fall if it falls
                                        if ( hunterItem.getWeight() > 0 )
                                                activities::Falling::getInstance().fall( *this );
                                }

                                hunterItem.animate() ;

                                SoundManager::getInstance().play( hunterItem.getKind(), "move" );
                        }
                        break;

                case activities::Activity::MovingNortheast:
                case activities::Activity::MovingNorthwest:
                case activities::Activity::MovingSoutheast:
                case activities::Activity::MovingSouthwest:
                        if ( ! hunterItem.isFrozen() )
                        {
                                if ( speedTimer->getValue() > hunterItem.getSpeed() )
                                {
                                        // try to move item
                                        if ( ! activities::Moving::getInstance().move( *this, false ) )
                                        {       // can’t move that way
                                                Activity thatActivity = getCurrentActivity() ;

                                                if ( thatActivity == activities::Activity::MovingNortheast
                                                                || thatActivity == activities::Activity::MovingNorthwest )
                                                {
                                                        // try moving north first and then west or east
                                                        setCurrentActivity( activities::Activity::MovingNorth );
                                                        if ( ! activities::Moving::getInstance().move( *this, false ) )
                                                                setCurrentActivity( thatActivity == activities::Activity::MovingNorthwest
                                                                                                        ? activities::Activity::MovingWest
                                                                                                        : activities::Activity::MovingEast );
                                                } else // MoveSoutheast or MoveSouthwest
                                                {
                                                        // try moving south first and then east or west
                                                        setCurrentActivity( activities::Activity::MovingSouth );
                                                        if ( ! activities::Moving::getInstance().move( *this, false ) )
                                                                setCurrentActivity( thatActivity == activities::Activity::MovingSoutheast
                                                                                                        ? activities::Activity::MovingEast
                                                                                                        : activities::Activity::MovingWest );
                                                }

                                                if ( hunterItem.getWeight() > 0 )
                                                        activities::Falling::getInstance().fall( *this );
                                        }
                                        else {
                                                updateDirection () ;
                                        }

                                        // reset timer to next cycle
                                        speedTimer->reset();
                                }

                                hunterItem.animate() ;

                                SoundManager::getInstance().play( hunterItem.getKind(), "move" );
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
                        if ( speedTimer->getValue() > hunterItem.getSpeed() )
                        {
                                activities::Displacing::getInstance().displace( *this, false );
                                setCurrentActivity( activities::Activity::Waiting );
                                speedTimer->reset();
                        }

                        // preserve inactivity for the frozen item
                        if ( hunterItem.isFrozen() )
                                setCurrentActivity( activities::Activity::Freeze );

                        break;

                case activities::Activity::Freeze:
                        hunterItem.setFrozen( true );
                        break;

                case activities::Activity::WakeUp:
                        hunterItem.setFrozen( false );
                        setCurrentActivity( activities::Activity::Waiting );
                        break;

                default:
                        ;
        }

        return present ;
}

void Hunter::updateDirection ()
{
        AvatarItemPtr whoToHunt = getItem().getMediator()->getActiveCharacter ();

        if ( whoToHunt != nilPointer ) {
                // a character above hunter is unseen
                if ( whoToHunt->getZ () < getItem().getZ () + getItem().getHeight () )
                {
                        if ( getNameOfBehavior() == "behavior of hunter in four directions"
                                        || getNameOfBehavior() == "behavior of waiting hunter in four directions" )
                        {
                                updateDirection4 ();
                                return ;
                        }
                        else if ( getNameOfBehavior() == "behavior of hunter in eight directions"
                                        || getNameOfBehavior() == "behavior of waiting hunter in eight directions" )
                        {
                                updateDirection8 ();
                                return ;
                        }
                }
        }

        setCurrentActivity( activities::Activity::Waiting );
}

void Hunter::updateDirection4 ()
{
        AvatarItemPtr whoToHunt = getItem().getMediator()->getActiveCharacter() ;

        if ( whoToHunt != nilPointer )
        {
                Item & hunterItem = getItem() ;

                int dx = hunterItem.getX() - whoToHunt->getX();
                int dy = hunterItem.getY() - whoToHunt->getY();

                if ( abs( dx ) < abs( dy ) )
                {
                        if ( dx > 0 ) setCurrentActivity( activities::Activity::MovingNorth );
                        else
                        if ( dx < 0 ) setCurrentActivity( activities::Activity::MovingSouth );
                        else
                        if ( dy > 0 ) setCurrentActivity( activities::Activity::MovingEast );
                        else
                        if ( dy < 0 ) setCurrentActivity( activities::Activity::MovingWest );
                }
                else if ( abs( dy ) < abs( dx ) )
                {
                        if ( dy > 0 ) setCurrentActivity( activities::Activity::MovingEast );
                        else
                        if ( dy < 0 ) setCurrentActivity( activities::Activity::MovingWest );
                        else
                        if ( dx > 0 ) setCurrentActivity( activities::Activity::MovingNorth );
                        else
                        if ( dx < 0 ) setCurrentActivity( activities::Activity::MovingSouth );
                }
        }
}

void Hunter::updateDirection8 ()
{
        AvatarItemPtr whoToHunt = getItem().getMediator()->getActiveCharacter() ;

        if ( whoToHunt != nilPointer )
        {
                Item & hunterItem = getItem() ;

                int dx = hunterItem.getX() - whoToHunt->getX();
                int dy = hunterItem.getY() - whoToHunt->getY();

                // change the direction to reach the character as quick as possible
                // look for the distances on X and Y between the hunter and the character

                if ( abs( dx ) < abs( dy ) )
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
                if ( hunterItem.getKind() == "throne-guard" && GameManager::getInstance().howManyFreePlanets() >= 4 )
                {
                        setCurrentActivity( activities::Activity::MovingSouthwest );
                }
        }
}

bool Hunter::createFullBody ()
{
        FreeItem & thisItem = dynamic_cast< FreeItem & >( getItem() );
        bool created = false;

        if ( thisItem.getKind() == "imperial-guard-head" && thisItem.canAdvanceTo( 0, 0, - Room::LayerHeight ) )
        {
                created = true;

                // create the new item at the same location
                FreeItemPtr newItem( new FreeItem (
                        * ItemDescriptions::descriptions().getDescriptionByKind( "imperial-guard" ),
                        thisItem.getX(), thisItem.getY(), thisItem.getZ() - Room::LayerHeight,
                        thisItem.getHeading() ) );

                newItem->setBehaviorOf( "behavior of hunter in four directions" );

                // switch off collisions for this item
                // otherwise it would be impossible to create a full-bodied guard
                thisItem.setIgnoreCollisions( true );

                thisItem.getMediator()->getRoom()->addFreeItem( newItem );
        }

        return created;
}

}
