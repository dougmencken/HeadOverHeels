
#include "FreeItem.hpp"
#include "DescriptionOfItem.hpp"
#include "Mediator.hpp"
#include "SoundManager.hpp"
#include "Color.hpp"

#ifdef DEBUG
#  define DEBUG_SHADOWS_AND_MASKS       0
#endif


namespace iso
{

FreeItem::FreeItem( const DescriptionOfItem* description, int x, int y, int z, const std::string& way )
        : Item ( description, z, way )
        , originalCellX( farFarAway )
        , originalCellY( farFarAway )
        , originalCellZ( farFarAway )
        , wantMask ( tritrue )
        , transparency ( 0 )
        , frozen ( false )
        , partOfDoor ( false )
        , shadedNonmaskedImage ( )
{
        xYet = x ;
        yYet = y ;
        if ( yYet < 0 ) yYet = 0 ;

        freshBothProcessedImages ();

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
        , partOfDoor( freeItem.partOfDoor )
        , shadedNonmaskedImage( new Picture( * freeItem.shadedNonmaskedImage ) )
{
}

void FreeItem::calculateOffset ()
{
        int offsetX = ( ( getX() - getY() ) << 1 ) + static_cast< int >( getWidthX() + getWidthY() ) - ( getRawImage().getWidth() >> 1 ) - 1 ;
        int offsetY = getX() + getY() + static_cast< int >( getWidthX() ) - getRawImage().getHeight() - getZ() ;
        setOffset( std::pair< int, int >( offsetX, offsetY ) );
}

void FreeItem::draw ()
{
        if ( transparency >= 100 ) /* item is fully transparent */ return ;

        if ( transparency == 0 )
        {
                allegro::drawSprite(
                        getProcessedImage().getAllegroPict(),
                        mediator->getRoom()->getX0 () + getOffsetX (),
                        mediator->getRoom()->getY0 () + getOffsetY ()
                ) ;
        }
        else
        {
                allegro::drawSpriteWithTransparency(
                        getProcessedImage().getAllegroPict(),
                        mediator->getRoom()->getX0 () + getOffsetX (),
                        mediator->getRoom()->getY0 () + getOffsetY (),
                        static_cast < unsigned char > ( 255 - 2.55 * this->transparency )
                ) ;
        }
}

void FreeItem::freshProcessedImage ()
{
        getProcessedImage().fillWithColor( Color() );
        allegro::bitBlit( getShadedNonmaskedImage().getAllegroPict(), getProcessedImage().getAllegroPict() );
        getProcessedImage().setName( "processed " + getShadedNonmaskedImage().getName() );

#if defined( DEBUG_SHADOWS_AND_MASKS ) && DEBUG_SHADOWS_AND_MASKS
        if ( getUniqueName().find( "bars" ) != std::string::npos )
        {
                std::cout << TERMINAL_COLOR_BLUE << "\"" << getProcessedImage().getName()
                                << "\" of " << whichKindOfItem() << " \"" << getUniqueName()
                                << "\" just refreshed from shaded nonmasked image"
                                << TERMINAL_COLOR_OFF << std::endl ;

                Color::multiplyWithColor( getProcessedImage(), Color::byName( "cyan" ) );
        }
#endif
}

void FreeItem::freshBothProcessedImages ()
{
        shadedNonmaskedImage = PicturePtr( new Picture( getRawImage () ) );
        shadedNonmaskedImage->setName( "shaded " + getRawImage().getName() );

#if defined( DEBUG_SHADOWS_AND_MASKS ) && DEBUG_SHADOWS_AND_MASKS
        if ( getUniqueName().find( "bars" ) != std::string::npos )
        {
                std::cout << TERMINAL_COLOR_RED << "\"" << getShadedNonmaskedImage().getName()
                                << "\" of " << whichKindOfItem() << " \"" << getUniqueName()
                                << "\" just refreshed from raw image"
                                << TERMINAL_COLOR_OFF << std::endl ;

                Color::multiplyWithColor( getShadedNonmaskedImage(), Color::byName( "yellow" ) );
        }
#endif

        freshProcessedImage ();
}

void FreeItem::updateImage ()
{
        std::pair< int, int > offsetBefore = getOffset() ;

        calculateOffset ();

        freshBothProcessedImages ();
        setWantShadow( true );
        setWantMaskTrue();

        // remask items in room
        mediator->wantToMaskWithFreeItemAt( *this, offsetBefore );
        mediator->wantToMaskWithFreeItem( *this );
}

void FreeItem::updateShadow ()
{
        Item::updateShadow ();

        // reshade items
        mediator->wantShadowFromFreeItem( *this );
}

void FreeItem::requestShadow()
{
        if ( getWantShadow() )
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

        if ( getWantMask().isTrue() )
                freshProcessedImage ();

        setWantMaskFalse();
}

bool FreeItem::addToPosition( int x, int y, int z )
{
        mediator->clearStackOfCollisions( );

        bool collisionFound = false;

        int xBefore = xYet ;
        int yBefore = yYet ;
        int zBefore = zYet ;

        std::pair< int, int > offsetBefore = getOffset() ;

        xYet += x ;
        yYet += y ;
        zYet += z ;

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
                        freshBothProcessedImages();
                        setWantShadow( true );
                        setWantMaskTrue();

                        calculateOffset ();

                        // mark to mask free items whose images overlap with its image
                        mediator->wantToMaskWithFreeItemAt( *this, offsetBefore );
                        mediator->wantToMaskWithFreeItem( *this );

                        // reshade items
                        mediator->wantShadowFromFreeItemAt( *this, xBefore, yBefore, zBefore );
                        mediator->wantShadowFromFreeItem( *this );

                        mediator->markToSortFreeItems ();
                }
        }

        // restore previous values when there’s a collision
        if ( collisionFound )
        {
                xYet = xBefore ;
                yYet = yBefore ;
                zYet = zBefore ;

                setOffset( offsetBefore );
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
                                yYet -- ;
                                xYet = previousX ;
                        }
                        // move player left when player hits right jamb
                        else if ( door->getRightJamb()->getUniqueName() == name &&
                                        this->getY() - static_cast< int >( getWidthY() )
                                                >= door->getRightJamb()->getY() - static_cast< int >( door->getRightJamb()->getWidthY() ) )
                        {
                                yYet ++ ;
                                xYet = previousX ;
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
                                xYet ++ ;
                                yYet = previousY ;
                        }
                        // move player left when player collides with right jamb
                        else if ( door->getRightJamb()->getUniqueName() == name &&
                                        this->getX() - static_cast< int >( getWidthX() )
                                                <= door->getRightJamb()->getX() + static_cast< int >( door->getRightJamb()->getWidthX() ) )
                        {
                                xYet -- ;
                                yYet = previousY ;
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

}
