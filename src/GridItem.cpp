
#include "GridItem.hpp"
#include "ItemData.hpp"
#include "Mediator.hpp"


namespace isomot
{

GridItem::GridItem( ItemData* itemData, int cx, int cy, int z, const Way& way )
        : Item( itemData, z, way )
{
        this->cell.first = cx;
        this->cell.second = cy;

        // free coordinates of grid item
        this->x = cx * this->dataOfItem->getWidthX();
        this->y = ( cy + 1 ) * this->dataOfItem->getWidthY() - 1;

        unsigned int orientation = way.getIntegerOfWay();
        if ( orientation == Nowhere ) orientation = 0;
        unsigned int position = itemData->howManyMotions() / itemData->howManyFramesForOrientations() * orientation;
        this->rawImage = itemData->getMotionAt( position );

        // may have no shadow
        if ( itemData->getWidthOfShadow() > 0 && itemData->getHeightOfShadow() > 0 )
        {
                this->shadow = itemData->getShadowAt( position );
        }
}

GridItem::~GridItem( )
{

}

void GridItem::draw( BITMAP* where )
{
        if ( this->processedImage )
        {
                draw_sprite( where, this->processedImage, mediator->getRoom()->getX0() + this->offset.first, mediator->getRoom()->getY0() + this->offset.second );
        }
        else if ( this->rawImage )
        {
                draw_sprite( where, this->rawImage, mediator->getRoom()->getX0() + this->offset.first, mediator->getRoom()->getY0() + this->offset.second );
        }
}

void GridItem::binProcessedImage()
{
        if ( this->processedImage != nilPointer )
        {
                destroy_bitmap( this->processedImage );
                this->processedImage = nilPointer;
        }
}

void GridItem::changeImage( BITMAP* newImage )
{
        // when there's no image for this item, just assign it
        // such case usually happens during construction of the item
        if ( this->rawImage == nilPointer )
        {
                this->rawImage = newImage;
        }
        // otherwise, change it
        else
        {
                // get a copy of this item before modifying it
                GridItem oldGridItem( *this );

                binProcessedImage();

                this->rawImage = newImage;

                // calculate displacement of new image unless it’s nil
                if ( newImage )
                {
                        if ( mediator->getDegreeOfShading() < 256 )
                        {
                                // shadows are on, then reshade this item
                                this->myShady = WantShadow;
                        }

                        // how many pixels this image is from the origin of room
                        this->offset.first = ( ( mediator->getRoom()->getSizeOfOneTile() * ( this->cell.first - this->cell.second ) ) << 1 ) - ( newImage->w >> 1 ) + 1;
                        this->offset.second = mediator->getRoom()->getSizeOfOneTile() * ( this->cell.first + this->cell.second + 2 ) - newImage->h - this->z - 1;
                }
                else
                {
                        this->offset.first = this->offset.second = 0;
                }

                // mark for masking every free item affected by previous image
                if ( oldGridItem.getRawImage () )
                        mediator->remaskWithGridItem( &oldGridItem );

                // mark for masking every free item affected by new image
                if ( this->rawImage )
                        mediator->remaskWithGridItem( this );
        }
}

void GridItem::changeShadow( BITMAP* newShadow )
{
        this->shadow = newShadow;

        if ( newShadow )
        {       // it's time to figure out which items to shade
                if ( mediator->getDegreeOfShading() < 256 )
                {
                        // reshade items when shadows are on
                        mediator->reshadeWithGridItem( this );
                }
        }
}

void GridItem::requestShadow( int column )
{
        if ( this->rawImage && this->myShady == WantShadow )
        {
                mediator->castShadowOnGridItem( this );

                // bin already shaded image
                if ( this->myShady != AlreadyShady && this->processedImage != nilPointer )
                {
                        destroy_bitmap( this->processedImage );
                        this->processedImage = nilPointer;
                }

                // to reshade at next cycle
                this->myShady = NoShadow;
        }
}

bool GridItem::addToZ( int value )
{
        return updatePosition( value, CoordinateZ, Add );
}

bool GridItem::updatePosition( int newValue, const Coordinate& whatToChange, const ChangeOrAdd& what )
{
        mediator->clearStackOfCollisions( );
        bool collisionFound = false;

        GridItem oldGridItem( *this );

        if ( whatToChange & CoordinateZ )
        {
                this->z = newValue + this->z * what;
        }

        // is there collision with floor
        if ( this->z < 0 )
        {
                mediator->pushCollision( Floor );
                collisionFound = true;
        }
        // or maybe with other items in room
        else
        {
                if ( ! ( collisionFound = mediator->findCollisionWithItem( this ) ) )
                {
                        // for grid element with image, mark to mask overlapping free items
                        if ( this->rawImage )
                        {
                                // how many pixels is image from origin of room
                                // change only value on Y axis because it depends on Z coordinate
                                this->offset.second = mediator->getRoom()->getSizeOfOneTile() * ( this->cell.first + this->cell.second + 2 ) - this->rawImage->h - this->z - 1;

                                mediator->remaskWithGridItem( &oldGridItem );
                                mediator->remaskWithGridItem( this );
                        }
                        else
                        {
                                this->offset.first = this->offset.second = 0;
                        }

                        // reshade items after change of position on Z
                        if ( whatToChange & CoordinateZ && mediator->getDegreeOfShading() < 256 )
                        {
                                if ( this->z > oldGridItem.getZ() )
                                        mediator->reshadeWithGridItem( this );
                                else
                                        mediator->reshadeWithGridItem( &oldGridItem );
                        }

                        mediator->activateGridItemsSorting();
                }
        }

        // restore previous values on collision
        if ( collisionFound )
        {
                this->z = oldGridItem.getZ();

                this->offset.second = oldGridItem.getOffsetY();
        }

        return ! collisionFound;
}

}
