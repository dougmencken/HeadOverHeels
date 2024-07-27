
#include "Item.hpp"

#include "DescriptionOfItem.hpp"
#include "ItemDescriptions.hpp"
#include "Mediator.hpp"
#include "Behavior.hpp"
#include "CreatorOfBehaviors.hpp"
#include "GameManager.hpp"
#include "PoolOfPictures.hpp"
#include "Way.hpp"

#include <util.hpp>

#include <algorithm> // std::for_each

#include <iostream>


Item::Item( const DescriptionOfItem & description, int z, const std::string & where )
      : Mediated(), Shady(),
        descriptionOfItem( & description ),
        uniqueName( description.getKind () + "." + util::makeRandomString( 12 ) ),
        originalKind( description.getKind () ),
        processedImage( new Picture( description.getWidthOfFrame(), description.getHeightOfFrame() ) ),
        xYet( 0 ),
        yYet( 0 ),
        zYet( z ),
        height( description.getHeight() ),
        heading( where ),
        currentFrame( firstFrame () ),
        backwardsMotion( false ),
        ignoreCollisions( true ),
        behavior( nilPointer ),
        carrier( "" ),
        transparency( 0 ),
        motionTimer( new Timer () )
{
        readGraphicsOfItem ();

        // item with more than one frame per orientation is animated
        if ( getDescriptionOfItem().howManyFramesPerOrientation() > 1 )
                motionTimer->go() ;
}

Item::Item( const Item & item )
      : Mediated( item ), Shady( item.wantShadow ),
        descriptionOfItem( item.descriptionOfItem ),
        uniqueName( item.uniqueName + " copy" ),
        originalKind( item.originalKind ),
        processedImage( new Picture( * item.processedImage ) ),
        xYet( item.xYet ),
        yYet( item.yYet ),
        zYet( item.zYet ),
        height( item.height ),
        heading( item.heading ),
        currentFrame( item.currentFrame ),
        backwardsMotion( item.backwardsMotion ),
        ignoreCollisions( item.ignoreCollisions ),
        behavior( nilPointer ),
        carrier( item.carrier ),
        transparency( item.transparency ),
        motionTimer( new Timer () )
{
        for ( std::vector< PicturePtr >::const_iterator it = item.frames.begin (); it != item.frames.end (); ++ it )
        {
                this->frames.push_back( PicturePtr( new Picture( ( *it )->getAllegroPict () ) ) );
        }

        for ( std::vector< PicturePtr >::const_iterator it = item.shadows.begin (); it != item.shadows.end (); ++ it )
        {
                this->shadows.push_back( PicturePtr( new Picture( ( *it )->getAllegroPict () ) ) );
        }

        if ( getDescriptionOfItem().howManyFramesPerOrientation() > 1 )
                motionTimer->go() ;
}

Item::~Item( )
{
        frames.clear ();
        shadows.clear ();
}

bool Item::updateItem ()
{
        return ( this->behavior != nilPointer ) ? this->behavior->update() : true ;
}

/* private */
void Item::readGraphicsOfItem ()
{
        frames.clear ();
        shadows.clear ();

        if ( ! getDescriptionOfItem().isPartOfDoor() && ! getDescriptionOfItem().getNameOfPicturesFile().empty() )
        {
                createFrames() ;
                currentFrame = firstFrame ();

                if ( getDescriptionOfItem().getWidthOfShadow() > 0 && getDescriptionOfItem().getHeightOfShadow() > 0 )
                        createShadowFrames() ;
        }

        if ( frames.empty() )
                addFrame( Picture( getDescriptionOfItem().getWidthOfFrame(), getDescriptionOfItem().getHeightOfFrame() ) );

        updateImage ();
}

void Item::animate()
{
        if ( getDescriptionOfItem().howManyFramesPerOrientation() > 1 )
        {
                // is it time to change frame
                if ( motionTimer->getValue() > getDescriptionOfItem().getDelayBetweenFrames() )
                {
                        size_t newFrame = currentFrame ;

                        if ( ! backwardsMotion ) // forwards motion
                        {
                                if ( animationFinished() )
                                        newFrame = firstFrame() ;
                                else
                                        ++ newFrame ;
                        }
                        else // backwards motion
                        {
                                if ( animationFinished() )
                                        newFrame = firstFrame() + getDescriptionOfItem().howManyFramesPerOrientation() - 1 ;
                                else
                                        -- newFrame ;
                        }

                        changeFrame( newFrame );

                        motionTimer->go() ;
                }
        }
}

