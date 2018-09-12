
#include "Hunter.hpp"
#include "Item.hpp"
#include "ItemData.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "MoveKindOfActivity.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"
#include "GameManager.hpp"


namespace isomot
{

Hunter::Hunter( Item * item, const std::string & behavior ) :
        Behavior( item, behavior )
        , guardData( nilPointer )
{
        speedTimer = new Timer();
        speedTimer->go ();
}

Hunter::~Hunter()
{
        delete speedTimer;
}

bool Hunter::update ()
{
        Mediator* mediator = this->item->getMediator();
        PlayerItem* characterToHunt = mediator->getActiveCharacter();

        bool alive = true;

        switch ( activity )
        {
                case Wait:
                // if hunter is not a waiting one, activate it yet
                if ( getNameOfBehavior() == "behavior of hunter in four directions" ||
                                getNameOfBehavior() == "behavior of hunter in eight directions" )
                {
                        SoundManager::getInstance()->play( this->item->getLabel(), activity );
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
                                SoundManager::getInstance()->play( this->item->getLabel(), activity );
                        }

                        // animate item, and when it waits too
                        this->item->animate();
                }
                break;

                case MoveNorth:
                case MoveSouth:
                case MoveEast:
                case MoveWest:
                        // bin original item when full-bodied guard is created
                        if ( getNameOfBehavior() == "behavior of waiting hunter in four directions" && createFullBody () )
                        {
                                alive = false;
                        }
                        else if ( ! dynamic_cast< FreeItem* >( this->item )->isFrozen() ) // item is active and it’s time to move
                        {
                                if ( speedTimer->getValue() > this->item->getSpeed() )
                                {
                                        // move item
                                        MoveKindOfActivity::getInstance()->move( this, &activity, false );

                                        // reset timer to next cycle
                                        speedTimer->reset();

                                        // see if direction changes
                                        activity = calculateDirection( activity );

                                        // fall if you have to
                                        if ( this->item->getWeight() > 0 )
                                        {
                                                FallKindOfActivity::getInstance()->fall( this );
                                        }
                                }

                                this->item->animate();

                                // play sound of movement
                                SoundManager::getInstance()->play( this->item->getLabel(), activity );
                        }
                        break;

                case MoveNortheast:
                case MoveNorthwest:
                case MoveSoutheast:
                case MoveSouthwest:
                        if ( ! dynamic_cast< FreeItem* >( this->item )->isFrozen() )
                        {
                                if ( speedTimer->getValue() > this->item->getSpeed() )
                                {
                                        // move item
                                        if ( ! MoveKindOfActivity::getInstance()->move( this, &activity, false ) )
                                        {
                                                if ( activity == MoveNortheast || activity == MoveNorthwest )
                                                {
                                                        ActivityOfItem tempActivity = MoveNorth;
                                                        if ( ! MoveKindOfActivity::getInstance()->move( this, &tempActivity, false ) )
                                                        {
                                                                activity = ( activity == MoveNortheast ? MoveEast : MoveWest );
                                                                if ( this->item->getWeight() > 0 )
                                                                {
                                                                        FallKindOfActivity::getInstance()->fall( this );
                                                                }
                                                        }
                                                }
                                                else
                                                {
                                                        ActivityOfItem tempActivity = MoveSouth;
                                                        if ( ! MoveKindOfActivity::getInstance()->move( this, &tempActivity, false ) )
                                                        {
                                                                activity = ( activity == MoveSoutheast ? MoveEast : MoveWest );
                                                                if ( this->item->getWeight() > 0 )
                                                                {
                                                                        FallKindOfActivity::getInstance()->fall( this );
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
                                SoundManager::getInstance()->play( this->item->getLabel(), activity );
                        }
                        break;

                case DisplaceNorth:
                case DisplaceSouth:
                case DisplaceEast:
                case DisplaceWest:
                case DisplaceNortheast:
                case DisplaceNorthwest:
                case DisplaceSoutheast:
                case DisplaceSouthwest:
                        // when item is active and it’s time to move
                        if ( speedTimer->getValue() > this->item->getSpeed() )
                        {
                                DisplaceKindOfActivity::getInstance()->displace( this, &activity, false );
                                activity = Wait;
                                speedTimer->reset();
                        }

                        // preserve inactivity for frozen item
                        if ( dynamic_cast< FreeItem* >( this->item )->isFrozen() )
                        {
                                activity = Freeze;
                        }
                        break;

                case Freeze:
                        dynamic_cast< FreeItem* >( this->item )->setFrozen( true );
                        break;

                case WakeUp:
                        dynamic_cast< FreeItem* >( this->item )->setFrozen( false );
                        activity = Wait;
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

        return Wait;
}

ActivityOfItem Hunter::calculateDirection4( const ActivityOfItem& activity )
{
        PlayerItem* characterToHunt = this->item->getMediator()->getActiveCharacter();

        if ( characterToHunt != nilPointer ) // if there’s active player in room
        {
                int dx = this->item->getX() - characterToHunt->getX();
                int dy = this->item->getY() - characterToHunt->getY();

                if ( abs( dy ) > abs( dx ) )
                {
                        if ( dx > 0 )
                        {
                                changeActivityOfItem( MoveNorth );
                        }
                        else if ( dx < 0 )
                        {
                                changeActivityOfItem( MoveSouth );
                        }
                        else
                        {
                                if ( dy > 0 )
                                        changeActivityOfItem( MoveEast );
                                else if ( dy < 0 )
                                        changeActivityOfItem( MoveWest );
                        }
                }
                else if ( abs( dy ) < abs( dx ) )
                {
                        if ( dy > 0 )
                        {
                                changeActivityOfItem( MoveEast );
                        }
                        else if ( dy < 0 )
                        {
                                changeActivityOfItem( MoveWest );
                        }
                        else
                        {
                                if ( dx > 0 )
                                        changeActivityOfItem( MoveNorth );
                                else if ( dx < 0 )
                                        changeActivityOfItem( MoveSouth );
                        }
                }
        }

        return activity;
}

ActivityOfItem Hunter::calculateDirection8( const ActivityOfItem& activity )
{
        PlayerItem* characterToHunt = this->item->getMediator()->getActiveCharacter();

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
                                        changeActivityOfItem( MoveNortheast );
                                else if ( dy < -1 )
                                        changeActivityOfItem( MoveNorthwest );
                                else
                                        changeActivityOfItem( MoveNorth );
                        }
                        else if ( dx < -1 )
                        {
                                if ( dy > 1 )
                                        changeActivityOfItem( MoveSoutheast );
                                else if ( dy < -1 )
                                        changeActivityOfItem( MoveSouthwest );
                                else
                                        changeActivityOfItem( MoveSouth );
                        }
                        else
                        {
                                if ( dy > 0 )
                                        changeActivityOfItem( MoveEast );
                                else if ( dy < 0 )
                                        changeActivityOfItem( MoveWest );
                        }
                }
                else if ( abs( dy ) < abs( dx ) )
                {
                        if ( dy > 1 )
                        {
                                if ( dx > 1 )
                                        changeActivityOfItem( MoveNortheast );
                                else if ( dx < -1 )
                                        changeActivityOfItem( MoveSoutheast );
                                else
                                        changeActivityOfItem( MoveEast );
                        }
                        else if ( dy < -1 )
                        {
                                if ( dx > 1 )
                                        changeActivityOfItem( MoveNorthwest );
                                else if ( dx < -1 )
                                        changeActivityOfItem( MoveSouthwest );
                                else
                                        changeActivityOfItem( MoveWest );
                        }
                        else
                        {
                                if ( dx > 0 )
                                        changeActivityOfItem( MoveNorth );
                                else if ( dx < 0 )
                                        changeActivityOfItem( MoveSouth );
                        }
                }

                // guardian of throne flees from player with four crowns
                if ( item->getLabel() == "throne-guard" && GameManager::getInstance()->countFreePlanets() >= 4 )
                {
                        changeActivityOfItem( MoveSouthwest );
                }
        }

        return activity;
}

bool Hunter::createFullBody()
{
        FreeItem* thisItem = dynamic_cast< FreeItem* >( this->item );
        bool created = false;

        if ( thisItem->getLabel() == "imperial-guard-head" && thisItem->canAdvanceTo( 0, 0, -LayerHeight ) )
        {
                created = true;

                // create new item in the same location
                FreeItem* newItem = new FreeItem( guardData,
                                                  thisItem->getX(), thisItem->getY(), thisItem->getZ() - LayerHeight,
                                                  thisItem->getOrientation() );

                newItem->assignBehavior( "behavior of hunter in four directions", nilPointer );

                // switch off collisions for this item
                // otherwise it’s impossible to create full-bodied guard
                thisItem->setCollisionDetector( false );

                thisItem->getMediator()->getRoom()->addFreeItem( newItem );
        }

        return created;
}

void Hunter::setMoreData( void * data )
{
        this->guardData = reinterpret_cast< ItemData * >( data );
}

}
