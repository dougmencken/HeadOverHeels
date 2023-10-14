
#include "GridItem.hpp"
#include "DescriptionOfItem.hpp"
#include "Mediator.hpp"

#include <cassert>


namespace iso
{

GridItem::GridItem( const DescriptionOfItem* description, int cx, int cy, int z, const std::string& way )
        : Item( description, z, way )
        , cell( std::pair< int, int >( cx, cy ) )
{
        // free coordinates of grid item
        this->xYet = cx * getWidthX() ;
        this->yYet = ( cy + 1 ) * getWidthY() - 1 ;

        // look for collisions
        setIgnoreCollisions( false );
}

GridItem::~GridItem( )
{
}

void GridItem::calculateOffset ()
{
        assert( mediator != nilPointer );

        int offsetX = ( ( mediator->getRoom()->getSizeOfOneTile() * ( cell.first - cell.second ) ) << 1 ) - ( getRawImage().getWidth() >> 1 ) + 1 ;
        int offsetY = mediator->getRoom()->getSizeOfOneTile() * ( cell.first + cell.second + 2 ) - getRawImage().getHeight() - this->getZ() - 1 ;
        setOffset( std::pair< int, int >( offsetX, offsetY ) );
}

void GridItem::draw ()
{
        allegro::drawSprite( getProcessedImage().getAllegroPict(),
                mediator->getRoom()->getX0() + getOffsetX(), mediator->getRoom()->getY0() + getOffsetY() );
}

void GridItem::updateImage ()
{
        int xBefore = xYet ;
        int yBefore = yYet ;
        int zBefore = zYet ;

        std::pair< int, int > offsetBefore = getOffset() ;

        Item::updateImage ();

        setWantShadow( true );

        calculateOffset ();

        // remask items
        mediator->wantToMaskWithGridItemAt( *this, xBefore, yBefore, zBefore, offsetBefore );
        mediator->wantToMaskWithGridItem( *this );
}

void GridItem::updateShadow ()
{
        Item::updateShadow ();

        // reshade items
        mediator->wantShadowFromGridItem( *this );
}

bool GridItem::addToPosition( int x, int y, int z )
{
        mediator->clearStackOfCollisions( );

        bool collisionFound = false;

        if ( x != 0 )
                std::cout << "can’t change position on X for grid item, ignoring x = " << x << std::endl ;
        if ( y != 0 )
                std::cout << "can’t change position on Y for grid item, ignoring y = " << y << std::endl ;

        int zBefore = zYet ;

        std::pair< int, int > offsetBefore = getOffset() ;

        this->zYet += z ;

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
                        // change only value on Y because it depends on 3D Z coordinate
                        int newOffsetY = mediator->getRoom()->getSizeOfOneTile() * ( cell.first + cell.second + 2 ) - getRawImage().getHeight() - this->getZ() - 1 ;
                        setOffset( std::pair< int, int >( getOffsetX(), newOffsetY ) );

                        // mark to mask overlapping free items
                        mediator->wantToMaskWithGridItemAt( *this, getX(), getY(), zBefore, offsetBefore );
                        mediator->wantToMaskWithGridItem( *this );

                        // reshade items
                        mediator->wantShadowFromGridItem( *this );

                        mediator->markToSortGridItems ();
                }
        }

        // restore previous values on collision
        if ( collisionFound )
        {
                this->zYet = zBefore ;

                setOffset( offsetBefore );
        }

        return ! collisionFound;
}

unsigned int GridItem::getColumnOfGrid () const
{
        assert( mediator != nilPointer );
        return mediator->getRoom()->getTilesX() * getCellY() + getCellX();
}

}
