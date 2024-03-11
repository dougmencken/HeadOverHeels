
#include "GridItem.hpp"

#include "DescriptionOfItem.hpp"
#include "Mediator.hpp"


GridItem::GridItem( const DescriptionOfItem & description, int cx, int cy, int z, const std::string & where )
        : Item( description, z, where )
        , cell( std::pair< int, int >( cx, cy ) )
        , imageOffset( std::pair< int, int >( 0, 0 ) )
{
        // the free coordinates of this grid item
        // the widths of a grid item are equal to the room’s getSizeOfOneTile() = Room::Single_Tile_Size
        setX( cx * getWidthX() );
        setY( ( cy + 1 ) * getWidthY() - 1 );

        // look for collisions
        setIgnoreCollisions( false );
}

GridItem::~GridItem( )
{
}

void GridItem::updateImageOffset ()
{
        int oneTileLong = ( mediator != nilPointer ) ? mediator->getRoom()->getSizeOfOneTile() : Room::Single_Tile_Size ;

        int offsetX = ( ( oneTileLong * ( cell.first - cell.second ) ) << 1 ) - ( getRawImage().getWidth() >> 1 ) + 1 ;
        int offsetY = oneTileLong * ( cell.first + cell.second + 2 ) - getRawImage().getHeight() - this->getZ() - 1 ;

        this->imageOffset = std::pair< int, int >( offsetX, offsetY );
}

void GridItem::draw ()
{
        allegro::drawSprite( getProcessedImage().getAllegroPict(),
                mediator->getRoom()->getX0() + getImageOffsetX(), mediator->getRoom()->getY0() + getImageOffsetY() );
}

void GridItem::updateImage ()
{
        const int xBefore = getX() ;
        const int yBefore = getY() ;
        const int zBefore = getZ() ;

        std::pair< int, int > imageOffsetBefore = this->imageOffset ;

        Item::updateImage ();

        setWantShadow( true );

        updateImageOffset ();

        // remask items
        mediator->wantToMaskWithGridItemAt( *this, xBefore, yBefore, zBefore, imageOffsetBefore );
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
        mediator->clearCollisions() ;

        bool collisionFound = false;

        if ( x != 0 )
                std::cout << "can’t change the position on X for grid item \"" << getUniqueName() << ",\" ignoring x = " << x << std::endl ;
        if ( y != 0 )
                std::cout << "can’t change the position on Y for grid item \"" << getUniqueName() << ",\" ignoring y = " << y << std::endl ;

        const int zBefore = getZ() ;

        std::pair< int, int > imageOffsetBefore = this->imageOffset ;

        setZ( zBefore + z );

        // is there collision with floor
        if ( this->getZ() < 0 )
        {
                mediator->addCollisionWith( "some tile of floor" );
                collisionFound = true;
        }
        // or maybe with other items in room
        else
        {
                collisionFound = mediator->collectCollisionsWith( this->getUniqueName() );
                if ( ! collisionFound )
                {
                        // change only the offset on Y because it depends on the 3D Z coordinate
                        int newOffsetY = mediator->getRoom()->getSizeOfOneTile() * ( cell.first + cell.second + 2 ) - getRawImage().getHeight() - this->getZ() - 1 ;
                        this->imageOffset = std::pair< int, int >( getImageOffsetX(), newOffsetY );

                        // mark to mask overlapping free items
                        mediator->wantToMaskWithGridItemAt( *this, getX(), getY(), zBefore, imageOffsetBefore );
                        mediator->wantToMaskWithGridItem( *this );

                        // reshade items
                        mediator->wantShadowFromGridItem( *this );

                        mediator->markToSortGridItems ();
                }
        }

        // restore previous values on collision
        if ( collisionFound )
        {
                setZ( zBefore );
                this->imageOffset = imageOffsetBefore ;
        }

        return ! collisionFound;
}

unsigned int GridItem::getColumnOfGrid () const
{
        assert( mediator != nilPointer );
        return mediator->getRoom()->getTilesX() * getCellY() + getCellX();
}

