
#include "Miniature.hpp"

#include "Color.hpp"
#include "FlickeringColor.hpp"
#include "GameManager.hpp"
#include "GameMap.hpp"
#include "ConnectedRooms.hpp"
#include "Mediator.hpp"
#include "DescriptionOfItem.hpp"
#include "UnlikelyToHappenException.hpp"

#include <sstream>

#ifdef DEBUG
#  define DEBUG_MINIATURES      1
#  define WRITE_MINIATURES      0
#endif

#if defined( WRITE_MINIATURES ) && WRITE_MINIATURES
#  include "ospaths.hpp"
#endif


Miniature::Miniature( const Room & roomForMiniature, unsigned short sizeOfSquare, bool withRoomInfo )
        : theImage( nilPointer )
        , room( roomForMiniature )
        , squareSize( Miniature::the_default_square_size )
        , withTextAboutRoom( withRoomInfo )
        , textAboutRoomIsWritten( false )
        , isThereRoomAbove( false )
        , isThereRoomBelow( false )
        , northDoorEasternCorner( Miniature::corner_not_set, Miniature::corner_not_set )
        , eastDoorNorthernCorner( Miniature::corner_not_set, Miniature::corner_not_set )
        , southDoorEasternCorner( Miniature::corner_not_set, Miniature::corner_not_set )
        , westDoorNorthernCorner( Miniature::corner_not_set, Miniature::corner_not_set )
{
        setSquareSize( sizeOfSquare );

        const ConnectedRooms * connections = this->room.getConnections() ;
        if ( connections != nilPointer ) {
                if ( ! connections->getConnectedRoomAt( "above" ).empty() )
                        this->isThereRoomAbove = true ;
                if ( ! connections->getConnectedRoomAt( "below" ).empty() )
                        this->isThereRoomBelow = true ;
        }

        setDrawingOffset( 0, 0 );

# if defined( DEBUG_MINIATURES ) && DEBUG_MINIATURES
        std::cout << "constructed Miniature( room \"" << roomForMiniature.getNameOfRoomDescriptionFile() << "\""
                        << ", sizeOfSquare=" << sizeOfSquare
                        << ", withRoomInfo=" << ( withRoomInfo ? "true" : "false" )
                        << " )" << std::endl ;
# endif
}

/* protected */
std::pair < unsigned int, unsigned int > Miniature::calculateSize () const
{
        const unsigned int cellsX = this->room.getCellsAlongX ();
        const unsigned int cellsY = this->room.getCellsAlongY ();

        unsigned int height = ( cellsX + cellsY ) * getSquareSize() ;
        unsigned int width = height << 1 ;

        if ( this->isThereRoomAbove ) height += ( getSquareSize() << 2 );
        if ( this->isThereRoomBelow ) height += ( getSquareSize() << 2 );

        if ( this->withTextAboutRoom ) {
                width += Miniature::room_info_shift_x ;
                height += Miniature::room_info_shift_y ;

                const std::string & roomFile = this->room.getNameOfRoomDescriptionFile() ;
                std::string whichRoom = util::stringEndsWith( roomFile, ".xml" ) ? roomFile.substr( 0, roomFile.length() - 4 ) : roomFile ;
                unsigned int roomFileStringWidth = whichRoom.length() * /* width of a single letter */ 8 ;

                if ( roomFileStringWidth > width ) width = roomFileStringWidth ;
        }

        return std::pair< unsigned int, unsigned int >( width, height ) ;
}

/* protected */
void Miniature::createImage ()
{
        if ( this->theImage == nilPointer ) {
                std::pair < unsigned int, unsigned int > dimensions = calculateSize() ;
                this->theImage = new NamedPicture( dimensions.first, dimensions.second );

                std::ostringstream theNameOfMiniature ;
                theNameOfMiniature << "Miniature of room " << this->room.getNameOfRoomDescriptionFile() << " with " << getSquareSize() << "-pixel squares" ;
                this->theImage->setName( theNameOfMiniature.str () );

        # if defined( DEBUG_MINIATURES ) && DEBUG_MINIATURES
                std::cout << "\"" << this->theImage->getName() << "\""
                                << " has a size of " << this->theImage->getWidth() << " x " << this->theImage->getHeight()
                                << std::endl ;
        # endif
        }
}