bool Item::animationFinished() const
{
        if ( atExtraFrame() ) return true ;

        if ( backwardsMotion )
                return currentFrame == firstFrame() ;
        else
                return currentFrame + 1 == firstFrame() + getDescriptionOfItem().howManyFramesPerOrientation() ;
}

bool Item::atExtraFrame() const
{
        return currentFrame >= howManyFrames() - getDescriptionOfItem().howManyExtraFrames() ;
}

void Item::metamorphInto( const std::string & newKind, const std::string & initiatedBy )
{
        const DescriptionOfItem * description = ItemDescriptions::descriptions().getDescriptionByKind( newKind );
        if ( description == nilPointer ) return ;

        this->descriptionOfItem = description ;

        std::cout << "metamorphosis of item \"" << getUniqueName ()
                        << "\" into \"" << getDescriptionOfItem().getKind ()
                        << "\" initiated by \"" << initiatedBy << "\"" << std::endl ;

        doForthMotion ();

        readGraphicsOfItem ();
}

void Item::updateImage ()
{
        freshProcessedImage ();
}

void Item::updateShadow ()
{
}

void Item::freshProcessedImage()
{
        processedImage->fillWithColor( Color::keyColor () );
        allegro::bitBlit( /* from */ getRawImage().getAllegroPict(), /* to */ processedImage->getAllegroPict() );
        processedImage->setName( "processed " + getRawImage().getName() );
}

void Item::changeHeading( const std::string & where )
{
        if ( this->heading != where )
        {
                this->heading = where ;

                changeFrame( firstFrame() );
        }
}

void Item::changeFrame( unsigned int newFrame )
{
        if ( howManyFrames() > newFrame )
        {
                if ( currentFrame != newFrame )
                {
                        currentFrame = newFrame ;

                        updateImage ();

                        if ( ! shadows.empty() ) updateShadow ();
                }
        }
}

bool Item::canAdvanceTo( int x, int y, int z )
{
        // the coordinates before
        int originalX = xYet ;
        int originalY = yYet ;
        int originalZ = zYet ;

        assert( getMediator() != nilPointer );
        getMediator()->clearCollisions ();

        bool collisionFound = false;

        // the new coordinates
        xYet += x ;
        yYet += y ;
        zYet += z ;

        // look for collisions with walls
        if ( this->getX() < getMediator()->getRoom()->getLimitAt( "north" ) ) {
                getMediator()->addCollisionWith( "some segment of the north wall" );
        }
        else if ( this->getX() + this->getWidthX() > getMediator()->getRoom()->getLimitAt( "south" ) ) {
                getMediator()->addCollisionWith( "some segment of the south wall" );
        }
        if ( this->getY() >= getMediator()->getRoom()->getLimitAt( "west" ) ) {
                getMediator()->addCollisionWith( "some segment of the west wall" );
        }
        else if ( this->getY() - this->getWidthY() + 1 < getMediator()->getRoom()->getLimitAt( "east" ) ) {
                getMediator()->addCollisionWith( "some segment of the east wall" );
        }

        // look for a collision with floor
        if ( this->getZ() < 0 && getMediator()->getRoom()->hasFloor() ) {
                getMediator()->addCollisionWith( "some tile of floor" );
        }

        collisionFound = getMediator()->isThereAnyCollision ();
        if ( ! collisionFound ) {
                // look for collisions with other items in the room
                collisionFound = getMediator()->collectCollisionsWith( this->getUniqueName() );
        }

        // restore the original coordinates
        xYet = originalX ;
        yYet = originalY ;
        zYet = originalZ ;

        return ! collisionFound ;
}

bool Item::overlapsWith( const Item & item ) const
{
        return  ( this->getX() < item.getX() + item.getWidthX() ) &&
                        ( item.getX() < this->getX() + this->getWidthX() ) &&
                ( this->getY() > item.getY() - item.getWidthY() ) &&
                        ( item.getY() > this->getY() - this->getWidthY() ) &&
                ( this->getZ() < item.getZ() + item.getHeight() ) &&
                        ( item.getZ() < this->getZ() + this->getHeight() ) ;
}

bool Item::doGraphicsOverlap( const Item & item ) const
{
        return doGraphicsOverlapAt( item, item.getImageOffsetX(), item.getImageOffsetY() );
}

