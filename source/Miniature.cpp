
#include "Miniature.hpp"

#include "Color.hpp"
#include "FlickeringColor.hpp"
#include "GameManager.hpp"
#include "GameMap.hpp"
#include "ConnectedRooms.hpp"
#include "Mediator.hpp"
#include "DescriptionOfItem.hpp"
#include "MayNotBePossible.hpp"

#include <sstream>

#ifdef DEBUG
#  define DEBUG_MINIATURES      1
#  define WRITE_MINIATURES      1
#endif

#if defined( WRITE_MINIATURES ) && WRITE_MINIATURES
#  include "ospaths.hpp"
#endif


Miniature::Miniature( const Room & roomForMiniature, unsigned short singleTileSize, bool withRoomInfo )
        : theImage( nilPointer )
        , room( roomForMiniature )
        , sizeOfTile( Miniature::the_default_size_of_tile )
        , withTextAboutRoom( withRoomInfo )
        , textAboutRoomIsWritten( false )
        , isThereRoomAbove( false )
        , northDoorEasternCorner( Miniature::corner_not_set, Miniature::corner_not_set )
        , eastDoorNorthernCorner( Miniature::corner_not_set, Miniature::corner_not_set )
        , southDoorEasternCorner( Miniature::corner_not_set, Miniature::corner_not_set )
        , westDoorNorthernCorner( Miniature::corner_not_set, Miniature::corner_not_set )
{
        setSizeOfTile( singleTileSize );

        const ConnectedRooms * connections = this->room.getConnections() ;
        if ( connections != nilPointer && ! connections->getConnectedRoomAt( "above" ).empty() )
                this->isThereRoomAbove = true ;

        setOffsetOnScreen( 0, 0 );

# if defined( DEBUG_MINIATURES ) && DEBUG_MINIATURES
        std::cout << "constructed Miniature( room \"" << roomForMiniature.getNameOfRoomDescriptionFile() << "\""
                        << ", singleTileSize=" << singleTileSize
                        << ", withRoomInfo=" << ( withRoomInfo ? "true" : "false" )
                        << " )" << std::endl ;
# endif
}

