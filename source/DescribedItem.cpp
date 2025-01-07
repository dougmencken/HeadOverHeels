
#include "DescribedItem.hpp"

#include "ItemDescriptions.hpp"
#include "Mediator.hpp"
#include "PoolOfPictures.hpp"
#include "MayNotBePossible.hpp"

#include <sstream>


void DescribedItem::metamorphInto( const std::string & newKind, const std::string & initiatedBy )
{
        const DescriptionOfItem * description = ItemDescriptions::descriptions().getDescriptionByKind( newKind );
        if ( description == nilPointer ) {
                std::cerr << "item \"" << getUniqueName() << "\" doesn’t metamorph"
                                << " to the non-existent kind \"" << newKind
                                << "\" initiated by \"" << initiatedBy << "\"" << std::endl ;
                return ;
        }

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
        Mediator & mediator = * getMediator() ;

        mediator.clearCollisions ();
        bool collisionFound = false ;

        // the new coordinates
        setX( originalX + dx );
        setY( originalY + dy );
        setZ( originalZ + dz );

        // look for collisions with walls
        if ( this->getX() < mediator.getRoom()->getLimitAt( "north" ) ) {
                mediator.addCollisionWith( "some segment of the north wall" );
        }
        else if ( this->getX() + this->getWidthX() > mediator.getRoom()->getLimitAt( "south" ) ) {
                mediator.addCollisionWith( "some segment of the south wall" );
        }
        if ( this->getY() >= mediator.getRoom()->getLimitAt( "west" ) ) {
                mediator.addCollisionWith( "some segment of the west wall" );
        }
        else if ( this->getY() - this->getWidthY() + 1 < mediator.getRoom()->getLimitAt( "east" ) ) {
                mediator.addCollisionWith( "some segment of the east wall" );
        }

        // look for a collision with the floor
        if ( this->getZ() < 0 && mediator.getRoom()->hasFloor() ) {
                mediator.addCollisionWith( "some tile of floor" );
        }

        collisionFound = mediator.isThereAnyCollision ();
        if ( ! collisionFound ) {
                // look for collisions with other items in the room
                collisionFound = mediator.collectCollisionsWith( this->getUniqueName() );
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
        allegro::bitBlit( /* from */ getCurrentRawImage().getAllegroPict(), /* to */ this->processedImage->getAllegroPict() );
        this->processedImage->setName( "processed " + getCurrentRawImage().getName() );
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

/* private */
void DescribedItem::readGraphicsOfItem ()
{
        clearFrames ();

        if ( ! getDescriptionOfItem().getNameOfPicturesFile().empty() && ! getDescriptionOfItem().isPartOfDoor() ) {
                createFrames() ;

                if ( getDescriptionOfItem().getWidthOfShadow() > 0 && getDescriptionOfItem().getHeightOfShadow() > 0 )
                        createShadowFrames() ;
        }

        size_t howManyFrames = howManyFramesAtAll() ;

        std::ostringstream readOrAbsent ;
        readOrAbsent << "graphics for item \"" << getUniqueName() << "\" " ;
        if ( getKind() != getOriginalKind() ) readOrAbsent << "(\"" << getKind() << "\") " ;
        if ( howManyFrames > 0 )
                readOrAbsent << "consisting of " << howManyFrames << " frame" << ( howManyFrames == 1 ? " " : "s " ) << "were read" ;
        else
                readOrAbsent << "are absent" ;

        readOrAbsent << ", the current frame sequence is \"" << getCurrentFrameSequence() << "\"" ;
        std::cout << readOrAbsent.str() << std::endl ;

        if ( howManyFrames == 0 )
                addFrameTo( "", new Picture( getDescriptionOfItem().getWidthOfFrame(), getDescriptionOfItem().getHeightOfFrame() ) );
}

#ifdef DEBUG
#  define SAVE_ITEM_FRAMES      0
#endif

/* private */
void DescribedItem::createFrames ()
{
        const DescriptionOfItem & description = getDescriptionOfItem() ;

        if ( description.getWidthOfFrame() == 0 || description.getHeightOfFrame() == 0 )
                throw MayNotBePossible( "zero width or height of frame for item \"" + getKind() + "\"" );
        if ( description.getNameOfPicturesFile().empty() )
                throw MayNotBePossible( "empty file name with graphics for item \"" + getKind() + "\"" );

        PicturePtr picture = PoolOfPictures::getPoolOfPictures().getOrLoadAndGetOrMakeAndGet(
                                        description.getNameOfPicturesFile(),
                                                description.getWidthOfFrame(), description.getHeightOfFrame() );

        // decompose the image into frames
        // they are
        //    frames of animation ( 1 or more )
        //    frames of animation
        //    ... ( × "orientations": 1, 2, or 4 times )
        //    extra frames ( 0 or more )

        std::vector< std::string > orientations ;
        unsigned int howManyOrientations = description.howManyOrientations() ;
        if ( ! getCurrentFrameSequence().empty() && howManyOrientations == 1 )
                orientations.push_back( getCurrentFrameSequence() );
        else
                orientations.push_back( "south" );

        if ( howManyOrientations > 1 ) /* south and west */ {
                orientations.push_back( "west" );

                if ( howManyOrientations > 2 ) /* south, west, north, east */ {
                        orientations.push_back( "north" );
                        orientations.push_back( "east" );
                }
        }

        std::vector< Picture* > rawFrames;

        for ( unsigned int y = 0; y < picture->getHeight(); y += description.getHeightOfFrame() ) {
                for ( unsigned int x = 0; x < picture->getWidth(); x += description.getWidthOfFrame() )
                {
                        Picture* rawFrame = new Picture( description.getWidthOfFrame(), description.getHeightOfFrame() );
                        allegro::bitBlit( picture->getAllegroPict(), rawFrame->getAllegroPict(), x, y, 0, 0, rawFrame->getWidth(), rawFrame->getHeight() );
                        rawFrame->setName( description.getKind () + " " + util::toStringWithOrdinalSuffix( rawFrames.size() ) + " raw frame" );
                # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                        rawFrame->saveAsPNG( ospaths::homePath() );
                # endif
                        rawFrames.push_back( rawFrame );
                }
        }

        unsigned int rawRow = ( rawFrames.size() - description.howManyExtraFrames() ) / howManyOrientations ;

        for ( unsigned int o = 0 ; o < howManyOrientations ; o ++ ) {
                for ( unsigned int f = 0 ; f < description.howManyFramesPerOrientation() ; f ++ )
                {
                        Picture * animationFrame = new Picture( * rawFrames[ ( o * rawRow ) + description.getFrameAt( f ) ] );
                        animationFrame->setName( description.getKind () + " " +
                                                        orientations[ o ] + " orientation " +
                                                        util::toStringWithOrdinalSuffix( f ) + " frame" );

                # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                        animationFrame->saveAsPNG( ospaths::homePath() );
                # endif

                        addFrameTo( orientations[ o ], animationFrame );
                }
        }

        for ( unsigned int ex = 0 ; ex < description.howManyExtraFrames() ; ex ++ )
        {
                Picture * extraFrame = new Picture( * rawFrames[ ex + ( rawRow * howManyOrientations ) ] );
                extraFrame->setName( description.getKind () + " " + util::toStringWithOrdinalSuffix( ex ) + " extra frame" );
        # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                extraFrame->saveAsPNG( ospaths::homePath() );
        # endif
                addFrameTo( "extra", extraFrame );
        }

        std::for_each( rawFrames.begin (), rawFrames.end (), DeleteIt() );
}

/* private */
void DescribedItem::createShadowFrames ()
{
        const DescriptionOfItem & description = getDescriptionOfItem() ;

        if ( description.getWidthOfShadow() == 0 || description.getHeightOfShadow() == 0 )
                throw MayNotBePossible( "zero width or height of shadow for item \"" + getKind() + "\"" );
        if ( description.getNameOfShadowsFile().empty() )
                throw MayNotBePossible( "empty file name with graphics of shadow for item \"" + getKind() + "\"" );

        PicturePtr picture = PoolOfPictures::getPoolOfPictures().getOrLoadAndGetOrMakeAndGet(
                                        description.getNameOfShadowsFile(),
                                                description.getWidthOfShadow(), description.getHeightOfShadow() );

        // decompose the image of shadow into frames

        std::vector< std::string > orientations ;
        unsigned int howManyOrientations = description.howManyOrientations() ;
        if ( ! getCurrentFrameSequence().empty() && howManyOrientations == 1 )
                orientations.push_back( getCurrentFrameSequence() );
        else
                orientations.push_back( "south" );

        if ( howManyOrientations > 1 ) /* south and west */ {
                orientations.push_back( "west" );

                if ( howManyOrientations > 2 ) /* south, west, north, east */ {
                        orientations.push_back( "north" );
                        orientations.push_back( "east" );
                }
        }

        std::vector< Picture* > rawShadows;

        for ( unsigned int y = 0; y < picture->getHeight(); y += description.getHeightOfShadow() ) {
                for ( unsigned int x = 0; x < picture->getWidth(); x += description.getWidthOfShadow() )
                {
                        Picture* rawShadow = new Picture( description.getWidthOfShadow(), description.getHeightOfShadow() );
                        allegro::bitBlit( picture->getAllegroPict(), rawShadow->getAllegroPict(), x, y, 0, 0, rawShadow->getWidth(), rawShadow->getHeight() );
                        rawShadow->setName( description.getKind () + " " + util::toStringWithOrdinalSuffix( rawShadows.size() ) + " raw shadow" );
                # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                        rawShadow->saveAsPNG( ospaths::homePath() );
                # endif
                        rawShadows.push_back( rawShadow );
                }
        }

        unsigned int rawRow = ( rawShadows.size() - description.howManyExtraFrames() ) / howManyOrientations ;

        for ( unsigned int o = 0 ; o < howManyOrientations ; o ++ ) {
                for ( unsigned int f = 0 ; f < description.howManyFramesPerOrientation() ; f ++ )
                {
                        Picture * shadowFrame = new Picture( * rawShadows[ ( o * rawRow ) + description.getFrameAt( f ) ] );
                        shadowFrame->setName( description.getKind () + " " +
                                                orientations[ o ] + " orientation " +
                                                util::toStringWithOrdinalSuffix( f ) + " shadow" );

                # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                        shadowFrame->saveAsPNG( ospaths::homePath() );
                # endif

                        addFrameOfShadowTo( orientations[ o ], shadowFrame );
                }
        }

        for ( unsigned int ex = 0 ; ex < description.howManyExtraFrames() ; ex ++ )
        {
                Picture * extraShadow = new Picture( * rawShadows[ ex + ( rawRow * howManyOrientations ) ] );
                extraShadow->setName( description.getKind () + " " + util::toStringWithOrdinalSuffix( ex ) + " extra shadow" );
        # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                extraShadow->saveAsPNG( ospaths::homePath() );
        # endif
                addFrameOfShadowTo( "extra", extraShadow );
        }

        std::for_each( rawShadows.begin(), rawShadows.end(), DeleteIt () );
}
