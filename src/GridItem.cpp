
#include "GridItem.hpp"
#include "ItemData.hpp"
#include "Mediator.hpp"

#include <cassert>


namespace iso
{

GridItem::GridItem( const ItemData* itemData, int cx, int cy, int z, const Way& way )
        : Item( itemData, z, way )
        , cell( std::pair< int, int >( cx, cy ) )
{
        // free coordinates of grid item
        this->xPos = cx * getWidthX();
        this->yPos = ( cy + 1 ) * getWidthY() - 1;

        setCollisionDetector( true );
}

GridItem::~GridItem( )
{

}

void GridItem::draw( const allegro::Pict& where )
{
        if ( this->processedImage != nilPointer )
        {
                allegro::drawSprite( where, this->processedImage->getAllegroPict(),
                        mediator->getRoom()->getX0() + this->offset.first, mediator->getRoom()->getY0() + this->offset.second );
        }
        else if ( this->rawImage != nilPointer )
        {
                allegro::drawSprite( where, this->rawImage->getAllegroPict(),
                        mediator->getRoom()->getX0() + this->offset.first, mediator->getRoom()->getY0() + this->offset.second );
        }
}

void GridItem::changeImage( const Picture* newImage )
{
        if ( this->rawImage == nilPointer )
        {
                // when there's no image for this item, just assign it
                // such case happens during construction of the item
                this->rawImage = newImage;
                return;
        }

        // get a copy of this item before modifying it
        GridItemPtr copyOfItem ( new GridItem ( *this ) );

        binProcessedImage();

        this->rawImage = newImage;

        if ( newImage != nilPointer )
        {
                // reshade this item
                binProcessedImage();
                setWantShadow( true );

                // how many pixels is this image from the origin of room
                this->offset.first = ( ( mediator->getRoom()->getSizeOfOneTile() * ( this->cell.first - this->cell.second ) ) << 1 ) - ( newImage->getWidth() >> 1 ) + 1;
                this->offset.second = mediator->getRoom()->getSizeOfOneTile() * ( this->cell.first + this->cell.second + 2 ) - newImage->getHeight() - this->getZ() - 1;
        }
        else
        {
                this->offset.first = this->offset.second = 0;
        }

        // remask every free item affected by previous image
        if ( copyOfItem->getRawImage () != nilPointer )
                mediator->remaskWithGridItem( *copyOfItem );

        // remask every free item affected by new image
        if ( newImage != nilPointer )
                mediator->remaskWithGridItem( *this );
}

void GridItem::changeShadow( const Picture* newShadow )
{
        Item::changeShadow( newShadow );

        if ( newShadow != nilPointer && this->shadow != newShadow && this->rawImage != nilPointer )
        {
                // reshade items
                mediator->reshadeWithGridItem( *this );
        }
}

bool GridItem::addToPosition( int x, int y, int z )
{
        mediator->clearStackOfCollisions( );

        bool collisionFound = false;

        // copy item before moving it
        GridItemPtr copyOfItem ( new GridItem ( *this ) );

        if ( x != 0 )
                std::cout << "can’t change position on X for grid item, ignoring x = " << x << std::endl ;
        if ( y != 0 )
                std::cout << "can’t change position on Y for grid item, ignoring y = " << y << std::endl ;

        this->zPos += z;

        // is there collision with floor
        if ( this->getZ() < 0 )
        {
                mediator->pushCollision( "some tile of floor" );
                collisionFound = true;
        }
        // or maybe with other items in room
        else
        {
                collisionFound = mediator->lookForCollisionsOf( this->getUniqueName() );
                if ( ! collisionFound )
                {
                        // for grid element with image, mark to mask overlapping free items
                        if ( this->rawImage != nilPointer )
                        {
                                // how many pixels is image from origin of room
                                // change only value on Y axis because it depends on Z coordinate
                                this->offset.second = mediator->getRoom()->getSizeOfOneTile() * ( this->cell.first + this->cell.second + 2 ) - this->rawImage->getHeight() - this->getZ() - 1;

                                mediator->remaskWithGridItem( *copyOfItem );
                                mediator->remaskWithGridItem( *this );
                        }
                        else
                        {
                                this->offset.first = this->offset.second = 0;
                        }

                        // reshade items
                        if ( this->getZ() > copyOfItem->getZ() )
                                mediator->reshadeWithGridItem( *this );
                        else if ( this->getZ() < copyOfItem->getZ() )
                                mediator->reshadeWithGridItem( *copyOfItem );

                        mediator->needsToSortGridItems ();
                }
        }

        // restore previous values on collision
        if ( collisionFound )
        {
                this->zPos = copyOfItem->getZ();

                this->offset = copyOfItem->getOffset();
        }

        return ! collisionFound;
}

unsigned int GridItem::getColumnOfGrid () const
{
        assert( mediator != nilPointer );
        return mediator->getRoom()->getTilesX() * getCellY() + getCellX();
}

}