/* protected */
std::pair < unsigned int, unsigned int > Miniature::calculateSize () const
{
        const unsigned int tilesX = this->room.getTilesOnX ();
        const unsigned int tilesY = this->room.getTilesOnY ();

        unsigned int height = ( tilesX + tilesY ) * this->sizeOfTile ;
        unsigned int width = height << 1 ;

        if ( this->isThereRoomAbove )
                height += ( getSizeOfTile() << 2 );

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
                theNameOfMiniature << "Miniature of room " << this->room.getNameOfRoomDescriptionFile() << " with " << getSizeOfTile() << " pixel long tiles" ;
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

        const unsigned int tilesX = this->room.getTilesOnX() ;
        const unsigned int tilesY = this->room.getTilesOnY() ;

        unsigned int firstTileX = 0 ;
        unsigned int firstTileY = 0 ;
        unsigned int lastTileX = tilesX - 1 ;
        unsigned int lastTileY = tilesY - 1 ;

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
                firstTileX ++ ;

        if ( doors[ "east" ] != nilPointer || doors[ "eastnorth" ] != nilPointer || doors[ "eastsouth" ] != nilPointer )
                firstTileY ++ ;

        if ( doors[ "south" ] != nilPointer || doors[ "southeast" ] != nilPointer || doors[ "southwest" ] != nilPointer )
                -- lastTileX ;

        if ( doors[ "west" ] != nilPointer || doors[ "westnorth" ] != nilPointer || doors[ "westsouth" ] != nilPointer )
                -- lastTileY ;

        bool narrowRoomAlongX = ( lastTileY == firstTileY + 1 ) ;
        bool narrowRoomAlongY = ( lastTileX == firstTileX + 1 ) ;

        const Color& roomColor = Color::byName( room.getColor () );

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

        drawIsoSquare( toDrawHere, tilesX, tilesY, roomColor.multiply( Color::byName( "gray" ) ) );

        // draw walls

        int minXne = firstTileX ;
        int minYne = firstTileY ;
        int maxXsw = lastTileX ;
        int maxYsw = lastTileY ;

        const std::vector< std::vector< GridItemPtr > > & gridItemsInRoom = room.getGridItems();

        for ( unsigned int column = 0; column < gridItemsInRoom.size(); column ++ ) {
                for ( std::vector< GridItemPtr >::const_iterator gi = gridItemsInRoom[ column ].begin (); gi != gridItemsInRoom[ column ].end (); ++ gi )
                {
                        const GridItem & item = *( *gi ) ;

                        std::string kind = item.getKind ();
                        int tileX = item.getCellX();
                        int tileY = item.getCellY();

                        if ( kind == "invisible-wall-x" )
                        {
                                int newPos = tileY - 1 ;
                                if ( newPos < maxYsw ) maxYsw = newPos ;
                        }
                        else if ( kind.find( "wall-x" ) != std::string::npos )
                        {
                                int newPos = tileY + 1 ;
                                if ( newPos > minYne ) minYne = newPos ;
                        }

                        if ( kind == "invisible-wall-y" )
                        {
                                int newPos = tileX - 1 ;
                                if ( newPos < maxXsw ) maxXsw = newPos ;
                        }
                        else if ( kind.find( "wall-y" ) != std::string::npos )
                        {
                                int newPos = tileX + 1 ;
                                if ( newPos > minXne ) minXne = newPos ;
                        }
                }
        }

        const std::pair< int, int > & origin = getOriginOfRoom() ;

        for ( unsigned int unsignedTileX = firstTileX ; unsignedTileX <= lastTileX ; unsignedTileX ++ )
        {
                int tileX = static_cast< int >( unsignedTileX );

                if ( tileX >= minXne )
                {
                        if ( ! ( doors[ "east" ] != nilPointer
                                        && ( tileX == doors[ "east" ]->getCellX() + 1 || tileX == doors[ "east" ]->getCellX() ) ) &&
                                ! ( doors[ "eastnorth" ] != nilPointer
                                        && ( tileX == doors[ "eastnorth" ]->getCellX() + 1 || tileX == doors[ "eastnorth" ]->getCellX() ) ) &&
                                ! ( doors[ "eastsouth" ] != nilPointer
                                        && ( tileX == doors[ "eastsouth" ]->getCellX() + 1 || tileX == doors[ "eastsouth" ]->getCellX() ) ) )
                        {
                                drawIsoTile (
                                        toDrawHere,
                                        unsignedTileX, firstTileY,
                                        roomColor.multiply( Color::whiteColor() ),
                                        /* loX */ true, false, false, false );
                        }
                }

                if ( tileX <= maxXsw )
                {
                        if ( ! ( doors[ "west" ] != nilPointer
                                        && ( tileX == doors[ "west" ]->getCellX() + 1 || tileX == doors[ "west" ]->getCellX() ) ) &&
                                ! ( doors[ "westnorth" ] != nilPointer
                                        && ( tileX == doors[ "westnorth" ]->getCellX() + 1 || tileX == doors[ "westnorth" ]->getCellX() ) ) &&
                                ! ( doors[ "westsouth" ] != nilPointer
                                        && ( tileX == doors[ "westsouth" ]->getCellX() + 1 || tileX == doors[ "westsouth" ]->getCellX() ) ) )
                        {
                                drawIsoTile (
                                        toDrawHere,
                                        unsignedTileX, lastTileY,
                                        roomColor.multiply( Color::whiteColor() ),
                                        false, false, /* hiX */ true, false );
                        }
                }

                if ( narrowRoomAlongX )
                {
                        if ( doors[ "north" ] != nilPointer )
                        {
                                drawIsoTile ( toDrawHere, 0, firstTileY,
                                                roomColor.multiply( Color::byName( "gray 75% white" ) ),
                                                /* loX */ true, false, false, false );

                                drawIsoTile ( toDrawHere, 0, lastTileY,
                                                roomColor.multiply( Color::byName( "gray 75% white" ) ),
                                                false, false, /* hiX */ true, false );

                                int cornerX = origin.first  + ( 1 - firstTileY ) * ( getSizeOfTile() << 1 ) ;
                                int cornerY = origin.second + ( 1 + firstTileY ) * getSizeOfTile() ;
                                setNorthDoorEasternCorner( cornerX, cornerY ) ;

                        # if defined( DEBUG_MINIATURES ) && DEBUG_MINIATURES
                                const std::pair< int, int > & corner = getNorthDoorEasternCorner() ;
                                toDrawHere.drawPixelAt( corner.first, corner.second, Color::byName( "blue" ).toAllegroColor() );
                        # endif
                        }

                        if ( doors[ "south" ] != nilPointer )
                        {
                                drawIsoTile ( toDrawHere, tilesX - 1, firstTileY,
                                                roomColor.multiply( Color::byName( "gray 75% white" ) ),
                                                /* loX */ true, false, false, false );

                                drawIsoTile ( toDrawHere, tilesX - 1, lastTileY,
                                                roomColor.multiply( Color::byName( "gray 75% white" ) ),
                                                false, false, /* hiX */ true, false );

                                int cornerX = origin.first + ( tilesX - 1 - firstTileY ) * ( getSizeOfTile() << 1 ) - 2 ;
                                int cornerY = origin.second + ( tilesX - 1 + firstTileY ) * getSizeOfTile() - 1 ;
                                setSouthDoorEasternCorner( cornerX, cornerY );

                        # if defined( DEBUG_MINIATURES ) && DEBUG_MINIATURES
                                const std::pair< int, int > & corner = getSouthDoorEasternCorner() ;
                                toDrawHere.drawPixelAt( corner.first, corner.second, Color::byName( "red" ).toAllegroColor() );
                        # endif
                        }
                }
        }

        for ( unsigned int unsignedTileY = firstTileY ; unsignedTileY <= lastTileY ; unsignedTileY ++ )
        {
                int tileY = static_cast< int >( unsignedTileY );

                if ( tileY <= maxYsw )
                {
                        if ( ! ( doors[ "south" ] != nilPointer
                                        && ( tileY == doors[ "south" ]->getCellY() + 1 || tileY == doors[ "south" ]->getCellY() ) ) &&
                                ! ( doors[ "southeast" ] != nilPointer
                                        && ( tileY == doors[ "southeast" ]->getCellY() + 1 || tileY == doors[ "southeast" ]->getCellY() ) ) &&
                                ! ( doors[ "southwest" ] != nilPointer
                                        && ( tileY == doors[ "southwest" ]->getCellY() + 1 || tileY == doors[ "southwest" ]->getCellY() ) ) )
                        {
                                drawIsoTile (
                                        toDrawHere,
                                        lastTileX, unsignedTileY,
                                        roomColor.multiply( Color::whiteColor() ),
                                        false, false, false, /* hiY */ true );
                        }
                }

                if ( tileY >= minYne )
                {
                        if ( ! ( doors[ "north" ] != nilPointer
                                        && ( tileY == doors[ "north" ]->getCellY() + 1 || tileY == doors[ "north" ]->getCellY() ) ) &&
                                ! ( doors[ "northeast" ] != nilPointer
                                        && ( tileY == doors[ "northeast" ]->getCellY() + 1 || tileY == doors[ "northeast" ]->getCellY() ) ) &&
                                ! ( doors[ "northwest" ] != nilPointer
                                        && ( tileY == doors[ "northwest" ]->getCellY() + 1 || tileY == doors[ "northwest" ]->getCellY() ) ) )
                        {
                                drawIsoTile (
                                        toDrawHere,
                                        firstTileX, unsignedTileY,
                                        roomColor.multiply( Color::whiteColor() ),
                                        false, /* loY */ true, false, false );
                        }
                }

                if ( narrowRoomAlongY )
                {
                        if ( doors[ "east" ] != nilPointer )
                        {
                                drawIsoTile ( toDrawHere, firstTileX, 0,
                                                roomColor.multiply( Color::byName( "gray 75% white" ) ),
                                                false, /* loY */ true, false, false );

                                drawIsoTile ( toDrawHere, lastTileX, 0,
                                                roomColor.multiply( Color::byName( "gray 75% white" ) ),
                                                false, false, false, /* hiY */ true );

                                int cornerX = origin.first  + ( firstTileX - 1 ) * ( getSizeOfTile() << 1 ) ;
                                int cornerY = origin.second + ( firstTileX + 1 ) * getSizeOfTile() ;
                                setEastDoorNorthernCorner( cornerX, cornerY );

                        # if defined( DEBUG_MINIATURES ) && DEBUG_MINIATURES
                                const std::pair< int, int > & corner = getEastDoorNorthernCorner() ;
                                toDrawHere.drawPixelAt( corner.first, corner.second, Color::byName( "blue" ).toAllegroColor() );
                        # endif
                        }

                        if ( doors[ "west" ] != nilPointer )
                        {
                                drawIsoTile ( toDrawHere, firstTileX, tilesY - 1,
                                                roomColor.multiply( Color::byName( "gray 75% white" ) ),
                                                false, /* loY */ true, false, false );

                                drawIsoTile ( toDrawHere, lastTileX, tilesY - 1,
                                                roomColor.multiply( Color::byName( "gray 75% white" ) ),
                                                false, false, false, /* hiY */ true );

                                int cornerX = origin.first + ( firstTileX - ( tilesY - 1 ) ) * ( getSizeOfTile() << 1 ) + 2 ;
                                int cornerY = origin.second + ( firstTileX + tilesY - 1 ) * getSizeOfTile() - 1 ;
                                setWestDoorNorthernCorner( cornerX, cornerY );

                        # if defined( DEBUG_MINIATURES ) && DEBUG_MINIATURES
                                const std::pair< int, int > & corner = getWestDoorNorthernCorner() ;
                                toDrawHere.drawPixelAt( corner.first, corner.second, Color::byName( "red" ).toAllegroColor() );
                        # endif
                        }
                }
        }

        const ConnectedRooms * connections = room.getConnections() ;
        if ( connections == nilPointer ) throw MayNotBePossible( "nil connections for room " + room.getNameOfRoomDescriptionFile() );

        for ( unsigned int column = 0; column < gridItemsInRoom.size(); column ++ )
        {
                const std::vector< GridItemPtr >& columnOfItems = gridItemsInRoom[ column ];

                for ( std::vector< GridItemPtr >::const_iterator gi = columnOfItems.begin (); gi != columnOfItems.end (); ++ gi )
                {
                        const GridItem& item = *( *gi ) ;

                        std::string kind = item.getKind ();
                        int tileX = item.getCellX();
                        int tileY = item.getCellY();

                        // show teleports

                        const std::string & roomToTeleport = connections->getConnectedRoomAt( "via teleport" );
                        const std::string & roomToTeleportToo = connections->getConnectedRoomAt( "via second teleport" );

                        if ( kind == "teleport" && ! roomToTeleport.empty() )
                                fillIsoTile( toDrawHere, tileX, tileY, Color::byName( "yellow" ) );
                        else
                        if ( kind == "teleport-too" && ! roomToTeleportToo.empty() )
                                fillIsoTile( toDrawHere, tileX, tileY, Color::byName( "magenta" ) );

                        // show triple room’s walls

                        if ( kind.find( "wall-x" ) != std::string::npos )
                        {
                                if ( kind.find( "invisible-wall" ) != std::string::npos )
                                        drawIsoTile( toDrawHere,
                                                        tileX, tileY,
                                                        roomColor.multiply( Color::whiteColor() ),
                                                        true, false, false, false );
                                else
                                        drawIsoTile( toDrawHere,
                                                        tileX, tileY,
                                                        roomColor.multiply( Color::whiteColor() ),
                                                        false, false, true, false );
                        }
                        if ( kind.find( "wall-y" ) != std::string::npos )
                        {
                                if ( kind.find( "invisible-wall" ) != std::string::npos )
                                        drawIsoTile( toDrawHere,
                                                        tileX, tileY,
                                                        roomColor.multiply( Color::whiteColor() ),
                                                        false, true, false, false );
                                else
                                        drawIsoTile( toDrawHere,
                                                        tileX, tileY,
                                                        roomColor.multiply( Color::whiteColor() ),
                                                        false, false, false, true );
                        }
                }
        }

        // paint the final room

        if ( room.getNameOfRoomDescriptionFile() == "finalroom.xml" )
        {
                for ( unsigned int x = 0 ; x < tilesX ; ++ x ) {
                        for ( unsigned int y = 0 ; y < tilesY ; ++ y )
                        {
                                drawIsoTile( toDrawHere, x, y, Color::byName( "green" ), true, true, true, true ) ;
                                fillIsoTileInside( toDrawHere, x, y, Color::byName( "yellow" ), true ) ;
                        }
                }
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

                std::ostringstream roomTilesText ;
                roomTilesText << this->room.getTilesOnX() << "x" << this->room.getTilesOnY() ;
                allegro::textOut( roomTilesText.str(), 0, 12, roomColor );

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

        int leftX = this->offsetOnScreen.first ;
        int topY = this->offsetOnScreen.second ;

        // draw the image of miniature on the screen
        allegro::drawSprite( this->theImage->getAllegroPict(), leftX, topY );

        const std::pair< int, int > & roomOrigin = getOriginOfRoom() ;
        std::pair< int, int > roomOriginOnScreen( roomOrigin.first + leftX + ( this->withTextAboutRoom ? Miniature::room_info_shift_x : 0 ),
                                                  roomOrigin.second + topY + ( this->withTextAboutRoom ? Miniature::room_info_shift_y : 0 ) );

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
                        unsigned int roomTileSize = room.getSizeOfOneTile() ;

                        // free coordinates to tile coordinates
                        int tileX = ( item.getX() - ( ( roomTileSize - descriptionOfItem.getWidthX() ) >> 1 ) ) / roomTileSize ;
                        int tileY = ( ( item.getY() + ( ( roomTileSize - descriptionOfItem.getWidthY() ) >> 1 ) + 1 ) / roomTileSize ) - 1 ;

                        fillIsoTile( theScreen, roomOriginOnScreen, tileX, tileY, Color::byName( "orange" ) );
                }

                // the bonus rabbits

                if ( kind == "extra-life" || kind == "big-jumps" || kind == "quick-steps" || kind == "shield" )
                {
                        unsigned int roomTileSize = room.getSizeOfOneTile() ;

                        // free coordinates to tile coordinates
                        int tileX = ( item.getX() - ( ( roomTileSize - descriptionOfItem.getWidthX() ) >> 1 ) ) / roomTileSize ;
                        int tileY = ( ( item.getY() + ( ( roomTileSize - descriptionOfItem.getWidthY() ) >> 1 ) + 1 ) / roomTileSize ) - 1 ;

                        fillIsoTile( theScreen, roomOriginOnScreen, tileX, tileY, Color::byName( "cyan" ) );
                }

                // the reincarnation fish or the mortal fish

                if ( kind == "reincarnation-fish" || kind == "mortal-fish" )
                {
                        unsigned int roomTileSize = room.getSizeOfOneTile() ;

                        // free coordinates to tile coordinates
                        int tileX = ( item.getX() - ( ( roomTileSize - descriptionOfItem.getWidthX() ) >> 1 ) ) / roomTileSize ;
                        int tileY = ( ( item.getY() + ( ( roomTileSize - descriptionOfItem.getWidthY() ) >> 1 ) + 1 ) / roomTileSize ) - 1 ;

                        fillIsoTile( theScreen, roomOriginOnScreen, tileX, tileY,
                                        ( kind == "reincarnation-fish" ) ? Color::byName( "green" ) : /* kind == "mortal-fish" */ Color::byName( "red" ) );
                }
        }

        // show the characters

        const std::vector< AvatarItemPtr >& charactersInRoom = room.getCharactersYetInRoom() ;

        for ( std::vector< AvatarItemPtr >::const_iterator pi = charactersInRoom.begin (); pi != charactersInRoom.end (); ++ pi )
        {
                const AvatarItem & character = *( *pi ) ;

                unsigned int roomTileSize = room.getSizeOfOneTile() ;

                // the range of tiles where the character is
                int xBegin = character.getX() / roomTileSize;
                int xEnd = ( character.getX() + character.getWidthX() - 1 ) / roomTileSize;
                int yBegin = ( character.getY() - character.getWidthY() + 1 ) / roomTileSize;
                int yEnd = character.getY() / roomTileSize;

                /* for ( int x = xBegin ; x <= xEnd ; ++ x )
                        for ( int y = yBegin ; y <= yEnd ; ++ y )
                                drawIsoTile( theScreen, roomOriginOnScreen, x, y, Color::byName( "blue" ), true, true, true, true ) ; */

                int deltaWx = ( roomTileSize - character.getWidthX() ) >> 1 ;
                int deltaWy = ( roomTileSize - character.getWidthY() ) >> 1 ;

                int tileX = ( character.getX() > deltaWx ) ? ( character.getX() - deltaWx ) / roomTileSize : 0 ;
                int tileY = ( ( character.getY() + deltaWy + 1 ) / roomTileSize ) - 1 ;
                if ( tileY < 0 ) tileY = 0 ;

                if ( xBegin == xEnd && yBegin == yEnd ) {
                        tileX = xBegin ;
                        tileY = yEnd ;
                }
                else {
                        if ( tileX < xBegin ) tileX = xBegin ;
                        else if ( tileX > xEnd ) tileX = xEnd ;

                        if ( tileY < yBegin ) tileY = yBegin ;
                        else if ( tileY > yEnd ) tileY = yEnd ;
                }

                fillIsoTileInside( theScreen, roomOriginOnScreen, tileX, tileY,
                        character.isActiveCharacter() ? FlickeringColor::flickerWhiteAndTransparent() : FlickeringColor::flickerGray75AndTransparent(),
                        true );
        }

        // show when there’s a room above or below

        const ConnectedRooms * connections = getRoom().getConnections() ;
        if ( connections == nilPointer ) throw MayNotBePossible( "nil connections for room " + getRoom().getNameOfRoomDescriptionFile() );

        const std::string & roomAbove = connections->getConnectedRoomAt( "above" );
        const std::string & roomBelow = connections->getConnectedRoomAt( "below" );

        if ( ! roomAbove.empty() || ! roomBelow.empty() )
        {
                int miniatureMidX = this->theImage->getWidth() >> 1 ;
                int aboveY = -2 ;
                int belowY = this->theImage->getHeight() ;
                aboveY -= getSizeOfTile() << 1 ;
                belowY += getSizeOfTile() << 1 ;

                drawVignetteAboveOrBelow( theScreen,
                                                miniatureMidX + leftX,
                                                aboveY + roomOriginOnScreen.second, belowY + roomOriginOnScreen.second,
                                                Color::byName( "green" ).toAllegroColor(),
                                                ! roomAbove.empty(), ! roomBelow.empty() );
        }
}

