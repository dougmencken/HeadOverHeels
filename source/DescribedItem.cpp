
#include "DescribedItem.hpp"

#include "ItemDescriptions.hpp"
#include "Mediator.hpp"
#include "PoolOfPictures.hpp"

#include "MayNotBePossible.hpp"
#include "NoSuchPictureException.hpp"

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

        const DescriptionOfItem & description = getDescriptionOfItem() ;

        if ( ! description.getNameOfPicturesFile().empty() && ! description.isPartOfDoor() ) {
                createFrames() ;

                if ( description.getWidthOfShadow() > 0 && description.getHeightOfShadow() > 0 )
                        createShadowFrames() ;
        }

        unsigned int framesPerOrientation = howManyFramesInTheCurrentSequence() ;

        if ( ! description.isPartOfDoor() ) {
                if ( framesPerOrientation != description.howManyFramesPerOrientation() )
                        throw MayNotBePossible( "the current sequence for item \"" + getKind()
                                                + "\" has more or less frames (" + util::number2string( framesPerOrientation )
                                                + ") than the number of frames per each orientation ("
                                                + util::number2string( description.howManyFramesPerOrientation() ) + ")" );
        } else
          if ( framesPerOrientation != 0 ) // images of door lintel and jambs are cut out from the door picture
                                           // thus no frames are read from file
                        throw MayNotBePossible( "the number of frames (" + util::number2string( framesPerOrientation )
                                                + ") for a door part \"" + getKind() + "\" is not zero" );

        size_t soManyFrames = ( framesPerOrientation * description.howManyOrientations() ) + description.howManyExtraFrames() ;

        std::ostringstream readOrAbsent ;
        readOrAbsent << "graphics for item \"" << getUniqueName() << "\" " ;
        if ( getKind() != getOriginalKind() ) readOrAbsent << "(\"" << getKind() << "\") " ;
        if ( soManyFrames > 0 )
                readOrAbsent << "consisting of " << soManyFrames << " frame" << ( soManyFrames == 1 ? " " : "s " ) << "were read" ;
        else
        if ( description.isPartOfDoor() )
                readOrAbsent << "will be cut out from the door picture" ;
        else
                readOrAbsent << "are absent" ;

        std::cout << readOrAbsent.str() << std::endl ;
}

NamedPicture & DescribedItem::getNthShadowIn ( const std::string & sequence, unsigned int n ) const /* throws NoSuchPictureException */
{
        std::map< std::string, std::vector< NamedPicturePtr > >::const_iterator it = this->shadows.begin() ;
        for ( ; it != this->shadows.end() ; ++ it )
                if ( it->first == sequence && n < it->second.size() )
                        return *( it->second[ n ] );

        std::string message = "there’s no " + util::toStringWithOrdinalSuffix( n ) + " shadow in \"" + sequence + "\" for \"" + getUniqueName() + "\"" ;
        std::cerr << message << std::endl ;
        throw NoSuchPictureException( message );
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

        std::vector< Picture* > rawFrames;

        for ( unsigned int y = 0; y < picture->getHeight(); y += description.getHeightOfFrame() ) {
                for ( unsigned int x = 0; x < picture->getWidth(); x += description.getWidthOfFrame() )
                {
                        Picture* rawFrame = new Picture( description.getWidthOfFrame(), description.getHeightOfFrame() );
                        allegro::bitBlit( picture->getAllegroPict(), rawFrame->getAllegroPict(), x, y, 0, 0, rawFrame->getWidth(), rawFrame->getHeight() );
                        rawFrames.push_back( rawFrame );
                }
        }

        // split frames by orientations

        unsigned int howManyOrientations = description.howManyOrientations() ;
        std::vector< std::string > orientations = whatOrientations() ;

        unsigned int rawRow = ( rawFrames.size() - description.howManyExtraFrames() ) / howManyOrientations ;

        for ( unsigned int o = 0 ; o < howManyOrientations ; o ++ ) {
                for ( unsigned int f = 0 ; f < description.howManyFramesPerOrientation() ; f ++ )
                {
                        NamedPicture * animationFrame = new NamedPicture( * rawFrames[ ( o * rawRow ) + description.getFrameAt( f ) ] );
                        animationFrame->setName( description.getKind () + " " +
                                                        "\"" + orientations[ o ] + "\" orientation " +
                                                        util::toStringWithOrdinalSuffix( f ) + " frame" );

                # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                        animationFrame->saveAsPNG( ospaths::homePath() );
                # endif

                        addFrameTo( orientations[ o ], animationFrame );
                }
        }

        // add extra frames, if any

        for ( unsigned int extra = 0 ; extra < description.howManyExtraFrames() ; extra ++ )
        {
                NamedPicture * extraFrame = new NamedPicture( * rawFrames[ extra + ( rawRow * howManyOrientations ) ] );
                extraFrame->setName( description.getKind () + " " + util::toStringWithOrdinalSuffix( extra ) + " extra frame" );
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

        std::vector< Picture* > rawShadows;

        for ( unsigned int y = 0; y < picture->getHeight(); y += description.getHeightOfShadow() ) {
                for ( unsigned int x = 0; x < picture->getWidth(); x += description.getWidthOfShadow() )
                {
                        Picture* rawShadow = new Picture( description.getWidthOfShadow(), description.getHeightOfShadow() );
                        allegro::bitBlit( picture->getAllegroPict(), rawShadow->getAllegroPict(), x, y, 0, 0, rawShadow->getWidth(), rawShadow->getHeight() );
                        rawShadows.push_back( rawShadow );
                }
        }

        // split frames of shadow by orientations

        unsigned int howManyOrientations = description.howManyOrientations() ;
        std::vector< std::string > orientations = whatOrientations() ;

        unsigned int rawRow = ( rawShadows.size() - description.howManyExtraFrames() ) / howManyOrientations ;

        for ( unsigned int o = 0 ; o < howManyOrientations ; o ++ ) {
                for ( unsigned int f = 0 ; f < description.howManyFramesPerOrientation() ; f ++ )
                {
                        NamedPicture * shadowFrame = new NamedPicture( * rawShadows[ ( o * rawRow ) + description.getFrameAt( f ) ] );
                        shadowFrame->setName( description.getKind () + " " +
                                                "\"" + orientations[ o ] + "\" orientation " +
                                                util::toStringWithOrdinalSuffix( f ) + " shadow" );

                # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                        shadowFrame->saveAsPNG( ospaths::homePath() );
                # endif

                        addFrameOfShadowTo( orientations[ o ], shadowFrame );
                }
        }

        // add extra shadow frames, if any

        for ( unsigned int extra = 0 ; extra < description.howManyExtraFrames() ; extra ++ )
        {
                NamedPicture * extraShadow = new NamedPicture( * rawShadows[ extra + ( rawRow * howManyOrientations ) ] );
                extraShadow->setName( description.getKind () + " " + util::toStringWithOrdinalSuffix( extra ) + " extra shadow" );
        # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                extraShadow->saveAsPNG( ospaths::homePath() );
        # endif
                addFrameOfShadowTo( "extra", extraShadow );
        }

        std::for_each( rawShadows.begin(), rawShadows.end(), DeleteIt () );
}