/* protected */
void Miniature::composeImage ()
{
        if ( this->theImage == nilPointer ) createImage() ;

        const unsigned int cellsX = this->room.getCellsAlongX() ;
        const unsigned int cellsY = this->room.getCellsAlongY() ;

        unsigned int firstCellX = 0 ;
        unsigned int firstCellY = 0 ;
        unsigned int lastCellX = cellsX - 1 ;
        unsigned int lastCellY = cellsY - 1 ;

        std::map< std::string, Door* > doors ;

        static std::string sides [] = { "south", "west", "north", "east" };
        for ( unsigned int s = 0 ; s < 4 ; ++ s ) {
                const std::string & side = sides[ s ] ;
                doors[ side ] = getRoom().getDoorOn( side );
        }

        static std::string bigroomsides [] = { "northeast", "northwest", "eastnorth", "eastsouth",
                                                        "southeast", "southwest", "westnorth", "westsouth" };
        for ( unsigned int s = 0 ; s < 8 ; ++ s ) {
                const std::string & side = bigroomsides[ s ] ;
                doors[ side ] = getRoom().getDoorOn( side );
        }

        if ( doors[ "north" ] != nilPointer || doors[ "northeast" ] != nilPointer || doors[ "northwest" ] != nilPointer )
                firstCellX ++ ;

        if ( doors[ "east" ] != nilPointer || doors[ "eastnorth" ] != nilPointer || doors[ "eastsouth" ] != nilPointer )
                firstCellY ++ ;

        if ( doors[ "south" ] != nilPointer || doors[ "southeast" ] != nilPointer || doors[ "southwest" ] != nilPointer )
                -- lastCellX ;

        if ( doors[ "west" ] != nilPointer || doors[ "westnorth" ] != nilPointer || doors[ "westsouth" ] != nilPointer )
                -- lastCellY ;

        bool narrowRoomAlongX = ( lastCellY == firstCellY + 1 ) ;
        bool narrowRoomAlongY = ( lastCellX == firstCellX + 1 ) ;

        const Color & roomColor = Color::byName( room.getColor () );

        const allegro::Pict & toDrawHere = this->theImage->getAllegroPict ();

        // draw doors

        if ( doors[ "east" ] != nilPointer && ! narrowRoomAlongY )
        {
                unsigned int eastDoorXmid = doors[ "east" ]->getCellX() + 1 ;
                drawEastDoorOnMiniature( toDrawHere,
                        eastDoorXmid, doors[ "east" ]->getCellY(),
                        roomColor.multiply( Color::whiteColor() ) );
        }
        if ( doors[ "eastnorth" ] != nilPointer )
        {
                unsigned int eastnorthDoorXmid = doors[ "eastnorth" ]->getCellX() + 1 ;
                drawEastDoorOnMiniature( toDrawHere,
                        eastnorthDoorXmid, doors[ "eastnorth" ]->getCellY(),
                        roomColor.multiply( Color::whiteColor() ) );
        }
        if ( doors[ "eastsouth" ] != nilPointer )
        {
                unsigned int eastsouthDoorXmid = doors[ "eastsouth" ]->getCellX() + 1 ;
                drawEastDoorOnMiniature( toDrawHere,
                        eastsouthDoorXmid, doors[ "eastsouth" ]->getCellY(),
                        roomColor.multiply( Color::whiteColor() ) );
        }

        if ( doors[ "north" ] != nilPointer && ! narrowRoomAlongX )
        {
                unsigned int northDoorYmid = doors[ "north" ]->getCellY() + 1 ;
                drawNorthDoorOnMiniature( toDrawHere,
                        doors[ "north" ]->getCellX(), northDoorYmid,
                        roomColor.multiply( Color::whiteColor() ) );
        }
        if ( doors[ "northeast" ] != nilPointer )
        {
                unsigned int northeastDoorYmid = doors[ "northeast" ]->getCellY() + 1 ;
                drawNorthDoorOnMiniature( toDrawHere,
                        doors[ "northeast" ]->getCellX(), northeastDoorYmid,
                        roomColor.multiply( Color::whiteColor() ) );
        }
        if ( doors[ "northwest" ] != nilPointer )
        {
                unsigned int northwestDoorYmid = doors[ "northwest" ]->getCellY() + 1 ;
                drawNorthDoorOnMiniature( toDrawHere,
                        doors[ "northwest" ]->getCellX(), northwestDoorYmid,
                        roomColor.multiply( Color::whiteColor() ) );
        }

        if ( doors[ "west" ] != nilPointer && ! narrowRoomAlongY )
        {
                unsigned int westDoorXmid = doors[ "west" ]->getCellX() + 1 ;
                drawWestDoorOnMiniature( toDrawHere,
                        westDoorXmid, doors[ "west" ]->getCellY(),
                        roomColor.multiply( Color::whiteColor() ) );
        }
        if ( doors[ "westnorth" ] != nilPointer )
        {
                unsigned int westnorthDoorXmid = doors[ "westnorth" ]->getCellX() + 1 ;
                drawWestDoorOnMiniature( toDrawHere,
                        westnorthDoorXmid, doors[ "westnorth" ]->getCellY(),
                        roomColor.multiply( Color::whiteColor() ) );
        }
        if ( doors[ "westsouth" ] != nilPointer )
        {
                unsigned int westsouthDoorXmid = doors[ "westsouth" ]->getCellX() + 1 ;
                drawWestDoorOnMiniature( toDrawHere,
                        westsouthDoorXmid, doors[ "westsouth" ]->getCellY(),
                        roomColor.multiply( Color::whiteColor() ) );
        }

        if ( doors[ "south" ] != nilPointer && ! narrowRoomAlongX )
        {
                unsigned int southDoorYmid = doors[ "south" ]->getCellY() + 1 ;
                drawSouthDoorOnMiniature( toDrawHere,
                        doors[ "south" ]->getCellX(), southDoorYmid,
                        roomColor.multiply( Color::whiteColor() ) );
        }
        if ( doors[ "southeast" ] != nilPointer )
        {
                unsigned int southeastDoorYmid = doors[ "southeast" ]->getCellY() + 1 ;
                drawSouthDoorOnMiniature( toDrawHere,
                        doors[ "southeast" ]->getCellX(), southeastDoorYmid,
                        roomColor.multiply( Color::whiteColor() ) );
        }
        if ( doors[ "southwest" ] != nilPointer )
        {
                unsigned int southwestDoorYmid = doors[ "southwest" ]->getCellY() + 1 ;
                drawSouthDoorOnMiniature( toDrawHere,
                        doors[ "southwest" ]->getCellX(), southwestDoorYmid,
                        roomColor.multiply( Color::whiteColor() ) );
        }

        // draw the boundaries of room

        drawBigIsoRectangle( toDrawHere, cellsX, cellsY, roomColor.multiply( Color::byName( "gray" ) ) );

        // draw walls

        int minXne = firstCellX ;
        int minYne = firstCellY ;
        int maxXsw = lastCellX ;
        int maxYsw = lastCellY ;

        const std::vector< std::vector< GridItemPtr > > & gridItemsInRoom = room.getGridItems() ;

        for ( unsigned int column = 0; column < gridItemsInRoom.size(); column ++ ) {
                for ( std::vector< GridItemPtr >::const_iterator gi = gridItemsInRoom[ column ].begin (); gi != gridItemsInRoom[ column ].end (); ++ gi )
                {
                        const GridItem & item = *( *gi ) ;

                        std::string kind = item.getKind ();
                        int cx = item.getCellX() ;
                        int cy = item.getCellY() ;

                        if ( kind == "invisible-wall-x" )
                        {
                                int newPos = cy - 1 ;
                                if ( newPos < maxYsw ) maxYsw = newPos ;
                        }
                        else if ( kind.find( "wall-x" ) != std::string::npos )
                        {
                                int newPos = cy + 1 ;
                                if ( newPos > minYne ) minYne = newPos ;
                        }

                        if ( kind == "invisible-wall-y" )
                        {
                                int newPos = cx - 1 ;
                                if ( newPos < maxXsw ) maxXsw = newPos ;
                        }
                        else if ( kind.find( "wall-y" ) != std::string::npos )
                        {
                                int newPos = cx + 1 ;
                                if ( newPos > minXne ) minXne = newPos ;
                        }
                }
        }

        const std::pair< int, int > & origin = getOriginOfRoom() ;

        for ( unsigned int cx_unsigned = firstCellX ; cx_unsigned <= lastCellX ; ++ cx_unsigned )
        {
                int cx = static_cast< int >( cx_unsigned );

                if ( cx >= minXne )
                {
                        if ( ! ( doors[ "east" ] != nilPointer
                                        && ( cx == doors[ "east" ]->getCellX() + 1 || cx == doors[ "east" ]->getCellX() ) ) &&
                                ! ( doors[ "eastnorth" ] != nilPointer
                                        && ( cx == doors[ "eastnorth" ]->getCellX() + 1 || cx == doors[ "eastnorth" ]->getCellX() ) ) &&
                                ! ( doors[ "eastsouth" ] != nilPointer
                                        && ( cx == doors[ "eastsouth" ]->getCellX() + 1 || cx == doors[ "eastsouth" ]->getCellX() ) ) )
                        {
                                drawIsoSquare (
                                        toDrawHere,
                                        cx_unsigned, firstCellY,
                                        roomColor.multiply( Color::whiteColor() ),
                                        /* loX */ true, false, false, false );
                        }
                }

                if ( cx <= maxXsw )
                {
                        if ( ! ( doors[ "west" ] != nilPointer
                                        && ( cx == doors[ "west" ]->getCellX() + 1 || cx == doors[ "west" ]->getCellX() ) ) &&
                                ! ( doors[ "westnorth" ] != nilPointer
                                        && ( cx == doors[ "westnorth" ]->getCellX() + 1 || cx == doors[ "westnorth" ]->getCellX() ) ) &&
                                ! ( doors[ "westsouth" ] != nilPointer
                                        && ( cx == doors[ "westsouth" ]->getCellX() + 1 || cx == doors[ "westsouth" ]->getCellX() ) ) )
                        {
                                drawIsoSquare (
                                        toDrawHere,
                                        cx_unsigned, lastCellY,
                                        roomColor.multiply( Color::whiteColor() ),
                                        false, false, /* hiX */ true, false );
                        }
                }

                if ( narrowRoomAlongX )
                {
                        if ( doors[ "north" ] != nilPointer )
                        {
                                drawIsoSquare ( toDrawHere, 0, firstCellY,
                                                roomColor.multiply( Color::byName( "gray 75% white" ) ),
                                                /* loX */ true, false, false, false );

                                drawIsoSquare ( toDrawHere, 0, lastCellY,
                                                roomColor.multiply( Color::byName( "gray 75% white" ) ),
                                                false, false, /* hiX */ true, false );

                                int cornerX = origin.first  + ( 1 - firstCellY ) * ( getSquareSize() << 1 ) ;
                                int cornerY = origin.second + ( 1 + firstCellY ) * getSquareSize() ;
                                setNorthDoorEasternCorner( cornerX, cornerY ) ;

                        # if defined( DEBUG_MINIATURES ) && DEBUG_MINIATURES
                                const std::pair< int, int > & corner = getNorthDoorEasternCorner() ;
                                toDrawHere.drawPixelAt( corner.first, corner.second, Color::byName( "blue" ).toAllegroColor() );
                        # endif
                        }

                        if ( doors[ "south" ] != nilPointer )
                        {
                                drawIsoSquare ( toDrawHere, cellsX - 1, firstCellY,
                                                roomColor.multiply( Color::byName( "gray 75% white" ) ),
                                                /* loX */ true, false, false, false );

                                drawIsoSquare ( toDrawHere, cellsX - 1, lastCellY,
                                                roomColor.multiply( Color::byName( "gray 75% white" ) ),
                                                false, false, /* hiX */ true, false );

                                int cornerX = origin.first + ( cellsX - 1 - firstCellY ) * ( getSquareSize() << 1 ) - 2 ;
                                int cornerY = origin.second + ( cellsX - 1 + firstCellY ) * getSquareSize() - 1 ;
                                setSouthDoorEasternCorner( cornerX, cornerY );

                        # if defined( DEBUG_MINIATURES ) && DEBUG_MINIATURES
                                const std::pair< int, int > & corner = getSouthDoorEasternCorner() ;
                                toDrawHere.drawPixelAt( corner.first, corner.second, Color::byName( "red" ).toAllegroColor() );
                        # endif
                        }
                }
        }

        for ( unsigned int cy_unsigned = firstCellY ; cy_unsigned <= lastCellY ; ++ cy_unsigned )
        {
                int cy = static_cast< int >( cy_unsigned );

                if ( cy <= maxYsw )
                {
                        if ( ! ( doors[ "south" ] != nilPointer
                                        && ( cy == doors[ "south" ]->getCellY() + 1 || cy == doors[ "south" ]->getCellY() ) ) &&
                                ! ( doors[ "southeast" ] != nilPointer
                                        && ( cy == doors[ "southeast" ]->getCellY() + 1 || cy == doors[ "southeast" ]->getCellY() ) ) &&
                                ! ( doors[ "southwest" ] != nilPointer
                                        && ( cy == doors[ "southwest" ]->getCellY() + 1 || cy == doors[ "southwest" ]->getCellY() ) ) )
                        {
                                drawIsoSquare (
                                        toDrawHere,
                                        lastCellX, cy_unsigned,
                                        roomColor.multiply( Color::whiteColor() ),
                                        false, false, false, /* hiY */ true );
                        }
                }

                if ( cy >= minYne )
                {
                        if ( ! ( doors[ "north" ] != nilPointer
                                        && ( cy == doors[ "north" ]->getCellY() + 1 || cy == doors[ "north" ]->getCellY() ) ) &&
                                ! ( doors[ "northeast" ] != nilPointer
                                        && ( cy == doors[ "northeast" ]->getCellY() + 1 || cy == doors[ "northeast" ]->getCellY() ) ) &&
                                ! ( doors[ "northwest" ] != nilPointer
                                        && ( cy == doors[ "northwest" ]->getCellY() + 1 || cy == doors[ "northwest" ]->getCellY() ) ) )
                        {
                                drawIsoSquare (
                                        toDrawHere,
                                        firstCellX, cy_unsigned,
                                        roomColor.multiply( Color::whiteColor() ),
                                        false, /* loY */ true, false, false );
                        }
                }

                if ( narrowRoomAlongY )
                {
                        if ( doors[ "east" ] != nilPointer )
                        {
                                drawIsoSquare ( toDrawHere, firstCellX, 0,
                                                roomColor.multiply( Color::byName( "gray 75% white" ) ),
                                                false, /* loY */ true, false, false );

                                drawIsoSquare ( toDrawHere, lastCellX, 0,
                                                roomColor.multiply( Color::byName( "gray 75% white" ) ),
                                                false, false, false, /* hiY */ true );

                                int cornerX = origin.first  + ( firstCellX - 1 ) * ( getSquareSize() << 1 ) ;
                                int cornerY = origin.second + ( firstCellX + 1 ) * getSquareSize() ;
                                setEastDoorNorthernCorner( cornerX, cornerY );

                        # if defined( DEBUG_MINIATURES ) && DEBUG_MINIATURES
                                const std::pair< int, int > & corner = getEastDoorNorthernCorner() ;
                                toDrawHere.drawPixelAt( corner.first, corner.second, Color::byName( "blue" ).toAllegroColor() );
                        # endif
                        }

                        if ( doors[ "west" ] != nilPointer )
                        {
                                drawIsoSquare ( toDrawHere, firstCellX, cellsY - 1,
                                                roomColor.multiply( Color::byName( "gray 75% white" ) ),
                                                false, /* loY */ true, false, false );

                                drawIsoSquare ( toDrawHere, lastCellX, cellsY - 1,
                                                roomColor.multiply( Color::byName( "gray 75% white" ) ),
                                                false, false, false, /* hiY */ true );

                                int cornerX = origin.first + ( firstCellX - ( cellsY - 1 ) ) * ( getSquareSize() << 1 ) + 2 ;
                                int cornerY = origin.second + ( firstCellX + cellsY - 1 ) * getSquareSize() - 1 ;
                                setWestDoorNorthernCorner( cornerX, cornerY );

                        # if defined( DEBUG_MINIATURES ) && DEBUG_MINIATURES
                                const std::pair< int, int > & corner = getWestDoorNorthernCorner() ;
                                toDrawHere.drawPixelAt( corner.first, corner.second, Color::byName( "red" ).toAllegroColor() );
                        # endif
                        }
                }
        }

        const ConnectedRooms * connections = room.getConnections() ;
        if ( connections == nilPointer ) throw UnlikelyToHappenException( "nil connections for room " + room.getNameOfRoomDescriptionFile() );

        for ( unsigned int column = 0; column < gridItemsInRoom.size(); column ++ )
        {
                const std::vector< GridItemPtr >& columnOfItems = gridItemsInRoom[ column ];

                for ( std::vector< GridItemPtr >::const_iterator gi = columnOfItems.begin (); gi != columnOfItems.end (); ++ gi )
                {
                        const GridItem& item = *( *gi ) ;

                        std::string kind = item.getKind ();
                        int cx = item.getCellX() ;
                        int cy = item.getCellY() ;

                        // show teleports

                        const std::string & roomToTeleport = connections->getConnectedRoomAt( "via teleport" );
                        const std::string & roomToTeleportToo = connections->getConnectedRoomAt( "via second teleport" );

                        if ( kind == "teleport" && ! roomToTeleport.empty() )
                                fillIsoSquare( toDrawHere, cx, cy, Color::byName( "yellow" ) );
                        else
                        if ( kind == "teleport-too" && ! roomToTeleportToo.empty() )
                                fillIsoSquare( toDrawHere, cx, cy, Color::byName( "magenta" ) );

                        // show triple room’s walls

                        if ( kind.find( "wall-x" ) != std::string::npos )
                        {
                                if ( kind.find( "invisible-wall" ) != std::string::npos )
                                        drawIsoSquare( toDrawHere,
                                                        cx, cy,
                                                        roomColor.multiply( Color::whiteColor() ),
                                                        true, false, false, false );
                                else
                                        drawIsoSquare( toDrawHere,
                                                        cx, cy,
                                                        roomColor.multiply( Color::whiteColor() ),
                                                        false, false, true, false );
                        }
                        if ( kind.find( "wall-y" ) != std::string::npos )
                        {
                                if ( kind.find( "invisible-wall" ) != std::string::npos )
                                        drawIsoSquare( toDrawHere,
                                                        cx, cy,
                                                        roomColor.multiply( Color::whiteColor() ),
                                                        false, true, false, false );
                                else
                                        drawIsoSquare( toDrawHere,
                                                        cx, cy,
                                                        roomColor.multiply( Color::whiteColor() ),
                                                        false, false, false, true );
                        }
                }
        }

        // paint the final room

        if ( room.getNameOfRoomDescriptionFile() == "finalroom.xml" )
        {
                for ( unsigned int x = 0 ; x < cellsX ; ++ x ) {
                        for ( unsigned int y = 0 ; y < cellsY ; ++ y )
                        {
                                drawIsoSquare( toDrawHere, x, y, Color::byName( "green" ), true, true, true, true ) ;
                                fillIsoSquareInside( toDrawHere, x, y, Color::byName( "yellow" ), true ) ;
                        }
                }
        }

        // show when there’s a room above or below

        if ( this->isThereRoomAbove || this->isThereRoomBelow )
        {
                int plusYforAbove = this->isThereRoomAbove ? ( getSquareSize() << 2 ) : 0 ;
                int plusYforBelow = this->isThereRoomBelow ? ( getSquareSize() << 2 ) : 0 ;

                NamedPicture * imageWithVignettes = new NamedPicture( this->theImage->getWidth(), this->theImage->getHeight() + plusYforAbove + plusYforBelow );
                imageWithVignettes->setName( this->theImage->getName() + " (with vignettes)" );

                if ( this->isThereRoomAbove ) {
                        NamedPicture * justMiniature = this->theImage ;
                        allegro::bitBlit( justMiniature->getAllegroPict(), imageWithVignettes->getAllegroPict(), 0, plusYforAbove ) ;
                        this->theImage = imageWithVignettes ;
                        delete justMiniature ;
                }

                unsigned int height = ( this->room.getCellsAlongX() + this->room.getCellsAlongY() ) * getSquareSize() ;
                int miniatureMidX = height ; // = width >> 1 where width = height << 1
                int aboveY = plusYforAbove - ( getSquareSize() << 1 ) + 1 ;
                int belowY = height + getSquareSize() ;

                drawVignetteAboveOrBelow( this->theImage->getAllegroPict(),
                                                miniatureMidX, aboveY, belowY,
                                                Color::byName( "green" ).toAllegroColor(),
                                                this->isThereRoomAbove, this->isThereRoomBelow );
        }

        // add text about the room

        if ( this->withTextAboutRoom && ! this->textAboutRoomIsWritten ) {
                NamedPicture * imageWithRoomInfo = new NamedPicture( this->theImage->getWidth(), this->theImage->getHeight() );
                imageWithRoomInfo->setName( this->theImage->getName() + " (with text about room)" );

                NamedPicture * justMiniature = this->theImage ;
                this->theImage = imageWithRoomInfo ;

                const allegro::Pict & previousWhereToDraw = allegro::Pict::getWhereToDraw() ;
                allegro::Pict::setWhereToDraw( imageWithRoomInfo->getAllegroPict() );

                allegro::bitBlit( justMiniature->getAllegroPict(), Miniature::room_info_shift_x, Miniature::room_info_shift_y ) ;
                delete justMiniature ;

                // write it

                const AllegroColor & roomColor = Color::byName( this->room.getColor() ).toAllegroColor() ;

                const std::string & roomFile = this->room.getNameOfRoomDescriptionFile() ;
                std::string whichRoom = util::stringEndsWith( roomFile, ".xml" ) ? roomFile.substr( 0, roomFile.length() - 4 ) : roomFile ;
                allegro::textOut( whichRoom, 0, 0, roomColor );

                std::ostringstream roomSizeText ;
                roomSizeText << this->room.getCellsAlongX() << "x" << this->room.getCellsAlongY() ;
                allegro::textOut( roomSizeText.str(), 0, 12, roomColor );

                allegro::Pict::setWhereToDraw( previousWhereToDraw );

                this->textAboutRoomIsWritten = true ;
        }

# if defined( WRITE_MINIATURES ) && WRITE_MINIATURES
        this->theImage->saveAsPNG( ospaths::homePath() );

        // and reload image from file
        std::string imageName = this->theImage->getName() ;
        delete this->theImage ;
        this->theImage = new NamedPicture( ospaths::homePath(), imageName );
# endif
}

