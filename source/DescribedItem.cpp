
#include "DescribedItem.hpp"

#include "ItemDescriptions.hpp"
#include "Mediator.hpp"
#include "PoolOfPictures.hpp"


void DescribedItem::metamorphInto( const std::string & newKind, const std::string & initiatedBy )
{
        const DescriptionOfItem * description = ItemDescriptions::descriptions().getDescriptionByKind( newKind );
        if ( description == nilPointer ) return ;

        this->descriptionOfItem = description ;

        std::cout << "metamorphosis of item \"" << getUniqueName()
                        << "\" into \"" << getDescriptionOfItem().getKind ()
                        << "\" initiated by \"" << initiatedBy << "\"" << std::endl ;

        readGraphicsOfItem ();

        setupAnimation () ;
}

bool DescribedItem::canAdvanceTo( int dx, int dy, int dz )
{
        // the coordinates before
        int originalX = getX() ;
        int originalY = getY() ;
        int originalZ = getZ() ;

        assert( getMediator() != nilPointer );
        getMediator()->clearCollisions ();

        bool collisionFound = false ;

        // the new coordinates
        setX( originalX + dx );
        setY( originalY + dy );
        setZ( originalZ + dz );

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

        // look for a collision with the floor
        if ( this->getZ() < 0 && getMediator()->getRoom()->hasFloor() ) {
                getMediator()->addCollisionWith( "some tile of floor" );
        }

        collisionFound = getMediator()->isThereAnyCollision ();
        if ( ! collisionFound ) {
                // look for collisions with other items in the room
                collisionFound = getMediator()->collectCollisionsWith( this->getUniqueName() );
        }

        // restore the original coordinates
        setX( originalX );
        setY( originalY );
        setZ( originalZ );

        return ! collisionFound ;
}

bool DescribedItem::overlapsWith( const DescribedItem & anotherItem ) const
{
        return  ( this->getX() < anotherItem.getX() + anotherItem.getWidthX() ) &&
                        ( anotherItem.getX() < this->getX() + this->getWidthX() ) &&
                ( this->getY() > anotherItem.getY() - anotherItem.getWidthY() ) &&
                        ( anotherItem.getY() > this->getY() - this->getWidthY() ) &&
                ( this->getZ() < anotherItem.getZ() + anotherItem.getHeight() ) &&
                        ( anotherItem.getZ() < this->getZ() + this->getHeight() ) ;
}

void DescribedItem::freshProcessedImage()
{
        this->processedImage->fillWithColor( Color::keyColor () );
        allegro::bitBlit( /* from */ getRawImage().getAllegroPict(), /* to */ this->processedImage->getAllegroPict() );
        this->processedImage->setName( "processed " + getRawImage().getName() );
}

unsigned int DescribedItem::firstFrameWhenHeading ( const std::string & where ) const
{
        if ( getDescriptionOfItem().howManyOrientations() > 1 ) {
                unsigned int orientOccident = where.empty() ? 0 : Way( where ).getIntegerOfWay() ;
                return getDescriptionOfItem().howManyFramesPerOrientation() * orientOccident ;
        }

        return AbstractItem::firstFrame() ;
}

void DescribedItem::animate()
{
        if ( ! isAnimated () ) return ;

        // is it time to change frame
        if ( this->animationTimer.getValue() > getDescriptionOfItem().getDelayBetweenFrames() )
        {
                unsigned int newFrame = getCurrentFrame() ;

                if ( ! isAnimatedBackwards () ) // forwards motion
                {
                        if ( isAnimationFinished() )
                                newFrame = firstFrame() ;
                        else
                                ++ newFrame ;
                }
                else // backwards motion
                {
                        if ( isAnimationFinished() )
                                newFrame = firstFrame() + getDescriptionOfItem().howManyFramesPerOrientation() - 1 ;
                        else
                                -- newFrame ;
                }

                changeFrame( newFrame );

                this->animationTimer.go() ; // reset the timer
        }
}

bool DescribedItem::isAnimationFinished() const
{
        if ( atExtraFrame() ) return true ;

        if ( isAnimatedBackwards () )
                return getCurrentFrame() == firstFrame() ;
        else
                return getCurrentFrame() + 1 == firstFrame() + getDescriptionOfItem().howManyFramesPerOrientation() ;
}

bool DescribedItem::atExtraFrame() const
{
        return getCurrentFrame() >= howManyFrames() - getDescriptionOfItem().howManyExtraFrames() ;
}