void Miniature::drawVignetteAboveOrBelow( const allegro::Pict & where, int midX, int aboveY, int belowY, const Color& color, bool drawAbove, bool drawBelow )
{
        if ( color.isFullyTransparent () ) return ;

        if ( ! drawAbove && ! drawBelow ) return ;

        const allegro::Pict & previousWhere = allegro::Pict::getWhereToDraw() ;
        allegro::Pict::setWhereToDraw( where );

        const unsigned int linesEven = ( ( getSizeOfTile() + 1 ) >> 1 ) << 1 ;

        if ( drawAbove ) {
                // draw the middle line
                allegro::drawLine( midX, aboveY - linesEven + 1, midX, aboveY - ( linesEven << 1 ), color.toAllegroColor() );

                int pos = 0;
                for ( unsigned int off = linesEven ; off > 0 ; off -- , pos ++ )
                {
                        allegro::drawLine( midX - off, aboveY - pos, midX - off, aboveY - pos - linesEven, color.toAllegroColor() );
                        allegro::drawLine( midX + off, aboveY - pos, midX + off, aboveY - pos - linesEven, color.toAllegroColor() );
                }
        }

        if ( drawBelow ) {
                // the middle line
                allegro::drawLine( midX, belowY + linesEven - 1, midX, belowY + ( linesEven << 1 ), color.toAllegroColor() );

                int pos = 0;
                for ( unsigned int off = linesEven ; off > 0 ; off -- , pos ++ )
                {
                        allegro::drawLine( midX - off, belowY + pos, midX - off, belowY + pos + linesEven, color.toAllegroColor() );
                        allegro::drawLine( midX + off, belowY + pos, midX + off, belowY + pos + linesEven, color.toAllegroColor() );
                }
        }

        allegro::Pict::setWhereToDraw( previousWhere );
}