void Miniature::draw ()
{
        if ( this->theImage == nilPointer ) composeImage() ;

        int leftX = this->drawingOffset.first ;
        int topY = this->drawingOffset.second ;

        // draw the image of miniature on the screen
        allegro::drawSprite( this->theImage->getAllegroPict(), leftX, topY );

        const std::pair< int, int > & roomOrigin = getOriginOfRoom() ;
        std::pair< int, int > roomOriginOnScreen( roomOrigin.first + leftX + ( this->withTextAboutRoom ? Miniature::room_info_shift_x : 0 ),
                                                  roomOrigin.second + topY + ( this->withTextAboutRoom ? Miniature::room_info_shift_y : 0 )
                                                                                + ( this->isThereRoomAbove ? ( getSquareSize() << 2 ) : 0 ) );

        const allegro::Pict & theScreen = allegro::Pict::getWhereToDraw() ;

        // items the player can take

        const std::vector< FreeItemPtr > & freeItemsInRoom = room.getFreeItems();

        for ( std::vector< FreeItemPtr >::const_iterator fi = freeItemsInRoom.begin (); fi != freeItemsInRoom.end (); ++ fi )
        {
                if ( *fi == nilPointer || ( *fi )->whichItemClass() == "avatar item" ) continue ;
                const FreeItem & item = *( *fi ) ;

                const DescriptionOfItem & descriptionOfItem = item.getDescriptionOfItem () ;
                const std::string & kind = descriptionOfItem.getKind ();

                // the tools

                if ( kind == "handbag" || kind == "horn" || kind == "donuts" )
                {
                        unsigned int oneGridCell = room.getSizeOfOneCell() ;

                        // free coordinates to grid cell coordinates
                        int cx = ( item.getX() - ( ( oneGridCell - descriptionOfItem.getWidthX() ) >> 1 ) ) / oneGridCell ;
                        int cy = ( ( item.getY() + ( ( oneGridCell - descriptionOfItem.getWidthY() ) >> 1 ) + 1 ) / oneGridCell ) - 1 ;

                        fillIsoSquare( theScreen, roomOriginOnScreen, cx, cy, Color::byName( "orange" ) );
                }

                // the bonus rabbits

                if ( kind == "extra-life" || kind == "big-jumps" || kind == "quick-steps" || kind == "shield" )
                {
                        unsigned int oneGridCell = room.getSizeOfOneCell() ;

                        // free coordinates to grid cell coordinates
                        int cx = ( item.getX() - ( ( oneGridCell - descriptionOfItem.getWidthX() ) >> 1 ) ) / oneGridCell ;
                        int cy = ( ( item.getY() + ( ( oneGridCell - descriptionOfItem.getWidthY() ) >> 1 ) + 1 ) / oneGridCell ) - 1 ;

                        fillIsoSquare( theScreen, roomOriginOnScreen, cx, cy, Color::byName( "cyan" ) );
                }

                // the reincarnation fish or the mortal fish

                if ( kind == "reincarnation-fish" || kind == "mortal-fish" )
                {
                        unsigned int oneGridCell = room.getSizeOfOneCell() ;

                        // free coordinates to grid cell coordinates
                        int cx = ( item.getX() - ( ( oneGridCell - descriptionOfItem.getWidthX() ) >> 1 ) ) / oneGridCell ;
                        int cy = ( ( item.getY() + ( ( oneGridCell - descriptionOfItem.getWidthY() ) >> 1 ) + 1 ) / oneGridCell ) - 1 ;

                        fillIsoSquare( theScreen, roomOriginOnScreen, cx, cy,
                                        ( kind == "reincarnation-fish" ) ? Color::byName( "green" ) : /* kind == "mortal-fish" */ Color::byName( "red" ) );
                }
        }

        // show the characters

        const std::vector< AvatarItemPtr > & charactersInRoom = room.getCharactersYetInRoom() ;

        for ( std::vector< AvatarItemPtr >::const_iterator pi = charactersInRoom.begin (); pi != charactersInRoom.end (); ++ pi )
        {
                const AvatarItem & character = *( *pi ) ;

                unsigned int oneGridCell = room.getSizeOfOneCell() ;

                // the range of squares where the character is
                int xBegin = character.getX() / oneGridCell ;
                int xEnd = ( character.getX() + character.getWidthX() - 1 ) / oneGridCell ;
                int yBegin = ( character.getY() - character.getWidthY() + 1 ) / oneGridCell ;
                int yEnd = character.getY() / oneGridCell ;

                /* for ( int cx = xBegin ; cx <= xEnd ; ++ cx )
                        for ( int cy = yBegin ; cy <= yEnd ; ++ cy )
                                drawIsoSquare( theScreen, roomOriginOnScreen, cx, cy, Color::byName( "blue" ), true, true, true, true ) ; */

                int deltaWx = ( oneGridCell - character.getWidthX() ) >> 1 ;
                int deltaWy = ( oneGridCell - character.getWidthY() ) >> 1 ;

                int cx = ( character.getX() > deltaWx ) ? ( character.getX() - deltaWx ) / oneGridCell : 0 ;
                int cy = ( ( character.getY() + deltaWy + 1 ) / oneGridCell ) - 1 ;
                if ( cy < 0 ) cy = 0 ;

                if ( xBegin == xEnd && yBegin == yEnd ) {
                        cx = xBegin ;
                        cy = yEnd ;
                }
                else {
                        if ( cx < xBegin ) cx = xBegin ;
                        else if ( cx > xEnd ) cx = xEnd ;

                        if ( cy < yBegin ) cy = yBegin ;
                        else if ( cy > yEnd ) cy = yEnd ;
                }

                fillIsoSquareInside( theScreen, roomOriginOnScreen, cx, cy,
                        character.isActiveCharacter() ? FlickeringColor::flickerWhiteAndTransparent() : FlickeringColor::flickerGray75AndTransparent(),
                        true );
        }
}

