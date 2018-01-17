
#include "Special.hpp"
#include "Item.hpp"
#include "ItemData.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "BonusManager.hpp"
#include "SoundManager.hpp"


namespace isomot
{

Special::Special( Item * item, const std::string & behavior ) :
        Behavior( item, behavior )
{
        disappearanceTimer = new Timer();
        speedTimer = new Timer();
        fallTimer = new Timer();

        disappearanceTimer->go();
        speedTimer->go();
        fallTimer->go();
}

Special::~Special( )
{
        delete disappearanceTimer;
}

bool Special::update ()
{
        bool isGone = false;

        Mediator* mediator = item->getMediator();

        switch ( activity )
        {
                case Wait:
                        // is there an item above this one
                        if ( ! item->checkPosition( 0, 0, 1, Add ) )
                        {
                                bool takeIt = true;
                                Item* itemAbove = mediator->findCollisionPop( );

                                // is that above item a player which may take this bonus
                                if ( dynamic_cast< PlayerItem * >( itemAbove ) && mayTake( itemAbove ) )
                                {
                                        // get collisions of player item with items below it
                                        itemAbove->checkPosition( 0, 0, -1, Add );

                                        // if that player item is just above bonus then it’s okay to take it
                                        // but when it’s on several items then look if all these items are also special or volatile
                                        if ( mediator->depthOfStackOfCollisions() > 1 )
                                        {
                                                takeIt = ( mediator->collisionWithByBehavior( "behavior of something special" ) == 0 ) &&
                                                                ( mediator->collisionWithByBehavior( "behavior of disappearance on jump into" ) == 0 ) &&
                                                                ( mediator->collisionWithByBehavior( "behavior of disappearance on touch" ) == 0 ) ;
                                        }

                                        // disappear on take
                                        if ( takeIt )
                                        {
                                                activity = Vanish;
                                                this->sender = itemAbove; // player is yet sender

                                                disappearanceTimer->reset();
                                        }
                                }
                        }

                        item->animate();

                        // fall if it’s not taken
                        if ( activity != Vanish )
                        {
                                activity = Fall;
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
                case DisplaceUp:
                        // if player touches bonus item and may take this bonus then it’s taken
                        if ( dynamic_cast< PlayerItem * >( sender ) && mayTake( sender ) )
                        {
                                activity = Vanish;
                        }
                        // some other item moves this bonus
                        else if ( speedTimer->getValue() > item->getSpeed() )
                        {
                                whatToDo = DisplaceKindOfActivity::getInstance();
                                whatToDo->displace( this, &activity, true );

                                // after displacement, back to "fall" activity
                                activity = Fall;

                                speedTimer->reset();
                        }
                        break;

                case ForceDisplaceNorth:
                case ForceDisplaceSouth:
                case ForceDisplaceEast:
                case ForceDisplaceWest:
                        // bonus item is on conveyor belt
                        if ( speedTimer->getValue() > item->getSpeed() )
                        {
                                whatToDo = DisplaceKindOfActivity::getInstance();
                                whatToDo->displace( this, &activity, true );

                                // after displacement, back to "fall" activity
                                activity = Fall;

                                speedTimer->reset();
                        }
                        break;

                case Fall:
                        // is it fall in room without floor
                        if ( item->getZ() == 0 && item->getMediator()->getRoom()->getKindOfFloor() == "none" )
                        {
                                isGone = true;
                        }
                        // is it time to fall
                        else if ( fallTimer->getValue() > item->getWeight() )
                        {
                                whatToDo = FallKindOfActivity::getInstance();
                                if ( ! whatToDo->fall( this ) )
                                {
                                        activity = Wait;
                                }

                                fallTimer->reset();

                                // look if bonus falls on player
                                if ( ! item->checkPosition( 0, 0, -1, Add ) )
                                {
                                        Item* itemBelow = mediator->findCollisionPop( );

                                        // is that below item a player which may take this bonus
                                        if ( dynamic_cast< PlayerItem * >( itemBelow ) && mayTake( itemBelow ) )
                                        {
                                                // get collisions of player item with items above it
                                                itemBelow->checkPosition( 0, 0, 1, Add );

                                                // if that player item is just below bonus then it’s okay to take it
                                                bool takeIt = ( mediator->depthOfStackOfCollisions() <= 1 );

                                                // disappear on take
                                                if ( takeIt )
                                                {
                                                        activity = Vanish;
                                                        this->sender = itemBelow; // player is yet sender

                                                        disappearanceTimer->reset();
                                                }
                                        }
                                }
                        }
                        break;

                case Vanish:
                        if ( disappearanceTimer->getValue() > 0.100 )
                        {
                                isGone = true;

                                // play sound of taking
                                SoundManager::getInstance()->play( item->getLabel(), activity );

                                // bonus item disappears from room once it is taken
                                BonusManager::getInstance()->markBonusAsAbsent( item->getMediator()->getRoom()->getNameOfFileWithDataAboutRoom(), item->getLabel() );

                                takeMagicItem( static_cast< PlayerItem* >( sender ) );

                                // create item "bubbles" in the place of magic item
                                FreeItem* bubbles = new FreeItem( bubblesData, item->getX(), item->getY(), item->getZ(), Nowhere );

                                bubbles->assignBehavior( "behavior of disappearance in time", 0 );
                                bubbles->setCollisionDetector( false );

                                // add to current room
                                mediator->getRoom()->addFreeItem( bubbles );
                        }
                        break;

                default:
                        ;
        }

        return isGone;
}

bool Special::mayTake( Item* sender )
{
        std::string player = sender->getLabel();
        std::string magicItem = this->item->getLabel();

        return  ( player == "head"      &&  ( magicItem == "donuts" ||
                                                magicItem == "extra-life" ||
                                                magicItem == "high-speed" ||
                                                magicItem == "shield" ||
                                                magicItem == "crown" ||
                                                magicItem == "horn" ||
                                                magicItem == "reincarnation-fish" ) )
                ||

                ( player == "heels"     &&  ( magicItem == "extra-life" ||
                                                magicItem == "high-jumps" ||
                                                magicItem == "shield" ||
                                                magicItem == "crown" ||
                                                magicItem == "handbag" ||
                                                magicItem == "reincarnation-fish" ) )
                ||

                ( player == "headoverheels"  &&  ( magicItem == "donuts" ||
                                                magicItem == "extra-life" ||
                                                magicItem == "shield" ||
                                                magicItem == "crown" ||
                                                magicItem == "horn" ||
                                                magicItem == "handbag" ||
                                                magicItem == "reincarnation-fish" ) ) ;
}

void Special::takeMagicItem( PlayerItem* who )
{
        std::string magicItem = this->item->getLabel();

        if ( magicItem == "donuts" )
        {
                const unsigned short DonutsPerBox = 6 ;
                who->addDoughnuts( DonutsPerBox );
        }
        else if ( magicItem == "extra-life" )
        {
                who->addLives( 2 );
        }
        else if ( magicItem == "high-speed" )
        {
                who->activateHighSpeed();
        }
        else if ( magicItem == "high-jumps" )
        {
                who->addHighJumps( 10 );
        }
        else if ( magicItem == "shield" )
        {
                who->activateShield();
        }
        else if ( magicItem == "crown" )
        {
                who->liberatePlanet();
        }
        else if ( magicItem == "horn" || magicItem == "handbag" )
        {
                who->takeTool( magicItem );
        }
        else if ( magicItem == "reincarnation-fish" )
        {
                who->saveAt( this->item->getX (), this->item->getY (), this->item->getZ () );
        }
}

void Special::setMoreData( void* data )
{
        this->bubblesData = reinterpret_cast< ItemData* >( data );
}

}
