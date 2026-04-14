
#include "Door.hpp"

#include "Color.hpp"
#include "DescriptionOfItem.hpp"
#include "ItemDescriptions.hpp"
#include "FreeItem.hpp"
#include "Mediator.hpp"
#include "GameManager.hpp"
#include "PoolOfPictures.hpp"
#include "UnlikelyToHappenException.hpp"

#include "util.hpp"
#include "ospaths.hpp"

#ifdef DEBUG
#  define SAVE_ITEM_FRAMES      0
#  define GRAYSCALE_JAMBS       0
#endif


Door::Door( const std::string & kind, int cx, int cy, int z, const std::string & on )
        : kindOfDoor( kind )
        , cellX( cx )
        , cellY( cy )
        , elevation( z )
        , onWhichSide( on )
        , leftLimit( 1 << 22 )
        , rightLimit( 1 << 22 )
        , leftJamb( nilPointer )
        , rightJamb( nilPointer )
        , lintel( nilPointer )
{
        // make sure the graphics of this door is loaded

        const DescriptionOfItem * whatIsLintel = ItemDescriptions::descriptions().getDescriptionByKind( kind + "~lintel" );

        if ( whatIsLintel == nilPointer ) {
                std::string message = "no description for the parts of " + kind ;
                std::cerr << message << std::endl ;
                throw UnlikelyToHappenException( message ) ;
        }

        const std::string & doorImageFile = whatIsLintel->getNameOfFramesFile() ;
        const NamedPicturePtr & pictureOfDoor = PoolOfPictures::getPoolOfPictures().getOrLoadAndGet( doorImageFile );

        if ( pictureOfDoor == nilPointer ) {
                std::string message = "the door graphics \"" + doorImageFile + "\" is absent" ;
                std::cerr << message << std::endl ;
                throw UnlikelyToHappenException( message ) ;
        }
}

/* static */
NamedPicture* Door::cutOutLintel( const NamedPicturePtr & doorImage,
                                        unsigned int widthX, unsigned int widthY, unsigned int height,
                                        unsigned int leftJambWidthX, unsigned int leftJambWidthY,
                                        unsigned int rightJambWidthX, unsigned int rightJambWidthY,
                                        const std::string & on )
{
        unsigned int lintelWidth = ( widthX + widthY ) << 1  ;
        unsigned int lintelHeight = height + widthY + widthX ;

        NamedPicture * lintel = new NamedPicture( lintelWidth, lintelHeight );

        bool ns = ( on == "north" || on == "south" || on == "northeast" || on == "northwest" || on == "southeast" || on == "southwest" );
        if ( ns ) {
                unsigned int copiedHeight = height + widthX ;
                allegro::bitBlit( doorImage->getAllegroPict(), lintel->getAllegroPict(), 0, 0, 0, 0, lintelWidth, copiedHeight );

                int noPixel = lintelWidth - ( ( rightJambWidthX + rightJambWidthY ) << 1 ) + 1 ;
                int yStart = noPixel ;
                int yEnd = noPixel - 1 ;

                unsigned int firstX = lintelWidth ;
                for ( unsigned int y = copiedHeight ; y < lintelHeight ; y ++ ) {
                        for ( unsigned int x = firstX ; x > 0 ; x -- )
                        {
                                int xPixel = static_cast< int >( x ) - 1 ;
                                if ( y != copiedHeight && xPixel == noPixel ) {
                                        if ( noPixel > yEnd ) noPixel -- ;
                                        else {
                                                yStart += 2 ;
                                                noPixel = yStart ;
                                        }
                                }
                                else if ( y < height + ( widthX << 1 ) || xPixel < yEnd )
                                        lintel->putPixelAt( xPixel, y, doorImage->getPixelAt( xPixel, y ) );
                        }
                        firstX -= 2 ;
                }
        }
        else {
                unsigned int copiedHeight = height + widthY ;
                allegro::bitBlit( doorImage->getAllegroPict(), lintel->getAllegroPict(), 0, 0, 0, 0, lintelWidth, copiedHeight );

                int noPixel = ( ( leftJambWidthX + leftJambWidthY ) << 1 ) - 2 ;
                int yStart = noPixel ;
                int yEnd = noPixel + 1 ;

                unsigned int firstX = 0 ;
                for ( unsigned int y = copiedHeight ; y < lintelHeight ; y ++ ) {
                        for ( unsigned int x = firstX ; x < lintelWidth ; x ++ )
                        {
                                int xPixel = static_cast< int >( x ) ;
                                if ( y != copiedHeight && xPixel == noPixel ) {
                                        if ( noPixel < yEnd ) noPixel ++ ;
                                        else {
                                                yStart -= 2 ;
                                                noPixel = yStart ;
                                        }
                                }
                                else if ( y < height + ( widthY << 1 ) || xPixel > yEnd )
                                        lintel->putPixelAt( xPixel, y, doorImage->getPixelAt( xPixel, y ) );
                        }
                        firstX += 2 ;
                }
        }

        lintel->setName( "lintel of " + doorImage->getName() );
        return lintel ;
}

