
#include "FreeItem.hpp"
#include "ItemData.hpp"
#include "Mediator.hpp"
#include "SoundManager.hpp"
#include "Color.hpp"


namespace iso
{

FreeItem::FreeItem( const ItemData* itemData, int x, int y, int z, const Way& way )
        : Item ( itemData, z, way )
        , originalCellX( -1024 )
        , originalCellY( -1024 )
        , originalCellZ( -1024 )
        , wantMask ( tritrue )
        , transparency ( 0 )
        , frozen ( false )
        , shadedNonmaskedImage ( nilPointer )
        , partOfDoor( false )
{
        xPos = x;
        yPos = y;
        if ( yPos < 0 ) yPos = 0;

        setCollisionDetector( true );
}

FreeItem::FreeItem( const FreeItem& freeItem )
        : Item( freeItem ), Drawable()
        , originalCellX( freeItem.originalCellX )
        , originalCellY( freeItem.originalCellY )
        , originalCellZ( freeItem.originalCellZ )
        , wantMask( freeItem.wantMask )
        , transparency( freeItem.transparency )
        , frozen( freeItem.frozen )
        , shadedNonmaskedImage( nilPointer )
        , partOfDoor( freeItem.partOfDoor )
{
        if ( freeItem.shadedNonmaskedImage != nilPointer )
        {
                this->shadedNonmaskedImage = new Picture( *freeItem.shadedNonmaskedImage );
        }
}

FreeItem::~FreeItem()
{
        delete shadedNonmaskedImage ;
}

void FreeItem::draw( const allegro::Pict& where )
{
        if ( transparency >= 100 ) /* item is fully transparent */ return ;

        const Picture* imageToDraw = this->rawImage;
        if ( this->shadedNonmaskedImage != nilPointer ) imageToDraw = this->shadedNonmaskedImage;
        if ( this->processedImage != nilPointer ) imageToDraw = this->processedImage;
        if ( imageToDraw == nilPointer ) return;

        if ( transparency == 0 )
        {
                allegro::drawSprite(
                        where,
                        imageToDraw->getAllegroPict(),
                        mediator->getRoom()->getX0 () + this->offset.first,
                        mediator->getRoom()->getY0 () + this->offset.second
                ) ;
        }
        else
        {
                allegro::drawSpriteWithTransparency(
                        where,
                        imageToDraw->getAllegroPict(),
                        mediator->getRoom()->getX0 () + this->offset.first,
                        mediator->getRoom()->getY0 () + this->offset.second,
                        static_cast < unsigned char > ( 255 - 2.55 * this->transparency )
                ) ;
        }
}

void FreeItem::binBothProcessedImages()
{
        Item::binProcessedImage ();

        setShadedNonmaskedImage( nilPointer );
}

void FreeItem::changeImage( const Picture* newImage )
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
                FreeItemPtr copyOfItem ( new FreeItem ( *this ) );

                if ( newImage != nilPointer )
                {
                        // recalculate how many pixels is this image from point of room’s origin
                        this->offset.first = ( ( getX() - getY() ) << 1 ) + static_cast< int >( getWidthX() + getWidthY() ) - ( newImage->getWidth() >> 1 ) - 1;
                        this->offset.second = getX() + getY() + static_cast< int >( getWidthX() ) - newImage->getHeight() - getZ();
                }
                else
                {
                        this->offset.first = this->offset.second = 0;
                }

                this->rawImage = newImage;

                binBothProcessedImages() ;
                setWantShadow( true );
                setWantMaskTrue();

                // remask with old image
                if ( copyOfItem->getRawImage() != nilPointer )
                        mediator->remaskWithFreeItem( *copyOfItem );

                // remask with new image
                if ( newImage != nilPointer )
                        mediator->remaskWithFreeItem( *this );
        }
}

void FreeItem::changeShadow( const Picture* newShadow )
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
                        mediator->reshadeWithFreeItem( *this );
                }
        }
}

void FreeItem::requestShadow()
{
        if ( rawImage != nilPointer && getWantShadow() )
        {
                mediator->castShadowOnFreeItem( *this );

                if ( ! getWantShadow() )
                {
                        setWantMaskTrue();
                }
        }
}

void FreeItem::requestMask()
{
        mediator->maskFreeItem( *this );

        if ( getWantMask().isTrue() && this->processedImage != nilPointer )
                binProcessedImage();

        setWantMaskFalse();
}

