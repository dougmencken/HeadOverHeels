
#include "Item.hpp"
#include "ItemData.hpp"
#include "Mediator.hpp"
#include "Behavior.hpp"

#include <iostream>


namespace isomot
{

Item::Item( ItemData* data, int z, const Way& way )
      : Mediated(), Shady(),
        label( data->getLabel() ),
        uniqueName( label + "." + makeRandomString( 12 ) ),
        dataOfItem( data ),
        x( 0 ),
        y( 0 ),
        z( z ),
        orientation( way ),
        rawImage( nilPointer ),
        shadow( nilPointer ),
        processedImage( nilPointer ),
        offset( std::pair< int, int >( 0, 0 ) ),
        motionTimer( /* more in body */ nilPointer ),
        behavior( nilPointer ),
        anchor( nilPointer ),
        frameIndex( 0 ),
        backwardMotion( false )
{
        // item with more than one frame per direction has animation
        if ( ( data->howManyMotions() - data->howManyExtraFrames() ) / data->howManyFramesForOrientations() > 1 )
        {
                motionTimer = new Timer ();
                motionTimer->go();
        }

        ///std::cout << "created item with unique random name \"" << uniqueName << "\"" << std::endl ;
}

Item::Item( const Item& item )
      : Mediated( item ), Shady( item.wantShadow ),
        label( item.label ),
        uniqueName( item.uniqueName + " copy" ),
        dataOfItem( item.dataOfItem ),
        x( item.x ),
        y( item.y ),
        z( item.z ),
        orientation( item.orientation ),
        rawImage( item.rawImage ),
        shadow( item.shadow ),
        processedImage( /* more in body */ nilPointer ),
        offset( item.offset ),
        motionTimer( /* more in body */ nilPointer ),
        behavior( nilPointer ),
        anchor( item.anchor ),
        frameIndex( item.frameIndex ),
        backwardMotion( item.backwardMotion )
{
        if ( item.motionTimer != nilPointer )
        {
                motionTimer = new Timer ();
                motionTimer->go();
        }

        if ( item.processedImage != nilPointer )
        {
                this->processedImage = create_bitmap_ex( 32, item.processedImage->w, item.processedImage->h );
                blit( item.processedImage, this->processedImage, 0, 0, 0, 0, this->processedImage->w, this->processedImage->h );
        }
}

Item::~Item( )
{
        delete this->behavior;
        delete this->motionTimer;

        destroy_bitmap( this->processedImage );
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
        if ( ( dataOfItem->howManyMotions() - dataOfItem->howManyExtraFrames() ) / dataOfItem->howManyFramesForOrientations() > 1 )
        {
                // is it time to change frames
                if ( motionTimer->getValue() > dataOfItem->getDelayBetweenFrames() )
                {
                        // forward motion
                        if ( ! backwardMotion )
                        {
                                if ( ++frameIndex >= dataOfItem->howManyFrames() )
                                {
                                        frameIndex = 0;
                                        cycle = true;
                                }
                        }
                        // backward motion
                        else
                        {
                                if ( frameIndex-- <= 0 )
                                {
                                        frameIndex = dataOfItem->howManyFrames() - 1;
                                        cycle = true;
                                }
                        }

                        // which frame to show yet
                        int framesNumber = ( dataOfItem->howManyMotions() - dataOfItem->howManyExtraFrames() ) / dataOfItem->howManyFramesForOrientations();
                        unsigned int orientOccident = ( orientation.getIntegerOfWay() == Nowhere ? 0 : orientation.getIntegerOfWay() );
                        int currentFrame = dataOfItem->getFrameAt( frameIndex ) + ( dataOfItem->howManyFramesForOrientations() > 1 ? framesNumber * orientOccident : 0 );

                        // change frame of animation
                        if ( this->rawImage != nilPointer && this->rawImage != dataOfItem->getMotionAt( currentFrame ) )
                        {
                                changeImage( dataOfItem->getMotionAt( currentFrame ) );

                                if ( this->shadow != nilPointer )
                                {
                                        changeShadow( dataOfItem->getShadowAt( currentFrame ) );
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
        this->frameIndex = 0;
        this->backwardMotion = false;
}

void Item::changeShadow( BITMAP* newShadow )
{
        if ( this->shadow != newShadow )
        {
                this->shadow = newShadow;
                // don’t bin old shadow, it may be re~used
        }
}

void Item::setProcessedImage( BITMAP* newImage )
{
        if ( processedImage != newImage )
        {
                destroy_bitmap( processedImage );
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
        if ( dataOfItem->howManyFramesForOrientations() > 1 )
        {
                // get frame for new orientation
                unsigned int orientOccident = ( way.getIntegerOfWay() == Nowhere ? 0 : way.getIntegerOfWay() );
                unsigned int frame = ( ( dataOfItem->howManyMotions() - dataOfItem->howManyExtraFrames() ) / dataOfItem->howManyFramesForOrientations() ) * orientOccident;

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

void Item::changeFrame( const unsigned int frameIndex )
{
        // if index of frame is within bounds of vector with sequence of animation
        if ( dataOfItem->howManyMotions() > frameIndex )
        {
                this->frameIndex = frameIndex;

                // change frame
                if ( this->rawImage != nilPointer && this->rawImage != dataOfItem->getMotionAt( frameIndex ) )
                {
                        changeImage( dataOfItem->getMotionAt( frameIndex ) );

                        // update shadow too
                        if ( this->shadow != nilPointer )
                                changeShadow( dataOfItem->getShadowAt( frameIndex ) );
                }
        }
}

bool Item::checkPosition( int x, int y, int z, const ChangeOrAdd& what )
{
        // coordinates before change
        int px = this->x;
        int py = this->y;
        int pz = this->z;

        mediator->clearStackOfCollisions( );

        bool collisionFound = false;

        // new coordinates
        this->x = x + this->x * what;
        this->y = y + this->y * what;
        this->z = z + this->z * what;

        // look for collision with wall
        if ( this->x < mediator->getRoom()->getLimitAt( "north" ) )
        {
                mediator->pushCollision( "some segment of north wall" );
        }
        else if ( this->x + static_cast< int >( this->getWidthX() ) > mediator->getRoom()->getLimitAt( "south" ) )
        {
                mediator->pushCollision( "some segment of south wall" );
        }
        if ( this->y >= mediator->getRoom()->getLimitAt( "west" ) )
        {
                mediator->pushCollision( "some segment of west wall" );
        }
        else if ( this->y - static_cast< int >( this->getWidthY() ) + 1 < mediator->getRoom()->getLimitAt( "east" ) )
        {
                mediator->pushCollision( "some segment of east wall" );
        }

        // look for collision with floor
        if ( this->z < 0 )
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
        this->x = px;
        this->y = py;
        this->z = pz;

        return ! collisionFound;
}

void Item::assignBehavior( const std::string& behavior, void* extraData )
{
        this->behavior = Behavior::createBehaviorByName( this, behavior, extraData );
}

void Item::setForthMotion ()
{
        this->backwardMotion = false;
        this->frameIndex = 0;
}

void Item::setReverseMotion ()
{
        this->backwardMotion = true;
        this->frameIndex = dataOfItem->howManyFrames() - 1;
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

unsigned short Item::howManyFramesForOrientations() const
{
        return dataOfItem->howManyFramesForOrientations() ;
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