void Miniature::drawVignetteAboveOrBelow( const allegro::Pict & where, int midX, int aboveY, int belowY, const Color& color, bool drawAbove, bool drawBelow )
{
        if ( color.isFullyTransparent () ) return ;

        if ( ! drawAbove && ! drawBelow ) return ;

        const allegro::Pict & previousWhere = allegro::Pict::getWhereToDraw() ;
        allegro::Pict::setWhereToDraw( where );

        const unsigned int linesEven = ( ( getSquareSize() + 1 ) >> 1 ) << 1 ;
        const bool minSquareSize = ( getSquareSize() == 2 );

        if ( drawAbove ) {
                // draw the middle line
                allegro::drawLine( midX, aboveY - linesEven + 1, midX, aboveY - ( linesEven << 1 ) + ( minSquareSize ? 0 : 1 ), color.toAllegroColor() );

                int pos = 0;
                for ( unsigned int off = linesEven ; off > 0 ; off -- , pos ++ )
                {
                        allegro::drawLine( midX - off, aboveY - pos, midX - off, aboveY - pos - linesEven, color.toAllegroColor() );
                        allegro::drawLine( midX + off, aboveY - pos, midX + off, aboveY - pos - linesEven, color.toAllegroColor() );
                }
        }

        if ( drawBelow ) {
                // the middle line
                allegro::drawLine( midX, belowY + linesEven - 1, midX, belowY + ( linesEven << 1 ) - ( minSquareSize ? 0 : 1 ), color.toAllegroColor() );

                int pos = 0;
                for ( unsigned int off = linesEven ; off > 0 ; off -- , pos ++ )
                {
                        allegro::drawLine( midX - off, belowY + pos, midX - off, belowY + pos + linesEven, color.toAllegroColor() );
                        allegro::drawLine( midX + off, belowY + pos, midX + off, belowY + pos + linesEven, color.toAllegroColor() );
                }
        }

        allegro::Pict::setWhereToDraw( previousWhere );
}