bool Item::doGraphicsOverlapAt( const Item & item, int x, int y ) const
{
        int thisImageWidth = this->getRawImage().getWidth() ;
        int thisImageHeight = this->getRawImage().getHeight() ;
        int thatImageWidth = item.getRawImage().getWidth() ;
        int thatImageHeight = item.getRawImage().getHeight() ;

        return  ( this->getImageOffsetX() < x + thatImageWidth ) && ( x < this->getImageOffsetX() + thisImageWidth )
                        && ( this->getImageOffsetY() < y + thatImageHeight ) && ( y < this->getImageOffsetY() + thisImageHeight ) ;
}

void Item::setBehaviorOf( const std::string & nameOfBehavior )
{
        this->behavior = behaviors::CreatorOfBehaviors::createBehaviorByName( * this , nameOfBehavior );
}

void Item::doForthMotion ()
{
        backwardsMotion = false ;
        currentFrame = firstFrame() ;
}

void Item::doBackwardsMotion ()
{
        backwardsMotion = true ;
        currentFrame = firstFrame() + getDescriptionOfItem().howManyFramesPerOrientation() - 1 ;
}

const std::string & Item::getKind () const
{
        return getDescriptionOfItem().getKind () ;
}

unsigned int Item::getUnsignedWidthX () const
{
        return getDescriptionOfItem().getWidthX() ;
}

unsigned int Item::getUnsignedWidthY () const
{
        return getDescriptionOfItem().getWidthY() ;
}

bool Item::isMortal() const
{
        return getDescriptionOfItem().isMortal() ;
}

unsigned int Item::firstFrameWhenHeading ( const std::string & where ) const
{
        if ( getDescriptionOfItem().howManyOrientations() > 1 )
        {
                unsigned int orientOccident = where.empty() ? 0 : Way( where ).getIntegerOfWay() ;
                return getDescriptionOfItem().howManyFramesPerOrientation() * orientOccident ;
        }

        return 0 ;
}

float Item::getSpeed() const
{
        return getDescriptionOfItem().getSpeed() ;
}

float Item::getWeight() const
{
        return getDescriptionOfItem().getWeight() ;
}

float Item::getDelayBetweenFrames() const
{
        return getDescriptionOfItem().getDelayBetweenFrames() ;
}

#ifdef DEBUG
#  define SAVE_ITEM_FRAMES      0
#endif

void Item::createFrames ()
{
        if ( getDescriptionOfItem().getWidthOfFrame() == 0 || getDescriptionOfItem().getHeightOfFrame() == 0 )
        {
                std::cerr << "zero width or height of frame at Item::createFrames" << std::endl ;
                return ;
        }

        assert( ! getDescriptionOfItem().getNameOfPicturesFile().empty() );

        PicturePtr picture = PoolOfPictures::getPoolOfPictures().getOrLoadAndGetOrMakeAndGet(
                                        getDescriptionOfItem().getNameOfPicturesFile(),
                                                getDescriptionOfItem().getWidthOfFrame(), getDescriptionOfItem().getHeightOfFrame() );

        // decompose image into frames
        // they are
        //    frames of animation ( 1 or more )
        //    frames of animation
        //    ... ( × "orientations": 1, 2, or 4 times )
        //    extra frames ( 0 or more )

        std::vector< Picture* > rawFrames;

        for ( unsigned int y = 0; y < picture->getHeight(); y += getDescriptionOfItem().getHeightOfFrame() )
        {
                for ( unsigned int x = 0; x < picture->getWidth(); x += getDescriptionOfItem().getWidthOfFrame() )
                {
                        Picture* rawFrame = new Picture( getDescriptionOfItem().getWidthOfFrame(), getDescriptionOfItem().getHeightOfFrame() );
                        allegro::bitBlit( picture->getAllegroPict(), rawFrame->getAllegroPict(), x, y, 0, 0, rawFrame->getWidth(), rawFrame->getHeight() );
                        rawFrame->setName( getDescriptionOfItem().getKind () + " " + util::toStringWithOrdinalSuffix( rawFrames.size() ) + " raw frame" );
                # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                        rawFrame->saveAsPNG( ospaths::homePath() );
                # endif
                        rawFrames.push_back( rawFrame );
                }
        }

        unsigned int rawRow = ( rawFrames.size() - getDescriptionOfItem().howManyExtraFrames() ) / getDescriptionOfItem().howManyOrientations();

        for ( unsigned int o = 0 ; o < getDescriptionOfItem().howManyOrientations() ; o ++ )
        {
                for ( unsigned int f = 0 ; f < getDescriptionOfItem().howManyFramesPerOrientation() ; f ++ )
                {
                        Picture animationFrame( * rawFrames[ ( o * rawRow ) + getDescriptionOfItem().getFrameAt( f ) ] );
                        animationFrame.setName( getDescriptionOfItem().getKind () + " " +
                                                        util::toStringWithOrdinalSuffix( o ) + " orientation " +
                                                        util::toStringWithOrdinalSuffix( f ) + " frame" );

                # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                        animationFrame.saveAsPNG( ospaths::homePath() );
                # endif

                        addFrame( animationFrame );
                }
        }
        for ( unsigned int e = 0 ; e < getDescriptionOfItem().howManyExtraFrames() ; e ++ )
        {
                Picture extraFrame( * rawFrames[ e + ( rawRow * getDescriptionOfItem().howManyOrientations() ) ] );
                extraFrame.setName( getDescriptionOfItem().getKind () + " " + util::toStringWithOrdinalSuffix( e ) + " extra frame" );
        # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                extraFrame.saveAsPNG( ospaths::homePath() );
        # endif
                addFrame( extraFrame );
        }

        std::for_each( rawFrames.begin (), rawFrames.end (), DeleteIt() );
}

