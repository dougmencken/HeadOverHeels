
#include "Hunter.hpp"

#include "Isomot.hpp"
#include "Item.hpp"
#include "DescriptionOfItem.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "MoveKindOfActivity.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"
#include "GameManager.hpp"


namespace iso
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
        PlayerItemPtr characterToHunt = mediator->getActiveCharacter();

        bool alive = true;

        switch ( activity )
        {
                case Activity::Wait:
                        // if hunter is not a waiting one, activate it yet
                        if ( getNameOfBehavior() == "behavior of hunter in four directions" ||
                                        getNameOfBehavior() == "behavior of hunter in eight directions" )
                        {
                                SoundManager::getInstance().play( this->item->getLabel(), activity );
                                activity = calculateDirection( activity );
                        }
                        // otherwise check if player is within defined rectangle near hunter to activate
                        else
                        {
                                const unsigned int sizeOfRectangleInTiles = 3;
                                const int delta = mediator->getRoom()->getSizeOfOneTile() * sizeOfRectangleInTiles;

                                if ( characterToHunt != nilPointer  &&
                                        characterToHunt->getX() > this->item->getX() - delta  &&
                                        characterToHunt->getX() < this->item->getX() + static_cast< int >( this->item->getWidthX() ) + delta  &&
                                        characterToHunt->getY() > this->item->getY() - delta  &&
                                        characterToHunt->getY() < this->item->getY() + static_cast< int >( this->item->getWidthY() ) + delta )
                                {
                                        activity = calculateDirection( activity );
                                }

                                // eight-directional waiting hunter emits sound when it waits
                                if ( getNameOfBehavior() == "behavior of waiting hunter in eight directions" )
                                {
                                        SoundManager::getInstance().play( this->item->getLabel(), activity );
                                }

                                // animate item, and when it waits too
                                this->item->animate();
                        }
                break;

                case Activity::MoveNorth:
                case Activity::MoveSouth:
                case Activity::MoveEast:
                case Activity::MoveWest:
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
                                        MoveKindOfActivity::getInstance().move( this, &activity, false );

                                        // reset timer to next cycle
                                        speedTimer->reset();

                                        // see if direction changes
                                        activity = calculateDirection( activity );

                                        // fall if you have to
                                        if ( this->item->getWeight() > 0 )
                                        {
                                                FallKindOfActivity::getInstance().fall( this );
                                        }
                                }

                                this->item->animate();

                                // play sound of movement
                                SoundManager::getInstance().play( this->item->getLabel(), activity );
                        }
                        break;

                case Activity::MoveNortheast:
                case Activity::MoveNorthwest:
                case Activity::MoveSoutheast:
                case Activity::MoveSouthwest:
                        if ( ! dynamic_cast< FreeItem& >( * this->item ).isFrozen() )
                        {
                                if ( speedTimer->getValue() > this->item->getSpeed() )
                                {
                                        // move item
                                        if ( ! MoveKindOfActivity::getInstance().move( this, &activity, false ) )
                                        {
                                                if ( activity == Activity::MoveNortheast || activity == Activity::MoveNorthwest )
                                                {
                                                        ActivityOfItem tempActivity = Activity::MoveNorth;
                                                        if ( ! MoveKindOfActivity::getInstance().move( this, &tempActivity, false ) )
                                                        {
                                                                activity = ( activity == Activity::MoveNortheast ? Activity::MoveEast : Activity::MoveWest );
                                                                if ( this->item->getWeight() > 0 )
                                                                {
                                                                        FallKindOfActivity::getInstance().fall( this );
                                                                }
                                                        }
                                                }
                                                else
                                                {
                                                        ActivityOfItem tempActivity = Activity::MoveSouth;
                                                        if ( ! MoveKindOfActivity::getInstance().move( this, &tempActivity, false ) )
                                                        {
                                                                activity = ( activity == Activity::MoveSoutheast ? Activity::MoveEast : Activity::MoveWest );
                                                                if ( this->item->getWeight() > 0 )
                                                                {
                                                                        FallKindOfActivity::getInstance().fall( this );
                                                                }
                                                        }
                                                }
                                        }
                                        else
                                        {
                                                // see if direction changes
                                                activity = calculateDirection( activity );
                                        }

                                        // reset timer to next cycle
                                        speedTimer->reset();
                                }

                                this->item->animate();

                                // play sound of movement
                                SoundManager::getInstance().play( this->item->getLabel(), activity );
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
                        // when item is active and it’s time to move
                        if ( speedTimer->getValue() > this->item->getSpeed() )
                        {
                                DisplaceKindOfActivity::getInstance().displace( this, &activity, false );
                                activity = Activity::Wait;
                                speedTimer->reset();
                        }

                        // preserve inactivity for frozen item
                        if ( dynamic_cast< FreeItem& >( * this->item ).isFrozen() )
                        {
                                activity = Activity::Freeze;
                        }
                        break;

                case Activity::Freeze:
                        dynamic_cast< FreeItem& >( * this->item ).setFrozen( true );
                        break;

                case Activity::WakeUp:
                        dynamic_cast< FreeItem& >( * this->item ).setFrozen( false );
                        activity = Activity::Wait;
                        break;

                default:
                        ;
        }

        return ! alive;
}