void Miniature::drawEastDoorOnMiniature( const allegro::Pict & where, unsigned int x, unsigned int y, const Color & color )
{
        if ( color.isFullyTransparent () ) return ;

        const std::pair< int, int > & roomOrigin = getOriginOfRoom() ;

        drawIsoSquare( where, x - 1, y, color, false, /* loY */ true, false, false );

        {
                int px = roomOrigin.first + ( ( x - 1 ) - ( y + 1 ) ) * ( getSquareSize() << 1 ) ;
                int py = roomOrigin.second + ( ( y + 1 ) + ( x - 1 ) ) * getSquareSize() ;

                setEastDoorNorthernCorner( px, py );
                where.drawPixelAt( px, py, color.toAllegroColor() );
                where.drawPixelAt( px + 1, py, color.toAllegroColor() );
        }

        drawIsoSquare( where, x, y, color, false, false, false, /* hiY */ true );

        {
                int px = roomOrigin.first + ( ( x + 1 ) - ( y + 1 ) ) * ( getSquareSize() << 1 ) ;
                int py = roomOrigin.second + ( ( y + 1 ) + ( x + 1 ) ) * getSquareSize() ;

                where.drawPixelAt( px - 2, py - 1, color.toAllegroColor() );
                where.drawPixelAt( px - 1, py - 1, color.toAllegroColor() );
        }

# if defined( DEBUG_MINIATURES ) && DEBUG_MINIATURES
        const std::pair< int, int > & corner = getEastDoorNorthernCorner() ;
        where.drawPixelAt( corner.first, corner.second, Color::byName( "blue" ).toAllegroColor() );
# endif
}