/* static */
NamedPicture* Door::cutOutLeftJamb( const NamedPicturePtr & doorImage,
                                        unsigned int widthX, unsigned int widthY, unsigned int height,
                                        /* unsigned int lintelWidthX, */ unsigned int lintelWidthY, unsigned int lintelHeight,
                                        const std::string & on )
{
        bool ns = ( on == "north" || on == "south" || on == "northeast" || on == "northwest" || on == "southeast" || on == "southwest" );
        unsigned int widthCorrection = ( ns ? 7 : 0 );
        int yCorrection = ( ns ? -1 : 0 );

        NamedPicture * leftJamb = new NamedPicture( (( widthX + widthY ) << 1) + widthCorrection, height + widthY + widthX );

        allegro::bitBlit( doorImage->getAllegroPict(), leftJamb->getAllegroPict(),
                                yCorrection, lintelHeight + lintelWidthY - widthY + yCorrection,
                                0, 0,
                                leftJamb->getWidth(), leftJamb->getHeight() );

# if defined( GRAYSCALE_JAMBS ) && GRAYSCALE_JAMBS
        Color::pictureToGrayscale( leftJamb );
# endif

        leftJamb->setName( "left jamb of " + doorImage->getName() );
        return leftJamb ;
}

/* static */
NamedPicture* Door::cutOutRightJamb( const NamedPicturePtr & doorImage,
                                        unsigned int widthX, unsigned int widthY, unsigned int height,
                                        unsigned int lintelWidthX, /* unsigned int lintelWidthY, */ unsigned int lintelHeight,
                                        const std::string & on )
{
        bool ns = ( on == "north" || on == "south" || on == "northeast" || on == "northwest" || on == "southeast" || on == "southwest" );
        unsigned int widthCorrection = ( ns ? 0 : 7 );
        int yCorrection = ( ns ? 0 : -2 );

        NamedPicture * rightJamb = new NamedPicture( (( widthX + widthY ) << 1) + widthCorrection, height + widthY + widthX );

        allegro::bitBlit( doorImage->getAllegroPict(), rightJamb->getAllegroPict(),
                                doorImage->getWidth() - rightJamb->getWidth(), lintelHeight + lintelWidthX - widthY + yCorrection,
                                0, 0,
                                rightJamb->getWidth(), rightJamb->getHeight() );

# if defined( GRAYSCALE_JAMBS ) && GRAYSCALE_JAMBS
        Color::pictureToGrayscale( rightJamb );
# endif

        rightJamb->setName( "right jamb of " + doorImage->getName() );
        return rightJamb ;
}

