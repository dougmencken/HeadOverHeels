
#include "Bonus.hpp"

#include "AvatarItem.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "BonusManager.hpp"
#include "SoundManager.hpp"
#include "Volatile.hpp"


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

        FreeItem & bonusItem = dynamic_cast< FreeItem & >( getItem () );
        Mediator * mediator = bonusItem.getMediator() ;

        switch ( getCurrentActivity () )
        {
                case activities::Activity::Waiting:
                        // is there an item above this one
                        if ( ! bonusItem.canAdvanceTo( 0, 0, 1 ) )
                        {
                                DescribedItemPtr itemAbove = mediator->findCollisionPop( );

                                // can that above item take this bonus
                                if ( itemAbove != nilPointer
                                                && itemAbove->whichItemClass() == "avatar item"
                                                        && mayTake( dynamic_cast< AvatarItem & >( *itemAbove ) ) ) {
                                        changeActivityDueTo( activities::Activity::Vanishing, itemAbove );
                                        disappearanceTimer->go() ;
                                }
                        }

                        bonusItem.animate() ;

                        // fall if it’s not taken
                        if ( getCurrentActivity() != activities::Activity::Vanishing )
                                setCurrentActivity( activities::Activity::Falling, Motion2D::rest() );

                        break;

                case activities::Activity::Pushed:
                case activities::Activity::PushedUp:
                {
                        const AbstractItemPtr & taker = getWhatAffectedThisBehavior ();

                        // if may take it
                        if ( taker != nilPointer
                                        && taker->whichItemClass() == "avatar item"
                                                && mayTake( dynamic_cast< AvatarItem & >( *taker ) ) ) {
                                changeActivityDueTo( activities::Activity::Vanishing, taker );
                        }
                        // otherwise some other item is pushing the bonus
                        else if ( speedTimer->getValue() > bonusItem.getSpeed() )
                        {
                                activities::Displacing::getInstance().displace( *this, true );

                                // to falling
                                setCurrentActivity( activities::Activity::Falling, Motion2D::rest() );

                                speedTimer->go() ;
                        }
                }
                        break;

                case activities::Activity::Dragged:
                        // the bonus item is on a conveyor
                        if ( speedTimer->getValue() > bonusItem.getSpeed() )
                        {
                                activities::Displacing::getInstance().displace( *this, true );

                                // to falling
                                setCurrentActivity( activities::Activity::Falling, Motion2D::rest() );

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
                                        beWaiting() ;

                                fallTimer->go() ;

                                // look if the bonus falls on a character
                                if ( ! bonusItem.canAdvanceTo( 0, 0, -1 ) )
                                {
                                        DescribedItemPtr itemBelow = mediator->findCollisionPop( );

                                        // can that below item take this bonus
                                        if ( itemBelow != nilPointer
                                                        && itemBelow->whichItemClass() == "avatar item"
                                                                && mayTake( dynamic_cast< AvatarItem & >( *itemBelow ) ) ) {
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
                        break ;

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

                                const AbstractItemPtr & taker = getWhatAffectedThisBehavior ();
                                if ( taker != nilPointer && taker->whichItemClass() == "avatar item" )
                                        takeIt( dynamic_cast< AvatarItem & >( * taker ) );

                                if ( bonusItem.getOriginalKind() != "crown" )
                                {
                                        bonusItem.setIgnoreCollisions( true );

                                        bonusItem.changeHeightTo( 0 );
                                        bonusItem.metamorphInto( "bubbles", "vanishing bonus item" );
                                        bonusItem.setBehaviorOf( behaviors::Volatile::after_a_while );
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

bool Bonus::mayTake( const AvatarItem & taker )
{
        // only a character can take a bonus
        /* if ( taker.whichItemClass() != "avatar item" ) return false ; */ // guaranteed by the class of taker

        const std::string & magicItem = dynamic_cast< DescribedItem & >( getItem() ).getOriginalKind ();

        if ( magicItem == "extra-life" || magicItem == "shield" ||
                magicItem == "reincarnation-fish" || magicItem == "crown" )
        {
                return true ;
        }

        return ( taker.isHead() && ( magicItem == "quick-steps" || magicItem == "horn" || magicItem == "donuts" ) )
                        || ( taker.isHeels()  && ( magicItem == "high-jumps" || magicItem == "handbag" ) )
                || ( taker.isHeadOverHeels() && ( magicItem == "handbag" || magicItem == "horn" || magicItem == "donuts" ) ) ;
}

void Bonus::takeIt( AvatarItem & whoTakes )
{
        const std::string & whichItem = dynamic_cast< DescribedItem & >( getItem() ).getOriginalKind ();

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
