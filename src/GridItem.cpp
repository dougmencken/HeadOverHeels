
#include "GridItem.hpp"
#include "ItemData.hpp"
#include "Mediator.hpp"

#include <cassert>


namespace isomot
{

GridItem::GridItem( ItemData* itemData, int cx, int cy, int z, const Way& way )
        : Item( itemData, z, way )
        , cell( std::pair< int, int >( cx, cy ) )
{
        // free coordinates of grid item
        this->x = cx * this->getWidthX();
        this->y = ( cy + 1 ) * this->getWidthY() - 1;

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
        if ( this->processedImage != nilPointer )
        {
                allegro::drawSprite( where, this->processedImage, mediator->getRoom()->getX0() + this->offset.first, mediator->getRoom()->getY0() + this->offset.second );
        }
        else if ( this->rawImage != nilPointer )
        {
                allegro::drawSprite( where, this->rawImage, mediator->getRoom()->getX0() + this->offset.first, mediator->getRoom()->getY0() + this->offset.second );
        }
}

void GridItem::changeImage( BITMAP* newImage )
{
        if ( this->rawImage == nilPointer )
        {
                // when there's no image for this item, just assign it
                // such case usually happens during construction of the item
                this->rawImage = newImage;
                return;
        }

        // get a copy of this item before modifying it
        GridItem copyOfItem( *this );

        binProcessedImage();

        this->rawImage = newImage;

        // calculate displacement of new image unless it’s nil
        if ( newImage != nilPointer )
        {
                if ( mediator->getDegreeOfShading() < 256 )
                {
                        // shadows are on, then reshade this item
                        binProcessedImage();
                        setWantShadow( true );
                }

                // how many pixels is this image from the origin of room
                this->offset.first = ( ( mediator->getRoom()->getSizeOfOneTile() * ( this->cell.first - this->cell.second ) ) << 1 ) - ( newImage->w >> 1 ) + 1;
                this->offset.second = mediator->getRoom()->getSizeOfOneTile() * ( this->cell.first + this->cell.second + 2 ) - newImage->h - this->z - 1;
        }
        else
        {
                this->offset.first = this->offset.second = 0;
        }

        // remask every free item affected by previous image
        if ( copyOfItem.getRawImage () )
                mediator->remaskWithGridItem( &copyOfItem );

        // remask every free item affected by new image
        if ( newImage != nilPointer )
                mediator->remaskWithGridItem( this );
}

void GridItem::changeShadow( BITMAP* newShadow )
{
        Item::changeShadow( newShadow );

        if ( newShadow != nilPointer && this->shadow != newShadow && this->rawImage != nilPointer )
        {
                if ( mediator->getDegreeOfShading() < 256 )
                {
                        // reshade items when shadows are on
                        mediator->reshadeWithGridItem( this );
                }
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

        GridItem copyOfItem( *this );

        if ( whatToChange & CoordinateZ )
        {
                this->z = newValue + this->z * what;
        }

        // is there collision with floor
        if ( this->z < 0 )
        {
                mediator->pushCollision( "some tile of floor" );
                collisionFound = true;
        }
        // or maybe with other items in room
        else
        {
                collisionFound = mediator->findCollisionWithItem( this );
                if ( ! collisionFound )
                {
                        // for grid element with image, mark to mask overlapping free items
                        if ( this->rawImage != nilPointer )
                        {
                                // how many pixels is image from origin of room
                                // change only value on Y axis because it depends on Z coordinate
                                this->offset.second = mediator->getRoom()->getSizeOfOneTile() * ( this->cell.first + this->cell.second + 2 ) - this->rawImage->h - this->z - 1;

                                mediator->remaskWithGridItem( &copyOfItem );
                                mediator->remaskWithGridItem( this );
                        }
                        else
                        {
                                this->offset.first = this->offset.second = 0;
                        }

                        // reshade items after change of position on Z
                        if ( whatToChange & CoordinateZ && mediator->getDegreeOfShading() < 256 )
                        {
                                if ( this->z > copyOfItem.getZ() )
                                        mediator->reshadeWithGridItem( this );
                                else
                                        mediator->reshadeWithGridItem( &copyOfItem );
                        }

                        mediator->activateGridItemsSorting();
                }
        }

        // restore previous values on collision
        if ( collisionFound )
        {
                this->z = copyOfItem.getZ();

                this->offset = copyOfItem.getOffset();
        }

        return ! collisionFound;
}

int GridItem::getColumnOfGrid () const
{
        assert( mediator != nilPointer );
        return mediator->getRoom()->getTilesX() * getCellY() + getCellX();
}

}
