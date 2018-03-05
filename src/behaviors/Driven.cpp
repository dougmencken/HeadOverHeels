
#include "Driven.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "MoveKindOfActivity.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"


namespace isomot
{

Driven::Driven( Item* item, const std::string& behavior )
        : Behavior( item, behavior )
{
        running = false;

        speedTimer = new Timer();
        fallTimer = new Timer();
        speedTimer->go();
        fallTimer->go();
}

Driven::~Driven( )
{
        delete speedTimer;
        delete fallTimer;
}

bool Driven::update ()
{
        FreeItem* freeItem = dynamic_cast< FreeItem * >( this->item );
        Mediator* mediator = freeItem->getMediator();
        bool isGone = false;
        bool playerFound = false;

        switch ( activity )
        {
                case Wait:
                        if ( running )
                        {
                                switch ( freeItem->getOrientation().getIntegerOfWay() )
                                {
                                        case North:
                                                changeActivityOfItem( MoveNorth );
                                                break;

                                        case South:
                                                changeActivityOfItem( MoveSouth );
                                                break;

                                        case East:
                                                changeActivityOfItem( MoveEast );
                                                break;

                                        case West:
                                                changeActivityOfItem( MoveWest );
                                                break;

                                        default:
                                                ;
                                }
                        }
                        // when stopped, see if there is a character on it and use its orientation to begin moving
                        else
                        {
                                if ( ! freeItem->checkPosition( 0, 0, 1, Add ) )
                                {
                                        while ( ! mediator->isStackOfCollisionsEmpty() && ! playerFound )
                                        {
                                                Item * item = mediator->findCollisionPop ();

                                                if ( dynamic_cast< PlayerItem * >( item ) )
                                                {
                                                        playerFound = true;
                                                        running = true;

                                                        switch ( item->getOrientation().getIntegerOfWay () )
                                                        {
                                                                case North:
                                                                        changeActivityOfItem( MoveNorth );
                                                                        break;

                                                                case South:
                                                                        changeActivityOfItem( MoveSouth );
                                                                        break;

                                                                case East:
                                                                        changeActivityOfItem( MoveEast );
                                                                        break;

                                                                case West:
                                                                        changeActivityOfItem( MoveWest );
                                                                        break;

                                                                default:
                                                                        ;
                                                        }
                                                }
                                        }
                                }
                        }
                        break;

                case MoveNorth:
                case MoveSouth:
                case MoveEast:
                case MoveWest:
                        // item is active and it is time to move
                        if ( ! freeItem->isFrozen() )
                        {
                                if ( speedTimer->getValue() > freeItem->getSpeed() )
                                {
                                        if ( ! MoveKindOfActivity::getInstance()->move( this, &activity, true ) )
                                        {
                                                running = false;
                                                activity = Wait;

                                                // emit sound of collision
                                                SoundManager::getInstance()->play( freeItem->getLabel(), Collision );
                                        }

                                        speedTimer->reset();
                                }

                                freeItem->animate();
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
                        // is it time to move
                        if ( speedTimer->getValue() > freeItem->getSpeed() )
                        {
                                if ( ! DisplaceKindOfActivity::getInstance()->displace( this, &activity, true ) )
                                {
                                        activity = Wait;
                                }

                                speedTimer->reset();
                        }

                        // inactive item continues to be inactive
                        if ( freeItem->isFrozen() )
                        {
                                activity = Freeze;
                        }
                        break;

                case Fall:
                        // look for reaching floor in a room without floor
                        if ( freeItem->getZ() == 0 && freeItem->getMediator()->getRoom()->getKindOfFloor() == "none" )
                        {
                                // item disappears
                                isGone = true;
                        }
                        // is it time to fall
                        else if ( fallTimer->getValue() > freeItem->getWeight() )
                        {
                                if ( ! FallKindOfActivity::getInstance()->fall( this ) )
                                {
                                        activity = Wait;
                                }

                                fallTimer->reset();
                        }
                        break;

                case Freeze:
                        freeItem->setFrozen( true );
                        break;

                case WakeUp:
                        freeItem->setFrozen( false );
                        activity = Wait;
                        break;

                default:
                        ;
        }

        return isGone;
}

}
