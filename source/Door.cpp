
#include "Door.hpp"

#include "Color.hpp"
#include "DescriptionOfItem.hpp"
#include "ItemDescriptions.hpp"
#include "FreeItem.hpp"
#include "Mediator.hpp"
#include "GameManager.hpp"
#include "MayNotBePossible.hpp"

#include "util.hpp"
#include "ospaths.hpp"

#ifdef DEBUG
#  define SAVE_ITEM_FRAMES      0
#  define GRAYSCALE_JAMBS       0
#endif


Door::Door( const std::string & kind, int cx, int cy, int z, const std::string & where )
        : kindOfDoor( kind )
        , cellX( cx )
        , cellY( cy )
        , theZ( z )
        , whereIsDoor( where )
        , leftJambImage( nilPointer )
        , rightJambImage( nilPointer )
        , lintelImage( nilPointer )
        , leftJamb( nilPointer )
        , rightJamb( nilPointer )
        , lintel( nilPointer )
{
        const DescriptionOfItem & whatIsLintel = * ItemDescriptions::descriptions ().getDescriptionByKind( kindOfDoor + "~lintel" );

        // load the graphics of this door

        const std::string & graphicsOfDoor = whatIsLintel.getNameOfPicturesFile () ;
        const std::string & chosenSet = GameManager::getInstance().getChosenGraphicsSet() ;

        autouniqueptr< allegro::Pict > pictureOfDoor( allegro::Pict::fromPNGFile(
                ospaths::pathToFile( ospaths::sharePath() + chosenSet, graphicsOfDoor )
        ) );
        if ( ! pictureOfDoor->isNotNil() ) {
                std::string message = "the graphics of door \"" + graphicsOfDoor + "\"" + " from set \"" + chosenSet + "\" is absent" ;
                std::cerr << message << std::endl ;
                throw MayNotBePossible( message ) ;
        }

        std::string scenery = kindOfDoor.substr( 0, kindOfDoor.find( "-" ) );

        // cut out the left jamb

        const DescriptionOfItem & whatIsLeftJamb = * ItemDescriptions::descriptions ().getDescriptionByKind( kindOfDoor + "~leftjamb" );

        leftJambImage = cutOutLeftJamb( *pictureOfDoor,
                                        whatIsLeftJamb.getWidthX(), whatIsLeftJamb.getWidthY(), whatIsLeftJamb.getHeight(),
                                        whatIsLintel.getWidthY(), whatIsLintel.getHeight(),
                                        where );
        leftJambImage->setName( "left jamb of door on " + scenery + " at " + where );

        // cut out the right jamb

        const DescriptionOfItem & whatIsRightJamb = * ItemDescriptions::descriptions ().getDescriptionByKind( kindOfDoor + "~rightjamb" );

        rightJambImage = cutOutRightJamb( *pictureOfDoor,
                                        whatIsRightJamb.getWidthX(), whatIsRightJamb.getWidthY(), whatIsRightJamb.getHeight(),
                                        whatIsLintel.getWidthX(), whatIsLintel.getHeight(),
                                        where );
        rightJambImage->setName( "right jamb of door on " + scenery + " at " + where );

        // cut out the lintel

        lintelImage = cutOutLintel( *pictureOfDoor,
                                        whatIsLintel.getWidthX(), whatIsLintel.getWidthY(), whatIsLintel.getHeight(),
                                        whatIsLeftJamb.getWidthX(), whatIsLeftJamb.getWidthY(),
                                        whatIsRightJamb.getWidthX(), whatIsRightJamb.getWidthY(),
                                        where );
        lintelImage->setName( "lintel of door on " + scenery + " at " + where );

# if defined( SAVE_ITEM_FRAMES ) && SAVE_ITEM_FRAMES

        leftJambImage->saveAsPNG( ospaths::homePath() );
        rightJambImage->saveAsPNG( ospaths::homePath() );
        lintelImage->saveAsPNG( ospaths::homePath() );

# endif
}

Door::~Door()
{
        delete this->leftJambImage ;
        delete this->rightJambImage ;
        delete this->lintelImage ;
}

