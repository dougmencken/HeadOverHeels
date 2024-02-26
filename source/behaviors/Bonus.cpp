
#include "Bonus.hpp"

#include "FreeItem.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "BonusManager.hpp"
#include "SoundManager.hpp"


namespace behaviors
{

Bonus::Bonus( const ItemPtr & item, const std::string & behavior )
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
        bool isGone = false ;

        Mediator* mediator = item->getMediator();

        switch ( activity )
        {
                case activities::Activity::Waiting:
                        // is there an item above this one
                        if ( ! item->canAdvanceTo( 0, 0, 1 ) )
                        {
                                ItemPtr itemAbove = mediator->findCollisionPop( );

                                // is that above item the character which may take this bonus
                                if ( itemAbove != nilPointer
                                        && itemAbove->whichItemClass() == "avatar item"
                                                && mayTake( itemAbove->getOriginalKind() ) )
                                {
                                        activity = activities::Activity::Vanishing ;
                                        affectedBy = itemAbove ; // vanishing is caused by the character

                                        disappearanceTimer->reset();
                                }
                        }

                        item->animate();

                        // fall if it’s not taken
                        if ( activity != activities::Activity::Vanishing )
                        {
                                activity = activities::Activity::Falling;
                        }
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
                        // if the character touches the bonus item and may take this bonus
                        if ( affectedBy->whichItemClass() == "avatar item" && mayTake( affectedBy->getOriginalKind() ) )
                        {
                                activity = activities::Activity::Vanishing ;
                        }
                        // otherwise it's some other item which pushes the bonus
                        else if ( speedTimer->getValue() > item->getSpeed() )
                        {
                                activities::Displacing::getInstance().displace( this, &activity, true );

                                // after displaced, back to falling
                                activity = activities::Activity::Falling;

                                speedTimer->reset();
                        }
                        break;

                case activities::Activity::DraggedNorth:
                case activities::Activity::DraggedSouth:
                case activities::Activity::DraggedEast:
                case activities::Activity::DraggedWest:
                        // the bonus item is on a conveyor
                        if ( speedTimer->getValue() > item->getSpeed() )
                        {
                                activities::Displacing::getInstance().displace( this, &activity, true );

                                // after displaced, back to falling
                                activity = activities::Activity::Falling;

                                speedTimer->reset();
                        }
                        break;

                case activities::Activity::Falling:
                        // is it fall in room without floor
                        if ( item->getZ() == 0 && ! item->getMediator()->getRoom()->hasFloor() )
                        {
                                isGone = true;
                        }
                        // is it time to fall
                        else if ( fallTimer->getValue() > item->getWeight() )
                        {
                                if ( ! activities::Falling::getInstance().fall( this ) )
                                {
                                        activity = activities::Activity::Waiting;
                                }

                                fallTimer->reset();

                                // look if the bonus falls on the character
                                if ( ! item->canAdvanceTo( 0, 0, -1 ) )
                                {
                                        ItemPtr itemBelow = mediator->findCollisionPop( );

                                        // is that below item the character which may take this bonus
                                        if ( itemBelow != nilPointer && itemBelow->whichItemClass() == "avatar item" && mayTake( itemBelow->getOriginalKind() ) )
                                        {
                                                // get collisions of the avatar item with the items above it
                                                itemBelow->canAdvanceTo( 0, 0, 1 );

                                                // if that avatar item is just below the bonus then it’s okay to take it
                                                bool takeIt = ( mediator->depthOfStackOfCollisions() <= 1 );

                                                // disappear on take
                                                if ( takeIt )
                                                {
                                                        activity = activities::Activity::Vanishing ;
                                                        affectedBy = itemBelow ; // vanishing is caused by the character

                                                        disappearanceTimer->reset();
                                                }
                                        }
                                }
                        }
                        break;

                case activities::Activity::Vanishing:
                        if ( disappearanceTimer->getValue() > 0.100 )
                        {
                                isGone = true ;

                                // play the sound of taking
                                SoundManager::getInstance().play( item->getKind (), "vanish" );

                                // a bonus item disappears from the room once it's taken
                                BonusManager::getInstance().markAsAbsent(
                                        BonusInRoom(
                                                item->getKind (),
                                                item->getMediator()->getRoom()->getNameOfRoomDescriptionFile()
                                        ) );

                                takeIt( dynamic_cast< AvatarItem & >( * affectedBy ) );

                                if ( item->getOriginalKind() != "crown" ) // no bubbles for crowns
                                {
                                        item->setIgnoreCollisions( true );

                                        item->changeHeightTo( 0 );
                                        item->metamorphInto( "bubbles", "vanishing bonus item" );
                                        item->setBehaviorOf( "behavior of disappearance in time" );

                                        isGone = false ;
                                }
                        }
                        break;

                default:
                        ;
        }

        return isGone;
}

bool Bonus::mayTake( const std::string & character )
{
        std::string magicItem = this->item->getOriginalKind();

        if ( magicItem == "extra-life" || magicItem == "shield" ||
                magicItem == "reincarnation-fish" || magicItem == "crown" )
        {
                return true ;
        }

        return  ( character == "head"     &&  ( magicItem == "quick-steps" ||
                                                magicItem == "horn" ||
                                                magicItem == "donuts" ) )
                ||

                ( character == "heels"    &&  ( magicItem == "high-jumps" ||
                                                magicItem == "handbag" ) )
                ||

                ( character == "headoverheels" && ( magicItem == "handbag" ||
                                                magicItem == "horn" ||
                                                magicItem == "donuts" ) ) ;
}

void Bonus::takeIt( AvatarItem & whoTakes )
{
        std::string whichItem = this->item->getOriginalKind () ;

        if ( whichItem == "donuts" )
        {
                const unsigned short DonutsPerBox = 6 ;
                whoTakes.addDonuts( DonutsPerBox );
        }
        else if ( whichItem == "extra-life" )
        {
                whoTakes.addLives( 2 );
        }
        else if ( whichItem == "quick-steps" )
        {
                whoTakes.activateBonusQuickSteps () ;
        }
        else if ( whichItem == "high-jumps" )
        {
                whoTakes.addBonusHighJumps( 10 );
        }
        else if ( whichItem == "shield" )
        {
                whoTakes.activateShield () ;
        }
        else if ( whichItem == "crown" )
        {
                whoTakes.liberateCurrentPlanet () ;
        }
        else if ( whichItem == "horn" || whichItem == "handbag" )
        {
                whoTakes.takeMagicTool( whichItem );
        }
        else if ( whichItem == "reincarnation-fish" )
        {
                whoTakes.saveAt( this->item->getX (), this->item->getY (), this->item->getZ () );
        }
}

}