void Miniature::drawWestDoorOnMiniature( const allegro::Pict & where, unsigned int x, unsigned int y, const Color & color )
{
        if ( color.isFullyTransparent () ) return ;

        const std::pair< int, int > & roomOrigin = getOriginOfRoom() ;

        drawIsoSquare( where, x - 1, y, color, false, /* loY */ true, false, false );

        {
                int px = roomOrigin.first + ( ( x - 1 ) - y ) * ( getSquareSize() << 1 ) ;
                int py = roomOrigin.second + ( y + ( x - 1 ) ) * getSquareSize() ;

                setWestDoorNorthernCorner( px + 2, py - 1 );
                where.drawPixelAt( px + 2, py - 1, color.toAllegroColor() );
                where.drawPixelAt( px + 3, py - 1, color.toAllegroColor() );
        }

        drawIsoSquare( where, x, y, color, false, false, false, /* hiY */ true );

        {
                int px = roomOrigin.first + ( ( x + 1 ) - y ) * ( getSquareSize() << 1 ) ;
                int py = roomOrigin.second + ( y + ( x + 1 ) ) * getSquareSize() ;

                where.drawPixelAt( px, py - 2, color.toAllegroColor() );
                where.drawPixelAt( px + 1, py - 2, color.toAllegroColor() );
        }

# if defined( DEBUG_MINIATURES ) && DEBUG_MINIATURES
        const std::pair< int, int > & corner = getWestDoorNorthernCorner() ;
        where.drawPixelAt( corner.first, corner.second, Color::byName( "red" ).toAllegroColor() );
# endif
}

