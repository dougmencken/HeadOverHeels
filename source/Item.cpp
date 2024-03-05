
#include "Item.hpp"

#include "DescriptionOfItem.hpp"
#include "ItemDescriptions.hpp"
#include "Mediator.hpp"
#include "Behavior.hpp"
#include "CreatorOfBehaviors.hpp"
#include "GameManager.hpp"
#include "Way.hpp"

#include <util.hpp>

#include <algorithm> // std::for_each

#include <iostream>


PoolOfPictures * Item::poolOfPictures = new PoolOfPictures( ) ;


Item::Item( const DescriptionOfItem* description, int z, const std::string& way )
      : Mediated(), Shady(),
        descriptionOfItem( description ),
        uniqueName( description->getKind () + "." + util::makeRandomString( 12 ) ),
        originalKind( description->getKind () ),
        processedImage( new Picture( description->getWidthOfFrame(), description->getHeightOfFrame() ) ),
        xYet( 0 ),
        yYet( 0 ),
        zYet( z ),
        height( description->getHeight() ),
        heading( way == "none" ? "nowhere" : way ),
        currentFrame( firstFrame () ),
        backwardsMotion( false ),
        ignoreCollisions( true ),
        motionTimer( new Timer () ),
        behavior( nilPointer ),
        carrier( "" )
{
        readGraphicsOfItem ();

        // item with more than one frame per orientation is animated
        if ( descriptionOfItem->howManyFramesPerOrientation() > 1 )
        {
                motionTimer->go();
        }
}

Item::Item( const Item& item )
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
        motionTimer( new Timer () ),
        behavior( nilPointer ),
        carrier( item.carrier )
{
        for ( std::vector< PicturePtr >::const_iterator it = item.motion.begin (); it != item.motion.end (); ++ it )
        {
                motion.push_back( PicturePtr( new Picture( ( *it )->getAllegroPict () ) ) );
        }

        for ( std::vector< PicturePtr >::const_iterator it = item.shadows.begin (); it != item.shadows.end (); ++ it )
        {
                shadows.push_back( PicturePtr( new Picture( ( *it )->getAllegroPict () ) ) );
        }

        if ( descriptionOfItem->howManyFramesPerOrientation() > 1 )
        {
                motionTimer->go();
        }
}

Item::~Item( )
{
        motion.clear ();
        shadows.clear ();
}

bool Item::updateItem ()
{
        return ( this->behavior != nilPointer ) ? ( ! this->behavior->update_returningdisappearance () ) : true ;
}

/* private */
void Item::readGraphicsOfItem ()
{
        motion.clear ();
        shadows.clear ();

        if ( ! descriptionOfItem->isPartOfDoor() && ! descriptionOfItem->getNameOfPicturesFile().empty() )
        {
                createFrames( this, *descriptionOfItem );

                currentFrame = firstFrame ();

                if ( descriptionOfItem->getWidthOfShadow() > 0 && descriptionOfItem->getHeightOfShadow() > 0 )
                        createShadowFrames( this, *descriptionOfItem );
        }

        if ( motion.empty() )
                addFrame( Picture( descriptionOfItem->getWidthOfFrame(), descriptionOfItem->getHeightOfFrame() ) );

        updateImage ();
}

void Item::animate()
{
        if ( descriptionOfItem->howManyFramesPerOrientation() > 1 )
        {
                // is it time to change frame
                if ( motionTimer->getValue() > descriptionOfItem->getDelayBetweenFrames() )
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
                                        newFrame = firstFrame() + descriptionOfItem->howManyFramesPerOrientation() - 1 ;
                                else
                                        -- newFrame ;
                        }

                        changeFrame( newFrame );

                        motionTimer->reset();
                }
        }
}

bool Item::animationFinished() const
{
        if ( atExtraFrame() ) return true ;

        if ( backwardsMotion )
                return currentFrame == firstFrame() ;
        else
                return currentFrame + 1 == firstFrame() + descriptionOfItem->howManyFramesPerOrientation() ;
}

bool Item::atExtraFrame() const
{
        return currentFrame >= howManyMotions() - descriptionOfItem->howManyExtraFrames() ;
}

void Item::metamorphInto( const std::string & newKind, const std::string & initiatedBy )
{
        const DescriptionOfItem * description = ItemDescriptions::descriptions().getDescriptionByKind( newKind );
        if ( description == nilPointer ) return ;

        std::cout << "metamorphosis of item \"" << getUniqueName ()
                        << "\" into \"" << description->getKind ()
                        << "\" initiated by \"" << initiatedBy << "\"" << std::endl ;

        this->descriptionOfItem = description ;

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
        if ( howManyMotions() > newFrame )
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

        mediator->clearCollisions ();

        bool collisionFound = false;

        // the new coordinates
        xYet += x ;
        yYet += y ;
        zYet += z ;

        // look for collisions with walls
        if ( this->getX() < mediator->getRoom()->getLimitAt( "north" ) ) {
                mediator->addCollisionWith( "some segment of wall at north" );
        }
        else if ( this->getX() + this->getWidthX() > mediator->getRoom()->getLimitAt( "south" ) ) {
                mediator->addCollisionWith( "some segment of wall at south" );
        }
        if ( this->getY() >= mediator->getRoom()->getLimitAt( "west" ) ) {
                mediator->addCollisionWith( "some segment of wall at west" );
        }
        else if ( this->getY() - this->getWidthY() + 1 < mediator->getRoom()->getLimitAt( "east" ) ) {
                mediator->addCollisionWith( "some segment of wall at east" );
        }

        // look for a collision with floor
        if ( this->getZ() < 0 && mediator->getRoom()->hasFloor() ) {
                mediator->addCollisionWith( "some tile of floor" );
        }

        collisionFound = mediator->isThereAnyCollision ();
        if ( ! collisionFound ) {
                // look for collisions with other items in the room
                collisionFound = mediator->collectCollisionsWith( this->getUniqueName() );
        }

        // restore the original coordinates
        xYet = originalX ;
        yYet = originalY ;
        zYet = originalZ ;

        return ! collisionFound ;
}