void Miniature::drawEastDoorOnMiniature( const allegro::Pict & where, unsigned int tileX, unsigned int tileY, const Color & color )
{
        if ( color.isFullyTransparent () ) return ;

        const std::pair< int, int > & roomOrigin = getOriginOfRoom() ;

        drawIsoTile( where, tileX - 1, tileY, color, false, /* loY */ true, false, false );

        {
                int x = roomOrigin.first + ( ( tileX - 1 ) - ( tileY + 1 ) ) * ( sizeOfTile << 1 ) ;
                int y = roomOrigin.second + ( ( tileY + 1 ) + ( tileX - 1 ) ) * sizeOfTile ;

                setEastDoorNorthernCorner( x, y );
                where.drawPixelAt( x, y, color.toAllegroColor() );
                where.drawPixelAt( x + 1, y, color.toAllegroColor() );
        }

        drawIsoTile( where, tileX, tileY, color, false, false, false, /* hiY */ true );

        {
                int x = roomOrigin.first + ( ( tileX + 1 ) - ( tileY + 1 ) ) * ( sizeOfTile << 1 ) ;
                int y = roomOrigin.second + ( ( tileY + 1 ) + ( tileX + 1 ) ) * sizeOfTile ;
                where.drawPixelAt( x - 2, y - 1, color.toAllegroColor() );
                where.drawPixelAt( x - 1, y - 1, color.toAllegroColor() );
        }

# if defined( DEBUG_MINIATURES ) && DEBUG_MINIATURES
        const std::pair< int, int > & corner = getEastDoorNorthernCorner() ;
        where.drawPixelAt( corner.first, corner.second, Color::byName( "blue" ).toAllegroColor() );
# endif
}

