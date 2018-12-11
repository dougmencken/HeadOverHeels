
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


Item::Item( const DescriptionOfItem* description, int z, const std::string& way )
      : Mediated(), Shady(),
        descriptionOfItem( description ),
        uniqueName( description->getLabel() + "." + makeRandomString( 12 ) ),
        originalLabel( description->getLabel() ),
        processedImage( new Picture( description->getWidthOfFrame(), description->getHeightOfFrame() ) ),
        height( description->getHeight() ),
        orientation( way == "none" ? Way( "nowhere" ) : Way( way ) ),
        currentFrame( firstFrame () ),
        backwardsMotion( false ),
        offset( std::pair< int, int >( 0, 0 ) ),
        collisionDetector( false ),
        motionTimer( new Timer () ),
        behavior( nilPointer ),
        anchor( ),
        xYet( 0 ),
        yYet( 0 ),
        zYet( z )
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
        originalLabel( item.originalLabel ),
        processedImage( new Picture( * item.processedImage ) ),
        height( item.height ),
        orientation( item.orientation ),
        currentFrame( item.currentFrame ),
        backwardsMotion( item.backwardsMotion ),
        offset( item.offset ),
        collisionDetector( item.collisionDetector ),
        motionTimer( new Timer () ),
        behavior( nilPointer ),
        anchor( item.anchor ),
        xYet( item.xYet ),
        yYet( item.yYet ),
        zYet( item.zYet )
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
        delete behavior;

        motion.clear ();
        shadows.clear ();
}

void Item::readGraphicsOfItem ()
{
        motion.clear ();
        shadows.clear ();

        if ( ! descriptionOfItem->isPartOfDoor() && ! descriptionOfItem->getNameOfFile().empty() )
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

bool Item::updateItem()
{
        bool isGone = false;

        if ( behavior != nilPointer )
                isGone = behavior->update();

        return isGone;
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

void Item::updateImage ()
{
        freshProcessedImage ();
}

void Item::updateShadow ()
{
}

void Item::freshProcessedImage()
{
        processedImage = PicturePtr( new Picture( getRawImage () ) );
        processedImage->setName( "processed " + getRawImage().getName() );
}

void Item::changeOrientation( const Way& way )
{
        if ( orientation.getIntegerOfWay() != way.getIntegerOfWay() )
        {
                this->orientation = way ;

                changeFrame( firstFrame() );
        }
}

void Item::changeFrame( size_t newFrame )
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
        // coordinates before change
        int originalX = xYet ;
        int originalY = yYet ;
        int originalZ = zYet ;

        mediator->clearStackOfCollisions( );

        bool collisionFound = false;

        // new coordinates
        xYet += x ;
        yYet += y ;
        zYet += z ;

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
        xYet = originalX ;
        yYet = originalY ;
        zYet = originalZ ;

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

bool Item::doGraphicsOverlap( const Item& item ) const
{
        return  ( this->getOffsetX() < item.getOffsetX() + static_cast< int >( item.getRawImage().getWidth() ) ) &&
                        ( item.getOffsetX() < this->getOffsetX() + static_cast< int >( this->getRawImage().getWidth() ) ) &&
                ( this->getOffsetY() < item.getOffsetY() + static_cast< int >( item.getRawImage().getHeight() ) ) &&
                        ( item.getOffsetY() < this->getOffsetY() + static_cast< int >( this->getRawImage().getHeight() ) ) ;
}

bool Item::doGraphicsOverlapAt( const Item& item, std::pair< int, int > offset ) const
{
        return  ( this->getOffsetX() < offset.first + static_cast< int >( item.getRawImage().getWidth() ) ) &&
                        ( offset.first < this->getOffsetX() + static_cast< int >( this->getRawImage().getWidth() ) ) &&
                ( this->getOffsetY() < offset.second + static_cast< int >( item.getRawImage().getHeight() ) ) &&
                        ( offset.second < this->getOffsetY() + static_cast< int >( this->getRawImage().getHeight() ) ) ;
}

void Item::setBehaviorOf( const std::string& behavior )
{
        delete this->behavior ;
        this->behavior = Behavior::createBehaviorByName( ItemPtr( this ), behavior );
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

size_t Item::firstFrameForOrientation ( const Way& way ) const
{
        if ( descriptionOfItem->howManyOrientations() > 1 )
        {
                unsigned int orientOccident = ( way.getIntegerOfWay() == Way::Nowhere ? 0 : way.getIntegerOfWay() );
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
                for ( unsigned int f = 0 ; f < description.howManyFramesPerOrientation() ; f ++ )
                {
                        Picture animationFrame( * rawFrames[ ( o * rawRow ) + description.getFrameAt( f ) ] );
                        animationFrame.setName( description.getLabel() + " " +
                                                        util::toStringWithOrdinalSuffix( o ) + " orientation " +
                                                        util::toStringWithOrdinalSuffix( f ) + " frame" );

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
                for ( unsigned int f = 0 ; f < description.howManyFramesPerOrientation() ; f ++ )
                {
                        Picture shadowFrame( * rawShadows[ ( o * rawRow ) + description.getFrameAt( f ) ] );
                        shadowFrame.setName( description.getLabel() + " " +
                                                util::toStringWithOrdinalSuffix( o ) + " orientation " +
                                                util::toStringWithOrdinalSuffix( f ) + " shadow" );

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
