
#include "FreeItem.hpp"

#include "DescriptionOfItem.hpp"
#include "Mediator.hpp"
#include "SoundManager.hpp"
#include "Color.hpp"

#include "NoSuchPictureException.hpp"

#ifdef DEBUG
#  define DEBUG_SHADOWS_AND_MASKS       0
#endif


FreeItem::FreeItem( const DescriptionOfItem & description, int x, int y, int z, const std::string & where )
        : DescribedItem( description )
        , theX( x )
        , theY( y >= 0 ? y : 0 )
        , theZ( z )
        , initialCellX( farFarAway )
        , initialCellY( farFarAway )
        , initialCellZ( farFarAway )
        , heading( "" )
        , wantMask ( tritrue )
        , frozen ( false )
        , partOfDoor ( false )
        , carrier( "" )
        , shadedNonmaskedImage( nilPointer )
{
        changeHeading( where );
        freshBothProcessedImages ();
}

FreeItem::FreeItem( const FreeItem & freeItem )
        : DescribedItem( freeItem ), Drawable()
        , theX( freeItem.theX )
        , theY( freeItem.theY )
        , theZ( freeItem.theZ )
        , initialCellX( freeItem.initialCellX )
        , initialCellY( freeItem.initialCellY )
        , initialCellZ( freeItem.initialCellZ )
        , heading( freeItem.heading )
        , wantMask( freeItem.wantMask )
        , frozen( freeItem.frozen )
        , partOfDoor( freeItem.partOfDoor )
        , carrier( freeItem.carrier )
        , shadedNonmaskedImage( ( freeItem.shadedNonmaskedImage != nilPointer ) ? new NamedPicture( * freeItem.shadedNonmaskedImage ) : nilPointer )
{}

void FreeItem::draw ()
{
        if ( getTransparency() >= 100 ) /* item is fully transparent */ return ;

        if ( getTransparency() == 0 ) {
                allegro::drawSprite(
                        getProcessedImage().getAllegroPict(),
                        getMediator()->getRoom().getX0 () + getImageOffsetX (),
                        getMediator()->getRoom().getY0 () + getImageOffsetY ()
                ) ;
        }
        else {
                allegro::drawSpriteWithTransparency(
                        getProcessedImage().getAllegroPict(),
                        getMediator()->getRoom().getX0 () + getImageOffsetX (),
                        getMediator()->getRoom().getY0 () + getImageOffsetY (),
                        static_cast < unsigned char > ( 255 - 2.55 * getTransparency() )
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
        if ( this->shadedNonmaskedImage == nilPointer )
                this->shadedNonmaskedImage.reset( new NamedPicture( getDescriptionOfItem().getWidthOfFrame(), getDescriptionOfItem().getHeightOfFrame() ) );

        this->shadedNonmaskedImage->fillWithColor( Color::keyColor () );

        try {
                const NamedPicture & currentRawImage = getCurrentRawImage() ;
                allegro::bitBlit( /* from */ currentRawImage.getAllegroPict(), /* to */ this->shadedNonmaskedImage->getAllegroPict() );
                this->shadedNonmaskedImage->setName( "shaded " + currentRawImage.getName() );
        } catch ( NoSuchPictureException const& e ) {
                this->shadedNonmaskedImage->setName( "empty shaded nonmasked image" );
        }

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
        getMediator()->wantMaskingWithFreeItemImageAt( *this, imageOffsetXBefore, imageOffsetYBefore );
        getMediator()->wantMaskingWithFreeItem( *this );
}

void FreeItem::requestShadow()
{
        if ( getWantShadow() ) {
                getMediator()->castShadowOnFreeItem( *this );

                if ( ! getWantShadow() ) setWantMaskTrue() ;
        }
}

void FreeItem::requestMask()
{
        getMediator()->maskFreeItem( *this );

        if ( getWantMask().isTrue() )
                freshProcessedImage ();

        setWantMaskFalse();
}

bool FreeItem::addToPosition( int x, int y, int z )
{
        getMediator()->clearCollisions ();

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
        if ( getX() < getMediator()->getRoom().getLimitAt( "north" ) )
        {
                getMediator()->addCollisionWith( "some segment of the north wall" );
        }
        else if ( getX() + getWidthX() > getMediator()->getRoom().getLimitAt( "south" ) )
        {
                getMediator()->addCollisionWith( "some segment of the south wall" );
        }
        if ( getY() >= getMediator()->getRoom().getLimitAt( "west" ) )
        {
                getMediator()->addCollisionWith( "some segment of the east wall" );
        }
        else if ( getY() - getWidthY() + 1 < getMediator()->getRoom().getLimitAt( "east" ) )
        {
                getMediator()->addCollisionWith( "some segment of the west wall" );
        }

        // collision with the floor
        if ( getZ() < 0 )
        {
                getMediator()->addCollisionWith( "some tile of floor" );
        }

        collisionFound = getMediator()->isThereAnyCollision ();
        if ( ! collisionFound )
        {
                // look for collision with other items in room
                collisionFound = getMediator()->collectCollisionsWith( this->getUniqueName() );
                if ( ! collisionFound ) // is it okay to move
                {
                        // reshade and remask
                        freshBothProcessedImages();
                        setWantShadow( true );
                        setWantMaskTrue();

                        // mark to remask
                        getMediator()->wantMaskingWithFreeItemImageAt( *this, imageOffsetXBefore, imageOffsetYBefore );
                        getMediator()->wantMaskingWithFreeItem( *this );

                        // reshade items
                        getMediator()->castShadowFromFreeItemAt( *this, xBefore, yBefore, zBefore );
                        getMediator()->castShadowFromFreeItem( *this );

                        getMediator()->markToSortFreeItems ();
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

void FreeItem::changeHeading( const std::string & where )
{
        if ( this->heading != where ) {
                this->heading = where ;
                setCurrentFrameSequence( this->heading );

                IF_DEBUG( std::cout << getUniqueName() << " is now heading " << getHeading() << std::endl )
        }
}

void FreeItem::reverseHeading ()
{
             if ( this->heading == "north" ) changeHeading( "south" );
        else if ( this->heading == "south" ) changeHeading( "north" );
        else if ( this->heading ==  "east" ) changeHeading( "west" );
        else if ( this->heading == "west"  ) changeHeading( "east" );
}

bool FreeItem::isCollidingWithJamb( const std::string & at, const std::string & collision, const int previousX, const int previousY )
{
        Door* door = getMediator()->getRoom().getDoorOn( at );
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
