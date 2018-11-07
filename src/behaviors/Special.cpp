
#include "Special.hpp"
#include "FreeItem.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "BonusManager.hpp"
#include "SoundManager.hpp"


namespace iso
{

Special::Special( const ItemPtr & item, const std::string & behavior )
        : Behavior( item, behavior )
        , disappearanceTimer( new Timer() )
        , speedTimer( new Timer() )
        , fallTimer( new Timer() )
{
        disappearanceTimer->go();
        speedTimer->go();
        fallTimer->go();
}

Special::~Special( )
{
}

bool Special::update ()
{
        bool isGone = false;

        Mediator* mediator = item->getMediator();

        switch ( activity )
        {
                case Activity::Wait:
                        // is there an item above this one
                        if ( ! item->canAdvanceTo( 0, 0, 1 ) )
                        {
                                ItemPtr itemAbove = mediator->findCollisionPop( );

                                // is that above item a player which may take this bonus
                                if ( itemAbove != nilPointer && itemAbove->whichKindOfItem() == "player item" && mayTake( itemAbove->getOriginalLabel() ) )
                                {
                                        bool takeIt = true;

                                        // get collisions of player item with items below it
                                        itemAbove->canAdvanceTo( 0, 0, -1 );

                                        // if that player item is just above bonus then it’s okay to take it
                                        // but when it’s on several items then look if all these items are also special or volatile
                                        if ( mediator->depthOfStackOfCollisions() > 1 )
                                        {
                                                takeIt = ( mediator->collisionWithByBehavior( "behavior of something special" ) == nilPointer ) &&
                                                                ( mediator->collisionWithByBehavior( "behavior of disappearance on jump into" ) == nilPointer ) &&
                                                                ( mediator->collisionWithByBehavior( "behavior of disappearance on touch" ) == nilPointer ) ;
                                        }

                                        // disappear on take
                                        if ( takeIt )
                                        {
                                                activity = Activity::Vanish;
                                                this->sender = itemAbove; // player is yet sender

                                                disappearanceTimer->reset();
                                        }
                                }
                        }

                        item->animate();

                        // fall if it’s not taken
                        if ( activity != Activity::Vanish )
                        {
                                activity = Activity::Fall;
                        }
                        break;

                case Activity::DisplaceNorth:
                case Activity::DisplaceSouth:
                case Activity::DisplaceEast:
                case Activity::DisplaceWest:
                case Activity::DisplaceNortheast:
                case Activity::DisplaceSoutheast:
                case Activity::DisplaceSouthwest:
                case Activity::DisplaceNorthwest:
                case Activity::DisplaceUp:
                        // if player touches bonus item and may take this bonus then it’s taken
                        if ( sender->whichKindOfItem() == "player item" && mayTake( sender->getOriginalLabel() ) )
                        {
                                activity = Activity::Vanish;
                        }
                        // some other item moves this bonus
                        else if ( speedTimer->getValue() > item->getSpeed() )
                        {
                                DisplaceKindOfActivity::getInstance().displace( this, &activity, true );

                                // after displacement, back to "fall" activity
                                activity = Activity::Fall;

                                speedTimer->reset();
                        }
                        break;

                case Activity::ForceDisplaceNorth:
                case Activity::ForceDisplaceSouth:
                case Activity::ForceDisplaceEast:
                case Activity::ForceDisplaceWest:
                        // bonus item is on conveyor belt
                        if ( speedTimer->getValue() > item->getSpeed() )
                        {
                                DisplaceKindOfActivity::getInstance().displace( this, &activity, true );

                                // after displacement, back to "fall" activity
                                activity = Activity::Fall;

                                speedTimer->reset();
                        }
                        break;

                case Activity::Fall:
                        // is it fall in room without floor
                        if ( item->getZ() == 0 && ! item->getMediator()->getRoom()->hasFloor() )
                        {
                                isGone = true;
                        }
                        // is it time to fall
                        else if ( fallTimer->getValue() > item->getWeight() )
                        {
                                if ( ! FallKindOfActivity::getInstance().fall( this ) )
                                {
                                        activity = Activity::Wait;
                                }

                                fallTimer->reset();

                                // look if bonus falls on player
                                if ( ! item->canAdvanceTo( 0, 0, -1 ) )
                                {
                                        ItemPtr itemBelow = mediator->findCollisionPop( );

                                        // is that below item a player which may take this bonus
                                        if ( itemBelow != nilPointer && itemBelow->whichKindOfItem() == "player item" && mayTake( itemBelow->getOriginalLabel() ) )
                                        {
                                                // get collisions of player item with items above it
                                                itemBelow->canAdvanceTo( 0, 0, 1 );

                                                // if that player item is just below bonus then it’s okay to take it
                                                bool takeIt = ( mediator->depthOfStackOfCollisions() <= 1 );

                                                // disappear on take
                                                if ( takeIt )
                                                {
                                                        activity = Activity::Vanish;
                                                        this->sender = itemBelow; // player is yet sender

                                                        disappearanceTimer->reset();
                                                }
                                        }
                                }
                        }
                        break;

                case Activity::Vanish:
                        if ( disappearanceTimer->getValue() > 0.100 )
                        {
                                isGone = true ;

                                // play sound of taking
                                SoundManager::getInstance().play( item->getLabel(), activity );

                                // bonus item disappears from room once it is taken
                                BonusManager::getInstance().markBonusAsAbsent( item->getMediator()->getRoom()->getNameOfFileWithDataAboutRoom(), item->getLabel() );

                                takeMagicItem( dynamic_cast< PlayerItem& >( *sender ) );

                                if ( item->getOriginalLabel() != "crown" ) // no bubbles for crowns
                                {
                                        item->setCollisionDetector( false );

                                        item->metamorphInto( "bubbles", "vanishing bonus item" );
                                        item->setBehaviorOf( "behavior of disappearance in time" );
                                        item->setHeight( 0 );

                                        isGone = false ;
                                }
                        }
                        break;

                default:
                        ;
        }

        return isGone;
}

bool Special::mayTake( const std::string& character )
{
        std::string magicItem = this->item->getOriginalLabel();

        return  ( character == "head"     &&  ( magicItem == "donuts" ||
                                                magicItem == "extra-life" ||
                                                magicItem == "high-speed" ||
                                                magicItem == "shield" ||
                                                magicItem == "crown" ||
                                                magicItem == "horn" ||
                                                magicItem == "reincarnation-fish" ) )
                ||

                ( character == "heels"    &&  ( magicItem == "extra-life" ||
                                                magicItem == "high-jumps" ||
                                                magicItem == "shield" ||
                                                magicItem == "crown" ||
                                                magicItem == "handbag" ||
                                                magicItem == "reincarnation-fish" ) )
                ||

                ( character == "headoverheels" && ( magicItem == "donuts" ||
                                                magicItem == "extra-life" ||
                                                magicItem == "shield" ||
                                                magicItem == "crown" ||
                                                magicItem == "horn" ||
                                                magicItem == "handbag" ||
                                                magicItem == "reincarnation-fish" ) ) ;
}

void Special::takeMagicItem( PlayerItem& whoTakes )
{
        std::string magicItem = this->item->getLabel();

        if ( magicItem == "donuts" )
        {
                const unsigned short DonutsPerBox = 6 ;
                whoTakes.addDoughnuts( DonutsPerBox );
        }
        else if ( magicItem == "extra-life" )
        {
                whoTakes.addLives( 2 );
        }
        else if ( magicItem == "high-speed" )
        {
                whoTakes.activateHighSpeed();
        }
        else if ( magicItem == "high-jumps" )
        {
                whoTakes.addHighJumps( 10 );
        }
        else if ( magicItem == "shield" )
        {
                whoTakes.activateShield();
        }
        else if ( magicItem == "crown" )
        {
                whoTakes.liberatePlanet();
        }
        else if ( magicItem == "horn" || magicItem == "handbag" )
        {
                whoTakes.takeTool( magicItem );
        }
        else if ( magicItem == "reincarnation-fish" )
        {
                whoTakes.saveAt( this->item->getX (), this->item->getY (), this->item->getZ () );
        }
}

}