void Miniature::drawWestDoorOnMiniature( const allegro::Pict & where, unsigned int tileX, unsigned int tileY, const Color & color )
{
        if ( color.isFullyTransparent () ) return ;

        const std::pair< int, int > & roomOrigin = getOriginOfRoom() ;

        drawIsoTile( where, tileX - 1, tileY, color, false, /* loY */ true, false, false );

        {
                int x = roomOrigin.first + ( ( tileX - 1 ) - tileY ) * ( sizeOfTile << 1 ) ;
                int y = roomOrigin.second + ( tileY + ( tileX - 1 ) ) * sizeOfTile ;

                setWestDoorNorthernCorner( x + 2, y - 1 );
                where.drawPixelAt( x + 2, y - 1, color.toAllegroColor() );
                where.drawPixelAt( x + 3, y - 1, color.toAllegroColor() );
        }

        drawIsoTile( where, tileX, tileY, color, false, false, false, /* hiY */ true );

        {
                int x = roomOrigin.first + ( ( tileX + 1 ) - tileY ) * ( sizeOfTile << 1 ) ;
                int y = roomOrigin.second + ( tileY + ( tileX + 1 ) ) * sizeOfTile ;
                where.drawPixelAt( x, y - 2, color.toAllegroColor() );
                where.drawPixelAt( x + 1, y - 2, color.toAllegroColor() );
        }

# if defined( DEBUG_MINIATURES ) && DEBUG_MINIATURES
        const std::pair< int, int > & corner = getWestDoorNorthernCorner() ;
        where.drawPixelAt( corner.first, corner.second, Color::byName( "red" ).toAllegroColor() );
# endif
}

