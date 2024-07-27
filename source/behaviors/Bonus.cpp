
#include "Bonus.hpp"

#include "AvatarItem.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "BonusManager.hpp"
#include "SoundManager.hpp"


namespace behaviors
{

Bonus::Bonus( FreeItem & item, const std::string & behavior )
        : Behavior( item, behavior )
        , disappearanceTimer( new Timer() )
        , speedTimer( new Timer() )
        , fallTimer( new Timer() )
{
        disappearanceTimer->go();
        speedTimer->go();
        fallTimer->go();
}

bool Bonus::update ()
{
        bool present = true ;

        Item & bonusItem = getItem ();
        Mediator * mediator = bonusItem.getMediator() ;

        switch ( getCurrentActivity () )
        {
                case activities::Activity::Waiting:
                        // is there an item above this one
                        if ( ! bonusItem.canAdvanceTo( 0, 0, 1 ) )
                        {
                                ItemPtr itemAbove = mediator->findCollisionPop( );

                                // can that above item take this bonus
                                if ( itemAbove != nilPointer && mayTake( * itemAbove ) ) {
                                        changeActivityDueTo( activities::Activity::Vanishing, itemAbove );
                                        disappearanceTimer->go() ;
                                }
                        }

                        bonusItem.animate() ;

                        // fall if it’s not taken
                        if ( getCurrentActivity() != activities::Activity::Vanishing )
                                setCurrentActivity( activities::Activity::Falling );

                        break;

                case activities::Activity::PushedNorth:
                case activities::Activity::PushedSouth:
                case activities::Activity::PushedEast:
                case activities::Activity::PushedWest:
                case activities::Activity::PushedNortheast:
                case activities::Activity::PushedSoutheast:
                case activities::Activity::PushedSouthwest:
                case activities::Activity::PushedNorthwest:
                case activities::Activity::PushedUp:
                {
                        const ItemPtr & taker = getWhatAffectedThisBehavior ();

                        // if may take it
                        if ( taker != nilPointer && mayTake( * taker ) ) {
                                changeActivityDueTo( activities::Activity::Vanishing, taker );
                        }
                        // otherwise some other item is pushing the bonus
                        else if ( speedTimer->getValue() > bonusItem.getSpeed() )
                        {
                                activities::Displacing::getInstance().displace( *this, true );

                                // to falling
                                setCurrentActivity( activities::Activity::Falling );

                                speedTimer->go() ;
                        }
                }
                        break;

                case activities::Activity::DraggedNorth:
                case activities::Activity::DraggedSouth:
                case activities::Activity::DraggedEast:
                case activities::Activity::DraggedWest:
                        // the bonus item is on a conveyor
                        if ( speedTimer->getValue() > bonusItem.getSpeed() )
                        {
                                activities::Displacing::getInstance().displace( *this, true );

                                // to falling
                                setCurrentActivity( activities::Activity::Falling );

                                speedTimer->go() ;
                        }
                        break;

                case activities::Activity::Falling:
                        if ( bonusItem.getZ() == 0 && ! bonusItem.getMediator()->getRoom()->hasFloor() )
                        {       // disappear when at the bottom of a room without floor
                                present = false ;
                        }
                        // is it time to fall
                        else if ( fallTimer->getValue() > bonusItem.getWeight() )
                        {
                                if ( ! activities::Falling::getInstance().fall( *this ) )
                                {
                                        // to waiting
                                        setCurrentActivity( activities::Activity::Waiting );
                                }

                                fallTimer->go() ;

                                // look if the bonus falls on a character
                                if ( ! bonusItem.canAdvanceTo( 0, 0, -1 ) )
                                {
                                        ItemPtr itemBelow = mediator->findCollisionPop( );

                                        // can that below item take this bonus
                                        if ( itemBelow != nilPointer && mayTake( * itemBelow ) )
                                        {
                                                // get collisions with the bonus and other items above
                                                itemBelow->canAdvanceTo( 0, 0, 1 );

                                                // if that avatar item is just below the bonus then it’s okay to take it
                                                if ( mediator->howManyCollisions() <= 1 ) {
                                                        // disappear when taken
                                                        changeActivityDueTo( activities::Activity::Vanishing, itemBelow );

                                                        disappearanceTimer->go() ;
                                                }
                                        }
                                }
                        }
                        break;

                case activities::Activity::Vanishing:
                        if ( disappearanceTimer->getValue() > 0.100 )
                        {
                                // play the sound of taking
                                SoundManager::getInstance().play( bonusItem.getKind (), "vanish" );

                                // a bonus item disappears from the room once it’s taken
                                BonusManager::getInstance().markAsAbsent(
                                        BonusInRoom(
                                                bonusItem.getKind (),
                                                bonusItem.getMediator()->getRoom()->getNameOfRoomDescriptionFile ()
                                        ) );

                                const ItemPtr & taker = getWhatAffectedThisBehavior ();
                                if ( taker != nilPointer && taker->whichItemClass() == "avatar item" )
                                        takeIt( dynamic_cast< AvatarItem & >( * taker ) );

                                if ( bonusItem.getOriginalKind() != "crown" )
                                {
                                        bonusItem.setIgnoreCollisions( true );

                                        bonusItem.changeHeightTo( 0 );
                                        bonusItem.metamorphInto( "bubbles", "vanishing bonus item" );
                                        bonusItem.setBehaviorOf( "behavior of disappearance in time" );
                                }
                                else
                                        present = false ; // no bubbles for a crown, just disappear
                        }
                        break;

                default:
                        ;
        }

        return present ;
}

bool Bonus::mayTake( const Item & taker )
{
        // only a character can take a bonus
        if ( taker.whichItemClass() != "avatar item" ) return false ;

        const AvatarItem & character = dynamic_cast< const AvatarItem & >( taker );

        const std::string & magicItem = getItem().getOriginalKind ();

        if ( magicItem == "extra-life" || magicItem == "shield" ||
                magicItem == "reincarnation-fish" || magicItem == "crown" )
        {
                return true ;
        }

        return     ( character.isHead() && ( magicItem == "quick-steps" ||
                                             magicItem == "horn" ||
                                             magicItem == "donuts" ) )
                || ( character.isHeels()  && ( magicItem == "high-jumps" ||
                                               magicItem == "handbag" ) )
                || ( character.isHeadOverHeels() && ( magicItem == "handbag" ||
                                                      magicItem == "horn" ||
                                                      magicItem == "donuts" ) ) ;
}

void Bonus::takeIt( AvatarItem & whoTakes )
{
        Item & it = getItem() ;
        const std::string & whichItem = it.getOriginalKind ();

        if ( whichItem == "donuts" ) {
                const unsigned short DonutsPerBox = 6 ;
                whoTakes.addDonuts( DonutsPerBox );
        }
        else if ( whichItem == "extra-life" ) {
                whoTakes.addLives( 2 );
        }
        else if ( whichItem == "quick-steps" ) {
                whoTakes.activateBonusQuickSteps () ;
        }
        else if ( whichItem == "high-jumps" ) {
                whoTakes.addBonusHighJumps( 10 );
        }
        else if ( whichItem == "shield" ) {
                whoTakes.activateShield () ;
        }
        else if ( whichItem == "crown" ) {
                whoTakes.liberateCurrentPlanet () ;
        }
        else if ( whichItem == "horn" || whichItem == "handbag" ) {
                whoTakes.takeMagicTool( whichItem );
        }
        else if ( whichItem == "reincarnation-fish" ) {
                whoTakes.saveGame () ;
        }
}

}
