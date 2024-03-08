
#include "FreeItem.hpp"

#include "DescriptionOfItem.hpp"
#include "Mediator.hpp"
#include "SoundManager.hpp"
#include "Color.hpp"

#ifdef DEBUG
#  define DEBUG_SHADOWS_AND_MASKS       0
#endif


FreeItem::FreeItem( const DescriptionOfItem* description, int x, int y, int z, const std::string & where )
        : Item ( description, z, where )
        , initialCellX( farFarAway )
        , initialCellY( farFarAway )
        , initialCellZ( farFarAway )
        , wantMask ( tritrue )
        , transparency ( 0 )
        , frozen ( false )
        , partOfDoor ( false )
        , shadedNonmaskedImage( new Picture( getRawImage() ) )
{
        setX( x );
        setY( y >= 0 ? y : 0 );

        freshBothProcessedImages ();

        // look for collisions
        setIgnoreCollisions( false );
}

FreeItem::FreeItem( const FreeItem & freeItem )
        : Item( freeItem ), Drawable()
        , initialCellX( freeItem.initialCellX )
        , initialCellY( freeItem.initialCellY )
        , initialCellZ( freeItem.initialCellZ )
        , wantMask( freeItem.wantMask )
        , transparency( freeItem.transparency )
        , frozen( freeItem.frozen )
        , partOfDoor( freeItem.partOfDoor )
        , shadedNonmaskedImage( new Picture( * freeItem.shadedNonmaskedImage ) )
{
}

int FreeItem::getImageOffsetX () const
{
        return ( ( getX() - getY() ) << 1 ) + getWidthX() + getWidthY() - ( getRawImage().getWidth() >> 1 ) - 1 ;
}

int FreeItem::getImageOffsetY () const
{
        return getX() + getY() + getWidthX() - getRawImage().getHeight() - getZ() ;
}

void FreeItem::draw ()
{
        if ( transparency >= 100 ) /* item is fully transparent */ return ;

        if ( transparency == 0 )
        {
                allegro::drawSprite(
                        getProcessedImage().getAllegroPict(),
                        mediator->getRoom()->getX0 () + getImageOffsetX (),
                        mediator->getRoom()->getY0 () + getImageOffsetY ()
                ) ;
        }
        else
        {
                allegro::drawSpriteWithTransparency(
                        getProcessedImage().getAllegroPict(),
                        mediator->getRoom()->getX0 () + getImageOffsetX (),
                        mediator->getRoom()->getY0 () + getImageOffsetY (),
                        static_cast < unsigned char > ( 255 - 2.55 * this->transparency )
                ) ;
        }
}

void FreeItem::freshProcessedImage ()
{
        getProcessedImage().fillWithColor( Color::keyColor () );
        allegro::bitBlit( getShadedNonmaskedImage().getAllegroPict(), getProcessedImage().getAllegroPict() );
        getProcessedImage().setName( "processed " + getShadedNonmaskedImage().getName() );

#if defined( DEBUG_SHADOWS_AND_MASKS ) && DEBUG_SHADOWS_AND_MASKS
        if ( getUniqueName().find( "bars" ) != std::string::npos ) {
                Color::multiplyWithColor( getProcessedImage(), Color::byName( "cyan" ) );
        }
#endif
}

void FreeItem::freshBothProcessedImages ()
{
        shadedNonmaskedImage->fillWithColor( Color::keyColor () );
        allegro::bitBlit( /* from */ getRawImage().getAllegroPict(), /* to */ shadedNonmaskedImage->getAllegroPict() );
        shadedNonmaskedImage->setName( "shaded " + getRawImage().getName() );

#if defined( DEBUG_SHADOWS_AND_MASKS ) && DEBUG_SHADOWS_AND_MASKS
        if ( getUniqueName().find( "bars" ) != std::string::npos ) {
                Color::multiplyWithColor( getShadedNonmaskedImage(), Color::byName( "yellow" ) );
        }
#endif

        freshProcessedImage ();
}

