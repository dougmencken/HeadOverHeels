
#include "Item.hpp"
#include "DescriptionOfItem.hpp"
#include "Mediator.hpp"
#include "Behavior.hpp"
#include "GameManager.hpp"

#include <algorithm> // std::for_each

#include <iostream>


namespace iso
{

PoolOfPictures * Item::poolOfPictures = new PoolOfPictures( ) ;


Item::Item( const DescriptionOfItem* description, int z, const Way& way )
      : Mediated(), Shady(),
        uniqueName( description->getLabel() + "." + makeRandomString( 12 ) ),
        originalLabel( description->getLabel() ),
        currentFrame( 0 ),
        backwardsMotion( false ),
        descriptionOfItem( description ),
        xPos( 0 ),
        yPos( 0 ),
        zPos( z ),
        height( description->getHeight() ),
        orientation( way ),
        offset( std::pair< int, int >( 0, 0 ) ),
        collisionDetector( false ),
        rawImage( nilPointer ),
        shadow( nilPointer ),
        processedImage( nilPointer ),
        motionTimer( new Timer () ),
        behavior( nilPointer ),
        anchor( "" )
{
        readGraphicsOfItem ();

        // item with more than one frame per orientation is animated
        if ( descriptionOfItem->howManyFrames() > 1 )
        {
                motionTimer->go();
        }
}

Item::Item( const Item& item )
      : Mediated( item ), Shady( item.wantShadow ),
        uniqueName( item.uniqueName + " copy" ),
        originalLabel( item.originalLabel ),
        currentFrame( item.currentFrame ),
        backwardsMotion( item.backwardsMotion ),
        descriptionOfItem( item.descriptionOfItem ),
        xPos( item.xPos ),
        yPos( item.yPos ),
        zPos( item.zPos ),
        height( item.height ),
        orientation( item.orientation ),
        offset( item.offset ),
        collisionDetector( item.collisionDetector ),
        rawImage( item.rawImage ),
        shadow( item.shadow ),
        processedImage( /* more in body */ nilPointer ),
        motionTimer( new Timer () ),
        behavior( nilPointer ),
        anchor( item.anchor )
{
        for ( std::vector< const Picture * >::const_iterator it = item.motion.begin (); it != item.motion.end (); ++ it )
        {
                motion.push_back( new Picture( ( *it )->getAllegroPict() ) );
        }

        for ( std::vector< const Picture * >::const_iterator it = item.shadows.begin (); it != item.shadows.end (); ++ it )
        {
                shadows.push_back( new Picture( ( *it )->getAllegroPict() ) );
        }

        if ( descriptionOfItem->howManyFrames() > 1 )
        {
                motionTimer->go();
        }

        if ( item.processedImage != nilPointer )
        {
                this->processedImage = new Picture( *item.processedImage );
        }
}

Item::~Item( )
{
        delete behavior;

        clearMotionFrames();
        clearShadowFrames();

        delete processedImage ;
}

void Item::readGraphicsOfItem ()
{
        clearMotionFrames ();
        clearShadowFrames ();

        if ( ! descriptionOfItem->isPartOfDoor() && ! descriptionOfItem->getNameOfFile().empty() )
        {
                createFrames( this, *descriptionOfItem );

                unsigned int way = orientation.getIntegerOfWay();
                if ( way == Way::Nowhere ) way = 0;
                unsigned int firstFrame = ( descriptionOfItem->howManyOrientations() > 1 ?
                                                descriptionOfItem->howManyFrames() * way : 0 );

                this->rawImage = getMotionAt( firstFrame );

                if ( descriptionOfItem->getWidthOfShadow() > 0 && descriptionOfItem->getHeightOfShadow() > 0 ) // item may have no shadow
                {
                        createShadowFrames( this, *descriptionOfItem );

                        this->shadow = getShadowAt( firstFrame );
                }
        }
        else if ( descriptionOfItem->getNameOfFile().empty() )
        {
                Picture invisibleFrame( descriptionOfItem->getWidthOfFrame(), descriptionOfItem->getHeightOfFrame() );
                addFrame( invisibleFrame );
        }
}

bool Item::updateItem()
{
        bool isGone = false;

        if ( behavior != nilPointer )
                isGone = behavior->update();

        return isGone;
}

bool Item::animate()
{
        bool cycle = false;

        // item with more than one frame per orientation is animated
        if ( descriptionOfItem->howManyFrames() > 1 )
        {
                // is it time to change frames
                if ( motionTimer->getValue() > descriptionOfItem->getDelayBetweenFrames() )
                {
                        // forwards motion
                        if ( ! backwardsMotion )
                        {
                                if ( ++ currentFrame >= descriptionOfItem->howManyFrames() )
                                {
                                        currentFrame = 0;
                                        cycle = true;
                                }
                        }
                        // backwards motion
                        else
                        {
                                if ( currentFrame -- <= 0 )
                                {
                                        currentFrame = descriptionOfItem->howManyFrames() - 1;
                                        cycle = true;
                                }
                        }

                        // which frame to show yet
                        unsigned int orientOccident = ( orientation.getIntegerOfWay() == Way::Nowhere ? 0 : orientation.getIntegerOfWay() );
                        unsigned int frame = currentFrame + ( descriptionOfItem->howManyOrientations() > 1 ? descriptionOfItem->howManyFrames() * orientOccident : 0 );

                        // change frame
                        if ( this->rawImage != nilPointer && this->rawImage != getMotionAt( frame ) )
                        {
                                changeImage( getMotionAt( frame ) );

                                if ( this->shadow != nilPointer )
                                {
                                        changeShadow( getShadowAt( frame ) );
                                }
                        }

                        motionTimer->reset();
                }
        }

        return cycle;
}

void Item::metamorphInto( const std::string& labelOfItem, const std::string& initiatedBy )
{
        const DescriptionOfItem * description = descriptionOfItem->getItemDescriptions()->getDescriptionByLabel( labelOfItem );
        if ( description == nilPointer ) return ;

        std::cout << "metamorphosis of item \"" << getUniqueName()
                        << "\" into \"" << description->getLabel()
                        << "\" initiated by \"" << initiatedBy << "\"" << std::endl ;

        this->descriptionOfItem = description;

        doForthMotion ();

        readGraphicsOfItem ();
}

void Item::changeShadow( const Picture* newShadow )
{
        if ( shadow != newShadow )
        {
                this->shadow = newShadow;
                // don’t bin old shadow, it is re~used
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
                return;
        }

        if ( descriptionOfItem->howManyOrientations() > 1 )
        {
                // get frame for new orientation
                unsigned int orientOccident = ( way.getIntegerOfWay() == Way::Nowhere ? 0 : way.getIntegerOfWay() );
                unsigned int frame = descriptionOfItem->howManyFrames() * orientOccident;

                if ( this->rawImage != nilPointer && this->rawImage != getMotionAt( frame ) )
                {
                        changeImage( getMotionAt( frame ) );

                        if ( this->shadow != nilPointer )
                                changeShadow( getShadowAt( frame ) );

                        this->orientation = way;
                }
        }
        else
        {
                this->orientation = way;
        }
}

void Item::changeFrame( const unsigned int newFrame )
{
        if ( howManyMotions() > newFrame )
        {
                if ( this->rawImage != nilPointer && this->rawImage != getMotionAt( newFrame ) )
                {
                        changeImage( getMotionAt( newFrame ) );

                        if ( this->shadow != nilPointer )
                                changeShadow( getShadowAt( newFrame ) );
                }

                this->currentFrame = newFrame;
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
                collisionFound = mediator->lookForCollisionsOf( this->getUniqueName() );
        }

        // restore original coordinates
        this->xPos = originalX;
        this->yPos = originalY;
        this->zPos = originalZ;

        return ! collisionFound;
}

bool Item::intersectsWith( const Item& item ) const
{
        return  ( this->getX() < item.getX() + static_cast< int >( item.getWidthX() ) ) &&
                        ( item.getX() < this->getX() + static_cast< int >( this->getWidthX() ) ) &&
                ( this->getY() > item.getY() - static_cast< int >( item.getWidthY() ) ) &&
                        ( item.getY() > this->getY() - static_cast< int >( this->getWidthY() ) ) &&
                ( this->getZ() < item.getZ() + static_cast< int >( item.getHeight() ) ) &&
                        ( item.getZ() < this->getZ() + static_cast< int >( this->getHeight() ) ) ;
}

void Item::setBehaviorOf( const std::string& behavior )
{
        delete this->behavior ;
        this->behavior = Behavior::createBehaviorByName( ItemPtr( this ), behavior );
}

void Item::doForthMotion ()
{
        this->backwardsMotion = false;
        this->currentFrame = 0;
}

void Item::doBackwardsMotion ()
{
        this->backwardsMotion = true;
        this->currentFrame = descriptionOfItem->howManyFrames() - 1;
}

const std::string& Item::getLabel() const
{
        return descriptionOfItem->getLabel() ;
}

unsigned int Item::getWidthX() const
{
        return descriptionOfItem->getWidthX() ;
}

unsigned int Item::getWidthY() const
{
        return descriptionOfItem->getWidthY() ;
}

bool Item::isMortal() const
{
        return descriptionOfItem->isMortal() ;
}

unsigned short Item::howManyOrientations() const
{
        return descriptionOfItem->howManyOrientations() ;
}

float Item::getSpeed() const
{
        return descriptionOfItem->getSpeed() ;
}

float Item::getWeight() const
{
        return descriptionOfItem->getWeight() ;
}

float Item::getDelayBetweenFrames() const
{
        return descriptionOfItem->getDelayBetweenFrames() ;
}

#ifdef DEBUG
#  define SAVE_ITEM_FRAMES      0
#endif

/* static */
void Item::createFrames( Item* item, const DescriptionOfItem& description )
{
        if ( description.getNameOfFile( ).empty() || description.getWidthOfFrame() == 0 || description.getHeightOfFrame() == 0 )
        {
                std::cerr << "either name of picture file is empty or zero width / height at Item::createFrames" << std::endl ;
                return ;
        }

        PicturePtr picture = getPoolOfPictures().getOrLoadAndGetOrMakeAndGet( description.getNameOfFile(), description.getWidthOfFrame(), description.getHeightOfFrame() );

        // decompose image into frames
        // they are
        //    frames of animation ( 1 or more )
        //    frames of animation
        //    ... ( × "orientations": 1, 2, or 4 times )
        //    extra frames ( 0 or more )

        std::vector< Picture* > rawFrames;

        for ( unsigned int y = 0; y < picture->getHeight(); y += description.getHeightOfFrame() )
        {
                for ( unsigned int x = 0; x < picture->getWidth(); x += description.getWidthOfFrame() )
                {
                        Picture* rawFrame = new Picture( description.getWidthOfFrame(), description.getHeightOfFrame() );
                        allegro::bitBlit( picture->getAllegroPict(), rawFrame->getAllegroPict(), x, y, 0, 0, rawFrame->getWidth(), rawFrame->getHeight() );
                        rawFrame->setName( description.getLabel() + " " + util::toStringWithOrdinalSuffix( rawFrames.size() ) + " raw frame" );
                # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                        rawFrame->saveAsPNG( iso::homePath() );
                # endif
                        rawFrames.push_back( rawFrame );
                }
        }

        unsigned int rawRow = ( rawFrames.size() - description.howManyExtraFrames() ) / description.howManyOrientations();

        for ( unsigned int o = 0 ; o < description.howManyOrientations() ; o ++ )
        {
                for ( unsigned int f = 0 ; f < description.howManyFrames() ; f ++ )
                {
                        Picture animationFrame( * rawFrames[ ( o * rawRow ) + description.getFrameAt( f ) ] );
                        animationFrame.setName( description.getLabel() + " " + util::toStringWithOrdinalSuffix( o ) + " row " + util::toStringWithOrdinalSuffix( f ) + " frame" );
                # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                        animationFrame.saveAsPNG( iso::homePath() );
                # endif
                        item->addFrame( animationFrame );
                }
        }
        for ( unsigned int e = 0 ; e < description.howManyExtraFrames() ; e ++ )
        {
                Picture extraFrame( * rawFrames[ e + ( rawRow * description.howManyOrientations() ) ] );
                extraFrame.setName( description.getLabel() + " " + util::toStringWithOrdinalSuffix( e ) + " extra frame" );
        # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                extraFrame.saveAsPNG( iso::homePath() );
        # endif
                item->addFrame( extraFrame );
        }

        std::for_each( rawFrames.begin (), rawFrames.end (), DeleteIt() );
}

/* static */
void Item::createShadowFrames( Item* item, const DescriptionOfItem& description )
{
        if ( description.getNameOfShadowFile( ).empty() || description.getWidthOfShadow() == 0 || description.getHeightOfShadow() == 0 )
        {
                std::cerr << "either name of shadow file is empty or zero width / height at Item::createShadowFrames" << std::endl ;
                return ;
        }

        PicturePtr picture = getPoolOfPictures().getOrLoadAndGetOrMakeAndGet( description.getNameOfShadowFile(), description.getWidthOfShadow(), description.getHeightOfShadow() );

        // decompose image into frames

        std::vector< Picture* > rawShadows;

        for ( unsigned int y = 0; y < picture->getHeight(); y += description.getHeightOfShadow() )
        {
                for ( unsigned int x = 0; x < picture->getWidth(); x += description.getWidthOfShadow() )
                {
                        Picture* rawShadow = new Picture( description.getWidthOfShadow(), description.getHeightOfShadow() );
                        allegro::bitBlit( picture->getAllegroPict(), rawShadow->getAllegroPict(), x, y, 0, 0, rawShadow->getWidth(), rawShadow->getHeight() );
                        rawShadow->setName( description.getLabel() + " " + util::toStringWithOrdinalSuffix( rawShadows.size() ) + " raw shadow" );
                # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                        rawShadow->saveAsPNG( iso::homePath() );
                # endif
                        rawShadows.push_back( rawShadow );
                }
        }

        unsigned int rawRow = ( rawShadows.size() - description.howManyExtraFrames() ) / description.howManyOrientations();

        for ( unsigned int o = 0 ; o < description.howManyOrientations() ; o ++ )
        {
                for ( unsigned int f = 0 ; f < description.howManyFrames() ; f ++ )
                {
                        Picture shadowFrame( * rawShadows[ ( o * rawRow ) + description.getFrameAt( f ) ] );
                        shadowFrame.setName( description.getLabel() + " " + util::toStringWithOrdinalSuffix( o ) + " row " + util::toStringWithOrdinalSuffix( f ) + " shadow" );
                # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                        shadowFrame.saveAsPNG( iso::homePath() );
                # endif
                        item->addFrameOfShadow( shadowFrame );
                }
        }
        for ( unsigned int e = 0 ; e < description.howManyExtraFrames() ; e ++ )
        {
                Picture extraShadow( * rawShadows[ e + ( rawRow * description.howManyOrientations() ) ] );
                extraShadow.setName( description.getLabel() + " " + util::toStringWithOrdinalSuffix( e ) + " extra shadow" );
        # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                extraShadow.saveAsPNG( iso::homePath() );
        # endif
                item->addFrameOfShadow( extraShadow );
        }

        std::for_each( rawShadows.begin (), rawShadows.end (), DeleteIt() );
}

}
