
#include "Hunter.hpp"
#include "Item.hpp"
#include "ItemData.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
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
{
        speedTimer = new HPC();
        speedTimer->start();
}

Hunter::~Hunter()
{
        delete speedTimer;
}

bool Hunter::update ()
{
        FreeItem* thisItem = dynamic_cast< FreeItem * >( this->item );
        PlayerItem* activePlayer = thisItem->getMediator()->getActivePlayer();
        Mediator* mediator = thisItem->getMediator();

        bool alive = true;

        switch ( activity )
        {
                case Wait:
                // if hunter is not a waiting one, activate it yet
                if ( getNameOfBehavior() == "behavior of hunter in four directions" ||
                                getNameOfBehavior() == "behavior of hunter in eight directions" )
                {
                        SoundManager::getInstance()->play( thisItem->getLabel(), activity );
                        activity = calculateDirection( activity );
                }
                // otherwise check if player is within defined rectangle near hunter to activate
                else
                {
                        const unsigned int sizeOfRectangleInTiles = 3;
                        const int delta = mediator->getRoom()->getSizeOfOneTile() * sizeOfRectangleInTiles;

                        if ( activePlayer != 0  &&
                                activePlayer->getX() > thisItem->getX() - delta  &&
                                activePlayer->getX() < thisItem->getX() + static_cast< int >( thisItem->getWidthX() ) + delta  &&
                                activePlayer->getY() > thisItem->getY() - delta  &&
                                activePlayer->getY() < thisItem->getY() + static_cast< int >( thisItem->getWidthY() ) + delta )
                        {
                                activity = calculateDirection( activity );
                        }

                        // eight-directional waiting hunter emits sound when it waits
                        if ( getNameOfBehavior() == "behavior of waiting hunter in eight directions" )
                        {
                                SoundManager::getInstance()->play( thisItem->getLabel(), activity );
                        }

                        // animate item, and when it waits too
                        thisItem->animate();
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
                        else if ( ! thisItem->isFrozen() ) // item is active and it’s time to move
                        {
                                if ( speedTimer->getValue() > thisItem->getSpeed() )
                                {
                                        // move item
                                        whatToDo->move( this, &activity, false );

                                        // reset timer to next cycle
                                        speedTimer->reset();

                                        // see if direction changes
                                        activity = calculateDirection( activity );

                                        // fall if you have to
                                        if ( thisItem->getWeight() > 0 )
                                        {
                                                FallKindOfActivity::getInstance()->fall( this );
                                        }
                                }

                                thisItem->animate();

                                // play sound of movement
                                SoundManager::getInstance()->play( thisItem->getLabel(), activity );
                        }
                        break;

                case MoveNortheast:
                case MoveNorthwest:
                case MoveSoutheast:
                case MoveSouthwest:
                        if ( ! thisItem->isFrozen() )
                        {
                                if ( speedTimer->getValue() > thisItem->getSpeed() )
                                {
                                        // move item
                                        if ( ! whatToDo->move( this, &activity, false ) )
                                        {
                                                if ( activity == MoveNortheast || activity == MoveNorthwest )
                                                {
                                                        ActivityOfItem tempActivity = MoveNorth;
                                                        if ( ! whatToDo->move( this, &tempActivity, false ) )
                                                        {
                                                                activity = ( activity == MoveNortheast ? MoveEast : MoveWest );
                                                                if ( thisItem->getWeight() > 0 )
                                                                {
                                                                        FallKindOfActivity::getInstance()->fall( this );
                                                                }
                                                        }
                                                }
                                                else
                                                {
                                                        ActivityOfItem tempActivity = MoveSouth;
                                                        if ( ! whatToDo->move( this, &tempActivity, false ) )
                                                        {
                                                                activity = ( activity == MoveSoutheast ? MoveEast : MoveWest );
                                                                if ( thisItem->getWeight() > 0 )
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

                                thisItem->animate();

                                // play sound of movement
                                SoundManager::getInstance()->play( thisItem->getLabel(), activity );
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
                        if ( speedTimer->getValue() > thisItem->getSpeed() )
                        {
                                whatToDo->displace( this, &activity, false ); // move item
                                activity = Wait;
                                speedTimer->reset();
                        }

                        // preserve inactivity for frozen item
                        if ( thisItem->isFrozen() )
                        {
                                activity = Freeze;
                        }
                        break;

                case Freeze:
                        thisItem->setFrozen( true );
                        break;

                case WakeUp:
                        thisItem->setFrozen( false );
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
        FreeItem* thisItem = dynamic_cast< FreeItem * >( this->item );
        PlayerItem* activePlayer = thisItem->getMediator()->getActivePlayer();

        if ( activePlayer != 0 ) // if there’s active player in room
        {
                int dx = thisItem->getX() - activePlayer->getX();
                int dy = thisItem->getY() - activePlayer->getY();

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
        FreeItem* thisItem = dynamic_cast< FreeItem* >( this->item );
        PlayerItem* activePlayer = thisItem->getMediator()->getActivePlayer();

        if ( activePlayer != 0 ) // if there’s active player in room
        {
                int dx = thisItem->getX() - activePlayer->getX();
                int dy = thisItem->getY() - activePlayer->getY();

                // get direction that allows to reach player as fast as possible
                // look on distances in X and Y between hunter and player

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

        if ( thisItem->getLabel() == "imperial-guard-head" && thisItem->checkPosition( 0, 0, -LayerHeight, Add ) )
        {
                created = true;

                // create new item in the same location
                FreeItem* newItem = new FreeItem( guardData,
                                                  thisItem->getX(), thisItem->getY(), thisItem->getZ() - LayerHeight,
                                                  thisItem->getOrientation() );

                newItem->assignBehavior( "behavior of hunter in four directions", 0 );

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