void Miniature::drawNorthDoorOnMiniature( const allegro::Pict & where, unsigned int x, unsigned int y, const Color & color )
{
        if ( color.isFullyTransparent () ) return ;

        const std::pair< int, int > & roomOrigin = getOriginOfRoom() ;

        drawIsoSquare( where, x, y - 1, color, /* loX */ true, false, false, false );

        {
                int px = roomOrigin.first + ( ( x + 1 ) - ( y - 1 ) ) * ( getSquareSize() << 1 ) ;
                int py = roomOrigin.second + ( ( y - 1 ) + ( x + 1 ) ) * getSquareSize() ;

                setNorthDoorEasternCorner( px, py );
                where.drawPixelAt( px, py, color.toAllegroColor() );
                where.drawPixelAt( px + 1, py, color.toAllegroColor() );
        }

        drawIsoSquare( where, x, y, color, false, false, /* hiX */ true, false );

        {
                int px = roomOrigin.first + ( ( x + 1 ) - ( y + 1 ) ) * ( getSquareSize() << 1 ) ;
                int py = roomOrigin.second + ( ( y + 1 ) + ( x + 1 ) ) * getSquareSize() ;

                where.drawPixelAt( px + 2, py - 1, color.toAllegroColor() );
                where.drawPixelAt( px + 3, py - 1, color.toAllegroColor() );
        }

# if defined( DEBUG_MINIATURES ) && DEBUG_MINIATURES
        const std::pair< int, int > & corner = getNorthDoorEasternCorner() ;
        where.drawPixelAt( corner.first, corner.second, Color::byName( "blue" ).toAllegroColor() );
# endif
}

void Miniature::drawSouthDoorOnMiniature( const allegro::Pict & where, unsigned int x, unsigned int y, const Color & color )
{
        if ( color.isFullyTransparent () ) return ;

        const std::pair< int, int > & roomOrigin = getOriginOfRoom() ;

        drawIsoSquare( where, x, y - 1, color, /* loX */ true, false, false, false );

        {
                int px = roomOrigin.first + ( x - ( y - 1 ) ) * ( getSquareSize() << 1 ) ;
                int py = roomOrigin.second + ( ( y - 1 ) + x ) * getSquareSize() ;

                setSouthDoorEasternCorner( px - 2, py - 1 );
                where.drawPixelAt( px - 2, py - 1, color.toAllegroColor() );
                where.drawPixelAt( px - 1, py - 1, color.toAllegroColor() );
        }

        drawIsoSquare( where, x, y, color, false, false, /* hiX */ true, false );

        {
                int px = roomOrigin.first + ( x - ( y + 1 ) ) * ( getSquareSize() << 1 ) ;
                int py = roomOrigin.second + ( ( y + 1 ) + x ) * getSquareSize() ;

                where.drawPixelAt( px, py - 2, color.toAllegroColor() );
                where.drawPixelAt( px + 1, py - 2, color.toAllegroColor() );
        }

# if defined( DEBUG_MINIATURES ) && DEBUG_MINIATURES
        const std::pair< int, int > & corner = getSouthDoorEasternCorner() ;
        where.drawPixelAt( corner.first, corner.second, Color::byName( "red" ).toAllegroColor() );
# endif
}

void Miniature::drawBigIsoRectangle( const allegro::Pict & where,
                                std::pair< int, int > origin,
                                unsigned int squaresX, unsigned int squaresY,
                                const Color & color )
{
        if ( color.isFullyTransparent () ) return ;

        unsigned int posX = origin.first ;
        unsigned int posY = origin.second ;

        for ( unsigned int square = 0 ; square < squaresX ; ++ square ) {
                for ( unsigned int pixel = 0 ; pixel < getSquareSize() ; ++ pixel )
                {
                        where.drawPixelAt( posX++, posY, color.toAllegroColor() );
                        where.drawPixelAt( posX++, posY++, color.toAllegroColor() );
                }
        }

        -- posX ; -- posY ;

        for ( unsigned int square = 0 ; square < squaresY ; ++ square ) {
                for ( unsigned int pixel = 0 ; pixel < getSquareSize() ; ++ pixel )
                {
                        where.drawPixelAt( posX--, posY, color.toAllegroColor() );
                        where.drawPixelAt( posX--, posY++, color.toAllegroColor() );
                }
        }

        posX = origin.first + 1 ;
        posY = origin.second ;

        for ( unsigned int square = 0 ; square < squaresY ; ++ square ) {
                for ( unsigned int pixel = 0 ; pixel < getSquareSize() ; ++ pixel )
                {
                        where.drawPixelAt( posX--, posY, color.toAllegroColor() );
                        where.drawPixelAt( posX--, posY++, color.toAllegroColor() );
                }
        }

        ++ posX ; -- posY ;

        for ( unsigned int square = 0 ; square < squaresX ; ++ square ) {
                for ( unsigned int pixel = 0 ; pixel < getSquareSize() ; ++ pixel )
                {
                        where.drawPixelAt( posX++, posY, color.toAllegroColor() );
                        where.drawPixelAt( posX++, posY++, color.toAllegroColor() );
                }
        }
}

void Miniature::drawIsoSquare( const allegro::Pict& where,
                                std::pair< int, int > origin,
                                int x, int y,
                                const Color& color,
                                bool loX, bool loY, bool hiX, bool hiY )
{
        if ( color.isFullyTransparent () ) return ;

        unsigned short oneSquare = getSquareSize() ;

        if ( loX ) {
                int px = origin.first  + ( x - y ) * ( oneSquare << 1 ) ;
                int py = origin.second + ( y + x ) * oneSquare ;

                for ( unsigned int pixel = 0 ; pixel < oneSquare ; pixel ++ ) {
                        where.drawPixelAt( px++, py, color.toAllegroColor() );
                        where.drawPixelAt( px++, py++, color.toAllegroColor() );
                }
        }

        if ( loY ) {
                int px = origin.first  + ( x - y ) * ( oneSquare << 1 ) + 1 ;
                int py = origin.second + ( y + x ) * oneSquare ;

                for ( unsigned int pixel = 0 ; pixel < oneSquare ; pixel ++ ) {
                        where.drawPixelAt( px--, py, color.toAllegroColor() );
                        where.drawPixelAt( px--, py++, color.toAllegroColor() );
                }
        }

        if ( hiX ) {
                int px = origin.first  + ( x - y ) * ( oneSquare << 1 ) - ( ( oneSquare - 1 ) << 1 ) ;
                int py = origin.second + ( y + x ) * oneSquare + ( oneSquare - 1 ) ;

                for ( unsigned int pixel = 0 ; pixel < oneSquare ; pixel ++ ) {
                        where.drawPixelAt( px++, py, color.toAllegroColor() );
                        where.drawPixelAt( px++, py++, color.toAllegroColor() );
                }
        }

        if ( hiY ) {
                int px = origin.first  + ( x - y ) * ( oneSquare << 1 ) + ( ( oneSquare - 1 ) << 1 ) + 1 ;
                int py = origin.second + ( y + x ) * oneSquare + ( oneSquare - 1 ) ;

                for ( unsigned int pixel = 0 ; pixel < oneSquare ; pixel ++ ) {
                        where.drawPixelAt( px--, py, color.toAllegroColor() );
                        where.drawPixelAt( px--, py++, color.toAllegroColor() );
                }
        }
}

