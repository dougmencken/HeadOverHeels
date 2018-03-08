
#include "FreeItem.hpp"
#include "ItemData.hpp"
#include "Mediator.hpp"


namespace isomot
{

FreeItem::FreeItem( ItemData* itemData, int x, int y, int z, const Way& way )
        : Item ( itemData, z, way )
        , myMask ( WantRemask )
        , transparency ( 0 )
        , collisionDetector ( true )
        , frozen ( false )
        , shadedNonmaskedImage ( nilPointer )
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
        : Item( freeItem )
        , myMask( freeItem.myMask )
        , transparency( freeItem.transparency )
        , collisionDetector( freeItem.collisionDetector )
        , frozen( freeItem.frozen )
        , shadedNonmaskedImage( nilPointer )
{
        if ( freeItem.shadedNonmaskedImage != nilPointer )
        {
                this->shadedNonmaskedImage = create_bitmap_ex( 32, freeItem.shadedNonmaskedImage->w, freeItem.shadedNonmaskedImage->h );
                blit( freeItem.shadedNonmaskedImage, this->shadedNonmaskedImage, 0, 0, 0, 0, this->shadedNonmaskedImage->w, this->shadedNonmaskedImage->h );
        }
}

FreeItem::~FreeItem()
{
        if ( shadedNonmaskedImage != nilPointer )
                destroy_bitmap( shadedNonmaskedImage );
}

void FreeItem::draw( BITMAP* where )
{
        if ( transparency >= 100 ) /* item is fully transparent */ return ;

        if ( transparency == 0 )
        {
                draw_sprite(
                        where,
                        this->processedImage ? this->processedImage : ( this->shadedNonmaskedImage ? this->shadedNonmaskedImage : this->rawImage ),
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
                        this->processedImage ? this->processedImage : ( this->shadedNonmaskedImage ? this->shadedNonmaskedImage : this->rawImage ),
                        mediator->getRoom()->getX0 () + this->offset.first,
                        mediator->getRoom()->getY0 () + this->offset.second
                ) ;
        }
}

void FreeItem::binProcessedImages()
{
        if ( this->processedImage != nilPointer )
        {
                destroy_bitmap( this->processedImage );
                this->processedImage = nilPointer;
        }

        if ( this->shadedNonmaskedImage != nilPointer )
        {
                destroy_bitmap( this->shadedNonmaskedImage );
                this->shadedNonmaskedImage = nilPointer;
        }
}

void FreeItem::changeImage( BITMAP* image )
{
        if ( image == nilPointer )
        {
                std::cout << "nil image at FreeItem.changeImage" << std::endl ;
        }

        if ( this->rawImage == nilPointer )
        {
                this->rawImage = image;
        }
        else if ( this->rawImage != image )
        {
                FreeItem oldFreeItem( *this );

                this->rawImage = nilPointer;

                // recalculate displacement, it is how many pixels is this image from point of room’s origin
                if ( image != nilPointer )
                {
                        this->offset.first = ( ( this->x - this->y ) << 1 ) + static_cast< int >( getWidthX() + getWidthY() ) - ( image->w >> 1 ) - 1;
                        this->offset.second = this->x + this->y + static_cast< int >( getWidthX() ) - image->h - this->z;
                }
                else
                {
                        this->offset.first = this->offset.second = 0;
                }

                binProcessedImages() ;

                this->rawImage = image;
                this->myShady = WantReshadow;
                this->myMask = WantRemask;

                // remask with old image
                if ( oldFreeItem.getRawImage() != nilPointer )
                {
                        mediator->remaskWithFreeItem( &oldFreeItem );
                }

                // remask with new image
                if ( image != nilPointer )
                {
                        mediator->remaskWithFreeItem( this );
                }
        }
}

void FreeItem::changeShadow( BITMAP* shadow )
{
        if ( this->shadow == nilPointer )
        {
                this->shadow = shadow;
        }
        else if ( this->shadow != shadow )
        {
                this->shadow = shadow;

                // reshade items
                if ( this->rawImage )
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
        if( this->rawImage && this->myShady == WantReshadow )
        {
                mediator->castShadowOnFreeItem( this );

                // Si el elemento se ha sombreado se marca para enmascararlo
                if ( this->myShady == AlreadyShady )
                {
                        this->myMask = WantRemask;
                }

                // Si no se ha podido sombrear entonces se destruye la imagen de sombreado
                // y se marca el elemento para enmascararlo
                if ( this->myShady == WantReshadow && this->shadedNonmaskedImage )
                {
                        destroy_bitmap( this->shadedNonmaskedImage );
                        this->shadedNonmaskedImage = nilPointer;
                        this->myMask = WantRemask;
                }
        }
}

void FreeItem::requestMask()
{
        mediator->maskFreeItem( this );

        if ( this->myMask == WantRemask && this->processedImage )
        {
                destroy_bitmap( this->processedImage );
                this->processedImage = nilPointer;
        }

        this->myShady = NoShadow;
        this->myMask = NoMask;
}

void FreeItem::maskImage( int x, int y, BITMAP* image )
{
        assert( image != nilPointer );

        // mask shaded image or raw image when item is not yet shaded
        BITMAP* currentImage = ( this->shadedNonmaskedImage != nilPointer ? this->shadedNonmaskedImage : this->rawImage );
        assert( currentImage != nilPointer );

        int inix = x - this->offset.first;                      // initial X
        if ( inix < 0 ) inix = 0;

        int iniy = y - this->offset.second;                     // initial Y
        if ( iniy < 0 ) iniy = 0;

        int endx = x - this->offset.first + image->w;           // final X
        if ( endx > currentImage->w ) endx = currentImage->w;

        int endy = y - this->offset.second + image->h;          // final Y
        if ( endy > currentImage->h ) endy = currentImage->h;

        // in principle, image of masked item is image of unmasked item, shaded or unshaded
        if ( this->processedImage == nilPointer )
        {
                this->processedImage = create_bitmap_ex( bitmap_color_depth( currentImage ), currentImage->w, currentImage->h );
        }

        if ( this->myMask == WantRemask )
        {
                blit( currentImage, this->processedImage, 0, 0, 0, 0, currentImage->w, currentImage->h );
                this->myMask = AlreadyMasked;
        }

        char increase1 = ( bitmap_color_depth( this->processedImage ) == 32 ? 4 : 3 );
        char increase2 = ( bitmap_color_depth( image ) == 32 ? 4 : 3 );

        int n2i = inix + this->offset.first - x;

        endx *= increase1;
        inix *= increase1;
        n2i *= increase2;
        #if IS_BIG_ENDIAN
                inix += bitmap_color_depth( currentImage ) == 32 ? 1 : 0 ;
                n2i += bitmap_color_depth( image ) == 32 ? 1 : 0;
        #endif

        int cRow = 0;           // row of pixels in currentImage
        int iRow = 0;           // row of pixels in image
        int cPixel = 0;         // pixel in row of currentImage
        int iPixel = 0;         // pixel in row of image

        for ( cRow = iniy, iRow = iniy + this->offset.second - y; cRow < endy; cRow++, iRow++ )
        {
                unsigned char* cln = this->processedImage->line[ cRow ];
                unsigned char* iln = image->line[ iRow ];

                for ( cPixel = inix, iPixel = n2i; cPixel < endx; cPixel += increase1, iPixel += increase2 )
                {
                        if ( iln[ iPixel ] != 255 || iln[ iPixel + 1 ] != 0 || iln[ iPixel + 2 ] != 255 )
                        {
                                cln[ cPixel ] = cln[ cPixel + 2 ] = 255;
                                cln[ cPixel + 1 ] = 0;
                        }
                }
        }
}

bool FreeItem::updatePosition( int newX, int newY, int newZ, const Coordinate& whatToChange, const ChangeOrAdd& how )
{
        mediator->clearStackOfCollisions( );

        bool collisionFound = false;

        // copy item before making the move
        FreeItem oldFreeItem( *this );

        if ( whatToChange & CoordinateX )
        {
                this->x = newX + this->x * how;
        }
        if ( whatToChange & CoordinateY )
        {
                this->y = newY + this->y * how;
        }
        if ( whatToChange & CoordinateZ )
        {
                this->z = newZ + this->z * how;
        }

        // look for collision with real wall, one which limits the room
        if ( this->x < mediator->getRoom()->getLimitAt( Way( "north" ) ) )
        {
                mediator->pushCollision( NorthWall );
        }
        else if ( this->x + static_cast< int >( getWidthX() ) > mediator->getRoom()->getLimitAt( Way( "south" ) ) )
        {
                mediator->pushCollision( SouthWall );
        }
        if ( this->y >= mediator->getRoom()->getLimitAt( Way( "west" ) ) )
        {
                mediator->pushCollision( WestWall );
        }
        else if ( this->y - static_cast< int >( getWidthY() ) + 1 < mediator->getRoom()->getLimitAt( Way( "east" ) ) )
        {
                mediator->pushCollision( EastWall );
        }

        // look for collision with floor
        if ( this->z < 0 )
        {
                mediator->pushCollision( Floor );
        }

        collisionFound = ! mediator->isStackOfCollisionsEmpty ();
        if ( ! collisionFound )
        {
                // look for collision with other items in room
                collisionFound = mediator->findCollisionWithItem( this );
                if ( ! collisionFound )
                {
                        // for item with image, mark to mask free items whose images overlap with its image
                        if ( this->rawImage )
                        {
                                // get how many pixels is this image from point of room’s origin
                                this->offset.first = ( ( this->x - this->y ) << 1 ) + getWidthX() + getWidthY() - ( this->rawImage->w >> 1 ) - 1;
                                this->offset.second = this->x + this->y + getWidthX() - this->rawImage->h - this->z;

                                // for both the previous position and the current position
                                mediator->remaskWithFreeItem( &oldFreeItem );
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
                                mediator->reshadeWithFreeItem( &oldFreeItem );
                                mediator->reshadeWithFreeItem( this );
                        }

                        // reshade and remask
                        this->myShady = WantReshadow;
                        this->myMask = WantRemask;

                        // rearrange list of free items
                        mediator->activateFreeItemsSorting();
                }
        }

        // restore previous values when there’s a collision
        if ( collisionFound )
        {
                this->x = oldFreeItem.getX();
                this->y = oldFreeItem.getY();
                this->z = oldFreeItem.getZ();

                this->offset = oldFreeItem.getOffset();
        }

        return ! collisionFound;
}

bool FreeItem::addToX ( int value )
{
        return this->updatePosition( value, 0, 0, CoordinateX, Add );
}

bool FreeItem::addToY ( int value )
{
        return this->updatePosition( 0, value, 0, CoordinateY, Add );
}

bool FreeItem::addToZ ( int value )
{
        return this->updatePosition( 0, 0, value, CoordinateZ, Add );
}

bool FreeItem::addToPosition( int x, int y, int z )
{
        return this->updatePosition( x, y, z, CoordinatesXYZ, Add );
}

void FreeItem::setShadedNonmaskedImage( BITMAP* newImage )
{
        if ( shadedNonmaskedImage != newImage )
        {
                destroy_bitmap( shadedNonmaskedImage );
                shadedNonmaskedImage = newImage;
        }
}

}
