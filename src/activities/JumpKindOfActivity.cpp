
#include "JumpKindOfActivity.hpp"
#include "Behavior.hpp"
#include "PlayerItem.hpp"
#include "Mediator.hpp"
#include "GameManager.hpp"


namespace iso
{

JumpKindOfActivity * JumpKindOfActivity::instance = nilPointer ;

JumpKindOfActivity& JumpKindOfActivity::getInstance()
{
        if ( instance == nilPointer )
        {
                instance = new JumpKindOfActivity();
        }

        return *instance;
}


JumpKindOfActivity::JumpKindOfActivity( ) : KindOfActivity()
{

}

JumpKindOfActivity::~JumpKindOfActivity( )
{

}

bool JumpKindOfActivity::jump( Behavior* behavior, ActivityOfItem* activity, unsigned int jumpPhase, const std::vector< std::pair< int /* xy */, int /* z */ > >& jumpVector )
{
        bool itemMoved = false;
        ActivityOfItem displaceActivity = Activity::Wait;
        PlayerItem& playerItem = dynamic_cast< PlayerItem& >( * behavior->getItem() );
        Mediator* mediator = playerItem.getMediator();

        int deltaXY = jumpVector[ jumpPhase ].first ;
        int deltaZ = jumpVector[ jumpPhase ].second ;

        if ( GameManager::getInstance().charactersFly() )
        {
                deltaXY = 0;

                if ( deltaZ < 0 ) deltaZ = 0;
                else deltaZ = 2;

                *activity = Activity::Fall;
        }

        // let’s move up
        if ( ! playerItem.addToZ( deltaZ ) )
        {
                // if can’t, raise pile of items above
                if ( deltaZ > 0 )
                {
                        while ( ! mediator->isStackOfCollisionsEmpty() )
                        {
                                std::string collision = mediator->popCollision ();

                                if ( collision == "ceiling" && playerItem.isActiveCharacter() )
                                {
                                        playerItem.setWayOfExit( "above" );
                                        continue ;
                                }

                                ItemPtr item = mediator->findItemByUniqueName( collision );
                                if ( item == nilPointer ) continue ;

                                // mortal thing is above
                                if ( item->isMortal() && ! playerItem.hasShield() )
                                {
                                        if ( ! GameManager::getInstance().isImmuneToCollisionsWithMortalItems () )
                                        {
                                                playerItem.getBehavior()->changeActivityOfItem( Activity::MeetMortalItem );
                                        }
                                }
                                else
                                {
                                        // non mortal free item
                                        if ( item->whichKindOfItem() == "free item" || item->whichKindOfItem() == "player item" )
                                        {
                                                // raise items recursively
                                                lift( playerItem, *item, deltaZ - ( jumpPhase > 0 && jumpPhase % 2 == 0 ? 1 : 2 ) );
                                        }
                                }
                        }

                        // yet you may ascend
                        playerItem.addToZ( deltaZ - ( jumpPhase > 0 && jumpPhase % 2 == 0 ? 1 : 2 ) );
                }
        }

        std::string orientation = playerItem.getOrientation() ;

        if ( orientation == "north" )
        {
                itemMoved = playerItem.addToX( - deltaXY );
                displaceActivity = Activity::DisplaceNorth ;
        }
        else if ( orientation == "south" )
        {
                itemMoved = playerItem.addToX( deltaXY );
                displaceActivity = Activity::DisplaceSouth ;
        }
        else if ( orientation == "east" )
        {
                itemMoved = playerItem.addToY( - deltaXY );
                displaceActivity = Activity::DisplaceEast ;
        }
        else if ( orientation == "west" )
        {
                itemMoved = playerItem.addToY( deltaXY );
                displaceActivity = Activity::DisplaceWest ;
        }

        // displace adjacent items when there’s horizontal collision
        if ( ! itemMoved || ( itemMoved && jumpPhase > 4 ) )
        {
                // is it okay to move items above
                // it is okay after fourth phase of jump so player can get rid of item above
                this->propagateActivityToAdjacentItems( playerItem, displaceActivity );
        }

        // end jump when it’s last phase
        if ( ( jumpPhase + 1 ) >= jumpVector.size() )
        {
                *activity = Activity::Fall;
        }

        return itemMoved ;
}

void JumpKindOfActivity::lift( FreeItem& sender, Item& item, int z )
{
        // only for item with behavior
        if ( item.getBehavior() != nilPointer )
        {
                // when item is volatile
                if ( item.getBehavior()->getNameOfBehavior () == "behavior of disappearance on touch" ||
                                item.getBehavior()->getNameOfBehavior () == "behavior of something special" )
                {
                        item.getBehavior()->changeActivityOfItem( Activity::DisplaceUp, sender );
                }
                // raise item when it’s not elevator
                else if ( item.getBehavior()->getNameOfBehavior () != "behavior of elevator" )
                {
                        // is there’s something above
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