void Miniature::drawNorthDoorOnMiniature( const allegro::Pict & where, unsigned int tileX, unsigned int tileY, const Color & color )
{
        if ( color.isFullyTransparent () ) return ;

        const std::pair< int, int > & roomOrigin = getOriginOfRoom() ;

        drawIsoTile( where, tileX, tileY - 1, color, /* loX */ true, false, false, false );

        {
                int x = roomOrigin.first + ( ( tileX + 1 ) - ( tileY - 1 ) ) * ( sizeOfTile << 1 ) ;
                int y = roomOrigin.second + ( ( tileY - 1 ) + ( tileX + 1 ) ) * sizeOfTile ;

                setNorthDoorEasternCorner( x, y );
                where.drawPixelAt( x, y, color.toAllegroColor() );
                where.drawPixelAt( x + 1, y, color.toAllegroColor() );
        }

        drawIsoTile( where, tileX, tileY, color, false, false, /* hiX */ true, false );

        {
                int x = roomOrigin.first + ( ( tileX + 1 ) - ( tileY + 1 ) ) * ( sizeOfTile << 1 ) ;
                int y = roomOrigin.second + ( ( tileY + 1 ) + ( tileX + 1 ) ) * sizeOfTile ;
                where.drawPixelAt( x + 2, y - 1, color.toAllegroColor() );
                where.drawPixelAt( x + 3, y - 1, color.toAllegroColor() );
        }

# if defined( DEBUG_MINIATURES ) && DEBUG_MINIATURES
        const std::pair< int, int > & corner = getNorthDoorEasternCorner() ;
        where.drawPixelAt( corner.first, corner.second, Color::byName( "blue" ).toAllegroColor() );
# endif
}

