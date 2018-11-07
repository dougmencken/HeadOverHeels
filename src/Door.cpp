
#include "Door.hpp"
#include "Color.hpp"
#include "ItemDataManager.hpp"
#include "FreeItem.hpp"
#include "Mediator.hpp"
#include "GameManager.hpp"

#ifdef DEBUG
#  define SAVE_ITEM_FRAMES      0
#  define GRAYSCALE_JAMBS       0
#endif


namespace iso
{

Door::Door( const ItemDataManager& itemDataManager, const std::string& label, int cx, int cy, int z, const std::string& way )
        : itemDataManager( itemDataManager )
        , labelOfDoor( label )
        , cellX( cx )
        , cellY( cy )
        , z( z )
        , positionOfDoor( way )
        , leftJambImage( nilPointer )
        , rightJambImage( nilPointer )
        , lintelImage( nilPointer )
        , leftJamb( nilPointer )
        , rightJamb( nilPointer )
        , lintel( nilPointer )
{
        /* std::cout << "creation of door \"" << labelOfDoor << "\"" << std::endl ; */

        const ItemData* lintelData = itemDataManager.findDataByLabel( labelOfDoor + "~lintel" );

        // load graphics of door

        autouniqueptr< allegro::Pict > pictureOfDoor( allegro::Pict::fromPNGFile( iso::pathToFile(
                iso::sharePath() + iso::GameManager::getInstance().getChosenGraphicSet(), lintelData->getNameOfFile( )
        ) ) );
        if ( ! pictureOfDoor->isNotNil() )
        {
                std::cerr <<
                        "picture of door \"" << lintelData->getNameOfFile( ) <<
                        "\" for \"" << iso::GameManager::getInstance().getChosenGraphicSet() <<
                        "\" is absent" << std::endl ;
                return ;
        }

        std::string scenery = labelOfDoor.substr( 0, labelOfDoor.find( "-" ) );

        // cut out left jamb

        const ItemData* leftJambData = itemDataManager.findDataByLabel( labelOfDoor + "~leftjamb" );

        leftJambImage = cutOutLeftJamb( *pictureOfDoor.get(),
                                        leftJambData->getWidthX(), leftJambData->getWidthY(), leftJambData->getHeight(),
                                        lintelData->getWidthY(), lintelData->getHeight(),
                                        way );
        leftJambImage->setName( "left jamb of door for " + scenery + " at " + way );

        // cut out right jamb

        const ItemData* rightJambData = itemDataManager.findDataByLabel( labelOfDoor + "~rightjamb" );

        rightJambImage = cutOutRightJamb( *pictureOfDoor.get(),
                                        rightJambData->getWidthX(), rightJambData->getWidthY(), rightJambData->getHeight(),
                                        lintelData->getWidthX(), lintelData->getHeight(),
                                        way );
        rightJambImage->setName( "right jamb of door for " + scenery + " at " + way );

        // cut out lintel

        lintelImage = cutOutLintel( *pictureOfDoor.get(),
                                        lintelData->getWidthX(), lintelData->getWidthY(), lintelData->getHeight(),
                                        leftJambData->getWidthX(), leftJambData->getWidthY(),
                                        rightJambData->getWidthX(), rightJambData->getWidthY(),
                                        way );
        lintelImage->setName( "lintel of door for " + scenery + " at " + way );

# if defined( SAVE_ITEM_FRAMES ) && SAVE_ITEM_FRAMES

        leftJambImage->saveAsPNG( iso::homePath() );
        rightJambImage->saveAsPNG( iso::homePath() );
        lintelImage->saveAsPNG( iso::homePath() );

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

                top->getAllegroPict().lock( false, true );

                for ( unsigned int yPic = height + widthX; yPic < topHeight; yPic++ )
                {
                        for ( unsigned int xPic = delta; xPic > 0; xPic-- )
                        {
                                if ( yPic != height + widthX && ( static_cast< int >( xPic ) - 1 ) == noPixel )
                                {
                                        if ( noPixel > yEnd )
                                        {
                                                noPixel--;
                                        }
                                        else
                                        {
                                                yStart += 2;
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

                top->getAllegroPict().unlock();
        }
        else
        {
                allegro::bitBlit( door, top->getAllegroPict(), 0, 0, 0, 0, topWidth, height + widthY );

                unsigned int delta = 0;
                int noPixel = ( ( leftJambWidthX + leftJambWidthY ) << 1 ) - 2;
                int yStart = noPixel;
                int yEnd = noPixel + 1;

                top->getAllegroPict().lock( false, true );

                for ( unsigned int yPic = height + widthY; yPic < topHeight; yPic++ )
                {
                        for ( unsigned int xPic = delta; xPic < topWidth; xPic++ )
                        {
                                if ( yPic != height + widthY && static_cast< int >( xPic ) == noPixel )
                                {
                                        if ( noPixel < yEnd )
                                        {
                                                noPixel++;
                                        }
                                        else
                                        {
                                                yStart -= 2;
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

                top->getAllegroPict().unlock();
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
                const ItemData* leftJambData = itemDataManager.findDataByLabel( labelOfDoor + "~leftjamb" );

                int tileSize = mediator->getRoom()->getSizeOfOneTile();

                if ( leftJambData != nilPointer && leftJambImage != nilPointer )
                {
                        int x = 0 ; int y = 0 ;

                        switch ( Way( getWhereIsDoor() ).getIntegerOfWay() )
                        {
                                case Way::North:
                                case Way::Northeast:
                                case Way::Northwest:
                                        x = cellX * tileSize + leftJambData->getWidthX() - 2;
                                        y = ( cellY + 2 ) * tileSize - 2;
                                        leftLimit = y + leftJambData->getWidthY();
                                        break;

                                case Way::South:
                                case Way::Southeast:
                                case Way::Southwest:
                                        x = cellX * tileSize;
                                        y = ( cellY + 2 ) * tileSize - 2;
                                        leftLimit = y + leftJambData->getWidthY();
                                        break;

                                case Way::East:
                                case Way::Eastnorth:
                                case Way::Eastsouth:
                                        x = cellX * tileSize;
                                        y = ( cellY + 1 ) * tileSize - 1;
                                        leftLimit = x + leftJambData->getWidthX();
                                        break;

                                case Way::West:
                                case Way::Westnorth:
                                case Way::Westsouth:
                                        x = cellX * tileSize;
                                        y = ( cellY + 1 ) * tileSize - leftJambData->getWidthY() + 1;
                                        leftLimit = x + leftJambData->getWidthX();
                                        break;

                                default:
                                        ;
                        }

                        leftJamb = FreeItemPtr( new FreeItem( leftJambData, x, y, Top, Way::Nowhere ) );
                        leftJamb->clearMotionFrames();



                        leftJamb->addFrame( *leftJambImage );
                        leftJamb->setUniqueName( leftJamb->getLabel() + " " + makeRandomString( 8 ) );
                }
        }

        return leftJamb;
}

FreeItemPtr Door::getRightJamb()
{
        if ( rightJamb == nilPointer )
        {
                int x( 0 ), y( 0 );
                int tileSize = mediator->getRoom()->getSizeOfOneTile();

                const ItemData* rightJambData = itemDataManager.findDataByLabel( labelOfDoor + "~rightjamb" );

                if ( rightJambData != nilPointer && rightJambImage != nilPointer )
                {
                        switch ( Way( getWhereIsDoor() ).getIntegerOfWay() )
                        {
                                case Way::North:
                                case Way::Northeast:
                                case Way::Northwest:
                                        x = cellX * tileSize + rightJambData->getWidthX() - 2;
                                        y = cellY * tileSize + rightJambData->getWidthY() - 1;
                                        rightLimit = y;
                                        break;

                                case Way::South:
                                case Way::Southeast:
                                case Way::Southwest:
                                        x = cellX * tileSize;
                                        y = cellY * tileSize + rightJambData->getWidthY() - 1;
                                        rightLimit = y;
                                        break;

                                case Way::East:
                                case Way::Eastnorth:
                                case Way::Eastsouth:
                                        x = ( cellX + 2 ) * tileSize - rightJambData->getWidthX() - 2;
                                        y = ( cellY + 1 ) * tileSize - 1;
                                        rightLimit = x;
                                        break;

                                case Way::West:
                                case Way::Westnorth:
                                case Way::Westsouth:
                                        x = ( cellX + 2 ) * tileSize - rightJambData->getWidthX() - 2;
                                        y = ( cellY + 1 ) * tileSize - rightJambData->getWidthY() + 1;
                                        rightLimit = x;
                                        break;

                                default:
                                        ;
                        }

                        rightJamb = FreeItemPtr( new FreeItem( rightJambData, x, y, Top, Way::Nowhere ) );
                        rightJamb->clearMotionFrames();
                        rightJamb->addFrame( *rightJambImage );
                        rightJamb->setUniqueName( rightJamb->getLabel() + " " + makeRandomString( 8 ) );
                }
        }

        return rightJamb;
}

FreeItemPtr Door::getLintel()
{
        if ( lintel == nilPointer )
        {
                int x( 0 ), y( 0 );
                int tileSize = mediator->getRoom()->getSizeOfOneTile();

                const ItemData* lintelData = itemDataManager.findDataByLabel( labelOfDoor + "~lintel" );

                if ( lintelData != nilPointer && lintelImage != nilPointer )
                {
                        switch ( Way( getWhereIsDoor() ).getIntegerOfWay() )
                        {
                                case Way::North:
                                case Way::Northeast:
                                case Way::Northwest:
                                        x = cellX * tileSize + lintelData->getWidthX() - 2;
                                        y = ( cellY + 2 ) * tileSize - 1;
                                        break;

                                case Way::South:
                                case Way::Southeast:
                                case Way::Southwest:
                                        x = cellX * tileSize;
                                        y = ( cellY + 2 ) * tileSize - 1;
                                        break;

                                case Way::East:
                                case Way::Eastnorth:
                                case Way::Eastsouth:
                                        x = cellX * tileSize;
                                        y = ( cellY + 1 ) * tileSize - 1;
                                        break;

                                case Way::West:
                                case Way::Westnorth:
                                case Way::Westsouth:
                                        x = cellX * tileSize;
                                        y = ( cellY + 1 ) * tileSize - lintelData->getWidthY() + 1;
                                        break;

                                default:
                                        ;
                        }

                        lintel = FreeItemPtr( new FreeItem( lintelData, x, y, Top, Way::Nowhere ) );
                        lintel->clearMotionFrames();
                        lintel->addFrame( *lintelImage );
                        lintel->setUniqueName( lintel->getLabel() + " " + makeRandomString( 8 ) );
                }
        }

        return lintel;
}

bool Door::isUnderDoor( int x, int y, int z ) const
{
        bool result = false;

        if ( z < 0 ) z = 0;

        switch ( Way( getWhereIsDoor() ).getIntegerOfWay() )
        {
                case Way::North:
                case Way::Northeast:
                case Way::Northwest:
                case Way::South:
                case Way::Southeast:
                case Way::Southwest:
                        result = ( y >= rightLimit && y <= leftLimit && z >= leftJamb->getZ () && z <= lintel->getZ () );
                        break;

                case Way::East:
                case Way::Eastnorth:
                case Way::Eastsouth:
                case Way::West:
                case Way::Westnorth:
                case Way::Westsouth:
                        result = ( x <= rightLimit && x >= leftLimit && z >= leftJamb->getZ () && z <= lintel->getZ () );
                        break;

                default:
                        ;
        }

        return result;
}

}