/* static */
NamedPicture* Door::cutOutLintel( const allegro::Pict& door, unsigned int widthX, unsigned int widthY, unsigned int height,
                                        unsigned int leftJambWidthX, unsigned int leftJambWidthY,
                                        unsigned int rightJambWidthX, unsigned int rightJambWidthY,
                                        const std::string& at )
{
        bool ns = ( at == "north" || at == "south" || at == "northeast" || at == "northwest" || at == "southeast" || at == "southwest" );

        unsigned int lintelWidth = ( widthX << 1 ) + ( widthY << 1 );
        unsigned int lintelHeight = height + widthY + widthX;

        NamedPicture * lintel = new NamedPicture( lintelWidth, lintelHeight );

        if ( ns )
        {
                allegro::bitBlit( door, lintel->getAllegroPict(), 0, 0, 0, 0, lintelWidth, height + widthX );

                unsigned int delta = lintelWidth;
                int noPixel = lintelWidth - ( ( rightJambWidthX + rightJambWidthY ) << 1 ) + 1;
                int yStart = noPixel;
                int yEnd = noPixel - 1;

                /////lintel->getAllegroPict().lockWriteOnly() ;

                for ( unsigned int yPic = height + widthX; yPic < lintelHeight; yPic++ ) {
                        for ( unsigned int xPic = delta; xPic > 0; xPic-- )
                        {
                                if ( yPic != height + widthX && ( static_cast< int >( xPic ) - 1 ) == noPixel )
                                {
                                        if ( noPixel > yEnd ) {
                                                noPixel -- ;
                                        }
                                        else {
                                                yStart += 2 ;
                                                noPixel = yStart;
                                        }
                                }
                                else if ( yPic < height + ( widthX << 1 ) || ( static_cast< int >( xPic ) - 1 ) < yEnd )
                                {
                                        lintel->getAllegroPict().putPixelAt( xPic - 1, yPic, door.getPixelAt( xPic - 1, yPic ) );
                                }
                        }

                        delta -= 2;
                }

                /////lintel->getAllegroPict().unlock() ;
        }
        else
        {
                allegro::bitBlit( door, lintel->getAllegroPict(), 0, 0, 0, 0, lintelWidth, height + widthY );

                unsigned int delta = 0;
                int noPixel = ( ( leftJambWidthX + leftJambWidthY ) << 1 ) - 2;
                int yStart = noPixel;
                int yEnd = noPixel + 1;

                /////lintel->getAllegroPict().lockWriteOnly() ;

                for ( unsigned int yPic = height + widthY; yPic < lintelHeight; yPic++ ) {
                        for ( unsigned int xPic = delta; xPic < lintelWidth; xPic++ )
                        {
                                if ( yPic != height + widthY && static_cast< int >( xPic ) == noPixel )
                                {
                                        if ( noPixel < yEnd ) {
                                                noPixel ++ ;
                                        }
                                        else {
                                                yStart -= 2 ;
                                                noPixel = yStart;
                                        }
                                }
                                else if ( yPic < height + ( widthY << 1 ) || static_cast< int >( xPic ) > yEnd )
                                {
                                        lintel->getAllegroPict().putPixelAt( xPic, yPic, door.getPixelAt( xPic, yPic ) );
                                }
                        }

                        delta += 2;
                }

                /////lintel->getAllegroPict().unlock() ;
        }

        return lintel ;
}

/* static */
NamedPicture* Door::cutOutLeftJamb( const allegro::Pict& door, unsigned int widthX, unsigned int widthY, unsigned int height,
                                        /* unsigned int lintelWidthX, */ unsigned int lintelWidthY, unsigned int lintelHeight,
                                        const std::string& at )
{
        bool ns = ( at == "north" || at == "south" || at == "northeast" || at == "northwest" || at == "southeast" || at == "southwest" );
        unsigned int fixWidth = ( ns ? 7 : 0 );
        int fixY = ( ns ? -1 : 0 );

        NamedPicture * leftJamb = new NamedPicture( ( widthX << 1 ) + fixWidth + ( widthY << 1 ) , height + widthY + widthX ) ;

        allegro::bitBlit( door, leftJamb->getAllegroPict(),
                                fixY, lintelHeight + lintelWidthY - widthY + fixY,
                                0, 0,
                                leftJamb->getWidth(), leftJamb->getHeight() );

# if defined( GRAYSCALE_JAMBS ) && GRAYSCALE_JAMBS
        Color::pictureToGrayscale( leftJamb );
# endif

        return leftJamb ;
}