const FreeItemPtr & Door::getLeftJamb()
{
        if ( this->leftJamb == nilPointer )
        {
                ItemDescriptions & descriptions = ItemDescriptions::descriptions() ;
                PoolOfPictures & imagePool = PoolOfPictures::getPoolOfPictures() ;

                const DescriptionOfItem & whatIsLeftJamb = * descriptions.getDescriptionByKind( getKind() + "~leftjamb" );

                const std::string & doorImageName = whatIsLeftJamb.getNameOfFramesFile() ;
                std::string leftJambName = "left jamb of " + doorImageName ;

                if ( imagePool.getPicture( leftJambName ) == nilPointer ) {
                        const DescriptionOfItem & whatIsLintel = * descriptions.getDescriptionByKind( getKind() + "~lintel" );

                        // cut out the left jamb
                        NamedPicture * leftJambCut = Door::cutOutLeftJamb( imagePool.getPicture( doorImageName ),
                                                whatIsLeftJamb.getWidthX(), whatIsLeftJamb.getWidthY(), whatIsLeftJamb.getHeight(),
                                                whatIsLintel.getWidthY(), whatIsLintel.getHeight(),
                                                getRoomSide() );

                        imagePool.putPicture( leftJambName, NamedPicturePtr( leftJambCut ) );

                # if defined( SAVE_ITEM_FRAMES ) && SAVE_ITEM_FRAMES
                        imagePool.getPicture( leftJambName )->saveAsPNG( ospaths::homePath() );
                # endif
                }

                const NamedPicturePtr & leftJambImage = imagePool.getPicture( leftJambName );
                if ( leftJambImage == nilPointer )
                        throw UnlikelyToHappenException( "nil image for the left jamb of " + getKind() );

                int oneCell = getMediator()->getRoom().getSizeOfOneCell ();

                int x = 0 ; int y = 0 ;

                switch ( Way( getRoomSide() ).toInteger() )
                {
                        case Way::North:
                        case Way::Northeast:
                        case Way::Northwest:
                                x = this->cellX * oneCell + whatIsLeftJamb.getWidthX() - 2 ;
                                y = ( this->cellY + 2 ) * oneCell - 2 ;
                                this->leftLimit = y + whatIsLeftJamb.getWidthY() ;
                                break ;

                        case Way::South:
                        case Way::Southeast:
                        case Way::Southwest:
                                x = this->cellX * oneCell ;
                                y = ( this->cellY + 2 ) * oneCell - 2 ;
                                this->leftLimit = y + whatIsLeftJamb.getWidthY() ;
                                break ;

                        case Way::East:
                        case Way::Eastnorth:
                        case Way::Eastsouth:
                                x = this->cellX * oneCell ;
                                y = ( this->cellY + 1 ) * oneCell - 1 ;
                                this->leftLimit = x + whatIsLeftJamb.getWidthX() ;
                                break ;

                        case Way::West:
                        case Way::Westnorth:
                        case Way::Westsouth:
                                x = this->cellX * oneCell ;
                                y = ( this->cellY + 1 ) * oneCell - whatIsLeftJamb.getWidthY() + 1 ;
                                this->leftLimit = x + whatIsLeftJamb.getWidthX() ;
                                break ;
                }

                leftJamb = FreeItemPtr( new FreeItem( whatIsLeftJamb, x, y, Room::FloorZ, onWhichSideOfTheFour() ) );
                leftJamb->setUniqueName( leftJamb->getKind() + "." + util::makeRandomString( 8 ) );
                leftJamb->addFrameTo( leftJamb->getHeading(), new NamedPicture( * leftJambImage ) );
                leftJamb->setMediator( getMediator() );
        }

        return this->leftJamb ;
}

