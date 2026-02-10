
#include "GridItem.hpp"

#include "DescriptionOfItem.hpp"
#include "Mediator.hpp"


GridItem::GridItem( const DescriptionOfItem & description, int cx, int cy, int z, const std::string & where )
        : DescribedItem( description )
        , cell( std::pair< int, int >( cx, cy ) )
        , theZ( z )
        , orientation( where )
        , imageOffset( std::pair< int, int >( 0, 0 ) )
{
        if ( ! where.empty() ) setCurrentFrameSequence( where );
        updateImageOffset() ;
}

void GridItem::updateImageOffset ( unsigned int imageWidth, unsigned int imageHeight )
{
        int oneTileLong = ( getMediator() != nilPointer ) ? getMediator()->getRoom()->getSizeOfOneTile() : Room::Single_Tile_Size ;

        int offsetX = ( ( oneTileLong * ( cell.first - cell.second ) ) << 1 ) - ( imageWidth >> 1 ) + 1 ;
        int offsetY = oneTileLong * ( cell.first + cell.second + 2 ) - imageHeight - this->getZ() - 1 ;

        this->imageOffset = std::pair< int, int >( offsetX, offsetY );
}

void GridItem::draw ()
{
        if ( getTransparency() >= 100 ) /* item is fully transparent */ return ;

        if ( getTransparency() == 0 ) {
                allegro::drawSprite( getProcessedImage().getAllegroPict(),
                                        getMediator()->getRoom()->getX0() + getImageOffsetX(),
                                        getMediator()->getRoom()->getY0() + getImageOffsetY() );
        } else {
                allegro::drawSpriteWithTransparency( getProcessedImage().getAllegroPict(),
                                        getMediator()->getRoom()->getX0 () + getImageOffsetX (),
                                        getMediator()->getRoom()->getY0 () + getImageOffsetY (),
                                        static_cast < unsigned char > ( 255 - 2.55 * getTransparency() ) );
        }
}

void GridItem::updateImage ()
{
        const int xBefore = getX() ;
        const int yBefore = getY() ;
        const int zBefore = getZ() ;

        std::pair< int, int > imageOffsetBefore = this->imageOffset ;

        DescribedItem::updateImage ();

        setWantShadow( true );

        updateImageOffset ();

        // remask items
        getMediator()->wantToMaskWithGridItemAt( *this, xBefore, yBefore, zBefore, imageOffsetBefore );
        getMediator()->wantToMaskWithGridItem( *this );
}

bool GridItem::addToPosition( int x, int y, int z )
{
        getMediator()->clearCollisions() ;

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
                getMediator()->addCollisionWith( "some tile of floor" );
                collisionFound = true;
        }
        // or maybe with other items in room
        else
        {
                collisionFound = getMediator()->collectCollisionsWith( this->getUniqueName() );
                if ( ! collisionFound )
                {
                        // change only the offset on Y because it depends on the 3D Z coordinate
                        int newOffsetY = getMediator()->getRoom()->getSizeOfOneTile() * ( cell.first + cell.second + 2 ) - getCurrentRawImage().getHeight() - this->getZ() - 1 ;
                        this->imageOffset = std::pair< int, int >( getImageOffsetX(), newOffsetY );

                        // mark to mask overlapping free items
                        getMediator()->wantToMaskWithGridItemAt( *this, getX(), getY(), zBefore, imageOffsetBefore );
                        getMediator()->wantToMaskWithGridItem( *this );

                        // reshade items
                        getMediator()->wantShadowFromGridItem( *this );

                        getMediator()->markToSortGridItems ();
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
        assert( getMediator() != nilPointer );
        return getMediator()->getRoom()->getTilesAlongX() * getCellY() + getCellX();
}