/* static */
NamedPicture* Door::cutOutRightJamb( const allegro::Pict& door, unsigned int widthX, unsigned int widthY, unsigned int height,
                                        unsigned int lintelWidthX, /* unsigned int lintelWidthY, */ unsigned int lintelHeight,
                                        const std::string& at )
{
        bool ns = ( at == "north" || at == "south" || at == "northeast" || at == "northwest" || at == "southeast" || at == "southwest" );
        unsigned int fixWidth = ( ns ? 0 : 7 );
        int fixY = ( ns ? 0 : -2 );

        NamedPicture * rightJamb = new NamedPicture( ( widthX << 1 ) + fixWidth + ( widthY << 1 ) , height + widthY + widthX ) ;

        allegro::bitBlit( door, rightJamb->getAllegroPict(),
                                door.getW() - rightJamb->getWidth(), lintelHeight + lintelWidthX - widthY + fixY,
                                0, 0,
                                rightJamb->getWidth(), rightJamb->getHeight() );

# if defined( GRAYSCALE_JAMBS ) && GRAYSCALE_JAMBS
        Color::pictureToGrayscale( rightJamb );
# endif

        return rightJamb ;
}

const FreeItemPtr & Door::getLeftJamb()
{
        if ( this->leftJamb == nilPointer )
        {
                if ( this->leftJambImage == nilPointer )
                        throw MayNotBePossible( "nil image for the left jamb of " + getKind() );

                const DescriptionOfItem & whatIsLeftJamb = * ItemDescriptions::descriptions ().getDescriptionByKind( kindOfDoor + "~leftjamb" );

                int oneTile = getMediator()->getRoom()->getSizeOfOneTile ();

                int x = 0 ; int y = 0 ;

                switch ( Way( getWhereIsDoor() ).getIntegerOfWay() )
                {
                        case Way::North:
                        case Way::Northeast:
                        case Way::Northwest:
                                x = cellX * oneTile + whatIsLeftJamb.getWidthX() - 2 ;
                                y = ( cellY + 2 ) * oneTile - 2 ;
                                this->leftLimit = y + whatIsLeftJamb.getWidthY() ;
                                break;

                        case Way::South:
                        case Way::Southeast:
                        case Way::Southwest:
                                x = cellX * oneTile ;
                                y = ( cellY + 2 ) * oneTile - 2 ;
                                this->leftLimit = y + whatIsLeftJamb.getWidthY() ;
                                break;

                        case Way::East:
                        case Way::Eastnorth:
                        case Way::Eastsouth:
                                x = cellX * oneTile ;
                                y = ( cellY + 1 ) * oneTile - 1 ;
                                this->leftLimit = x + whatIsLeftJamb.getWidthX() ;
                                break;

                        case Way::West:
                        case Way::Westnorth:
                        case Way::Westsouth:
                                x = cellX * oneTile ;
                                y = ( cellY + 1 ) * oneTile - whatIsLeftJamb.getWidthY() + 1 ;
                                this->leftLimit = x + whatIsLeftJamb.getWidthX() ;
                                break;

                        default:
                                ;
                }

                leftJamb = FreeItemPtr( new FreeItem( whatIsLeftJamb, x, y, Room::FloorZ, onWhichSideOfTheFour() ) );
                leftJamb->addFrameTo( onWhichSideOfTheFour(), new NamedPicture( leftJambImage->getWidth(), leftJambImage->getHeight() ) );
                allegro::bitBlit( leftJambImage->getAllegroPict(), leftJamb->getCurrentRawImageToChangeIt ().getAllegroPict() );
                leftJamb->getCurrentRawImageToChangeIt().setName( leftJambImage->getName() );
                leftJamb->freshBothProcessedImages ();
                leftJamb->setUniqueName( leftJamb->getKind () + " " + util::makeRandomString( 8 ) );
        }

        return this->leftJamb ;
}