const FreeItemPtr & Door::getRightJamb()
{
        if ( this->rightJamb == nilPointer )
        {
                ItemDescriptions & descriptions = ItemDescriptions::descriptions() ;
                PoolOfPictures & imagePool = PoolOfPictures::getPoolOfPictures() ;

                const DescriptionOfItem & whatIsRightJamb = * descriptions.getDescriptionByKind( getKind() + "~rightjamb" );

                const std::string & doorImageName = whatIsRightJamb.getNameOfFramesFile() ;
                std::string rightJambName = "right jamb of " + doorImageName ;

                if ( imagePool.getPicture( rightJambName ) == nilPointer ) {
                        const DescriptionOfItem & whatIsLintel = * descriptions.getDescriptionByKind( getKind() + "~lintel" );

                        // cut out the right jamb
                        NamedPicture * rightJambCut = Door::cutOutRightJamb( imagePool.getPicture( doorImageName ),
                                                whatIsRightJamb.getWidthX(), whatIsRightJamb.getWidthY(), whatIsRightJamb.getHeight(),
                                                whatIsLintel.getWidthX(), whatIsLintel.getHeight(),
                                                getRoomSide() );

                        imagePool.putPicture( rightJambName, NamedPicturePtr( rightJambCut ) );

                # if defined( SAVE_ITEM_FRAMES ) && SAVE_ITEM_FRAMES
                        imagePool.getPicture( rightJambName )->saveAsPNG( ospaths::homePath() );
                # endif
                }

                const NamedPicturePtr & rightJambImage = imagePool.getPicture( rightJambName );
                if ( rightJambImage == nilPointer )
                        throw UnlikelyToHappenException( "nil image for the right jamb of " + getKind() );

                int oneCell = getMediator()->getRoom().getSizeOfOneCell ();

                int x = 0 ; int y = 0 ;

                switch ( Way( getRoomSide() ).toInteger() )
                {
                        case Way::North:
                        case Way::Northeast:
                        case Way::Northwest:
                                x = this->cellX * oneCell + whatIsRightJamb.getWidthX() - 2 ;
                                y = this->cellY * oneCell + whatIsRightJamb.getWidthY() - 1 ;
                                this->rightLimit = y ;
                                break ;

                        case Way::South:
                        case Way::Southeast:
                        case Way::Southwest:
                                x = this->cellX * oneCell ;
                                y = this->cellY * oneCell + whatIsRightJamb.getWidthY() - 1 ;
                                this->rightLimit = y ;
                                break ;

                        case Way::East:
                        case Way::Eastnorth:
                        case Way::Eastsouth:
                                x = ( this->cellX + 2 ) * oneCell - whatIsRightJamb.getWidthX() - 2 ;
                                y = ( this->cellY + 1 ) * oneCell - 1 ;
                                this->rightLimit = x ;
                                break ;

                        case Way::West:
                        case Way::Westnorth:
                        case Way::Westsouth:
                                x = ( this->cellX + 2 ) * oneCell - whatIsRightJamb.getWidthX() - 2 ;
                                y = ( this->cellY + 1 ) * oneCell - whatIsRightJamb.getWidthY() + 1 ;
                                this->rightLimit = x ;
                                break ;
                }

                rightJamb = FreeItemPtr( new FreeItem( whatIsRightJamb, x, y, Room::FloorZ, onWhichSideOfTheFour() ) );
                rightJamb->setUniqueName( rightJamb->getKind() + "." + util::makeRandomString( 8 ) );
                rightJamb->addFrameTo( rightJamb->getHeading(), new NamedPicture( * rightJambImage ) );
                rightJamb->setMediator( getMediator() );
        }

        return this->rightJamb ;
}