void Miniature::drawSouthDoorOnMiniature( const allegro::Pict & where, unsigned int tileX, unsigned int tileY, const Color & color )
{
        if ( color.isFullyTransparent () ) return ;

        const std::pair< int, int > & roomOrigin = getOriginOfRoom() ;

        drawIsoTile( where, tileX, tileY - 1, color, /* loX */ true, false, false, false );

        {
                int x = roomOrigin.first + ( tileX - ( tileY - 1 ) ) * ( sizeOfTile << 1 ) ;
                int y = roomOrigin.second + ( ( tileY - 1 ) + tileX ) * sizeOfTile ;

                setSouthDoorEasternCorner( x - 2, y - 1 );
                where.drawPixelAt( x - 2, y - 1, color.toAllegroColor() );
                where.drawPixelAt( x - 1, y - 1, color.toAllegroColor() );
        }

        drawIsoTile( where, tileX, tileY, color, false, false, /* hiX */ true, false );

        {
                int x = roomOrigin.first + ( tileX - ( tileY + 1 ) ) * ( sizeOfTile << 1 ) ;
                int y = roomOrigin.second + ( ( tileY + 1 ) + tileX ) * sizeOfTile ;
                where.drawPixelAt( x, y - 2, color.toAllegroColor() );
                where.drawPixelAt( x + 1, y - 2, color.toAllegroColor() );
        }

# if defined( DEBUG_MINIATURES ) && DEBUG_MINIATURES
        const std::pair< int, int > & corner = getSouthDoorEasternCorner() ;
        where.drawPixelAt( corner.first, corner.second, Color::byName( "red" ).toAllegroColor() );
# endif
}

void Miniature::drawIsoSquare( const allegro::Pict & where,
                                std::pair< int, int > origin,
                                unsigned int tilesX, unsigned int tilesY,
                                const Color & color )
{
        if ( color.isFullyTransparent () ) return ;

        unsigned int posX = origin.first ;
        unsigned int posY = origin.second ;

        for ( unsigned int tile = 0 ; tile < tilesX ; ++ tile ) {
                for ( unsigned int pix = 0 ; pix < this->sizeOfTile ; ++ pix )
                {
                        where.drawPixelAt( posX++, posY, color.toAllegroColor() );
                        where.drawPixelAt( posX++, posY++, color.toAllegroColor() );
                }
        }

        -- posX ; -- posY ;

        for ( unsigned int tile = 0 ; tile < tilesY ; ++ tile ) {
                for ( unsigned int pix = 0 ; pix < this->sizeOfTile ; ++ pix )
                {
                        where.drawPixelAt( posX--, posY, color.toAllegroColor() );
                        where.drawPixelAt( posX--, posY++, color.toAllegroColor() );
                }
        }

        posX = origin.first + 1 ;
        posY = origin.second ;

        for ( unsigned int tile = 0 ; tile < tilesY ; ++ tile ) {
                for ( unsigned int pix = 0 ; pix < this->sizeOfTile ; ++ pix )
                {
                        where.drawPixelAt( posX--, posY, color.toAllegroColor() );
                        where.drawPixelAt( posX--, posY++, color.toAllegroColor() );
                }
        }

        ++ posX ; -- posY ;

        for ( unsigned int tile = 0 ; tile < tilesX ; ++ tile ) {
                for ( unsigned int pix = 0 ; pix < this->sizeOfTile ; ++ pix )
                {
                        where.drawPixelAt( posX++, posY, color.toAllegroColor() );
                        where.drawPixelAt( posX++, posY++, color.toAllegroColor() );
                }
        }
}

void Miniature::drawIsoTile( const allegro::Pict& where,
                                std::pair< int, int > origin,
                                int tileX, int tileY,
                                const Color& color,
                                bool loX, bool loY, bool hiX, bool hiY )
{
        if ( color.isFullyTransparent () ) return ;

        if ( loX ) {
                int x = origin.first  + ( tileX - tileY ) * ( sizeOfTile << 1 ) ;
                int y = origin.second + ( tileY + tileX ) * sizeOfTile ;

                for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ ) {
                        where.drawPixelAt( x++, y, color.toAllegroColor() );
                        where.drawPixelAt( x++, y++, color.toAllegroColor() );
                }
        }

        if ( loY ) {
                int x = origin.first  + ( tileX - tileY ) * ( sizeOfTile << 1 ) + 1 ;
                int y = origin.second + ( tileY + tileX ) * sizeOfTile ;

                for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ ) {
                        where.drawPixelAt( x--, y, color.toAllegroColor() );
                        where.drawPixelAt( x--, y++, color.toAllegroColor() );
                }
        }

        if ( hiX ) {
                int x = origin.first  + ( tileX - tileY ) * ( sizeOfTile << 1 ) - ( ( sizeOfTile - 1 ) << 1 ) ;
                int y = origin.second + ( tileY + tileX ) * sizeOfTile + ( sizeOfTile - 1 ) ;

                for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ ) {
                        where.drawPixelAt( x++, y, color.toAllegroColor() );
                        where.drawPixelAt( x++, y++, color.toAllegroColor() );
                }
        }

        if ( hiY ) {
                int x = origin.first  + ( tileX - tileY ) * ( sizeOfTile << 1 ) + ( ( sizeOfTile - 1 ) << 1 ) + 1 ;
                int y = origin.second + ( tileY + tileX ) * sizeOfTile + ( sizeOfTile - 1 ) ;

                for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ ) {
                        where.drawPixelAt( x--, y, color.toAllegroColor() );
                        where.drawPixelAt( x--, y++, color.toAllegroColor() );
                }
        }
}