void Miniature::fillIsoSquare( const allegro::Pict& where, std::pair< int, int > origin, int x, int y, const Color& color )
{
        if ( color.isFullyTransparent () ) return ;

        unsigned short oneSquare = getSquareSize() ;

        for ( unsigned int piw = 0 ; piw < oneSquare ; piw ++ )
        {
                int px =  origin.first + ( x - y ) * ( oneSquare << 1 ) - ( piw << 1 ) ;
                int py = origin.second + ( y + x ) * oneSquare + piw ;

                for ( unsigned int pixel = 0 ; pixel < oneSquare ; pixel ++ )
                {
                        where.drawPixelAt( px++, py, color.toAllegroColor() );
                        where.drawPixelAt( px++, py++, color.toAllegroColor() );
                }
        }
}

void Miniature::fillIsoSquareInside( const allegro::Pict& where, std::pair< int, int > origin, int x, int y, const Color& color, bool fullFill )
{
        if ( color.isFullyTransparent () ) return ;

        unsigned short oneSquare = getSquareSize() ;

        if ( oneSquare == 2 ) {
                int px =  origin.first + ( x - y ) * ( oneSquare << 1 ) ;
                int py = origin.second + ( y + x ) * oneSquare + 1 ;

                where.drawPixelAt( px, py, color.toAllegroColor() );
                where.drawPixelAt( px + 1, py, color.toAllegroColor() );
        }
        else {
                for ( unsigned short piw = 1 ; piw < ( oneSquare - 1 ) ; piw ++ )
                {
                        int px =  origin.first + ( x - y ) * ( oneSquare << 1 ) - ( piw << 1 ) + 2 ;
                        int py = origin.second + ( y + x ) * oneSquare + piw + 1 ;

                        for ( unsigned short pixel = 1 ; pixel < ( oneSquare - 1 ) ; pixel ++ )
                        {
                                if ( fullFill ) {
                                        where.drawPixelAt( px, py - 1, color.toAllegroColor() );
                                        where.drawPixelAt( px + 1, py - 1, color.toAllegroColor() );

                                        where.drawPixelAt( px, py + 1, color.toAllegroColor() );
                                        where.drawPixelAt( px + 1, py + 1, color.toAllegroColor() );

                                        where.drawPixelAt( px - 2, py, color.toAllegroColor() );
                                        where.drawPixelAt( px - 1, py, color.toAllegroColor() );

                                        where.drawPixelAt( px + 2, py, color.toAllegroColor() );
                                        where.drawPixelAt( px + 3, py, color.toAllegroColor() );
                                }

                                where.drawPixelAt( px++, py, color.toAllegroColor() );
                                where.drawPixelAt( px++, py++, color.toAllegroColor() );
                        }
                }
        }
}

void Miniature::setDrawingOffset ( int leftX, int topY )
{
        int plusX = 0 ;
        int plusY = this->isThereRoomAbove ? ( getSquareSize() << 2 ) : 0 ;

        this->drawingOffset = std::pair< int, int >( leftX + plusX, topY + plusY ) ;
}

bool Miniature::connectMiniature ( Miniature * that, const std::string & where, short gap )
{
        if ( that == nilPointer ) return false ;
        if ( this->theImage == nilPointer ) return false ;
        if ( that->theImage == nilPointer ) {
        # if defined( DEBUG_MINIATURES ) && DEBUG_MINIATURES
                std::cerr << "nil image of the miniature to connect to \"" << this->theImage->getName() << "\" in \"" << where << "\"" << std::endl ;
        # endif
                return false ;
        }

        that->setSquareSize( getSquareSize() ) ;

        /* const ConnectedRooms * connections = this->room.getConnections() ;
        if ( connections == nilPointer ) return false ;
        const std::string & fileOfConnectedRoom = connections->getConnectedRoomAt( where );
        if ( fileOfConnectedRoom.empty () ) return false ; */

        int shiftY = ( getSquareSize() << 1 ) + gap ;
        int shiftX = shiftY << 1 ;

        std::pair< int, int > doorCornerOfThis, doorCornerOfThat ;

        if ( where == "south" ) {
                if ( room.getDoorOn( "south" ) == nilPointer ) return false ;

                doorCornerOfThis = this->getSouthDoorEasternCorner() ;
                doorCornerOfThat = that->getNorthDoorEasternCorner() ;
        }
        else
        if ( where == "north" ) {
                if ( room.getDoorOn( "north" ) == nilPointer ) return false ;

                doorCornerOfThis = this->getNorthDoorEasternCorner() ;
                doorCornerOfThat = that->getSouthDoorEasternCorner() ;

                shiftX = - shiftX ;
                shiftY = - shiftY ;
        }
        else
        if ( where == "east" ) {
                if ( room.getDoorOn( "east" ) == nilPointer ) return false ;

                doorCornerOfThis = this->getEastDoorNorthernCorner() ;
                doorCornerOfThat = that->getWestDoorNorthernCorner() ;

                shiftY = - shiftY ;
        }
        else
        if ( where == "west" ) {
                if ( room.getDoorOn( "west" ) == nilPointer ) return false ;

                doorCornerOfThis = this->getWestDoorNorthernCorner() ;
                doorCornerOfThat = that->getEastDoorNorthernCorner() ;

                shiftX = - shiftX ;
        }
        else {
        # if defined( DEBUG_MINIATURES ) && DEBUG_MINIATURES
                std::cout << ":(( don’t yet know how to connect miniature \"" << this->theImage->getName() << "\" on \"" << where << "\"" << std::endl ;
        # endif
                return false ;
        }

        int adjacentDifferenceX = doorCornerOfThis.first - doorCornerOfThat.first ;
        int adjacentDifferenceY = doorCornerOfThis.second - doorCornerOfThat.second ;

        that->setDrawingOffset( this->drawingOffset.first + adjacentDifferenceX + shiftX, this->drawingOffset.second + adjacentDifferenceY + shiftY ) ;

# if defined( DEBUG_MINIATURES ) && DEBUG_MINIATURES
        std::cout << "\"" << this->theImage->getName() << "\" is connected to \""
                                << that->theImage->getName() << "\" in \"" << where << "\""
                                << std::endl ;
# endif

        return true ;
}