const FreeItemPtr & Door::getRightJamb()
{
        if ( this->rightJamb == nilPointer )
        {
                if ( this->rightJambImage == nilPointer )
                        throw MayNotBePossible( "nil image for the right jamb of " + getKind() );

                int oneTile = getMediator()->getRoom()->getSizeOfOneTile ();

                const DescriptionOfItem & whatIsRightJamb = * ItemDescriptions::descriptions ().getDescriptionByKind( kindOfDoor + "~rightjamb" );

                int x = 0 ; int y = 0 ;

                switch ( Way( getWhereIsDoor() ).getIntegerOfWay() )
                {
                        case Way::North:
                        case Way::Northeast:
                        case Way::Northwest:
                                x = cellX * oneTile + whatIsRightJamb.getWidthX() - 2 ;
                                y = cellY * oneTile + whatIsRightJamb.getWidthY() - 1 ;
                                this->rightLimit = y ;
                                break;

                        case Way::South:
                        case Way::Southeast:
                        case Way::Southwest:
                                x = cellX * oneTile ;
                                y = cellY * oneTile + whatIsRightJamb.getWidthY() - 1 ;
                                this->rightLimit = y ;
                                break;

                        case Way::East:
                        case Way::Eastnorth:
                        case Way::Eastsouth:
                                x = ( cellX + 2 ) * oneTile - whatIsRightJamb.getWidthX() - 2 ;
                                y = ( cellY + 1 ) * oneTile - 1 ;
                                this->rightLimit = x ;
                                break;

                        case Way::West:
                        case Way::Westnorth:
                        case Way::Westsouth:
                                x = ( cellX + 2 ) * oneTile - whatIsRightJamb.getWidthX() - 2 ;
                                y = ( cellY + 1 ) * oneTile - whatIsRightJamb.getWidthY() + 1 ;
                                this->rightLimit = x ;
                                break;

                        default:
                                ;
                }

                rightJamb = FreeItemPtr( new FreeItem( whatIsRightJamb, x, y, Room::FloorZ, onWhichSideOfTheFour() ) );
                rightJamb->addFrameTo( onWhichSideOfTheFour(), new NamedPicture( rightJambImage->getWidth(), rightJambImage->getHeight() ) );
                allegro::bitBlit( rightJambImage->getAllegroPict(), rightJamb->getCurrentRawImageToChangeIt ().getAllegroPict() );
                rightJamb->getCurrentRawImageToChangeIt().setName( rightJambImage->getName() );
                rightJamb->freshBothProcessedImages ();
                rightJamb->setUniqueName( rightJamb->getKind () + " " + util::makeRandomString( 8 ) );
        }

        return this->rightJamb ;
}

const FreeItemPtr & Door::getLintel()
{
        if ( this->lintel == nilPointer )
        {
                if ( this->lintelImage == nilPointer )
                        throw MayNotBePossible( "nil image for the lintel of " + getKind() );

                int oneTile = getMediator()->getRoom()->getSizeOfOneTile ();

                const DescriptionOfItem & whatIsLintel = * ItemDescriptions::descriptions ().getDescriptionByKind( kindOfDoor + "~lintel" );

                int x( 0 ), y( 0 );

                switch ( Way( getWhereIsDoor() ).getIntegerOfWay() )
                {
                        case Way::North:
                        case Way::Northeast:
                        case Way::Northwest:
                                x = cellX * oneTile + whatIsLintel.getWidthX() - 2 ;
                                y = ( cellY + 2 ) * oneTile - 1 ;
                                break;

                        case Way::South:
                        case Way::Southeast:
                        case Way::Southwest:
                                x = cellX * oneTile ;
                                y = ( cellY + 2 ) * oneTile - 1 ;
                                break;

                        case Way::East:
                        case Way::Eastnorth:
                        case Way::Eastsouth:
                                x = cellX * oneTile ;
                                y = ( cellY + 1 ) * oneTile - 1 ;
                                break;

                        case Way::West:
                        case Way::Westnorth:
                        case Way::Westsouth:
                                x = cellX * oneTile ;
                                y = ( cellY + 1 ) * oneTile - whatIsLintel.getWidthY() + 1 ;
                                break;

                        default:
                                ;
                }

                lintel = FreeItemPtr( new FreeItem( whatIsLintel, x, y, Room::FloorZ, onWhichSideOfTheFour() ) );
                lintel->addFrameTo( onWhichSideOfTheFour(), new NamedPicture( lintelImage->getWidth(), lintelImage->getHeight() ) );
                allegro::bitBlit( lintelImage->getAllegroPict(), lintel->getCurrentRawImageToChangeIt ().getAllegroPict() );
                lintel->getCurrentRawImageToChangeIt().setName( lintelImage->getName() );
                lintel->freshBothProcessedImages ();
                lintel->setUniqueName( lintel->getKind () + " " + util::makeRandomString( 8 ) );
        }

        return this->lintel ;
}

bool Door::isUnderDoor( int x, int y, int z ) const
{
        if ( z < 0 ) z = 0 ;

        bool under = ( leftJamb->getZ() <= z && z <= lintel->getZ() );

        const int oneCell = getMediator()->getRoom()->getSizeOfOneTile ();

        switch ( Way( getWhereIsDoor() ).getIntegerOfWay() )
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