void Miniature::fillIsoTile( const allegro::Pict& where, std::pair< int, int > origin, int tileX, int tileY, const Color& color )
{
        if ( color.isFullyTransparent () ) return ;

        for ( unsigned int piw = 0 ; piw < sizeOfTile ; piw ++ )
        {
                int x =  origin.first + ( tileX - tileY ) * ( sizeOfTile << 1 ) - ( piw << 1 ) ;
                int y = origin.second + ( tileY + tileX ) * sizeOfTile + piw ;

                for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ )
                {
                        where.drawPixelAt( x++, y, color.toAllegroColor() );
                        where.drawPixelAt( x++, y++, color.toAllegroColor() );
                }
        }
}

void Miniature::fillIsoTileInside( const allegro::Pict& where, std::pair< int, int > origin, int tileX, int tileY, const Color& color, bool fullFill )
{
        if ( color.isFullyTransparent () ) return ;

        if ( sizeOfTile == 2 )
        {
                int x =  origin.first + ( tileX - tileY ) * ( sizeOfTile << 1 ) ;
                int y = origin.second + ( tileY + tileX ) * sizeOfTile + 1 ;

                where.drawPixelAt( x++, y, color.toAllegroColor() );
                where.drawPixelAt( x++, y++, color.toAllegroColor() );
        }
        else
        {
                for ( unsigned short piw = 1 ; piw < ( sizeOfTile - 1 ) ; piw ++ )
                {
                        int x =  origin.first + ( tileX - tileY ) * ( sizeOfTile << 1 ) - ( piw << 1 ) + 2 ;
                        int y = origin.second + ( tileY + tileX ) * sizeOfTile + piw + 1 ;

                        for ( unsigned short pix = 1 ; pix < ( sizeOfTile - 1 ) ; pix ++ )
                        {
                                if ( fullFill )
                                {
                                        where.drawPixelAt( x, y - 1, color.toAllegroColor() );
                                        where.drawPixelAt( x + 1, y - 1, color.toAllegroColor() );

                                        where.drawPixelAt( x, y + 1, color.toAllegroColor() );
                                        where.drawPixelAt( x + 1, y + 1, color.toAllegroColor() );

                                        where.drawPixelAt( x - 2, y, color.toAllegroColor() );
                                        where.drawPixelAt( x - 1, y, color.toAllegroColor() );

                                        where.drawPixelAt( x + 2, y, color.toAllegroColor() );
                                        where.drawPixelAt( x + 3, y, color.toAllegroColor() );
                                }

                                where.drawPixelAt( x++, y, color.toAllegroColor() );
                                where.drawPixelAt( x++, y++, color.toAllegroColor() );
                        }
                }
        }
}

void Miniature::setOffsetOnScreen ( int leftX, int topY )
{
        int plusX = 0 ;
        int plusY = this->isThereRoomAbove ? ( getSizeOfTile() << 2 ) : 0 ;

        this->offsetOnScreen = std::pair< int, int >( leftX + plusX, topY + plusY ) ;
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

        that->setSizeOfTile( this->sizeOfTile ) ;

        /* const ConnectedRooms * connections = this->room.getConnections() ;
        if ( connections == nilPointer ) return false ;
        const std::string & fileOfConnectedRoom = connections->getConnectedRoomAt( where );
        if ( fileOfConnectedRoom.empty () ) return false ; */

        int shiftY = ( this->sizeOfTile << 1 ) + gap ;
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
                std::cout << ":(( don’t yet know how to connect miniature \"" << this->theImage->getName() << "\" in \"" << where << "\"" << std::endl ;
        # endif
                return false ;
        }

        int adjacentDifferenceX = doorCornerOfThis.first - doorCornerOfThat.first ;
        int adjacentDifferenceY = doorCornerOfThis.second - doorCornerOfThat.second ;

        that->setOffsetOnScreen( this->offsetOnScreen.first + adjacentDifferenceX + shiftX, this->offsetOnScreen.second + adjacentDifferenceY + shiftY ) ;

# if defined( DEBUG_MINIATURES ) && DEBUG_MINIATURES
        std::cout << "\"" << this->theImage->getName() << "\" is connected to \""
                                << that->theImage->getName() << "\" in \"" << where << "\""
                                << std::endl ;
# endif

        return true ;
}