const FreeItemPtr & Door::getLintel()
{
        if ( this->lintel == nilPointer )
        {
                ItemDescriptions & descriptions = ItemDescriptions::descriptions() ;
                PoolOfPictures & imagePool = PoolOfPictures::getPoolOfPictures() ;

                const DescriptionOfItem & whatIsLintel = * descriptions.getDescriptionByKind( getKind() + "~lintel" );

                const std::string & doorImageName = whatIsLintel.getNameOfFramesFile() ;
                std::string lintelName = "lintel of " + doorImageName ;

                if ( imagePool.getPicture( lintelName ) == nilPointer ) {
                        const DescriptionOfItem & whatIsLeftJamb = * descriptions.getDescriptionByKind( getKind() + "~leftjamb" );
                        const DescriptionOfItem & whatIsRightJamb = * descriptions.getDescriptionByKind( getKind() + "~rightjamb" );

                        // cut out the lintel
                        NamedPicture * lintelCut = Door::cutOutLintel( imagePool.getPicture( doorImageName ),
                                                whatIsLintel.getWidthX(), whatIsLintel.getWidthY(), whatIsLintel.getHeight(),
                                                whatIsLeftJamb.getWidthX(), whatIsLeftJamb.getWidthY(),
                                                whatIsRightJamb.getWidthX(), whatIsRightJamb.getWidthY(),
                                                getRoomSide() );

                        imagePool.putPicture( lintelName, NamedPicturePtr( lintelCut ) );

                # if defined( SAVE_ITEM_FRAMES ) && SAVE_ITEM_FRAMES
                        imagePool.getPicture( lintelName )->saveAsPNG( ospaths::homePath() );
                # endif
                }

                const NamedPicturePtr & lintelImage = imagePool.getPicture( lintelName );
                if ( lintelImage == nilPointer )
                        throw UnlikelyToHappenException( "nil image for the lintel of " + getKind() );

                int oneCell = getMediator()->getRoom().getSizeOfOneCell ();

                int x( 0 ), y( 0 );

                switch ( Way( getRoomSide() ).toInteger() )
                {
                        case Way::North:
                        case Way::Northeast:
                        case Way::Northwest:
                                x = this->cellX * oneCell + whatIsLintel.getWidthX() - 2 ;
                                y = ( this->cellY + 2 ) * oneCell - 1 ;
                                break ;

                        case Way::South:
                        case Way::Southeast:
                        case Way::Southwest:
                                x = this->cellX * oneCell ;
                                y = ( this->cellY + 2 ) * oneCell - 1 ;
                                break ;

                        case Way::East:
                        case Way::Eastnorth:
                        case Way::Eastsouth:
                                x = this->cellX * oneCell ;
                                y = ( this->cellY + 1 ) * oneCell - 1 ;
                                break ;

                        case Way::West:
                        case Way::Westnorth:
                        case Way::Westsouth:
                                x = this->cellX * oneCell ;
                                y = ( this->cellY + 1 ) * oneCell - whatIsLintel.getWidthY() + 1 ;
                                break ;
                }

                lintel = FreeItemPtr( new FreeItem( whatIsLintel, x, y, Room::FloorZ, onWhichSideOfTheFour() ) );
                lintel->setUniqueName( lintel->getKind() + "." + util::makeRandomString( 8 ) );
                lintel->addFrameTo( lintel->getHeading(), new NamedPicture( * lintelImage ) );
                lintel->setMediator( getMediator() );
        }

        return this->lintel ;
}

bool Door::isUnderDoor( int x, int y, int z ) const
{
        if ( z < 0 ) z = 0 ;

        bool under = ( leftJamb->getZ() <= z && z <= lintel->getZ() );

        const int oneCell = getMediator()->getRoom().getSizeOfOneCell ();

        switch ( Way( getRoomSide() ).toInteger() )
        {
                case Way::North:
                case Way::Northeast:
                case Way::Northwest:
                case Way::South:
                case Way::Southeast:
                case Way::Southwest:
                        under = under && ( this->rightLimit <= y && y <= this->leftLimit );
                        if ( under ) {
                                int doorFreeX = getCellX() * oneCell ;
                                under = ( doorFreeX - oneCell <= x && x <= doorFreeX + oneCell );
                        }
                        break;

                case Way::East:
                case Way::Eastnorth:
                case Way::Eastsouth:
                case Way::West:
                case Way::Westnorth:
                case Way::Westsouth:
                        under = under && ( this->leftLimit <= x && x <= this->rightLimit );
                        if ( under ) {
                                int doorFreeY = ( getCellY() + 1 ) * oneCell - 1 ;
                                under = ( doorFreeY - oneCell <= y && y <= doorFreeY + oneCell );
                        }
                        break;

                default:
                        ;
        }

        return under ;
}
