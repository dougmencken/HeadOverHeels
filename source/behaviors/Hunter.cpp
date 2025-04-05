
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

        bool isWaitingHunter = ( getNameOfBehavior().find( "waiting hunter" ) != std::string::npos );
        bool movesInEightDirections = ( getNameOfBehavior().find( "eight directions" ) != std::string::npos );

        bool present = true ;

        switch ( getCurrentActivity() )
        {
                case activities::Activity::Waiting:
                        // if this hunter is not a waiting one, activate it just now
                        if ( ! isWaitingHunter )
                        {
                                SoundManager::getInstance().play( hunterItem.getKind (), "wait" );
                                updateDirection() ;
                        }
                        // otherwise wait until a character is within a 3-tile rectangle around the hunter
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

                                // an eight-directional waiting hunter emits the sound while it waits
                                if ( movesInEightDirections )
                                        SoundManager::getInstance().play( hunterItem.getKind (), "wait" );
                        }

                        // animate a waiting hunter
                        hunterItem.animate ();

                        break ;

                case activities::Activity::Moving:
                        // bin the original item when the full-bodied guard is created
                        if ( isWaitingHunter && createFullBody () )
                        {
                                present = false ;
                        }
                        else
                        if ( ! hunterItem.isFrozen() )
                        {
                                if ( speedTimer->getValue() > hunterItem.getSpeed() )
                                {
                                        // move a hunter
                                        bool moved = activities::Moving::getInstance().move( *this, false );
                                        if ( moved )
                                                updateDirection () ;
                                        else
                                        if ( movesInEightDirections ) {
                                                // an eight-directional hunter can’t move the current way
                                                Motion2D vector = get2DVelocityVector() ;

                                                if ( vector.isMovingNortheast() || vector.isMovingNorthwest() )
                                                {
                                                        // try moving north first and then west or east
                                                        setCurrentActivity( activities::Activity::Moving, Motion2D::movingNorth() );
                                                        if ( ! activities::Moving::getInstance().move( *this, false ) )
                                                                setCurrentActivity(
                                                                        activities::Activity::Moving,
                                                                        vector.isMovingNorthwest()
                                                                                ? Motion2D::movingWest()
                                                                                : Motion2D::movingEast() );
                                                }
                                                else if ( vector.isMovingSoutheast() || vector.isMovingSouthwest() )
                                                {
                                                        // try moving south first and then east or west
                                                        setCurrentActivity( activities::Activity::Moving, Motion2D::movingSouth() );
                                                        if ( ! activities::Moving::getInstance().move( *this, false ) )
                                                                setCurrentActivity(
                                                                        activities::Activity::Moving,
                                                                        vector.isMovingSoutheast()
                                                                                ? Motion2D::movingEast()
                                                                                : Motion2D::movingWest() );
                                                }
                                        }

                                        // fall if it falls
                                        if ( hunterItem.getWeight() > 0 )
                                                activities::Falling::getInstance().fall( *this );

                                        speedTimer->go() ; // reset timer
                                }

                                hunterItem.animate() ;

                                SoundManager::getInstance().play( hunterItem.getKind(), "move" );
                        }

                        break ;

                case activities::Activity::Pushed:
                        // when item is active and it’s time to move
                        if ( speedTimer->getValue() > hunterItem.getSpeed() )
                        {
                                activities::Displacing::getInstance().displace( *this, false );
                                beWaiting() ;
                                speedTimer->go() ;
                        }

                        // preserve inactivity for the frozen item
                        if ( hunterItem.isFrozen() )
                                setCurrentActivity( activities::Activity::Freeze, Motion2D::rest() );

                        break;

                case activities::Activity::Freeze:
                        hunterItem.setFrozen( true );
                        break;

                case activities::Activity::WakeUp:
                        hunterItem.setFrozen( false );
                        beWaiting() ;
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
                FreeItem & hunter = dynamic_cast< FreeItem & >( getItem() );

                // a character above hunter is unseen
                if ( whoToHunt->getZ() < hunter.getZ() + hunter.getHeight() )
                {
                        if ( getNameOfBehavior().find( "four directions" ) != std::string::npos )
                        {
                                updateDirection4 ();
                                return ;
                        }
                        else if ( getNameOfBehavior().find( "eight directions" ) != std::string::npos )
                        {
                                updateDirection8 ();
                                return ;
                        }
                }
        }

        beWaiting () ;
}

