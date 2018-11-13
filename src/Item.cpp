
#include "Item.hpp"
#include "ItemData.hpp"
#include "ItemDataManager.hpp"
#include "Mediator.hpp"
#include "Behavior.hpp"
#include "GameManager.hpp"

#include <algorithm> // std::for_each

#include <iostream>


namespace iso
{

Item::Item( const ItemData* data, int z, const Way& way )
      : Mediated(), Shady(),
        uniqueName( data->getLabel() + "." + makeRandomString( 12 ) ),
        originalLabel( data->getLabel() ),
        currentFrame( 0 ),
        backwardsMotion( false ),
        dataOfItem( data ),
        xPos( 0 ),
        yPos( 0 ),
        zPos( z ),
        height( data->getHeight() ),
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
        if ( dataOfItem->howManyFrames() > 1 )
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
        dataOfItem( item.dataOfItem ),
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

        if ( dataOfItem->howManyFrames() > 1 )
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

        if ( ! dataOfItem->isPartOfDoor() && ! dataOfItem->getNameOfFile().empty() )
        {
                createFrames( this, *dataOfItem, iso::GameManager::getInstance().getChosenGraphicSet() );

                unsigned int way = orientation.getIntegerOfWay();
                if ( way == Way::Nowhere ) way = 0;
                unsigned int firstFrame = ( dataOfItem->howManyOrientations() > 1 ?
                                                dataOfItem->howManyFrames() * way : 0 );

                this->rawImage = getMotionAt( firstFrame );

                if ( dataOfItem->getWidthOfShadow() > 0 && dataOfItem->getHeightOfShadow() > 0 ) // item may have no shadow
                {
                        createShadowFrames( this, *dataOfItem, iso::GameManager::getInstance().getChosenGraphicSet() );

                        this->shadow = getShadowAt( firstFrame );
                }
        }
        else if ( dataOfItem->getNameOfFile().empty() )
        {
                Picture invisibleFrame( dataOfItem->getWidthOfFrame(), dataOfItem->getHeightOfFrame() );
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
        if ( dataOfItem->howManyFrames() > 1 )
        {
                // is it time to change frames
                if ( motionTimer->getValue() > dataOfItem->getDelayBetweenFrames() )
                {
                        // forwards motion
                        if ( ! backwardsMotion )
                        {
                                if ( ++ currentFrame >= dataOfItem->howManyFrames() )
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
                                        currentFrame = dataOfItem->howManyFrames() - 1;
                                        cycle = true;
                                }
                        }

                        // which frame to show yet
                        unsigned int orientOccident = ( orientation.getIntegerOfWay() == Way::Nowhere ? 0 : orientation.getIntegerOfWay() );
                        unsigned int frame = currentFrame + ( dataOfItem->howManyOrientations() > 1 ? dataOfItem->howManyFrames() * orientOccident : 0 );

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
        const ItemData * data = dataOfItem->getItemDataManager()->findDataByLabel( labelOfItem );
        if ( data == nilPointer ) return ;

        std::cout << "metamorphosis of item \"" << getUniqueName()
                        << "\" into \"" << data->getLabel()
                        << "\" initiated by \"" << initiatedBy << "\"" << std::endl ;

        this->dataOfItem = data;

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

        if ( dataOfItem->howManyOrientations() > 1 )
        {
                // get frame for new orientation
                unsigned int orientOccident = ( way.getIntegerOfWay() == Way::Nowhere ? 0 : way.getIntegerOfWay() );
                unsigned int frame = dataOfItem->howManyFrames() * orientOccident;

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

bool Item::isMortal() const
{
        return dataOfItem->isMortal() ;
}

unsigned short Item::howManyOrientations() const
{
        return dataOfItem->howManyOrientations() ;
}

float Item::getSpeed() const
{
        return dataOfItem->getSpeed() ;
}

float Item::getWeight() const
{
        return dataOfItem->getWeight() ;
}

float Item::getDelayBetweenFrames() const
{
        return dataOfItem->getDelayBetweenFrames() ;
}

#ifdef DEBUG
#  define SAVE_ITEM_FRAMES      0
#endif

/* static */
void Item::createFrames( Item* item, const ItemData& data, const char* gfxPrefix )
{
        if ( data.getNameOfFile( ).empty() || data.getWidthOfFrame() == 0 || data.getHeightOfFrame() == 0 )
        {
                std::cerr << "either name of picture file is empty or zero width / height at Item::createFrames" << std::endl ;
                return ;
        }

        autouniqueptr< allegro::Pict > picture( allegro::Pict::fromPNGFile (
                iso::pathToFile( iso::sharePath() + gfxPrefix, data.getNameOfFile() )
        ) );
        if ( ! picture->isNotNil() )
        {
                std::cerr << "picture \"" << data.getNameOfFile( ) << "\" of set \"" << gfxPrefix << "\" is absent" << std::endl ;
                return ;
        }

        // decompose image into frames
        // they are
        //    frames of animation ( 1 or more )
        //    frames of animation
        //    ... ( × "orientations": 1, 2, or 4 times )
        //    extra frames ( 0 or more )

        std::vector< Picture* > rawFrames;

        for ( unsigned int y = 0; y < picture->getH(); y += data.getHeightOfFrame() )
        {
                for ( unsigned int x = 0; x < picture->getW(); x += data.getWidthOfFrame() )
                {
                        Picture* rawFrame = new Picture( data.getWidthOfFrame(), data.getHeightOfFrame() );
                        allegro::bitBlit( *picture.get(), rawFrame->getAllegroPict(), x, y, 0, 0, rawFrame->getWidth(), rawFrame->getHeight() );
                        rawFrame->setName( data.getLabel() + " " + util::toStringWithOrdinalSuffix( rawFrames.size() ) + " raw frame" );
                # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                        rawFrame->saveAsPNG( iso::homePath() );
                # endif
                        rawFrames.push_back( rawFrame );
                }
        }

        unsigned int rawRow = ( rawFrames.size() - data.howManyExtraFrames() ) / data.howManyOrientations();

        for ( unsigned int o = 0 ; o < data.howManyOrientations() ; o ++ )
        {
                for ( unsigned int f = 0 ; f < data.howManyFrames() ; f ++ )
                {
                        Picture animationFrame( * rawFrames[ ( o * rawRow ) + data.getFrameAt( f ) ] );
                        animationFrame.setName( data.getLabel() + " " + util::toStringWithOrdinalSuffix( o ) + " row " + util::toStringWithOrdinalSuffix( f ) + " frame" );
                # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                        animationFrame.saveAsPNG( iso::homePath() );
                # endif
                        item->addFrame( animationFrame );
                }
        }
        for ( unsigned int e = 0 ; e < data.howManyExtraFrames() ; e ++ )
        {
                Picture extraFrame( * rawFrames[ e + ( rawRow * data.howManyOrientations() ) ] );
                extraFrame.setName( data.getLabel() + " " + util::toStringWithOrdinalSuffix( e ) + " extra frame" );
        # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                extraFrame.saveAsPNG( iso::homePath() );
        # endif
                item->addFrame( extraFrame );
        }

        std::for_each( rawFrames.begin (), rawFrames.end (), DeleteIt() );
}

/* static */
void Item::createShadowFrames( Item* item, const ItemData& data, const char* gfxPrefix )
{
        if ( data.getNameOfShadowFile( ).empty() || data.getWidthOfShadow() == 0 || data.getHeightOfShadow() == 0 )
        {
                std::cerr << "either name of shadow file is empty or zero width / height at Item::createShadowFrames" << std::endl ;
                return ;
        }

        autouniqueptr< allegro::Pict > picture( allegro::Pict::fromPNGFile (
                iso::pathToFile( iso::sharePath() + gfxPrefix, data.getNameOfShadowFile( ) )
        ) );
        if ( ! picture->isNotNil() )
        {
                std::cerr << "file of shadows \"" << data.getNameOfShadowFile( ) << "\" is absent for set \"" << gfxPrefix << "\"" << std::endl ;
                return ;
        }

        // decompose image into frames

        std::vector< Picture* > rawShadows;

        for ( unsigned int y = 0; y < picture->getH(); y += data.getHeightOfShadow() )
        {
                for ( unsigned int x = 0; x < picture->getW(); x += data.getWidthOfShadow() )
                {
                        Picture* rawShadow = new Picture( data.getWidthOfShadow(), data.getHeightOfShadow() );
                        allegro::bitBlit( *picture.get(), rawShadow->getAllegroPict(), x, y, 0, 0, rawShadow->getWidth(), rawShadow->getHeight() );
                        rawShadow->setName( data.getLabel() + " " + util::toStringWithOrdinalSuffix( rawShadows.size() ) + " raw shadow" );
                # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                        rawShadow->saveAsPNG( iso::homePath() );
                # endif
                        rawShadows.push_back( rawShadow );
                }
        }

        unsigned int rawRow = ( rawShadows.size() - data.howManyExtraFrames() ) / data.howManyOrientations();

        for ( unsigned int o = 0 ; o < data.howManyOrientations() ; o ++ )
        {
                for ( unsigned int f = 0 ; f < data.howManyFrames() ; f ++ )
                {
                        Picture shadowFrame( * rawShadows[ ( o * rawRow ) + data.getFrameAt( f ) ] );
                        shadowFrame.setName( data.getLabel() + " " + util::toStringWithOrdinalSuffix( o ) + " row " + util::toStringWithOrdinalSuffix( f ) + " shadow" );
                # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                        shadowFrame.saveAsPNG( iso::homePath() );
                # endif
                        item->addFrameOfShadow( shadowFrame );
                }
        }
        for ( unsigned int e = 0 ; e < data.howManyExtraFrames() ; e ++ )
        {
                Picture extraShadow( * rawShadows[ e + ( rawRow * data.howManyOrientations() ) ] );
                extraShadow.setName( data.getLabel() + " " + util::toStringWithOrdinalSuffix( e ) + " extra shadow" );
        # if  defined( SAVE_ITEM_FRAMES )  &&  SAVE_ITEM_FRAMES
                extraShadow.saveAsPNG( iso::homePath() );
        # endif
                item->addFrameOfShadow( extraShadow );
        }

        std::for_each( rawShadows.begin (), rawShadows.end (), DeleteIt() );
}

}
