
#include "Door.hpp"

#include "Color.hpp"
#include "DescriptionOfItem.hpp"
#include "ItemDescriptions.hpp"
#include "FreeItem.hpp"
#include "Mediator.hpp"
#include "GameManager.hpp"

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
        , z( z )
        , whereIsDoor( where )
        , leftJambImage( nilPointer )
        , rightJambImage( nilPointer )
        , lintelImage( nilPointer )
        , leftJamb( nilPointer )
        , rightJamb( nilPointer )
        , lintel( nilPointer )
{
        const DescriptionOfItem & whatIsLintel = * ItemDescriptions::descriptions ().getDescriptionByKind( kindOfDoor + "~lintel" );

        // load the graphics of door

        const std::string & graphicsOfDoor = whatIsLintel.getNameOfPicturesFile () ;
        const std::string & chosenSet = GameManager::getInstance().getChosenGraphicsSet() ;

        autouniqueptr< allegro::Pict > pictureOfDoor( allegro::Pict::fromPNGFile(
                ospaths::pathToFile( ospaths::sharePath() + chosenSet, graphicsOfDoor )
        ) );
        if ( ! pictureOfDoor->isNotNil() ) {
                std::cerr << "the graphics of door \"" << graphicsOfDoor << "\""
                                << " from set \"" << chosenSet << "\" is absent" << std::endl ;
                return ;
        }

        std::string scenery = kindOfDoor.substr( 0, kindOfDoor.find( "-" ) );

        // cut out the left jamb

        const DescriptionOfItem & whatIsLeftJamb = * ItemDescriptions::descriptions ().getDescriptionByKind( kindOfDoor + "~leftjamb" );

        leftJambImage = cutOutLeftJamb( *pictureOfDoor,
                                        whatIsLeftJamb.getWidthX(), whatIsLeftJamb.getWidthY(), whatIsLeftJamb.getHeight(),
                                        whatIsLintel.getWidthY(), whatIsLintel.getHeight(),
                                        where );
        leftJambImage->setName( "left jamb of door in " + scenery + " at " + where );

        // cut out the right jamb

        const DescriptionOfItem & whatIsRightJamb = * ItemDescriptions::descriptions ().getDescriptionByKind( kindOfDoor + "~rightjamb" );

        rightJambImage = cutOutRightJamb( *pictureOfDoor,
                                        whatIsRightJamb.getWidthX(), whatIsRightJamb.getWidthY(), whatIsRightJamb.getHeight(),
                                        whatIsLintel.getWidthX(), whatIsLintel.getHeight(),
                                        where );
        rightJambImage->setName( "right jamb of door in " + scenery + " at " + where );

        // cut out the lintel

        lintelImage = cutOutLintel( *pictureOfDoor,
                                        whatIsLintel.getWidthX(), whatIsLintel.getWidthY(), whatIsLintel.getHeight(),
                                        whatIsLeftJamb.getWidthX(), whatIsLeftJamb.getWidthY(),
                                        whatIsRightJamb.getWidthX(), whatIsRightJamb.getWidthY(),
                                        where );
        lintelImage->setName( "lintel of door in " + scenery + " at " + where );

# if defined( SAVE_ITEM_FRAMES ) && SAVE_ITEM_FRAMES

        leftJambImage->saveAsPNG( ospaths::homePath() );
        rightJambImage->saveAsPNG( ospaths::homePath() );
        lintelImage->saveAsPNG( ospaths::homePath() );

# endif
}

Door::~Door()
{
        delete leftJambImage ;
        delete rightJambImage ;
        delete lintelImage ;
}

/* static */
Picture* Door::cutOutLintel( const allegro::Pict& door, unsigned int widthX, unsigned int widthY, unsigned int height,
                                        unsigned int leftJambWidthX, unsigned int leftJambWidthY,
                                        unsigned int rightJambWidthX, unsigned int rightJambWidthY,
                                        const std::string& at )
{
        bool ns = ( at == "north" || at == "south" || at == "northeast" || at == "northwest" || at == "southeast" || at == "southwest" );

        unsigned int topWidth = ( widthX << 1 ) + ( widthY << 1 );
        unsigned int topHeight = height + widthY + widthX;

        Picture* top = new Picture( topWidth, topHeight );

        if ( ns )
        {
                allegro::bitBlit( door, top->getAllegroPict(), 0, 0, 0, 0, topWidth, height + widthX );

                unsigned int delta = topWidth;
                int noPixel = topWidth - ( ( rightJambWidthX + rightJambWidthY ) << 1 ) + 1;
                int yStart = noPixel;
                int yEnd = noPixel - 1;

                /////top->getAllegroPict().lockWriteOnly() ;

                for ( unsigned int yPic = height + widthX; yPic < topHeight; yPic++ ) {
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
                                        top->getAllegroPict().putPixelAt( xPic - 1, yPic, door.getPixelAt( xPic - 1, yPic ) );
                                }
                        }

                        delta -= 2;
                }

                /////top->getAllegroPict().unlock() ;
        }
        else
        {
                allegro::bitBlit( door, top->getAllegroPict(), 0, 0, 0, 0, topWidth, height + widthY );

                unsigned int delta = 0;
                int noPixel = ( ( leftJambWidthX + leftJambWidthY ) << 1 ) - 2;
                int yStart = noPixel;
                int yEnd = noPixel + 1;

                /////top->getAllegroPict().lockWriteOnly() ;

                for ( unsigned int yPic = height + widthY; yPic < topHeight; yPic++ ) {
                        for ( unsigned int xPic = delta; xPic < topWidth; xPic++ )
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
                                        top->getAllegroPict().putPixelAt( xPic, yPic, door.getPixelAt( xPic, yPic ) );
                                }
                        }

                        delta += 2;
                }

                /////top->getAllegroPict().unlock() ;
        }

        return top;
}

