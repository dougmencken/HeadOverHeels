
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
        {
                instance = new Jumping();
        }

        return *instance;
}


bool Jumping::jump( behaviors::Behavior* behavior, ActivityOfItem* activity, unsigned int jumpPhase, const std::vector< std::pair< int /* xy */, int /* z */ > >& jumpVector )
{
        bool itemMoved = false;
        ActivityOfItem displaceActivity = activities::Activity::Waiting;
        AvatarItem & characterItem = dynamic_cast< AvatarItem & >( * behavior->getItem() );
        Mediator* mediator = characterItem.getMediator();

        int deltaXY = jumpVector[ jumpPhase ].first ;
        int deltaZ = jumpVector[ jumpPhase ].second ;

        if ( GameManager::getInstance().charactersFly() )
        {
                deltaXY = 0;

                if ( deltaZ < 0 ) deltaZ = 0;
                else deltaZ = 2;

                *activity = activities::Activity::Fall;
        }

        // let’s move up
        if ( ! characterItem.addToZ( deltaZ ) )
        {
                // if can’t, raise the pile of items above
                if ( deltaZ > 0 )
                {
                        while ( ! mediator->isStackOfCollisionsEmpty() )
                        {
                                std::string collision = mediator->popCollision ();

                                if ( collision == "ceiling" && characterItem.isActiveCharacter() )
                                {
                                        characterItem.setWayOfExit( "above" );
                                        continue ;
                                }

                                ItemPtr item = mediator->findItemByUniqueName( collision );
                                if ( item == nilPointer ) continue ;

                                // a mortal thing is above
                                if ( item->isMortal() )
                                {
                                        characterItem.getBehavior()->setActivityOfItem( activities::Activity::MeetMortalItem );
                                }
                                else
                                {
                                        // non mortal free item
                                        if ( item->whichItemClass() == "free item" || item->whichItemClass() == "avatar item" )
                                        {
                                                // raise items recursively
                                                lift( characterItem, *item, deltaZ - ( jumpPhase > 0 && jumpPhase % 2 == 0 ? 1 : 2 ) );
                                        }
                                }
                        }

                        // yet you may ascend
                        characterItem.addToZ( deltaZ - ( jumpPhase > 0 && jumpPhase % 2 == 0 ? 1 : 2 ) );
                }
        }

        std::string orientation = characterItem.getOrientation() ;

        if ( orientation == "north" )
        {
                itemMoved = characterItem.addToX( - deltaXY );
                displaceActivity = activities::Activity::PushedNorth ;
        }
        else if ( orientation == "south" )
        {
                itemMoved = characterItem.addToX( deltaXY );
                displaceActivity = activities::Activity::PushedSouth ;
        }
        else if ( orientation == "east" )
        {
                itemMoved = characterItem.addToY( - deltaXY );
                displaceActivity = activities::Activity::PushedEast ;
        }
        else if ( orientation == "west" )
        {
                itemMoved = characterItem.addToY( deltaXY );
                displaceActivity = activities::Activity::PushedWest ;
        }

        // displace adjacent items when there’s horizontal collision
        if ( ! itemMoved || ( itemMoved && jumpPhase > 4 ) )
        {
                // is it okay to move items above
                // it is okay after the fourth phase of jump so the character can get rid of the item above
                PropagateActivity::toAdjacentItems( characterItem, displaceActivity );
        }

        // end jump when it’s the last phase
        if ( ( jumpPhase + 1 ) >= jumpVector.size() )
        {
                *activity = activities::Activity::Fall ;
        }

        return itemMoved ;
}

void Jumping::lift( FreeItem& sender, Item& item, int z )
{
        const autouniqueptr< Behavior > & behavior = item.getBehavior() ;
        if ( behavior != nilPointer )
        {
                // when item is volatile
                if ( behavior->getNameOfBehavior () == "behavior of disappearance on touch" ||
                                behavior->getNameOfBehavior () == "behavior of bonus" )
                {
                        behavior->changeActivityOfItemDueTo( activities::Activity::PushedUp, ItemPtr( &sender ) );
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
                                        ItemPtr topItem = mediator->findCollisionPop();

                                        if ( topItem != nilPointer )
                                        {
                                                // raise free items recursively
                                                lift( sender, *topItem, z );
                                        }
                                }

                                // ahora ya puede ascender
                                item.addToZ( z );
                        }
                }
        }
}

}
