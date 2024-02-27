
#include "Jumping.hpp"

#include "AvatarItem.hpp"
#include "Mediator.hpp"
#include "PropagateActivity.hpp"
#include "GameManager.hpp"


namespace activities
{

Jumping * Jumping::instance = nilPointer ;

Jumping & Jumping::getInstance()
{
        if ( instance == nilPointer )
                instance = new Jumping () ;

        return *instance;
}

bool Jumping::jump( behaviors::Behavior* behavior, Activity* activity, unsigned int jumpPhase, const std::vector< std::pair< int /* xy */, int /* z */ > >& jumpVector )
{
        bool jumped = false ;

        Activity displaceActivity = activities::Activity::Waiting ;

        AvatarItem & character = dynamic_cast< AvatarItem & >( * behavior->getItem() );
        Mediator* mediator = character.getMediator() ;

        int deltaXY = jumpVector[ jumpPhase ].first ;
        int deltaZ = jumpVector[ jumpPhase ].second ;

        if ( GameManager::getInstance().charactersFly() )
        {
                deltaXY = 0;

                if ( deltaZ < 0 ) deltaZ = 0;
                else deltaZ = 2;

                *activity = activities::Activity::Falling;
        }

        // let’s move up
        if ( ! character.addToZ( deltaZ ) )
        {
                // if can’t, raise the pile of items above
                if ( deltaZ > 0 )
                {
                        while ( ! mediator->isStackOfCollisionsEmpty() )
                        {
                                const std::string & collision = mediator->popCollision ();

                                if ( collision == "ceiling" && character.isActiveCharacter() )
                                {
                                        character.setWayOfExit( "above" );
                                        continue ;
                                }

                                ItemPtr item = mediator->findItemByUniqueName( collision );
                                if ( item == nilPointer ) continue ;

                                if ( item->isMortal() ) {
                                        // a lethal thing is above
                                        character.getBehavior()->setCurrentActivity( activities::Activity::MetLethalItem );
                                }
                                else {  // a harmless free item
                                        if ( item->whichItemClass() == "free item" || item->whichItemClass() == "avatar item" )
                                        {
                                                // raise items recursively
                                                lift( character, *item, deltaZ - ( jumpPhase > 0 && jumpPhase % 2 == 0 ? 1 : 2 ) );
                                        }
                                }
                        }

                        // yet you may ascend
                        character.addToZ( deltaZ - ( jumpPhase > 0 && jumpPhase % 2 == 0 ? 1 : 2 ) );
                }
        }

        const std::string & heading = character.getHeading () ;

        if ( heading == "north" )
        {
                jumped = character.addToX( - deltaXY );
                displaceActivity = activities::Activity::PushedNorth ;
        }
        else if ( heading == "south" )
        {
                jumped = character.addToX( deltaXY );
                displaceActivity = activities::Activity::PushedSouth ;
        }
        else if ( heading == "east" )
        {
                jumped = character.addToY( - deltaXY );
                displaceActivity = activities::Activity::PushedEast ;
        }
        else if ( heading == "west" )
        {
                jumped = character.addToY( deltaXY );
                displaceActivity = activities::Activity::PushedWest ;
        }

        // displace adjacent items when there’s a horizontal collision
        if ( ! jumped || ( jumped && jumpPhase > 4 ) )
        {
                // is it okay to move items above
                // it is okay after the fourth phase of jump so the character can get rid of the item above
                PropagateActivity::toAdjacentItems( character, displaceActivity );
        }

        // end jump when it’s the last phase
        if ( ( jumpPhase + 1 ) >= jumpVector.size() )
        {
                *activity = activities::Activity::Falling ;
        }

        return jumped ;
}

void Jumping::lift( FreeItem & sender, Item & item, int z )
{
        const autouniqueptr< Behavior > & behavior = item.getBehavior() ;
        if ( behavior != nilPointer )
        {
                // when item is volatile
                if ( behavior->getNameOfBehavior () == "behavior of disappearance on touch" ||
                                behavior->getNameOfBehavior () == "behavior of bonus" )
                {
                        behavior->changeActivityDueTo( activities::Activity::PushedUp, ItemPtr( &sender ) );
                }
                // raise if the item isn’t an elevator
                else if ( behavior->getNameOfBehavior () != "behavior of elevator" )
                {
                        // is there something above
                        if ( ! item.addToZ( z ) )
                        {
                                Mediator* mediator = item.getMediator();

                                while ( ! mediator->isStackOfCollisionsEmpty() )
                                {
                                        ItemPtr itemAtop = mediator->findCollisionPop();

                                        if ( itemAtop != nilPointer )
                                        {
                                                // raise free items recursively
                                                lift( sender, *itemAtop, z );
                                        }
                                }

                                // now it can move up
                                item.addToZ( z );
                        }
                }
        }
}

}
