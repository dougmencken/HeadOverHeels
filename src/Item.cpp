
#include "Item.hpp"
#include "ItemData.hpp"
#include "Mediator.hpp"
#include "Behavior.hpp"

#include <iostream>


namespace isomot
{

Item::Item( ItemData* data, int z, const Way& way )
      : Mediated(), Shady(),
        uniqueName( data->getLabel() + "." + makeRandomString( 12 ) ),
        originalLabel( data->getLabel() ),
        currentFrame( 0 ),
        backwardMotion( false ),
        dataOfItem( data ),
        xPos( 0 ),
        yPos( 0 ),
        zPos( z ),
        orientation( way ),
        offset( std::pair< int, int >( 0, 0 ) ),
        rawImage( nilPointer ),
        shadow( nilPointer ),
        processedImage( nilPointer ),
        motionTimer( /* more in body */ nilPointer ),
        behavior( nilPointer ),
        anchor( nilPointer )
{
        // item with more than one frame per direction has animation
        if ( ( data->howManyMotions() - data->howManyExtraFrames() ) / data->howManyFramesPerOrientation() > 1 )
        {
                motionTimer = new Timer ();
                motionTimer->go();
        }

        ///std::cout << "created item with unique random name \"" << uniqueName << "\"" << std::endl ;
}

Item::Item( const Item& item )
      : Mediated( item ), Shady( item.wantShadow ),
        uniqueName( item.uniqueName + " copy" ),
        originalLabel( item.originalLabel ),
        currentFrame( item.currentFrame ),
        backwardMotion( item.backwardMotion ),
        dataOfItem( item.dataOfItem ),
        xPos( item.xPos ),
        yPos( item.yPos ),
        zPos( item.zPos ),
        orientation( item.orientation ),
        offset( item.offset ),
        rawImage( item.rawImage ),
        shadow( item.shadow ),
        processedImage( /* more in body */ nilPointer ),
        motionTimer( /* more in body */ nilPointer ),
        behavior( nilPointer ),
        anchor( item.anchor )
{
        if ( item.motionTimer != nilPointer )
        {
                motionTimer = new Timer ();
                motionTimer->go();
        }

        if ( item.processedImage != nilPointer )
        {
                this->processedImage = new Picture( *item.processedImage );
        }
}

Item::~Item( )
{
        delete this->behavior;
        delete this->motionTimer;

        delete this->processedImage ;
}

bool Item::update()
{
        bool isGone = false;

        if ( behavior != nilPointer )
                isGone = behavior->update();

        return isGone;
}

bool Item::animate()
{
        bool cycle = false;

        // item with more than one frame per direction has animation
        if ( ( dataOfItem->howManyMotions() - dataOfItem->howManyExtraFrames() ) / dataOfItem->howManyFramesPerOrientation() > 1 )
        {
                // is it time to change frames
                if ( motionTimer->getValue() > dataOfItem->getDelayBetweenFrames() )
                {
                        // forward motion
                        if ( ! backwardMotion )
                        {
                                if ( ++ currentFrame >= dataOfItem->howManyFrames() )
                                {
                                        currentFrame = 0;
                                        cycle = true;
                                }
                        }
                        // backward motion
                        else
                        {
                                if ( currentFrame -- <= 0 )
                                {
                                        currentFrame = dataOfItem->howManyFrames() - 1;
                                        cycle = true;
                                }
                        }

                        // which frame to show yet
                        unsigned int orientOccident = ( orientation.getIntegerOfWay() == Way::Nowhere ? 0 : orientation.getIntegerOfWay() );
                        unsigned int orientations = ( dataOfItem->howManyMotions() - dataOfItem->howManyExtraFrames() ) / dataOfItem->howManyFramesPerOrientation();
                        unsigned int frame = dataOfItem->getFrameAt( currentFrame ) + ( dataOfItem->howManyFramesPerOrientation() > 1 ? orientations * orientOccident : 0 );

                        // change frame
                        if ( this->rawImage != nilPointer && frame < dataOfItem->howManyMotions() && this->rawImage != dataOfItem->getMotionAt( frame ) )
                        {
                                changeImage( dataOfItem->getMotionAt( frame ) );

                                if ( this->shadow != nilPointer )
                                {
                                        changeShadow( dataOfItem->getShadowAt( frame ) );
                                }
                        }

                        motionTimer->reset();
                }
        }

        return cycle;
}

void Item::changeItemData( ItemData* itemData, const std::string& initiatedBy )
{
        std::cout << "metamorphosis of data for item \"" << getUniqueName()
                        << "\" to data of \"" << itemData->getLabel()
                        << "\" initiated by \"" << initiatedBy << "\"" << std::endl ;

        this->dataOfItem = itemData;
        this->currentFrame = 0;
        this->backwardMotion = false;
}

void Item::changeShadow( Picture* newShadow )
{
        if ( shadow != newShadow )
        {
                this->shadow = newShadow;
                // don’t bin old shadow, it may be re~used
        }
}

void Item::setProcessedImage( Picture* newImage )
{
        if ( processedImage != newImage )
        {
                delete processedImage ;
                processedImage = newImage;
        }
}

void Item::changeOrientation( const Way& way )
{
        if ( this->orientation.getIntegerOfWay() == way.getIntegerOfWay() )
        {
                // nothing to change
                return;
        }

        // when item has different frames for orientations
        if ( dataOfItem->howManyFramesPerOrientation() > 1 )
        {
                // get frame for new orientation
                unsigned int orientOccident = ( way.getIntegerOfWay() == Way::Nowhere ? 0 : way.getIntegerOfWay() );
                unsigned int orientations = ( dataOfItem->howManyMotions() - dataOfItem->howManyExtraFrames() ) / dataOfItem->howManyFramesPerOrientation();
                unsigned int frame = orientations * orientOccident;

                if ( this->rawImage != nilPointer && frame < dataOfItem->howManyMotions() && this->rawImage != dataOfItem->getMotionAt( frame ) )
                {
                        this->orientation = way;

                        changeImage( dataOfItem->getMotionAt( frame ) );

                        if ( this->shadow != nilPointer )
                                changeShadow( dataOfItem->getShadowAt( frame ) );
                }
        }
        else
        {
                this->orientation = way;
        }
}

void Item::changeFrame( const unsigned int newFrame )
{
        if ( dataOfItem->howManyMotions() > newFrame )
        {
                this->currentFrame = newFrame;

                if ( this->rawImage != nilPointer && this->rawImage != dataOfItem->getMotionAt( newFrame ) )
                {
                        changeImage( dataOfItem->getMotionAt( newFrame ) );

                        if ( this->shadow != nilPointer )
                                changeShadow( dataOfItem->getShadowAt( newFrame ) );
                }
        }
}

bool Item::canAdvanceTo( int x, int y, int z )
{
        // coordinates before change
        int originalX = this->getX();
        int originalY = this->getY();
        int originalZ = this->getZ();

        mediator->clearStackOfCollisions( );

        bool collisionFound = false;

        // new coordinates
        this->xPos += x;
        this->yPos += y;
        this->zPos += z;

        // look for collision with wall
        if ( this->getX() < mediator->getRoom()->getLimitAt( "north" ) )
        {
                mediator->pushCollision( "some segment of wall at north" );
        }
        else if ( this->getX() + static_cast< int >( this->getWidthX() ) > mediator->getRoom()->getLimitAt( "south" ) )
        {
                mediator->pushCollision( "some segment of wall at south" );
        }
        if ( this->getY() >= mediator->getRoom()->getLimitAt( "west" ) )
        {
                mediator->pushCollision( "some segment of wall at west" );
        }
        else if ( this->getY() - static_cast< int >( this->getWidthY() ) + 1 < mediator->getRoom()->getLimitAt( "east" ) )
        {
                mediator->pushCollision( "some segment of wall at east" );
        }

        // look for collision with floor
        if ( this->getZ() < 0 )
        {
                mediator->pushCollision( "some tile of floor" );
        }

        collisionFound = ! mediator->isStackOfCollisionsEmpty ();
        if ( ! collisionFound )
        {
                // look for collisions with other items in room
                collisionFound = mediator->findCollisionWithItem( this );
        }

        // restore original coordinates
        this->xPos = originalX;
        this->yPos = originalY;
        this->zPos = originalZ;

        return ! collisionFound;
}

void Item::assignBehavior( const std::string& behavior, void* extraData )
{
        this->behavior = Behavior::createBehaviorByName( this, behavior, extraData );
}

void Item::setForthMotion ()
{
        this->backwardMotion = false;
        this->currentFrame = 0;
}

void Item::setReverseMotion ()
{
        this->backwardMotion = true;
        this->currentFrame = dataOfItem->howManyFrames() - 1;
}

const std::string& Item::getLabel() const
{
        return dataOfItem->getLabel() ;
}

unsigned int Item::getWidthX() const
{
        return dataOfItem->getWidthX() ;
}

unsigned int Item::getWidthY() const
{
        return dataOfItem->getWidthY() ;
}

unsigned int Item::getHeight() const
{
        return dataOfItem->getHeight() ;
}

void Item::setHeight( int height )
{
        dataOfItem->setHeight( height );
}

bool Item::isMortal() const
{
        return dataOfItem->isMortal() ;
}

unsigned short Item::howManyFramesPerOrientation() const
{
        return dataOfItem->howManyFramesPerOrientation() ;
}

double Item::getSpeed() const
{
        return dataOfItem->getSpeed() ;
}

double Item::getWeight() const
{
        return dataOfItem->getWeight() ;
}

double Item::getDelayBetweenFrames() const
{
        return dataOfItem->getDelayBetweenFrames() ;
}

unsigned int Item::countFrames() const
{
        return dataOfItem->howManyMotions();
}

}
