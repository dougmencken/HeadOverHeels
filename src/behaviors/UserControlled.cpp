
#include "UserControlled.hpp"
#include "Item.hpp"
#include "ItemData.hpp"
#include "ItemDataManager.hpp"
#include "PlayerItem.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "MoveKindOfActivity.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "JumpKindOfActivity.hpp"
#include "Doughnut.hpp"
#include "GameManager.hpp"
#include "SoundManager.hpp"


namespace isomot
{

UserControlled::UserControlled( Item * item, const std::string & behavior )
        : Behavior( item, behavior )
        , jumpIndex( 0 )
        , jumpFrames( 0 )
        , highJumpFrames( 0 )
        , automaticSteps( 0 )
        , automaticStepsThruDoor( 0 )
        , highSpeedSteps( 0 )
        , shieldSteps( 0 )
        , speedTimer( nilPointer )
        , fallTimer( nilPointer )
        , glideTimer( nilPointer )
        , blinkingTimer( nilPointer )
{
        fallFrames[ North ] = fallFrames[ South ] = fallFrames[ East ] = fallFrames[ West ] = 0xffffffff; // wtf is this magic mask for ?
}

UserControlled::~UserControlled()
{
        jumpVector.clear();
        highJumpVector.clear();

        delete speedTimer;
        delete fallTimer;
        delete glideTimer;
        delete blinkingTimer;
}

void UserControlled::changeActivityOfItem( const ActivityOfItem & activityOf, Item * sender )
{
        Behavior::changeActivityOfItem( activityOf, sender );

        if ( activityOf == BeginWayInTeletransport )
        {
                this->item->changeFrame( this->nullFrame );
        }
}

void UserControlled::wait( PlayerItem * player )
{
        player->wait();

        // Se comprueba si el jugador debe empezar a caer
        if ( FallKindOfActivity::getInstance()->fall( this ) )
        {
                speedTimer->reset();
                activity = Fall;

                // Si el jugador se detiene entonces se para la cuenta atrás del conejo que proporciona doble velocidad
                if ( player->getLabel() == "head" && player->getHighSpeed() > 0 && this->highSpeedSteps < 4 )
                {
                        this->highSpeedSteps = 0;
                }
        }
}

void UserControlled::move( PlayerItem * player )
{
        // move when character isn’t frozen
        if ( ! player->isFrozen() )
        {
                // apply effect of bunny of high speed
                double speed = player->getSpeed() / ( player->getHighSpeed() > 0 ? 2 : 1 );

                // is it time to move
                if ( speedTimer->getValue() > speed )
                {
                        // move it
                        bool moved = MoveKindOfActivity::getInstance()->move( this, &activity, true );

                        // decrement high speed
                        if ( player->getHighSpeed() > 0 && moved && activity != Fall )
                        {
                                this->highSpeedSteps++;

                                if ( this->highSpeedSteps == 8 )
                                {
                                        player->decreaseHighSpeed();
                                        this->highSpeedSteps = 4;
                                }
                        }

                        speedTimer->reset();

                        player->animate();
                }
        }
}

void UserControlled::autoMove( PlayerItem * player )
{
        // apply effect of bunny of high speed
        double speed = player->getSpeed() / ( player->getHighSpeed() > 0 ? 2 : 1 );

        // is it time to move
        if ( speedTimer->getValue() > speed )
        {
                // move it
                MoveKindOfActivity::getInstance()->move( this, &activity, true );

                speedTimer->reset();

                player->animate();

                if ( automaticStepsThruDoor > 0 )
                {
                        --automaticStepsThruDoor ;
                }
                else
                {
                        // done auto~moving
                        automaticStepsThruDoor = automaticSteps;
                        activity = Wait;
                }
        }

        // when done then stop playing sound of auto movement
        if ( activity == Wait )
        {
                SoundManager::getInstance()->stop( player->getLabel(), AutoMove );
        }
}

void UserControlled::displace( PlayerItem * player )
{
        // this item is moved by another one
        // when displacement couldn’t be performed due to collision then activity propagates to collided items
        if ( speedTimer->getValue() > player->getSpeed() )
        {
                DisplaceKindOfActivity::getInstance()->displace( this, &activity, true );

                activity = Wait;

                speedTimer->reset();
        }
}

void UserControlled::cancelDisplace( PlayerItem * player )
{
        if ( ! player->isFrozen() )
        {
                if ( speedTimer->getValue() > player->getSpeed() )
                {
                        // move it
                        MoveKindOfActivity::getInstance()->move( this, &activity, false );

                        speedTimer->reset();

                        player->animate();
                }
        }
}

void UserControlled::fall( PlayerItem * player )
{
        // is it time to lower by one unit
        if ( fallTimer->getValue() > player->getWeight() )
        {
                if ( FallKindOfActivity::getInstance()->fall( this ) )
                {
                        // change character’s image to frame of falling when there’s no collision yet
                        if ( player->checkPosition( 0, 0, -1, Add ) )
                        {
                                player->changeFrame( fallFrames[ player->getOrientation().getIntegerOfWay () ] );
                        }
                }
                else if ( activity != MeetMortalItem || ( activity == MeetMortalItem && player->hasShield() ) )
                {
                        activity = Wait;
                }

                fallTimer->reset();
        }

        if ( activity != Fall )
        {
                SoundManager::getInstance()->stop( player->getLabel(), Fall );
        }
}

void UserControlled::jump( PlayerItem * player )
{
        switch ( activity )
        {
                case Jump:
                        // look for item below
                        player->checkPosition( 0, 0, -1, Add );
                        // when on trampoline or with rabbit of high jumps, player makes big leap
                        activity = ( player->getMediator()->collisionWithByBehavior( "behavior of big leap for player" ) ||
                                        ( player->getHighJumps() > 0 && player->getLabel() == "heels" )
                                ? HighJump
                                : RegularJump );

                        if ( activity == HighJump )
                        {
                                if ( player->getLabel() == "heels" )
                                {
                                        player->decreaseHighJumps();
                                }
                                SoundManager::getInstance()->play( player->getLabel(), Rebound );
                        }
                        break;

                case RegularJump:
                        // is it time to jump
                        if ( speedTimer->getValue() > player->getSpeed() )
                        {
                                JumpKindOfActivity::getInstance()->jump( this, &activity, jumpVector, jumpIndex );

                                // to next phase of jump
                                jumpIndex++ ;
                                if ( activity == Fall ) jumpIndex = 0;

                                speedTimer->reset();

                                player->animate();
                        }
                        break;

                case HighJump:
                        // is it time to jump
                        if ( speedTimer->getValue() > player->getSpeed() )
                        {
                                JumpKindOfActivity::getInstance()->jump( this, &activity, highJumpVector, jumpIndex );

                                // to next phase of jump
                                jumpIndex++ ;
                                if ( activity == Fall ) jumpIndex = 0;

                                speedTimer->reset();

                                player->animate();
                        }
                        break;

                default:
                        ;
        }

        // when jump ends, stop sound of jumping
        if ( activity != Jump && activity != RegularJump && activity != HighJump )
        {
                SoundManager::getInstance()->stop( player->getLabel(), Jump );
        }

        // when player is active and is at maximum height of room it may go to room above
        if ( player->isActivePlayer() && player->getZ() >= MaxLayers * LayerHeight )
        {
                player->setWayOfExit( "up" );
        }
}

void UserControlled::glide( PlayerItem * player )
{
        ///if ( glideTimer->getValue() > player->getSpeed() / 2.0 )
        if ( glideTimer->getValue() > player->getWeight() )
        {
                // when there’s a collision then stop falling and return to waiting
                if ( ! FallKindOfActivity::getInstance()->fall( this ) &&
                        ( activity != MeetMortalItem || ( activity == MeetMortalItem && player->hasShield() ) ) )
                {
                        activity = Wait;
                }

                glideTimer->reset();
        }

        if ( speedTimer->getValue() > player->getSpeed() * ( player->getLabel() == "headoverheels" ? 2 : 1 ) )
        {
                ActivityOfItem subactivity;

                switch ( player->getOrientation().getIntegerOfWay () )
                {
                        case North:
                                subactivity = MoveNorth;
                                break;

                        case South:
                                subactivity = MoveSouth;
                                break;

                        case East:
                                subactivity = MoveEast;
                                break;

                        case West:
                                subactivity = MoveWest;
                                break;

                        default:
                                ;
                }

                MoveKindOfActivity::getInstance()->move( this, &subactivity, false );

                // pick picture of falling
                player->changeFrame( fallFrames[ player->getOrientation().getIntegerOfWay() ] );

                speedTimer->reset();
        }

        if ( activity != Glide )
        {
                // no gliding yet, so stop its sound
                SoundManager::getInstance()->stop( player->getLabel(), Glide );
        }
}

void UserControlled::wayInTeletransport( PlayerItem * player )
{
        switch ( activity )
        {
                case BeginWayInTeletransport:
                        // change to bubbles
                        player->changeItemData( itemDataManager->findItemByLabel( labelOfTransitionViaTeleport ), "begin way in teletransport" );

                        // reverse animation of bubbles
                        player->setReverseMotion();

                        // begin teleportation
                        activity = WayInTeletransport;
                        break;

                case WayInTeletransport:
                        // animate item, player appears in room when animation finishes
                        if ( player->animate() )
                        {
                                // back to original appearance of player
                                player->changeItemData( player->getOriginalDataOfItem(), "way in teletransport" );

                                activity = Wait;
                        }
                        break;

                default:
                        ;
        }
}

void UserControlled::wayOutTeletransport( PlayerItem * player )
{
        switch ( activity )
        {
                case BeginWayOutTeletransport:
                        // change to bubbles
                        player->changeItemData( itemDataManager->findItemByLabel( labelOfTransitionViaTeleport ), "begin way out teletransport" );

                        // begin teleportation
                        activity = WayOutTeletransport;
                        break;

                case WayOutTeletransport:
                        // animate item, change room when animation finishes
                        if ( player->animate() )
                        {
                                player->addToZ( -1 );
                                player->setWayOfExit( player->getMediator()->collisionWithByLabel( "teleport" ) ? "via teleport" : "via second teleport" );
                        }
                        break;

                default:
                        ;
        }
}

void UserControlled::collideWithMortalItem( PlayerItem* player )
{
        switch ( activity )
        {
                // player just met a bad guy
                case MeetMortalItem:
                        // do you have immunity
                        if ( ! player->hasShield() )
                        {
                                // change to bubbles retaining player’s label
                                player->changeItemData( itemDataManager->findItemByLabel( labelOfTransitionViaTeleport ), "collide with mortal item" );

                                activity = Vanish;
                        }
                        else
                        {
                                activity = Wait;
                        }
                        break;

                case Vanish:
                        // animate item, restart room when animation finishes
                        if ( player->animate() )
                        {
                                player->setWayOfExit( "rebuild room" );
                                player->loseLife();
                        }
                        break;

                default:
                        ;
        }
}

void UserControlled::useHooter( PlayerItem* player )
{
        // El jugador puede disparar si tiene la bocina y rosquillas
        if ( player->hasTool( "horn" ) && player->getDoughnuts() > 0 )
        {
                this->fireFromHooterIsPresent = true;

                ItemData* hooterData = this->itemDataManager->findItemByLabel( labelOfFireFromHooter );

                if ( hooterData != nilPointer )
                {
                        SoundManager::getInstance()->stop( player->getLabel(), FireDoughnut );

                        // create item at the same position as player
                        int z = player->getZ() + player->getHeight() - hooterData->getHeight();
                        FreeItem* donut = new FreeItem
                        (
                                hooterData,
                                player->getX(), player->getY(),
                                z < 0 ? 0 : z,
                                player->getOrientation()
                        );

                        donut->assignBehavior( "behavior of freezing doughnut", nilPointer );

                        Doughnut * behaviorOfDonut = dynamic_cast< Doughnut * >( donut->getBehavior() );
                        behaviorOfDonut->setPlayerItem( player );

                        // at first it shares the same position as player so it does not detect collisions
                        donut->setCollisionDetector( false );

                        player->getMediator()->getRoom()->addFreeItem( donut );

                        player->useDoughnut();

                        SoundManager::getInstance()->play( player->getLabel(), FireDoughnut );
                }
        }
}

void UserControlled::takeItem( PlayerItem * player )
{
        if ( player->hasTool( "handbag" ) )
        {
                Mediator* mediator = player->getMediator();
                Item* takenItem = nilPointer;

                // look for item below player
                if ( ! player->checkPosition( 0, 0, -1, Add ) )
                {
                        int coordinates = 0;

                        while ( ! mediator->isStackOfCollisionsEmpty() )
                        {
                                Item* bottomItem = mediator->findCollisionPop( );

                                // choose free pushable item less than or equal to 3/4 of size of one tile
                                if ( bottomItem != nilPointer && bottomItem->getBehavior() != nilPointer
                                        && ( bottomItem->getBehavior()->getNameOfBehavior() == "behavior of thing able to move by pushing" ||
                                                bottomItem->getBehavior()->getNameOfBehavior() == "behavior of big leap for player" )
                                        && bottomItem->getWidthX() <= ( mediator->getRoom()->getSizeOfOneTile() * 3 ) >> 2
                                        && bottomItem->getWidthY() <= ( mediator->getRoom()->getSizeOfOneTile() * 3 ) >> 2 )
                                {
                                        if ( bottomItem->getX() + bottomItem->getY() > coordinates )
                                        {
                                                coordinates = bottomItem->getX() + bottomItem->getY();
                                                takenItem = bottomItem;
                                        }
                                }
                        }

                        // take that item
                        if ( takenItem != nilPointer )
                        {
                                // get image of that item
                                BITMAP* takenItemImage = takenItem->getRawImage();
                                GameManager::getInstance()->setItemTaken( takenItemImage );

                                player->assignTakenItem( itemDataManager->findItemByLabel( takenItem->getLabel() ),
                                                                takenItemImage,
                                                                takenItem->getBehavior()->getNameOfBehavior () );

                                takenItem->getBehavior()->changeActivityOfItem( Vanish );
                                activity = ( activity == TakeAndJump ? Jump : TakenItem );

                                SoundManager::getInstance()->play( player->getLabel(), TakeItem );

                                std::cout << "took item \"" << takenItem->getLabel() << "\"" << std::endl ;
                        }
                }
        }

        if ( activity != TakenItem && activity != Jump )
        {
                activity = Wait;
        }
}

void UserControlled::dropItem( PlayerItem* player )
{
        if ( player->getTakenItemData() != nilPointer )
        {
                std::cout << "drop item \"" << player->getTakenItemData()->getLabel() << "\"" << std::endl ;

                // place dropped item just below player
                if ( player->addToZ( LayerHeight ) )
                {
                        FreeItem* freeItem = new FreeItem( player->getTakenItemData(),
                                                           player->getX(), player->getY(),
                                                           player->getZ() - LayerHeight,
                                                           Nowhere );

                        freeItem->assignBehavior( player->getTakenItemBehavior(), nilPointer );

                        player->getMediator()->getRoom()->addFreeItem( freeItem );

                        player->assignTakenItem( nilPointer, nilPointer, "still" );

                        // update activity
                        activity = ( activity == DropAndJump ? Jump : Wait );

                        GameManager::getInstance()->setItemTaken( nilPointer );

                        SoundManager::getInstance()->stop( player->getLabel(), Fall );
                        SoundManager::getInstance()->play( player->getLabel(), DropItem );
                }
                else
                {
                        // emit sound of o~ ou
                        SoundManager::getInstance()->play( player->getLabel(), Mistake );
                }
        }

        if ( activity != Jump )
        {
                activity = Wait;
        }
}

void UserControlled::setMoreData( void * data )
{
        this->itemDataManager = reinterpret_cast< ItemDataManager * >( data );
}

}
