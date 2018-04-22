
#include "JumpKindOfActivity.hpp"
#include "Behavior.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "Mediator.hpp"
#include "GameManager.hpp"


namespace isomot
{

JumpKindOfActivity * JumpKindOfActivity::instance = nilPointer ;

JumpKindOfActivity* JumpKindOfActivity::getInstance()
{
        if ( instance == nilPointer )
        {
                instance = new JumpKindOfActivity();
        }

        return instance;
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
        ActivityOfItem displaceActivity = Wait;
        PlayerItem* playerItem = dynamic_cast< PlayerItem * >( behavior->getItem() );
        Mediator* mediator = playerItem->getMediator();

        int deltaXY = jumpVector[ jumpPhase ].first ;
        int deltaZ = jumpVector[ jumpPhase ].second ;

        if ( GameManager::getInstance()->charactersFly() )
        {
                deltaXY = 0;

                if ( deltaZ < 0 ) deltaZ = 0;
                else deltaZ = 2;

                *activity = Fall;
        }

        // let’s move up
        if ( ! playerItem->addToZ( deltaZ ) )
        {
                // if can’t, raise pile of items above
                if ( deltaZ > 0 )
                {
                        while ( ! mediator->isStackOfCollisionsEmpty() )
                        {
                                std::string nameOfCollision = mediator->popCollision();
                                Item* item = mediator->findItemByUniqueName( nameOfCollision );

                                if ( item != nilPointer )
                                {
                                        // is it free item or grid item
                                        if ( item->whichKindOfItem() == "grid item" ||
                                                item->whichKindOfItem() == "free item" || item->whichKindOfItem() == "player item" )
                                        {
                                                // mortal thing is above
                                                if ( item->isMortal() && ! playerItem->hasShield() )
                                                {
                                                        if ( ! GameManager::getInstance()->isImmuneToCollisionsWithMortalItems () )
                                                        {
                                                                playerItem->getBehavior()->changeActivityOfItem( MeetMortalItem );
                                                        }
                                                }
                                                else
                                                {
                                                        FreeItem* freeItem = dynamic_cast< FreeItem * >( item );

                                                        // non mortal free item
                                                        if ( freeItem != nilPointer )
                                                        {
                                                                // raise items recursively
                                                                lift( playerItem, freeItem, deltaZ - ( jumpPhase > 0 && jumpPhase % 2 == 0 ? 1 : 2 ) );
                                                        }
                                                }
                                        }
                                }
                        }

                        // yet you may ascend
                        playerItem->addToZ( deltaZ - ( jumpPhase > 0 && jumpPhase % 2 == 0 ? 1 : 2 ) );
                }
        }

        switch ( playerItem->getOrientation().getIntegerOfWay() )
        {
                case North:
                        itemMoved = playerItem->addToX( - deltaXY );
                        displaceActivity = DisplaceNorth;
                        break;

                case South:
                        itemMoved = playerItem->addToX( deltaXY );
                        displaceActivity = DisplaceSouth;
                        break;

                case East:
                        itemMoved = playerItem->addToY( - deltaXY );
                        displaceActivity = DisplaceEast;
                        break;

                case West:
                        itemMoved = playerItem->addToY( deltaXY );
                        displaceActivity = DisplaceWest;
                        break;

                default:
                        ;
        }

        // displace adjacent items when there’s horizontal collision
        if ( ! itemMoved )
        {
                this->propagateActivityToAdjacentItems( playerItem, displaceActivity );
        }
        // is it okay to move items above
        // it is okay after fourth phase of jump so player can get rid of item above
        else if ( itemMoved && jumpPhase > 4 )
        {
                this->propagateActivityToItemsAbove( playerItem, displaceActivity );
        }

        // end jump when it’s last phase
        if ( ( jumpPhase + 1 ) >= jumpVector.size() )
        {
                *activity = Fall;
        }

        return itemMoved ;
}

void JumpKindOfActivity::lift( FreeItem* sender, FreeItem* freeItem, int z )
{
        // only for item with behavior
        if ( freeItem->getBehavior() != nilPointer )
        {
                // when item is volatile
                if ( freeItem->getBehavior()->getNameOfBehavior () == "behavior of disappearance on touch" ||
                                freeItem->getBehavior()->getNameOfBehavior () == "behavior of something special" )
                {
                        freeItem->getBehavior()->changeActivityOfItem( DisplaceUp, sender );
                }
                // raise item when it’s not elevator
                else if ( freeItem->getBehavior()->getNameOfBehavior () != "behavior of elevator" )
                {
                        // is there’s something above
                        if ( ! freeItem->addToZ( z ) )
                        {
                                Mediator* mediator = freeItem->getMediator();

                                while ( ! mediator->isStackOfCollisionsEmpty() )
                                {
                                        FreeItem* topItem = dynamic_cast< FreeItem * >( mediator->findCollisionPop( ) );

                                        if ( topItem != nilPointer )
                                        {
                                                // raise free items recursively
                                                lift( sender, topItem, z );
                                        }
                                }

                                // ahora ya puede ascender
                                freeItem->addToZ( z );
                        }
                }
        }
}

}