void Item::createShadowFrames ()
{
        if ( getDescriptionOfItem().getWidthOfShadow() == 0 || getDescriptionOfItem().getHeightOfShadow() == 0 )
        {
                std::cerr << "zero width or height of shadow at Item::createShadowFrames" << std::endl ;
                return ;
        }

        assert( ! getDescriptionOfItem().getNameOfShadowsFile().empty() );

        PicturePtr picture = PoolOfPictures::getPoolOfPictures().getOrLoadAndGetOrMakeAndGet(
                                        getDescriptionOfItem().getNameOfShadowsFile(),
                                                getDescriptionOfItem().getWidthOfShadow(), getDescriptionOfItem().getHeightOfShadow() );

        // decompose image into frames

        std::vector< Picture* > rawShadows;

        for ( unsigned int y = 0; y < picture->getHeight(); y += getDescriptionOfItem().getHeightOfShadow() )
        {
                for ( unsigned int x = 0; x < picture->getWidth(); x += getDescriptionOfItem().getWidthOfShadow() )
                {
                        Picture* rawShadow = new Picture( getDescriptionOfItem().getWidthOfShadow(), getDescriptionOfItem().getHeightOfShadow() );
                        allegro::bitBlit( picture->getAllegroPict(), rawShadow->getAllegroPict(), x, y, 0, 0, rawShadow->getWidth(), rawShadow->getHeight() );
                        rawShadow->setName( getDescriptionOfItem().getKind () + " " + util::toStringWithOrdinalSuffix( rawShadows.size() ) + " raw shadow" );
                # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                        rawShadow->saveAsPNG( ospaths::homePath() );
                # endif
                        rawShadows.push_back( rawShadow );
                }
        }

        unsigned int rawRow = ( rawShadows.size() - getDescriptionOfItem().howManyExtraFrames() ) / getDescriptionOfItem().howManyOrientations();

        for ( unsigned int o = 0 ; o < getDescriptionOfItem().howManyOrientations() ; o ++ )
        {
                for ( unsigned int f = 0 ; f < getDescriptionOfItem().howManyFramesPerOrientation() ; f ++ )
                {
                        Picture shadowFrame( * rawShadows[ ( o * rawRow ) + getDescriptionOfItem().getFrameAt( f ) ] );
                        shadowFrame.setName( getDescriptionOfItem().getKind () + " " +
                                                util::toStringWithOrdinalSuffix( o ) + " orientation " +
                                                util::toStringWithOrdinalSuffix( f ) + " shadow" );

                # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                        shadowFrame.saveAsPNG( ospaths::homePath() );
                # endif

                        addFrameOfShadow( shadowFrame );
                }
        }
        for ( unsigned int e = 0 ; e < getDescriptionOfItem().howManyExtraFrames() ; e ++ )
        {
                Picture extraShadow( * rawShadows[ e + ( rawRow * getDescriptionOfItem().howManyOrientations() ) ] );
                extraShadow.setName( getDescriptionOfItem().getKind () + " " + util::toStringWithOrdinalSuffix( e ) + " extra shadow" );
        # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                extraShadow.saveAsPNG( ospaths::homePath() );
        # endif
                addFrameOfShadow( extraShadow );
        }

        std::for_each( rawShadows.begin (), rawShadows.end (), DeleteIt() );
}