bool Item::crossesWith( const Item & item ) const
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
        currentFrame = firstFrame() + descriptionOfItem->howManyFramesPerOrientation() - 1 ;
}

const std::string & Item::getKind () const
{
        return descriptionOfItem->getKind () ;
}

unsigned int Item::getUnsignedWidthX () const
{
        return descriptionOfItem->getWidthX() ;
}

unsigned int Item::getUnsignedWidthY () const
{
        return descriptionOfItem->getWidthY() ;
}

bool Item::isMortal() const
{
        return descriptionOfItem->isMortal() ;
}

unsigned int Item::firstFrameWhenHeading ( const std::string & where ) const
{
        if ( descriptionOfItem->howManyOrientations() > 1 )
        {
                unsigned int integerOfWay = Way( where ).getIntegerOfWay() ;
                unsigned int orientOccident = ( integerOfWay == Way::Nowhere ? 0 : integerOfWay );
                return descriptionOfItem->howManyFramesPerOrientation() * orientOccident ;
        }

        return 0 ;
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
        if ( description.getNameOfPicturesFile().empty() || description.getWidthOfFrame() == 0 || description.getHeightOfFrame() == 0 )
        {
                std::cerr << "either name of picture file is empty or zero width / height at Item::createFrames" << std::endl ;
                return ;
        }

        PicturePtr picture = getPoolOfPictures().getOrLoadAndGetOrMakeAndGet( description.getNameOfPicturesFile(), description.getWidthOfFrame(), description.getHeightOfFrame() );

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
                        rawFrame->setName( description.getKind () + " " + util::toStringWithOrdinalSuffix( rawFrames.size() ) + " raw frame" );
                # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                        rawFrame->saveAsPNG( ospaths::homePath() );
                # endif
                        rawFrames.push_back( rawFrame );
                }
        }

        unsigned int rawRow = ( rawFrames.size() - description.howManyExtraFrames() ) / description.howManyOrientations();

        for ( unsigned int o = 0 ; o < description.howManyOrientations() ; o ++ )
        {
                for ( unsigned int f = 0 ; f < description.howManyFramesPerOrientation() ; f ++ )
                {
                        Picture animationFrame( * rawFrames[ ( o * rawRow ) + description.getFrameAt( f ) ] );
                        animationFrame.setName( description.getKind () + " " +
                                                        util::toStringWithOrdinalSuffix( o ) + " orientation " +
                                                        util::toStringWithOrdinalSuffix( f ) + " frame" );

                # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                        animationFrame.saveAsPNG( ospaths::homePath() );
                # endif

                        item->addFrame( animationFrame );
                }
        }
        for ( unsigned int e = 0 ; e < description.howManyExtraFrames() ; e ++ )
        {
                Picture extraFrame( * rawFrames[ e + ( rawRow * description.howManyOrientations() ) ] );
                extraFrame.setName( description.getKind () + " " + util::toStringWithOrdinalSuffix( e ) + " extra frame" );
        # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                extraFrame.saveAsPNG( ospaths::homePath() );
        # endif
                item->addFrame( extraFrame );
        }

        std::for_each( rawFrames.begin (), rawFrames.end (), DeleteIt() );
}

/* static */
void Item::createShadowFrames( Item* item, const DescriptionOfItem& description )
{
        if ( description.getNameOfShadowsFile( ).empty() || description.getWidthOfShadow() == 0 || description.getHeightOfShadow() == 0 )
        {
                std::cerr << "either name of shadow file is empty or zero width / height at Item::createShadowFrames" << std::endl ;
                return ;
        }

        PicturePtr picture = getPoolOfPictures().getOrLoadAndGetOrMakeAndGet( description.getNameOfShadowsFile(), description.getWidthOfShadow(), description.getHeightOfShadow() );

        // decompose image into frames

        std::vector< Picture* > rawShadows;

        for ( unsigned int y = 0; y < picture->getHeight(); y += description.getHeightOfShadow() )
        {
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

        unsigned int rawRow = ( rawShadows.size() - description.howManyExtraFrames() ) / description.howManyOrientations();

        for ( unsigned int o = 0 ; o < description.howManyOrientations() ; o ++ )
        {
                for ( unsigned int f = 0 ; f < description.howManyFramesPerOrientation() ; f ++ )
                {
                        Picture shadowFrame( * rawShadows[ ( o * rawRow ) + description.getFrameAt( f ) ] );
                        shadowFrame.setName( description.getKind () + " " +
                                                util::toStringWithOrdinalSuffix( o ) + " orientation " +
                                                util::toStringWithOrdinalSuffix( f ) + " shadow" );

                # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                        shadowFrame.saveAsPNG( ospaths::homePath() );
                # endif

                        item->addFrameOfShadow( shadowFrame );
                }
        }
        for ( unsigned int e = 0 ; e < description.howManyExtraFrames() ; e ++ )
        {
                Picture extraShadow( * rawShadows[ e + ( rawRow * description.howManyOrientations() ) ] );
                extraShadow.setName( description.getKind () + " " + util::toStringWithOrdinalSuffix( e ) + " extra shadow" );
        # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                extraShadow.saveAsPNG( ospaths::homePath() );
        # endif
                item->addFrameOfShadow( extraShadow );
        }

        std::for_each( rawShadows.begin (), rawShadows.end (), DeleteIt() );
}
