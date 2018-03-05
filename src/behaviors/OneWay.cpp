
#include "OneWay.hpp"
#include "Item.hpp"
#include "ItemData.hpp"
#include "FreeItem.hpp"
#include "MoveKindOfActivity.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"


namespace isomot
{

OneWay::OneWay( Item * item, const std::string & behavior, bool flying ) :
        Behavior( item, behavior )
        , isFlying( flying )
        , speedTimer( nilPointer )
        , fallTimer( nilPointer )
{
        speedTimer = new Timer();
        speedTimer->go();

        if ( ! flying )
        {
                fallTimer = new Timer();
                fallTimer->go();
        }
}

OneWay::~OneWay()
{
        delete speedTimer ;
        delete fallTimer ;
}

bool OneWay::update ()
{
        bool vanish = false;
        FreeItem * freeItem = dynamic_cast< FreeItem * >( this->item );

        switch ( activity )
        {
                case Wait:
                        letsMove();
                        break;

                case MoveNorth:
                case MoveSouth:
                case MoveEast:
                case MoveWest:
                        if ( ! freeItem->isFrozen() )
                        {
                                if ( speedTimer->getValue() > freeItem->getSpeed() )
                                {
                                        // move it
                                        if ( ! MoveKindOfActivity::getInstance()->move( this, &activity, true ) )
                                        {
                                                turnRound();

                                                // play sound of colliding
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
                case DisplaceSoutheast:
                case DisplaceSouthwest:
                case DisplaceNorthwest:
                        if ( ! this->isFlying )
                        {
                                // emit sound of displacing
                                SoundManager::getInstance()->play( freeItem->getLabel(), activity );

                                // displace this item by other one
                                DisplaceKindOfActivity::getInstance()->displace( this, &activity, true );

                                activity = Wait;

                                // preserve inactivity for frozen item
                                if ( freeItem->isFrozen() )
                                        activity = Freeze;
                        }
                        else
                        {
                                activity = Wait;
                        }
                        break;

                case Fall:
                        if ( ! this->isFlying )
                        {
                                // look for reaching floor in a room without floor
                                if ( freeItem->getZ() == 0 && freeItem->getMediator()->getRoom()->getKindOfFloor() == "none" )
                                {
                                        // item disappears
                                        vanish = true;
                                }
                                // is it time to fall
                                else if ( fallTimer->getValue() > freeItem->getWeight() )
                                {
                                        if ( ! FallKindOfActivity::getInstance()->fall( this ) )
                                        {
                                                // emit sound of falling down
                                                SoundManager::getInstance()->play( freeItem->getLabel(), activity );
                                                activity = Wait;
                                        }

                                        fallTimer->reset();
                                }
                        }
                        else
                        {
                                activity = Wait;
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

        return vanish;
}

void OneWay::letsMove()
{
        switch ( this->item->getOrientation().getIntegerOfWay () )
        {
                case North:
                        activity = MoveNorth;
                        break;

                case South:
                        activity = MoveSouth;
                        break;

                case East:
                        activity = MoveEast;
                        break;

                case West:
                        activity = MoveWest;
                        break;

                default:
                        ;
        }
}

void OneWay::turnRound()
{
        switch ( this->item->getOrientation().getIntegerOfWay () )
        {
                case North:
                        activity = MoveSouth;
                        this->item->changeOrientation( South );
                        break;

                case South:
                        activity = MoveNorth;
                        this->item->changeOrientation( North );
                        break;

                case East:
                        activity = MoveWest;
                        this->item->changeOrientation( West );
                        break;

                case West:
                        activity = MoveEast;
                        this->item->changeOrientation( East );
                        break;

                default:
                        ;
        }
}

}
