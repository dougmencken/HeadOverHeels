
#include "FreeItem.hpp"
#include "ItemData.hpp"
#include "Mediator.hpp"
#include "SoundManager.hpp"


namespace isomot
{

FreeItem::FreeItem( ItemData* itemData, int x, int y, int z, const Way& way )
        : Item ( itemData, z, way )
        , myMask ( WantRemask )
        , transparency ( 0 )
        , collisionDetector ( true )
        , frozen ( false )
        , shadedNonmaskedImage ( nilPointer )
        , partOfDoor( false )
{
        this->x = x;
        this->y = y;
        if ( y < 0 ) this->y = 0;

        // init frames
        int howManyFrames = ( getDataOfItem()->howManyMotions() - getDataOfItem()->howManyExtraFrames() ) / getDataOfItem()->howManyFramesForOrientations() ;
        unsigned int orientation = ( way.getIntegerOfWay() == Nowhere ? 0 : way.getIntegerOfWay() );
        int currentFrame = ( getDataOfItem()->howManyFramesForOrientations() > 1 ?
                                        getDataOfItem()->getFrameAt( getIndexOfFrame() ) + howManyFrames * orientation :
                                        getDataOfItem()->getFrameAt( 0 ) );

        this->rawImage = getDataOfItem()->getMotionAt( currentFrame );

        // get shadow
        if ( getDataOfItem()->getWidthOfShadow() > 0 && getDataOfItem()->getHeightOfShadow() > 0 )
        {
                this->shadow = getDataOfItem()->getShadowAt( currentFrame );
        }
}

FreeItem::FreeItem( const FreeItem& freeItem )
        : Item( freeItem ), Drawable()
        , myMask( freeItem.myMask )
        , transparency( freeItem.transparency )
        , collisionDetector( freeItem.collisionDetector )
        , frozen( freeItem.frozen )
        , shadedNonmaskedImage( nilPointer )
        , partOfDoor( freeItem.partOfDoor )
{
        if ( freeItem.shadedNonmaskedImage != nilPointer )
        {
                this->shadedNonmaskedImage = allegro::createPicture( freeItem.shadedNonmaskedImage->w, freeItem.shadedNonmaskedImage->h );
                allegro::bitBlit( freeItem.shadedNonmaskedImage, this->shadedNonmaskedImage );
        }
}

FreeItem::~FreeItem()
{
        allegro::binPicture( shadedNonmaskedImage );
}

void FreeItem::draw( allegro::Pict* where )
{
        if ( transparency >= 100 ) /* item is fully transparent */ return ;

        allegro::Pict* imageToDraw = this->rawImage;
        if ( this->shadedNonmaskedImage != nilPointer ) imageToDraw = this->shadedNonmaskedImage;
        if ( this->processedImage != nilPointer ) imageToDraw = this->processedImage;
        if ( imageToDraw == nilPointer ) return;

        if ( transparency == 0 )
        {
                allegro::drawSprite(
                        where,
                        imageToDraw,
                        mediator->getRoom()->getX0 () + this->offset.first,
                        mediator->getRoom()->getY0 () + this->offset.second
                ) ;
        }
        else
        {
                // draw item with transparency
                set_trans_blender( 0, 0, 0, static_cast < int > ( 256 - 2.56 * this->transparency ) );

                draw_trans_sprite(
                        where,
                        imageToDraw,
                        mediator->getRoom()->getX0 () + this->offset.first,
                        mediator->getRoom()->getY0 () + this->offset.second
                ) ;
        }
}

void FreeItem::binBothProcessedImages()
{
        Item::binProcessedImage ();

        setShadedNonmaskedImage( nilPointer );
}

void FreeItem::changeImage( allegro::Pict* newImage )
{
        if ( newImage == nilPointer )
        {
                std::cout << "nil image at FreeItem.changeImage" << std::endl ;
        }

        if ( this->rawImage == nilPointer )
        {
                this->rawImage = newImage;
                return;
        }

        if ( this->rawImage != newImage )
        {
                // copy this item before modifying it
                FreeItem copyOfItem( *this );

                if ( newImage != nilPointer )
                {
                        // recalculate how many pixels is this image from point of room’s origin
                        this->offset.first = ( ( this->x - this->y ) << 1 ) + static_cast< int >( getWidthX() + getWidthY() ) - ( newImage->w >> 1 ) - 1;
                        this->offset.second = this->x + this->y + static_cast< int >( getWidthX() ) - newImage->h - this->z;
                }
                else
                {
                        this->offset.first = this->offset.second = 0;
                }

                this->rawImage = newImage;

                binBothProcessedImages() ;
                setWantShadow( true );
                this->myMask = WantRemask;

                // remask with old image
                if ( copyOfItem.getRawImage() != nilPointer )
                        mediator->remaskWithFreeItem( &copyOfItem );

                // remask with new image
                if ( newImage != nilPointer )
                        mediator->remaskWithFreeItem( this );
        }
}

void FreeItem::changeShadow( allegro::Pict* newShadow )
{
        if ( this->shadow == nilPointer )
        {
                this->shadow = newShadow;
        }
        else if ( this->shadow != newShadow )
        {
                this->shadow = newShadow;

                // reshade items
                if ( this->rawImage != nilPointer )
                {
                        if ( mediator->getDegreeOfShading() < 256 )
                        {
                                mediator->reshadeWithFreeItem( this );
                        }
                }
        }
}

void FreeItem::requestShadow()
{
        if ( rawImage != nilPointer && getWantShadow() )
        {
                mediator->castShadowOnFreeItem( this );

                if ( ! getWantShadow() )
                {
                        this->myMask = WantRemask;
                }
        }
}

void FreeItem::requestMask()
{
        mediator->maskFreeItem( this );

        if ( this->myMask == WantRemask && this->processedImage != nilPointer )
                binProcessedImage();

        this->myMask = NoMask;
}

bool FreeItem::addToPosition( int x, int y, int z )
{
        mediator->clearStackOfCollisions( );

        bool collisionFound = false;

        // copy item before moving
        FreeItem copyOfItem( *this );

        this->x += x;
        this->y += y;
        this->z += z;

        // look for collision with real wall, one which limits the room
        if ( this->x < mediator->getRoom()->getLimitAt( "north" ) )
        {
                mediator->pushCollision( "some segment of wall at north" );
        }
        else if ( this->x + static_cast< int >( getWidthX() ) > mediator->getRoom()->getLimitAt( "south" ) )
        {
                mediator->pushCollision( "some segment of wall at south" );
        }
        if ( this->y >= mediator->getRoom()->getLimitAt( "west" ) )
        {
                mediator->pushCollision( "some segment of wall at west" );
        }
        else if ( this->y - static_cast< int >( getWidthY() ) + 1 < mediator->getRoom()->getLimitAt( "east" ) )
        {
                mediator->pushCollision( "some segment of wall at east" );
        }

        // look for collision with floor
        if ( this->z < 0 )
        {
                mediator->pushCollision( "some tile of floor" );
        }

        collisionFound = ! mediator->isStackOfCollisionsEmpty ();
        if ( ! collisionFound )
        {
                // look for collision with other items in room
                collisionFound = mediator->findCollisionWithItem( this );
                if ( ! collisionFound )  // is it okay to move
                {
                        // reshade and remask
                        binBothProcessedImages();
                        setWantShadow( true );
                        this->myMask = WantRemask;

                        // for item with image, mark to mask free items whose images overlap with its image
                        if ( this->rawImage != nilPointer )
                        {
                                // get how many pixels is this image from point of room’s origin
                                this->offset.first = ( ( this->x - this->y ) << 1 ) + getWidthX() + getWidthY() - ( this->rawImage->w >> 1 ) - 1;
                                this->offset.second = this->x + this->y + getWidthX() - this->rawImage->h - this->z;

                                // for both the previous position and the current position
                                mediator->remaskWithFreeItem( &copyOfItem );
                                mediator->remaskWithFreeItem( this );
                        }
                        else
                        {
                                this->offset.first = this->offset.second = 0;
                        }

                        // reshade items
                        if ( mediator->getDegreeOfShading() < 256 )
                        {
                                // for both the previous position and the current position
                                mediator->reshadeWithFreeItem( &copyOfItem );
                                mediator->reshadeWithFreeItem( this );
                        }

                        // rearrange list of free items
                        mediator->activateFreeItemsSorting();
                }
        }

        // restore previous values when there’s a collision
        if ( collisionFound )
        {
                this->x = copyOfItem.getX();
                this->y = copyOfItem.getY();
                this->z = copyOfItem.getZ();

                this->offset = copyOfItem.getOffset();
        }

        return ! collisionFound;
}

bool FreeItem::isCollidingWithDoor( const std::string& way, const std::string& name, const int previousX, const int previousY )
{
        Door* door = mediator->getRoom()->getDoorAt( way );
        if ( door == nilPointer ) return false ;

        int oldX = this->x;
        int oldY = this->y;

        switch ( Way( way ).getIntegerOfWay() )
        {
                // for rooms with north or south door
                case North:
                case Northeast:
                case Northwest:
                case South:
                case Southeast:
                case Southwest:
                        // move player right when player hits left jamb
                        if ( door->getLeftJamb()->getUniqueName() == name && this->y <= door->getLeftJamb()->getY() )
                        {
                                this->y--;
                                this->x = previousX;
                        }
                        // move player left when player collides with right jamb
                        else if ( door->getRightJamb()->getUniqueName() == name &&
                                        this->y - static_cast< int >( getDataOfItem()->getWidthY() )
                                                >= door->getRightJamb()->getY() - static_cast< int >( door->getRightJamb()->getWidthY() ) )
                        {
                                this->y++;
                                this->x = previousX;
                        }

                        break;

                // for rooms with east or west door
                case East:
                case Eastnorth:
                case Eastsouth:
                case West:
                case Westnorth:
                case Westsouth:
                        // move player right when player hits left jamb
                        if ( door->getLeftJamb()->getUniqueName() == name && this->x >= door->getLeftJamb()->getX() )
                        {
                                this->x++;
                                this->y = previousY;
                        }
                        // move player left when player collides with right jamb
                        else if ( door->getRightJamb()->getUniqueName() == name &&
                                        this->x - static_cast< int >( getDataOfItem()->getWidthX() )
                                                <= door->getRightJamb()->getX() + static_cast< int >( door->getRightJamb()->getWidthX() ) )
                        {
                                this->x--;
                                this->y = previousY;
                        }

                        break;

                default:
                        ;
        }

        if ( oldX != this->x || oldY != this->y )
        {
                isomot::SoundManager::getInstance()->play ( "door", isomot::Collision, /* loop */ false );
                return true ;
        }

        return false ;
}

bool FreeItem::isNotUnderDoorAt( const std::string& way )
{
        Door* door = mediator->getRoom()->getDoorAt( way );

        return ( door == nilPointer || ! door->isUnderDoor( this->x, this->y, this->z ) );
}

bool FreeItem::isUnderSomeDoor ()
{
        const std::map < std::string, Door* >& doors = mediator->getRoom()->getDoors();

        for ( std::map < std::string, Door* >::const_iterator iter = doors.begin () ; iter != doors.end (); ++ iter )
        {
                Door* door = iter->second ;
                if ( door != nilPointer && door->isUnderDoor( this->x, this->y, this->z ) )
                        return true;
        }

        return false;
}

void FreeItem::setShadedNonmaskedImage( allegro::Pict* newImage )
{
        if ( shadedNonmaskedImage != newImage )
        {
                allegro::binPicture( shadedNonmaskedImage );
                shadedNonmaskedImage = newImage;
        }
}

/* static */
void FreeItem::maskItemBehindImage( FreeItem* item, allegro::Pict* upwardImage, int x, int y )
{
        if ( item == nilPointer || upwardImage == nilPointer ) return;

        // mask shaded image or raw image when item is not yet shaded or shadows are off
        allegro::Pict* imageToMask = ( item->getShadedNonmaskedImage() != nilPointer ? item->getShadedNonmaskedImage() : item->getRawImage() );
        if ( imageToMask == nilPointer ) return;

        int inix = x - item->getOffsetX();      // initial X
        int endx = inix + upwardImage->w;       // final X
        int iniy = y - item->getOffsetY();      // initial Y
        int endy = iniy + upwardImage->h;       // final Y

        if ( inix < 0 ) inix = 0;
        if ( iniy < 0 ) iniy = 0;
        if ( endx > imageToMask->w ) endx = imageToMask->w;
        if ( endy > imageToMask->h ) endy = imageToMask->h;

        allegro::Pict* maskedImage = item->getProcessedImage();

        if ( maskedImage == nilPointer )
        {
                maskedImage = allegro::createPicture( imageToMask->w, imageToMask->h, allegro::colorDepthOf( imageToMask ) );
        }

        if ( item->whichMask() == WantRemask )
        {
                // in principle, image of masked item is image of unmasked item, shaded or not
                allegro::bitBlit( imageToMask, maskedImage );
                item->setWhichMask( AlreadyMasked );
        }

        char increase1 = ( allegro::colorDepthOf( maskedImage ) == 32 ? 4 : 3 );
        char increase2 = ( allegro::colorDepthOf( upwardImage ) == 32 ? 4 : 3 );

        int n2i = inix + item->getOffsetX() - x;

        endx *= increase1;
        inix *= increase1;
        n2i *= increase2;
        #if IS_BIG_ENDIAN
                inix += allegro::colorDepthOf( imageToMask ) == 32 ? 1 : 0 ;
                n2i += allegro::colorDepthOf( upwardImage ) == 32 ? 1 : 0;
        #endif

        int cRow = 0;           // row of pixels in imageToMask
        int iRow = 0;           // row of pixels in upwardImage
        int cPixel = 0;         // pixel in row of imageToMask
        int iPixel = 0;         // pixel in row of upwardImage

        for ( cRow = iniy, iRow = iniy + item->getOffsetY() - y; cRow < endy; cRow++, iRow++ )
        {
                unsigned char* cln = maskedImage->line[ cRow ];
                unsigned char* iln = upwardImage->line[ iRow ];

                for ( cPixel = inix, iPixel = n2i; cPixel < endx; cPixel += increase1, iPixel += increase2 )
                {
                        if ( iln[ iPixel ] != 255 || iln[ iPixel + 1 ] != 0 || iln[ iPixel + 2 ] != 255 )
                        {
                                cln[ cPixel ] = cln[ cPixel + 2 ] = 255;
                                cln[ cPixel + 1 ] = 0;
                        }
                }
        }

        item->setProcessedImage( maskedImage );
}

}