ActivityOfItem Hunter::calculateDirection( const ActivityOfItem& activity )
{
        if ( getNameOfBehavior() == "behavior of hunter in four directions" ||
                        getNameOfBehavior() == "behavior of waiting hunter in four directions" )
        {
                return calculateDirection4( activity );
        }
        else if ( getNameOfBehavior() == "behavior of hunter in eight directions" ||
                        getNameOfBehavior() == "behavior of waiting hunter in eight directions" )
        {
                return calculateDirection8( activity );
        }

        return Activity::Wait;
}

ActivityOfItem Hunter::calculateDirection4( const ActivityOfItem& activity )
{
        PlayerItemPtr characterToHunt = this->item->getMediator()->getActiveCharacter();

        if ( characterToHunt != nilPointer ) // if there’s active player in room
        {
                int dx = this->item->getX() - characterToHunt->getX();
                int dy = this->item->getY() - characterToHunt->getY();

                if ( abs( dy ) > abs( dx ) )
                {
                        if ( dx > 0 )
                        {
                                changeActivityOfItem( Activity::MoveNorth );
                        }
                        else if ( dx < 0 )
                        {
                                changeActivityOfItem( Activity::MoveSouth );
                        }
                        else
                        {
                                if ( dy > 0 )
                                        changeActivityOfItem( Activity::MoveEast );
                                else if ( dy < 0 )
                                        changeActivityOfItem( Activity::MoveWest );
                        }
                }
                else if ( abs( dy ) < abs( dx ) )
                {
                        if ( dy > 0 )
                        {
                                changeActivityOfItem( Activity::MoveEast );
                        }
                        else if ( dy < 0 )
                        {
                                changeActivityOfItem( Activity::MoveWest );
                        }
                        else
                        {
                                if ( dx > 0 )
                                        changeActivityOfItem( Activity::MoveNorth );
                                else if ( dx < 0 )
                                        changeActivityOfItem( Activity::MoveSouth );
                        }
                }
        }

        return activity;
}

ActivityOfItem Hunter::calculateDirection8( const ActivityOfItem& activity )
{
        PlayerItemPtr characterToHunt = this->item->getMediator()->getActiveCharacter();

        if ( characterToHunt != nilPointer ) // if there’s active player in room
        {
                int dx = this->item->getX() - characterToHunt->getX();
                int dy = this->item->getY() - characterToHunt->getY();

                // get direction that allows to reach player as fast as possible
                // look for distances on X and Y between hunter and player

                if ( abs( dy ) > abs( dx ) )
                {
                        if ( dx > 1 )
                        {
                                if ( dy > 1 )
                                        changeActivityOfItem( Activity::MoveNortheast );
                                else if ( dy < -1 )
                                        changeActivityOfItem( Activity::MoveNorthwest );
                                else
                                        changeActivityOfItem( Activity::MoveNorth );
                        }
                        else if ( dx < -1 )
                        {
                                if ( dy > 1 )
                                        changeActivityOfItem( Activity::MoveSoutheast );
                                else if ( dy < -1 )
                                        changeActivityOfItem( Activity::MoveSouthwest );
                                else
                                        changeActivityOfItem( Activity::MoveSouth );
                        }
                        else
                        {
                                if ( dy > 0 )
                                        changeActivityOfItem( Activity::MoveEast );
                                else if ( dy < 0 )
                                        changeActivityOfItem( Activity::MoveWest );
                        }
                }
                else if ( abs( dy ) < abs( dx ) )
                {
                        if ( dy > 1 )
                        {
                                if ( dx > 1 )
                                        changeActivityOfItem( Activity::MoveNortheast );
                                else if ( dx < -1 )
                                        changeActivityOfItem( Activity::MoveSoutheast );
                                else
                                        changeActivityOfItem( Activity::MoveEast );
                        }
                        else if ( dy < -1 )
                        {
                                if ( dx > 1 )
                                        changeActivityOfItem( Activity::MoveNorthwest );
                                else if ( dx < -1 )
                                        changeActivityOfItem( Activity::MoveSouthwest );
                                else
                                        changeActivityOfItem( Activity::MoveWest );
                        }
                        else
                        {
                                if ( dx > 0 )
                                        changeActivityOfItem( Activity::MoveNorth );
                                else if ( dx < 0 )
                                        changeActivityOfItem( Activity::MoveSouth );
                        }
                }

                // the guardian of throne flees from the player with four crowns
                if ( item->getLabel() == "throne-guard" && game::GameManager::getInstance().countFreePlanets() >= 4 )
                {
                        changeActivityOfItem( Activity::MoveSouthwest );
                }
        }

        return activity;
}

bool Hunter::createFullBody()
{
        FreeItem& thisItem = dynamic_cast< FreeItem& >( * this->item );
        bool created = false;

        if ( thisItem.getLabel() == "imperial-guard-head" && thisItem.canAdvanceTo( 0, 0, - Isomot::LayerHeight ) )
        {
                created = true;

                // create new item in the same location
                FreeItemPtr newItem( new FreeItem (
                        item->getDescriptionOfItem()->getItemDescriptions()->getDescriptionByLabel( "imperial-guard" ),
                        thisItem.getX(), thisItem.getY(), thisItem.getZ() - Isomot::LayerHeight,
                        thisItem.getOrientation() ) );

                newItem->setBehaviorOf( "behavior of hunter in four directions" );

                // switch off collisions for this item
                // otherwise it’s impossible to create full-bodied guard
                thisItem.setCollisionDetector( false );

                thisItem.getMediator()->getRoom()->addFreeItem( newItem );
        }

        return created;
}

}