/* static */
Picture* Door::cutOutLeftJamb( const allegro::Pict& door, unsigned int widthX, unsigned int widthY, unsigned int height,
                                        /* unsigned int lintelWidthX, */ unsigned int lintelWidthY, unsigned int lintelHeight,
                                        const std::string& at )
{
        bool ns = ( at == "north" || at == "south" || at == "northeast" || at == "northwest" || at == "southeast" || at == "southwest" );
        unsigned int fixWidth = ( ns ? 7 : 0 );
        int fixY = ( ns ? -1 : 0 );

        Picture* left = new Picture( ( widthX << 1 ) + fixWidth + ( widthY << 1 ) , height + widthY + widthX ) ;

        allegro::bitBlit( door, left->getAllegroPict(), fixY, lintelHeight + lintelWidthY - widthY + fixY, 0, 0, left->getWidth(), left->getHeight() );

# if defined( GRAYSCALE_JAMBS ) && GRAYSCALE_JAMBS
        Color::pictureToGrayscale( left );
# endif

        return left;
}

/* static */
Picture* Door::cutOutRightJamb( const allegro::Pict& door, unsigned int widthX, unsigned int widthY, unsigned int height,
                                        unsigned int lintelWidthX, /* unsigned int lintelWidthY, */ unsigned int lintelHeight,
                                        const std::string& at )
{
        bool ns = ( at == "north" || at == "south" || at == "northeast" || at == "northwest" || at == "southeast" || at == "southwest" );
        unsigned int fixWidth = ( ns ? 0 : 7 );
        int fixY = ( ns ? 0 : -2 );

        Picture* right = new Picture( ( widthX << 1 ) + fixWidth + ( widthY << 1 ) , height + widthY + widthX ) ;

        allegro::bitBlit( door, right->getAllegroPict(), door.getW() - right->getWidth(), lintelHeight + lintelWidthX - widthY + fixY, 0, 0, right->getWidth(), right->getHeight() );

# if defined( GRAYSCALE_JAMBS ) && GRAYSCALE_JAMBS
        Color::pictureToGrayscale( right );
# endif

        return right;
}

FreeItemPtr Door::getLeftJamb()
{
        if ( leftJamb == nilPointer )
        {
                const DescriptionOfItem & whatIsLeftJamb = * ItemDescriptions::descriptions ().getDescriptionByKind( kindOfDoor + "~leftjamb" );

                int oneTile = getMediator()->getRoom()->getSizeOfOneTile ();

                if ( leftJambImage != nilPointer )
                {
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

                        leftJamb = FreeItemPtr( new FreeItem( whatIsLeftJamb, x, y, Room::FloorZ, getWhereIsDoor() ) );
                        leftJamb->getRawImageToChangeIt().expandOrCropTo( leftJambImage->getWidth (), leftJambImage->getHeight () );
                        allegro::bitBlit( leftJambImage->getAllegroPict(), leftJamb->getRawImageToChangeIt ().getAllegroPict() );
                        leftJamb->getRawImageToChangeIt().setName( leftJambImage->getName() );
                        leftJamb->freshBothProcessedImages ();
                        leftJamb->setUniqueName( leftJamb->getKind () + " " + util::makeRandomString( 8 ) );
                }
        }

        return leftJamb;
}

FreeItemPtr Door::getRightJamb()
{
        if ( rightJamb == nilPointer )
        {
                int x( 0 ), y( 0 );
                int oneTile = getMediator()->getRoom()->getSizeOfOneTile ();

                const DescriptionOfItem & whatIsRightJamb = * ItemDescriptions::descriptions ().getDescriptionByKind( kindOfDoor + "~rightjamb" );

                if ( rightJambImage != nilPointer )
                {
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

                        rightJamb = FreeItemPtr( new FreeItem( whatIsRightJamb, x, y, Room::FloorZ, getWhereIsDoor() ) );
                        rightJamb->getRawImageToChangeIt().expandOrCropTo( rightJambImage->getWidth (), rightJambImage->getHeight () );
                        allegro::bitBlit( rightJambImage->getAllegroPict(), rightJamb->getRawImageToChangeIt ().getAllegroPict() );
                        rightJamb->getRawImageToChangeIt().setName( rightJambImage->getName() );
                        rightJamb->freshBothProcessedImages ();
                        rightJamb->setUniqueName( rightJamb->getKind () + " " + util::makeRandomString( 8 ) );
                }
        }

        return rightJamb;
}

FreeItemPtr Door::getLintel()
{
        if ( lintel == nilPointer )
        {
                int x( 0 ), y( 0 );
                int oneTile = getMediator()->getRoom()->getSizeOfOneTile ();

                const DescriptionOfItem & whatIsLintel = * ItemDescriptions::descriptions ().getDescriptionByKind( kindOfDoor + "~lintel" );

                if ( lintelImage != nilPointer )
                {
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

                        lintel = FreeItemPtr( new FreeItem( whatIsLintel, x, y, Room::FloorZ, getWhereIsDoor() ) );
                        lintel->getRawImageToChangeIt().expandOrCropTo( lintelImage->getWidth (), lintelImage->getHeight () );
                        allegro::bitBlit( lintelImage->getAllegroPict(), lintel->getRawImageToChangeIt ().getAllegroPict() );
                        lintel->getRawImageToChangeIt().setName( lintelImage->getName() );
                        lintel->freshBothProcessedImages ();
                        lintel->setUniqueName( lintel->getKind () + " " + util::makeRandomString( 8 ) );
                }
        }

        return lintel;
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