void FreeItem::updateImage ()
{
        const int imageOffsetXBefore = getImageOffsetX ();
        const int imageOffsetYBefore = getImageOffsetY ();

        freshBothProcessedImages ();
        setWantShadow( true );
        setWantMaskTrue();

        // remask
        mediator->wantToMaskWithFreeItemImageAt( *this, imageOffsetXBefore, imageOffsetYBefore );
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
        mediator->clearCollisions ();

        bool collisionFound = false;

        const int xBefore = getX ();
        const int yBefore = getY ();
        const int zBefore = getZ ();

        const int imageOffsetXBefore = getImageOffsetX ();
        const int imageOffsetYBefore = getImageOffsetY ();

        setX( xBefore + x );
        setY( yBefore + y );
        setZ( zBefore + z );

        // look for a collision with a wall
        if ( getX() < mediator->getRoom()->getLimitAt( "north" ) )
        {
                mediator->addCollisionWith( "some segment of the north wall" );
        }
        else if ( getX() + getWidthX() > mediator->getRoom()->getLimitAt( "south" ) )
        {
                mediator->addCollisionWith( "some segment of the south wall" );
        }
        if ( getY() >= mediator->getRoom()->getLimitAt( "west" ) )
        {
                mediator->addCollisionWith( "some segment of the east wall" );
        }
        else if ( getY() - getWidthY() + 1 < mediator->getRoom()->getLimitAt( "east" ) )
        {
                mediator->addCollisionWith( "some segment of the west wall" );
        }

        // collision with the floor
        if ( getZ() < 0 )
        {
                mediator->addCollisionWith( "some tile of floor" );
        }

        collisionFound = mediator->isThereAnyCollision ();
        if ( ! collisionFound )
        {
                // look for collision with other items in room
                collisionFound = mediator->collectCollisionsWith( this->getUniqueName() );
                if ( ! collisionFound ) // is it okay to move
                {
                        // reshade and remask
                        freshBothProcessedImages();
                        setWantShadow( true );
                        setWantMaskTrue();

                        // mark to remask
                        mediator->wantToMaskWithFreeItemImageAt( *this, imageOffsetXBefore, imageOffsetYBefore );
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
                setX( xBefore );
                setY( yBefore );
                setZ( zBefore );
        }

        return ! collisionFound;
}

bool FreeItem::isCollidingWithJamb( const std::string & at, const std::string & collision, const int previousX, const int previousY )
{
        Door* door = mediator->getRoom()->getDoorAt( at );
        if ( door == nilPointer ) return false ;

        // proceed only when colliding with a door’s jamb
        if ( door->getLeftJamb()->getUniqueName() != collision && door->getRightJamb()->getUniqueName() != collision ) return false ;

        const int xBefore = getX();
        const int yBefore = getY();

        switch ( Way( at ).getIntegerOfWay() )
        {
                // for rooms with the north or south door
                case Way::North:
                case Way::Northeast:
                case Way::Northwest:
                case Way::South:
                case Way::Southeast:
                case Way::Southwest:
                        // move the character right when it collides with the left jamb
                        if ( door->getLeftJamb()->getUniqueName() == collision && this->getY() <= door->getLeftJamb()->getY() )
                        {
                                setY( getY() - 1 ) ;
                                setX( previousX );
                        }
                        // move the character left when it hits the right jamb
                        else if ( door->getRightJamb()->getUniqueName() == collision
                                        && this->getY() - getWidthY() >= door->getRightJamb()->getY() - door->getRightJamb()->getWidthY() )
                        {
                                setY( getY() + 1 ) ;
                                setX( previousX );
                        }

                        break;

                // for rooms with the east or west door
                case Way::East:
                case Way::Eastnorth:
                case Way::Eastsouth:
                case Way::West:
                case Way::Westnorth:
                case Way::Westsouth:
                        // move the character right when it hits the left jamb
                        if ( door->getLeftJamb()->getUniqueName() == collision && this->getX() >= door->getLeftJamb()->getX() )
                        {
                                setX( getX() + 1 ) ;
                                setY( previousY );
                        }
                        // move the character left when it collides with the right jamb
                        else if ( door->getRightJamb()->getUniqueName() == collision
                                        && this->getX() - getWidthX() <= door->getRightJamb()->getX() + door->getRightJamb()->getWidthX() )
                        {
                                setX( getX() - 1 ) ;
                                setY( previousY );
                        }

                        break;

                default:
                        ;
        }

        if ( getX () != xBefore || yBefore != getY () )
        {
                SoundManager::getInstance().play ( "door", "collision", /* loop */ false );
                return true ;
        }

        return false ;
}