bool FreeItem::addToPosition( int x, int y, int z )
{
        mediator->clearStackOfCollisions( );

        bool collisionFound = false;

        // copy item before moving it
        FreeItemPtr copyOfItem ( new FreeItem ( *this ) );

        this->xPos += x;
        this->yPos += y;
        this->zPos += z;

        // look for collision with real wall, one which limits the room
        if ( getX() < mediator->getRoom()->getLimitAt( "north" ) )
        {
                mediator->pushCollision( "some segment of wall at north" );
        }
        else if ( getX() + static_cast< int >( getWidthX() ) > mediator->getRoom()->getLimitAt( "south" ) )
        {
                mediator->pushCollision( "some segment of wall at south" );
        }
        if ( getY() >= mediator->getRoom()->getLimitAt( "west" ) )
        {
                mediator->pushCollision( "some segment of wall at west" );
        }
        else if ( getY() - static_cast< int >( getWidthY() ) + 1 < mediator->getRoom()->getLimitAt( "east" ) )
        {
                mediator->pushCollision( "some segment of wall at east" );
        }

        // look for collision with floor
        if ( getZ() < 0 )
        {
                mediator->pushCollision( "some tile of floor" );
        }

        collisionFound = ! mediator->isStackOfCollisionsEmpty ();
        if ( ! collisionFound )
        {
                // look for collision with other items in room
                collisionFound = mediator->lookForCollisionsOf( this->getUniqueName() );
                if ( ! collisionFound )  // is it okay to move
                {
                        // reshade and remask
                        binBothProcessedImages();
                        setWantShadow( true );
                        setWantMaskTrue();

                        // for item with image, mark to mask free items whose images overlap with its image
                        if ( this->rawImage != nilPointer )
                        {
                                // get how many pixels is this image from point of room’s origin
                                this->offset.first = ( ( getX() - getY() ) << 1 ) + getWidthX() + getWidthY() - ( this->rawImage->getWidth() >> 1 ) - 1;
                                this->offset.second = getX() + getY() + getWidthX() - this->rawImage->getHeight() - getZ();

                                // for both the previous position and the current position
                                mediator->remaskWithFreeItem( *copyOfItem );
                                mediator->remaskWithFreeItem( *this );
                        }
                        else
                        {
                                this->offset.first = this->offset.second = 0;
                        }

                        // reshade items
                        // for both the previous position and the current position
                        mediator->reshadeWithFreeItem( *copyOfItem );
                        mediator->reshadeWithFreeItem( *this );

                        // rearrange list of free items
                        mediator->needsToSortFreeItems ();
                }
        }

        // restore previous values when there’s a collision
        if ( collisionFound )
        {
                this->xPos = copyOfItem->getX();
                this->yPos = copyOfItem->getY();
                this->zPos = copyOfItem->getZ();

                this->offset = copyOfItem->getOffset();
        }

        return ! collisionFound;
}

bool FreeItem::isCollidingWithDoor( const std::string& way, const std::string& name, const int previousX, const int previousY )
{
        Door* door = mediator->getRoom()->getDoorAt( way );
        if ( door == nilPointer ) return false ;

        int oldX = getX();
        int oldY = getY();

        switch ( Way( way ).getIntegerOfWay() )
        {
                // for rooms with north or south door
                case Way::North:
                case Way::Northeast:
                case Way::Northwest:
                case Way::South:
                case Way::Southeast:
                case Way::Southwest:
                        // move player right when player collides with left jamb
                        if ( door->getLeftJamb()->getUniqueName() == name && this->getY() <= door->getLeftJamb()->getY() )
                        {
                                this->yPos--;
                                this->xPos = previousX;
                        }
                        // move player left when player hits right jamb
                        else if ( door->getRightJamb()->getUniqueName() == name &&
                                        this->getY() - static_cast< int >( getDataOfItem()->getWidthY() )
                                                >= door->getRightJamb()->getY() - static_cast< int >( door->getRightJamb()->getWidthY() ) )
                        {
                                this->yPos++;
                                this->xPos = previousX;
                        }

                        break;

                // for rooms with east or west door
                case Way::East:
                case Way::Eastnorth:
                case Way::Eastsouth:
                case Way::West:
                case Way::Westnorth:
                case Way::Westsouth:
                        // move player right when player hits left jamb
                        if ( door->getLeftJamb()->getUniqueName() == name && this->getX() >= door->getLeftJamb()->getX() )
                        {
                                this->xPos++;
                                this->yPos = previousY;
                        }
                        // move player left when player collides with right jamb
                        else if ( door->getRightJamb()->getUniqueName() == name &&
                                        this->getX() - static_cast< int >( getDataOfItem()->getWidthX() )
                                                <= door->getRightJamb()->getX() + static_cast< int >( door->getRightJamb()->getWidthX() ) )
                        {
                                this->xPos--;
                                this->yPos = previousY;
                        }

                        break;

                default:
                        ;
        }

        if ( oldX != getX() || oldY != getY() )
        {
                iso::SoundManager::getInstance().play ( "door", Activity::Collision, /* loop */ false );
                return true ;
        }

        return false ;
}

bool FreeItem::isNotUnderDoorAt( const std::string& way )
{
        Door* door = mediator->getRoom()->getDoorAt( way );

        return ( door == nilPointer || ! door->isUnderDoor( getX(), getY(), getZ() ) );
}

bool FreeItem::isUnderSomeDoor ()
{
        const std::map < std::string, Door* >& doors = mediator->getRoom()->getDoors();

        for ( std::map < std::string, Door* >::const_iterator iter = doors.begin () ; iter != doors.end (); ++ iter )
        {
                Door* door = iter->second ;
                if ( door != nilPointer && door->isUnderDoor( getX(), getY(), getZ() ) )
                        return true;
        }

        return false;
}

void FreeItem::setShadedNonmaskedImage( Picture* newImage )
{
        if ( shadedNonmaskedImage != newImage )
        {
                delete shadedNonmaskedImage ;
                shadedNonmaskedImage = newImage;
        }
}

}