void Hunter::updateDirection4 ()
{
        AvatarItemPtr whoToHunt = getItem().getMediator()->getActiveCharacter() ;

        if ( whoToHunt != nilPointer )
        {
                FreeItem & hunterItem = dynamic_cast< FreeItem & >( getItem() );

                int dx = hunterItem.getX() - whoToHunt->getX();
                int dy = hunterItem.getY() - whoToHunt->getY();

                if ( abs( dx ) < abs( dy ) )
                {
                        if ( dx > 0 ) setCurrentActivity( activities::Activity::Moving, Motion2D::movingNorth() );
                        else
                        if ( dx < 0 ) setCurrentActivity( activities::Activity::Moving, Motion2D::movingSouth() );
                        else
                        if ( dy > 0 ) setCurrentActivity( activities::Activity::Moving, Motion2D::movingEast() );
                        else
                        if ( dy < 0 ) setCurrentActivity( activities::Activity::Moving, Motion2D::movingWest() );
                }
                else if ( abs( dy ) < abs( dx ) )
                {
                        if ( dy > 0 ) setCurrentActivity( activities::Activity::Moving, Motion2D::movingEast() );
                        else
                        if ( dy < 0 ) setCurrentActivity( activities::Activity::Moving, Motion2D::movingWest() );
                        else
                        if ( dx > 0 ) setCurrentActivity( activities::Activity::Moving, Motion2D::movingNorth() );
                        else
                        if ( dx < 0 ) setCurrentActivity( activities::Activity::Moving, Motion2D::movingSouth() );
                }
        }
}

void Hunter::updateDirection8 ()
{
        AvatarItemPtr whoToHunt = getItem().getMediator()->getActiveCharacter() ;

        if ( whoToHunt != nilPointer )
        {
                FreeItem & hunterItem = dynamic_cast< FreeItem & >( getItem() );

                int dx = hunterItem.getX() - whoToHunt->getX();
                int dy = hunterItem.getY() - whoToHunt->getY();

                // change the direction to reach the character as quick as possible
                // look for the distances on X and Y between the hunter and the character

                if ( abs( dx ) < abs( dy ) )
                {
                        if ( dx > 1 )
                        {
                                if ( dy > 1 )
                                        setCurrentActivity( activities::Activity::Moving, Motion2D::movingNortheast() );
                                else if ( dy < -1 )
                                        setCurrentActivity( activities::Activity::Moving, Motion2D::movingNorthwest() );
                                else
                                        setCurrentActivity( activities::Activity::Moving, Motion2D::movingNorth() );
                        }
                        else if ( dx < -1 )
                        {
                                if ( dy > 1 )
                                        setCurrentActivity( activities::Activity::Moving, Motion2D::movingSoutheast() );
                                else if ( dy < -1 )
                                        setCurrentActivity( activities::Activity::Moving, Motion2D::movingSouthwest() );
                                else
                                        setCurrentActivity( activities::Activity::Moving, Motion2D::movingSouth() );
                        }
                        else
                        {
                                if ( dy > 0 )
                                        setCurrentActivity( activities::Activity::Moving, Motion2D::movingEast() );
                                else if ( dy < 0 )
                                        setCurrentActivity( activities::Activity::Moving, Motion2D::movingWest() );
                        }
                }
                else if ( abs( dy ) < abs( dx ) )
                {
                        if ( dy > 1 )
                        {
                                if ( dx > 1 )
                                        setCurrentActivity( activities::Activity::Moving, Motion2D::movingNortheast() );
                                else if ( dx < -1 )
                                        setCurrentActivity( activities::Activity::Moving, Motion2D::movingSoutheast() );
                                else
                                        setCurrentActivity( activities::Activity::Moving, Motion2D::movingEast() );
                        }
                        else if ( dy < -1 )
                        {
                                if ( dx > 1 )
                                        setCurrentActivity( activities::Activity::Moving, Motion2D::movingNorthwest() );
                                else if ( dx < -1 )
                                        setCurrentActivity( activities::Activity::Moving, Motion2D::movingSouthwest() );
                                else
                                        setCurrentActivity( activities::Activity::Moving, Motion2D::movingWest() );
                        }
                        else
                        {
                                if ( dx > 0 )
                                        setCurrentActivity( activities::Activity::Moving, Motion2D::movingNorth() );
                                else if ( dx < 0 )
                                        setCurrentActivity( activities::Activity::Moving, Motion2D::movingSouth() );
                        }
                }

                // the guardian of throne flees from the player with four crowns
                if ( hunterItem.getKind() == "throne-guard" && GameManager::getInstance().howManyFreePlanets() >= 4 )
                {
                        setCurrentActivity( activities::Activity::Moving, Motion2D::movingSouthwest() );
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