/* private */
void DescribedItem::readGraphicsOfItem ()
{
        clearFrames ();

        if ( ! getDescriptionOfItem().getNameOfPicturesFile().empty() && ! getDescriptionOfItem().isPartOfDoor() ) {
                createFrames() ;

                if ( getDescriptionOfItem().getWidthOfShadow() > 0 && getDescriptionOfItem().getHeightOfShadow() > 0 )
                        createShadowFrames() ;
        }

        if ( howManyFrames() == 0 )
                addFrame( new Picture( getDescriptionOfItem().getWidthOfFrame(), getDescriptionOfItem().getHeightOfFrame() ) );
}

#ifdef DEBUG
#  define SAVE_ITEM_FRAMES      0
#endif

/* private */
void DescribedItem::createFrames ()
{
        if ( getDescriptionOfItem().getWidthOfFrame() == 0 || getDescriptionOfItem().getHeightOfFrame() == 0 ) {
                std::cerr << "zero width or height of frame at AbstractItem::createFrames" << std::endl ;
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

        for ( unsigned int y = 0; y < picture->getHeight(); y += getDescriptionOfItem().getHeightOfFrame() ) {
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

        for ( unsigned int o = 0 ; o < getDescriptionOfItem().howManyOrientations() ; o ++ ) {
                for ( unsigned int f = 0 ; f < getDescriptionOfItem().howManyFramesPerOrientation() ; f ++ )
                {
                        Picture * animationFrame = new Picture( * rawFrames[ ( o * rawRow ) + getDescriptionOfItem().getFrameAt( f ) ] );
                        animationFrame->setName( getDescriptionOfItem().getKind () + " " +
                                                        util::toStringWithOrdinalSuffix( o ) + " orientation " +
                                                        util::toStringWithOrdinalSuffix( f ) + " frame" );

                # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                        animationFrame->saveAsPNG( ospaths::homePath() );
                # endif

                        addFrame( animationFrame );
                }
        }

        for ( unsigned int ex = 0 ; ex < getDescriptionOfItem().howManyExtraFrames() ; ex ++ )
        {
                Picture * extraFrame = new Picture( * rawFrames[ ex + ( rawRow * getDescriptionOfItem().howManyOrientations() ) ] );
                extraFrame->setName( getDescriptionOfItem().getKind () + " " + util::toStringWithOrdinalSuffix( ex ) + " extra frame" );
        # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                extraFrame->saveAsPNG( ospaths::homePath() );
        # endif
                addFrame( extraFrame );
        }

        std::for_each( rawFrames.begin (), rawFrames.end (), DeleteIt() );
}

/* private */
void DescribedItem::createShadowFrames ()
{
        if ( getDescriptionOfItem().getWidthOfShadow() == 0 || getDescriptionOfItem().getHeightOfShadow() == 0 ) {
                std::cerr << "zero width or height of shadow at AbstractItem::createShadowFrames" << std::endl ;
                return ;
        }

        assert( ! getDescriptionOfItem().getNameOfShadowsFile().empty() );

        PicturePtr picture = PoolOfPictures::getPoolOfPictures().getOrLoadAndGetOrMakeAndGet(
                                        getDescriptionOfItem().getNameOfShadowsFile(),
                                                getDescriptionOfItem().getWidthOfShadow(), getDescriptionOfItem().getHeightOfShadow() );

        // decompose image into frames

        std::vector< Picture* > rawShadows;

        for ( unsigned int y = 0; y < picture->getHeight(); y += getDescriptionOfItem().getHeightOfShadow() ) {
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

        for ( unsigned int o = 0 ; o < getDescriptionOfItem().howManyOrientations() ; o ++ ) {
                for ( unsigned int f = 0 ; f < getDescriptionOfItem().howManyFramesPerOrientation() ; f ++ )
                {
                        Picture * shadowFrame = new Picture( * rawShadows[ ( o * rawRow ) + getDescriptionOfItem().getFrameAt( f ) ] );
                        shadowFrame->setName( getDescriptionOfItem().getKind () + " " +
                                                util::toStringWithOrdinalSuffix( o ) + " orientation " +
                                                util::toStringWithOrdinalSuffix( f ) + " shadow" );

                # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                        shadowFrame->saveAsPNG( ospaths::homePath() );
                # endif

                        addFrameOfShadow( shadowFrame );
                }
        }

        for ( unsigned int ex = 0 ; ex < getDescriptionOfItem().howManyExtraFrames() ; ex ++ )
        {
                Picture * extraShadow = new Picture( * rawShadows[ ex + ( rawRow * getDescriptionOfItem().howManyOrientations() ) ] );
                extraShadow->setName( getDescriptionOfItem().getKind () + " " + util::toStringWithOrdinalSuffix( ex ) + " extra shadow" );
        # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                extraShadow->saveAsPNG( ospaths::homePath() );
        # endif
                addFrameOfShadow( extraShadow );
        }

        std::for_each( rawShadows.begin(), rawShadows.end(), DeleteIt () );
}
