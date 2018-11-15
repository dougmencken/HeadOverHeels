
#include "Miniature.hpp"
#include "Color.hpp"
#include "FlickeringColor.hpp"
#include "Room.hpp"
#include "RoomConnections.hpp"
#include "Mediator.hpp"
#include "ItemData.hpp"


namespace iso
{

Miniature::Miniature( const Room& roomForMiniature, unsigned int sizeOfTileForMiniature )
        : room( roomForMiniature )
        , sizeOfTile( sizeOfTileForMiniature )
{
        if ( sizeOfTile < 2 ) sizeOfTile = 2 ;
}

void Miniature::draw( const allegro::Pict& where )
{
        const RoomConnections* connections = room.getConnections();
        assert( connections != nilPointer );

        Way wayToNextRoom( "nowhere" );

        std::string roomAbove = connections->findConnectedRoom( "up", &wayToNextRoom );
        std::string roomBelow = connections->findConnectedRoom( "down", &wayToNextRoom );
        std::string roomToTeleport = connections->findConnectedRoom( "via teleport", &wayToNextRoom );
        std::string roomToTeleportToo = connections->findConnectedRoom( "via second teleport", &wayToNextRoom );

        const unsigned int tilesX = room.getTilesX();
        const unsigned int tilesY = room.getTilesY();

        const unsigned int leftXmap = 24;
        const unsigned int topYmap = roomAbove.empty() ? 28 : 24 + ( 3 * sizeOfTile );

        const unsigned int iniX = leftXmap + ( tilesY * ( sizeOfTile << 1 ) );
        const unsigned int iniY = topYmap + ( sizeOfTile << 1 );

        unsigned int firstTileX = 0;
        unsigned int firstTileY = 0;
        unsigned int lastTileX = tilesX - 1;
        unsigned int lastTileY = tilesY - 1;

        Door* eastDoor = room.getDoorAt( "east" );
        Door* southDoor = room.getDoorAt( "south" );
        Door* northDoor = room.getDoorAt( "north" );
        Door* westDoor = room.getDoorAt( "west" );

        Door* eastnorthDoor = room.getDoorAt( "eastnorth" );
        Door* eastsouthDoor = room.getDoorAt( "eastsouth" );
        Door* southeastDoor = room.getDoorAt( "southeast" );
        Door* southwestDoor = room.getDoorAt( "southwest" );
        Door* northeastDoor = room.getDoorAt( "northeast" );
        Door* northwestDoor = room.getDoorAt( "northwest" );
        Door* westnorthDoor = room.getDoorAt( "westnorth" );
        Door* westsouthDoor = room.getDoorAt( "westsouth" );

        if ( northDoor != nilPointer || northeastDoor != nilPointer || northwestDoor != nilPointer )
                firstTileX++ ;

        if ( eastDoor != nilPointer || eastnorthDoor != nilPointer || eastsouthDoor != nilPointer )
                firstTileY++ ;

        if ( southDoor != nilPointer || southeastDoor != nilPointer || southwestDoor != nilPointer )
                lastTileX --;

        if ( westDoor != nilPointer || westnorthDoor != nilPointer || westsouthDoor != nilPointer )
                lastTileY --;

        bool narrowRoomAlongX = ( lastTileY == firstTileY + 1 ) ;
        bool narrowRoomAlongY = ( lastTileX == firstTileX + 1 ) ;

        // draw doors

        if ( eastDoor != nilPointer && ! narrowRoomAlongY )
        {
                unsigned int eastDoorXmid = eastDoor->getCellX() + 1;
                drawEastDoorOnMiniature( where, iniX, iniY, eastDoorXmid, eastDoor->getCellY(), Color::whiteColor() );
        }
        if ( eastnorthDoor != nilPointer )
        {
                unsigned int eastnorthDoorXmid = eastnorthDoor->getCellX() + 1;
                drawEastDoorOnMiniature( where, iniX, iniY, eastnorthDoorXmid, eastnorthDoor->getCellY(), Color::whiteColor() );
        }
        if ( eastsouthDoor != nilPointer )
        {
                unsigned int eastsouthDoorXmid = eastsouthDoor->getCellX() + 1;
                drawEastDoorOnMiniature( where, iniX, iniY, eastsouthDoorXmid, eastsouthDoor->getCellY(), Color::whiteColor() );
        }

        if ( northDoor != nilPointer && ! narrowRoomAlongX )
        {
                unsigned int northDoorYmid = northDoor->getCellY() + 1;
                drawNorthDoorOnMiniature( where, iniX, iniY, northDoor->getCellX(), northDoorYmid, Color::whiteColor() );
        }
        if ( northeastDoor != nilPointer )
        {
                unsigned int northeastDoorYmid = northeastDoor->getCellY() + 1;
                drawNorthDoorOnMiniature( where, iniX, iniY, northeastDoor->getCellX(), northeastDoorYmid, Color::whiteColor() );
        }
        if ( northwestDoor != nilPointer )
        {
                unsigned int northwestDoorYmid = northwestDoor->getCellY() + 1;
                drawNorthDoorOnMiniature( where, iniX, iniY, northwestDoor->getCellX(), northwestDoorYmid, Color::whiteColor() );
        }

        if ( westDoor != nilPointer && ! narrowRoomAlongY )
        {
                unsigned int westDoorXmid = westDoor->getCellX() + 1;
                drawWestDoorOnMiniature( where, iniX, iniY, westDoorXmid, westDoor->getCellY(), Color::whiteColor() );
        }
        if ( westnorthDoor != nilPointer )
        {
                unsigned int westnorthDoorXmid = westnorthDoor->getCellX() + 1;
                drawWestDoorOnMiniature( where, iniX, iniY, westnorthDoorXmid, westnorthDoor->getCellY(), Color::whiteColor() );
        }
        if ( westsouthDoor != nilPointer )
        {
                unsigned int westsouthDoorXmid = westsouthDoor->getCellX() + 1;
                drawWestDoorOnMiniature( where, iniX, iniY, westsouthDoorXmid, westsouthDoor->getCellY(), Color::whiteColor() );
        }

        if ( southDoor != nilPointer && ! narrowRoomAlongX )
        {
                unsigned int southDoorYmid = southDoor->getCellY() + 1;
                drawSouthDoorOnMiniature( where, iniX, iniY, southDoor->getCellX(), southDoorYmid, Color::whiteColor() );
        }
        if ( southeastDoor != nilPointer )
        {
                unsigned int southeastDoorYmid = southeastDoor->getCellY() + 1;
                drawSouthDoorOnMiniature( where, iniX, iniY, southeastDoor->getCellX(), southeastDoorYmid, Color::whiteColor() );
        }
        if ( southwestDoor != nilPointer )
        {
                unsigned int southwestDoorYmid = southwestDoor->getCellY() + 1;
                drawSouthDoorOnMiniature( where, iniX, iniY, southwestDoor->getCellX(), southwestDoorYmid, Color::whiteColor() );
        }

        // draw boundaries of room

        drawIsoSquare( where, iniX, iniY, tilesX, tilesY, Color::gray50Color() );

        // draw walls

        int minXne = firstTileX ;
        int minYne = firstTileY ;
        int maxXsw = lastTileX ;
        int maxYsw = lastTileY ;

        room.getMediator()->lockGridItemsMutex ();

        const std::vector< std::vector< GridItemPtr > > & gridItemsInRoom = room.getGridItems();

        for ( unsigned int column = 0; column < gridItemsInRoom.size(); column ++ )
        {
                const std::vector< GridItemPtr >& columnOfItems = gridItemsInRoom[ column ];

                for ( std::vector< GridItemPtr >::const_iterator gi = columnOfItems.begin (); gi != columnOfItems.end (); ++ gi )
                {
                        const GridItem& item = *( *gi ) ;

                        std::string label = item.getLabel();
                        int tileX = item.getCellX();
                        int tileY = item.getCellY();

                        if ( label == "invisible-wall-x" )
                        {
                                int newPos = tileY - 1 ;
                                if ( newPos < maxYsw ) maxYsw = newPos ;
                        }
                        else if ( label.find( "wall-x" ) != std::string::npos )
                        {
                                int newPos = tileY + 1 ;
                                if ( newPos > minYne ) minYne = newPos ;
                        }

                        if ( label == "invisible-wall-y" )
                        {
                                int newPos = tileX - 1 ;
                                if ( newPos < maxXsw ) maxXsw = newPos ;
                        }
                        else if ( label.find( "wall-y" ) != std::string::npos )
                        {
                                int newPos = tileX + 1 ;
                                if ( newPos > minXne ) minXne = newPos ;
                        }
                }
        }

        room.getMediator()->unlockGridItemsMutex ();

        for ( unsigned int tile = firstTileX ; tile <= lastTileX ; tile ++ )
        {
                if ( static_cast< int >( tile ) >= minXne )
                {
                        if ( ! ( eastDoor != nilPointer && ( static_cast< int >( tile ) == eastDoor->getCellX() + 1 || static_cast< int >( tile ) == eastDoor->getCellX() ) ) &&
                                ! ( eastnorthDoor != nilPointer && ( static_cast< int >( tile ) == eastnorthDoor->getCellX() + 1 || static_cast< int >( tile ) == eastnorthDoor->getCellX() ) ) &&
                                ! ( eastsouthDoor != nilPointer && ( static_cast< int >( tile ) == eastsouthDoor->getCellX() + 1 || static_cast< int >( tile ) == eastsouthDoor->getCellX() ) ) )
                        {
                                drawIsoTile (
                                        where, iniX, iniY,
                                        tile, firstTileY,
                                        Color::whiteColor(),
                                        true, false, false, false );
                        }
                }

                if ( static_cast< int >( tile ) <= maxXsw )
                {
                        if ( ! ( westDoor != nilPointer && ( static_cast< int >( tile ) == westDoor->getCellX() + 1 || static_cast< int >( tile ) == westDoor->getCellX() ) ) &&
                                ! ( westnorthDoor != nilPointer && ( static_cast< int >( tile ) == westnorthDoor->getCellX() + 1 || static_cast< int >( tile ) == westnorthDoor->getCellX() ) ) &&
                                ! ( westsouthDoor != nilPointer && ( static_cast< int >( tile ) == westsouthDoor->getCellX() + 1 || static_cast< int >( tile ) == westsouthDoor->getCellX() ) ) )
                        {
                                drawIsoTile (
                                        where, iniX, iniY,
                                        tile, lastTileY,
                                        Color::whiteColor(),
                                        false, false, true, false );
                        }
                }

                if ( narrowRoomAlongX )
                {
                        if ( northDoor != nilPointer )
                        {
                                drawIsoTile ( where, iniX, iniY, 0, firstTileY,
                                                Color::gray75Color(),
                                                true, false, false, false );

                                drawIsoTile ( where, iniX, iniY, 0, lastTileY,
                                                Color::gray75Color(),
                                                false, false, true, false );
                        }

                        if ( southDoor != nilPointer )
                        {
                                drawIsoTile ( where, iniX, iniY, tilesX - 1, firstTileY,
                                                Color::gray75Color(),
                                                true, false, false, false );

                                drawIsoTile ( where, iniX, iniY, tilesX - 1, lastTileY,
                                                Color::gray75Color(),
                                                false, false, true, false );
                        }
                }
        }

        for ( unsigned int tile = firstTileY ; tile <= lastTileY ; tile ++ )
        {
                if ( static_cast< int >( tile ) <= maxYsw )
                {
                        if ( ! ( southDoor != nilPointer && ( static_cast< int >( tile ) == southDoor->getCellY() + 1 || static_cast< int >( tile ) == southDoor->getCellY() ) ) &&
                                ! ( southeastDoor != nilPointer && ( static_cast< int >( tile ) == southeastDoor->getCellY() + 1 || static_cast< int >( tile ) == southeastDoor->getCellY() ) ) &&
                                ! ( southwestDoor != nilPointer && ( static_cast< int >( tile ) == southwestDoor->getCellY() + 1 || static_cast< int >( tile ) == southwestDoor->getCellY() ) ) )
                        {
                                drawIsoTile (
                                        where, iniX, iniY,
                                        lastTileX, tile,
                                        Color::whiteColor(),
                                        false, false, false, true );
                        }
                }

                if ( static_cast< int >( tile ) >= minYne )
                {
                        if ( ! ( northDoor != nilPointer && ( static_cast< int >( tile ) == northDoor->getCellY() + 1 || static_cast< int >( tile ) == northDoor->getCellY() ) ) &&
                                ! ( northeastDoor != nilPointer && ( static_cast< int >( tile ) == northeastDoor->getCellY() + 1 || static_cast< int >( tile ) == northeastDoor->getCellY() ) ) &&
                                ! ( northwestDoor != nilPointer && ( static_cast< int >( tile ) == northwestDoor->getCellY() + 1 || static_cast< int >( tile ) == northwestDoor->getCellY() ) ) )
                        {
                                drawIsoTile (
                                        where, iniX, iniY,
                                        firstTileX, tile,
                                        Color::whiteColor(),
                                        false, true, false, false );
                        }
                }

                if ( narrowRoomAlongY )
                {
                        if ( eastDoor != nilPointer )
                        {
                                drawIsoTile ( where, iniX, iniY, firstTileX, 0,
                                                Color::gray75Color(),
                                                false, true, false, false );

                                drawIsoTile ( where, iniX, iniY, lastTileX, 0,
                                                Color::gray75Color(),
                                                false, false, false, true );
                        }

                        if ( westDoor != nilPointer )
                        {
                                drawIsoTile ( where, iniX, iniY, firstTileX, tilesY - 1,
                                                Color::gray75Color(),
                                                false, true, false, false );

                                drawIsoTile ( where, iniX, iniY, lastTileX, tilesY - 1,
                                                Color::gray75Color(),
                                                false, false, false, true );
                        }
                }
        }

        room.getMediator()->lockGridItemsMutex ();

        for ( unsigned int column = 0; column < gridItemsInRoom.size(); column ++ )
        {
                const std::vector< GridItemPtr >& columnOfItems = gridItemsInRoom[ column ];

                for ( std::vector< GridItemPtr >::const_iterator gi = columnOfItems.begin (); gi != columnOfItems.end (); ++ gi )
                {
                        const GridItem& item = *( *gi ) ;

                        std::string label = item.getLabel();
                        int tileX = item.getCellX();
                        int tileY = item.getCellY();

                        // show teleports

                        if ( label == "teleport" && ! roomToTeleport.empty() )
                        {
                                fillIsoTile( where, iniX, iniY, tileX, tileY, Color::yellowColor() );
                        }
                        else if ( label == "teleport-too" && ! roomToTeleportToo.empty() )
                        {
                                fillIsoTile( where, iniX, iniY, tileX, tileY, Color::magentaColor() );
                        }

                        // show triple room’s walls

                        if ( label.find( "wall-x" ) != std::string::npos )
                        {
                                if ( label.find( "invisible-wall" ) != std::string::npos )
                                        drawIsoTile( where, iniX, iniY, tileX, tileY, Color::whiteColor(), true, false, false, false );
                                else
                                        drawIsoTile( where, iniX, iniY, tileX, tileY, Color::whiteColor(), false, false, true, false );
                        }
                        if ( label.find( "wall-y" ) != std::string::npos )
                        {
                                if ( label.find( "invisible-wall" ) != std::string::npos )
                                        drawIsoTile( where, iniX, iniY, tileX, tileY, Color::whiteColor(), false, true, false, false );
                                else
                                        drawIsoTile( where, iniX, iniY, tileX, tileY, Color::whiteColor(), false, false, false, true );
                        }
                }
        }

        room.getMediator()->unlockGridItemsMutex ();

        room.getMediator()->lockFreeItemsMutex ();

        const std::vector< FreeItemPtr > & freeItemsInRoom = room.getFreeItems();

        for ( std::vector< FreeItemPtr >::const_iterator fi = freeItemsInRoom.begin (); fi != freeItemsInRoom.end (); ++ fi )
        {
                if ( *fi == nilPointer || ( *fi )->whichKindOfItem() == "player item" ) continue ;
                const FreeItem& item = *( *fi ) ;

                const ItemData* dataOfItem = item.getDataOfItem();
                std::string label = dataOfItem->getLabel();

                // show tools

                if ( label == "handbag" || label == "horn" || label == "donuts" )
                {
                        unsigned int roomTileSize = room.getSizeOfOneTile() ;

                        // free coordinates to tile coordinates
                        int tileX = ( item.getX() - ( ( roomTileSize - dataOfItem->getWidthX() ) >> 1 ) ) / roomTileSize ;
                        int tileY = ( ( item.getY() + ( ( roomTileSize - dataOfItem->getWidthY() ) >> 1 ) + 1 ) / roomTileSize ) - 1 ;

                        fillIsoTile( where, iniX, iniY, tileX, tileY, Color::orangeColor() );
                }

                // show rabbit bonuses

                if ( label == "extra-life" || label == "high-jumps" || label == "high-speed" || label == "shield" )
                {
                        unsigned int roomTileSize = room.getSizeOfOneTile() ;

                        // free coordinates to tile coordinates
                        int tileX = ( item.getX() - ( ( roomTileSize - dataOfItem->getWidthX() ) >> 1 ) ) / roomTileSize ;
                        int tileY = ( ( item.getY() + ( ( roomTileSize - dataOfItem->getWidthY() ) >> 1 ) + 1 ) / roomTileSize ) - 1 ;

                        fillIsoTile( where, iniX, iniY, tileX, tileY, Color::cyanColor() );
                }

                // show fish

                if ( label == "reincarnation-fish" || label == "mortal-fish" )
                {
                        unsigned int roomTileSize = room.getSizeOfOneTile() ;

                        // free coordinates to tile coordinates
                        int tileX = ( item.getX() - ( ( roomTileSize - dataOfItem->getWidthX() ) >> 1 ) ) / roomTileSize ;
                        int tileY = ( ( item.getY() + ( ( roomTileSize - dataOfItem->getWidthY() ) >> 1 ) + 1 ) / roomTileSize ) - 1 ;

                        if ( label == "reincarnation-fish" )
                                fillIsoTile( where, iniX, iniY, tileX, tileY, Color::greenColor() );
                        else if ( label == "mortal-fish" )
                                fillIsoTile( where, iniX, iniY, tileX, tileY, Color::redColor() );
                }
        }

        room.getMediator()->unlockFreeItemsMutex ();

        // show characters in room

        const std::vector< PlayerItemPtr >& charactersInRoom = room.getPlayersYetInRoom();

        for ( std::vector< PlayerItemPtr >::const_iterator pi = charactersInRoom.begin (); pi != charactersInRoom.end (); ++ pi )
        {
                const PlayerItem& character = *( *pi ) ;

                unsigned int roomTileSize = room.getSizeOfOneTile() ;

                // range of tiles where character is
                int xBegin = character.getX() / roomTileSize;
                int xEnd = ( character.getX() + character.getWidthX() - 1 ) / roomTileSize;
                int yBegin = ( character.getY() - character.getWidthY() + 1 ) / roomTileSize;
                int yEnd = character.getY() / roomTileSize;

                /* for ( int x = xBegin ; x <= xEnd ; ++ x )
                        for ( int y = yBegin ; y <= yEnd ; ++ y )
                                drawIsoTile( where, iniX, iniY, x, y, Color::blueColor(), true, true, true, true ) ; */

                int deltaWx = ( roomTileSize - character.getWidthX() ) >> 1 ;
                int deltaWy = ( roomTileSize - character.getWidthY() ) >> 1 ;

                int tileX = ( character.getX() > deltaWx ) ? ( character.getX() - deltaWx ) / roomTileSize : 0 ;
                int tileY = ( ( character.getY() + deltaWy + 1 ) / roomTileSize ) - 1 ;
                if ( tileY < 0 ) tileY = 0 ;

                if ( xBegin == xEnd && yBegin == yEnd )
                {
                        tileX = xBegin ;
                        tileY = yEnd ;
                }
                else
                {
                        if ( tileX < xBegin ) tileX = xBegin ;
                        else if ( tileX > xEnd ) tileX = xEnd ;

                        if ( tileY < yBegin ) tileY = yBegin ;
                        else if ( tileY > yEnd ) tileY = yEnd ;
                }

                fillIsoTileInside( where, iniX, iniY, tileX, tileY, FlickeringColor::flickerWhiteAndTransparent(), true );
        }

        // show when there’s a room above or below

        if ( ! roomAbove.empty() || ! roomBelow.empty() )
        {
                int miniatureMidX = leftXmap + ( tilesY + tilesX ) * sizeOfTile;
                int abovePointerY = iniY - 2;
                int belowPointerY = iniY + ( tilesY + tilesX ) * sizeOfTile;
                abovePointerY -= sizeOfTile << 1;
                belowPointerY += sizeOfTile << 1;

                AllegroColor greenColor = Color::greenColor().toAllegroColor() ;

                const unsigned int linesEven = ( ( sizeOfTile + 1 ) >> 1 ) << 1;

                if ( ! roomAbove.empty() )
                {
                        // draw middle line
                        allegro::drawLine( where, miniatureMidX, abovePointerY - linesEven + 1, miniatureMidX, abovePointerY - ( linesEven << 1 ), greenColor );

                        int pos = 0;
                        for ( unsigned int off = linesEven ; off > 0 ; off -- , pos ++ )
                        {
                                allegro::drawLine( where, miniatureMidX - off, abovePointerY - pos, miniatureMidX - off, abovePointerY - pos - linesEven, greenColor );
                                allegro::drawLine( where, miniatureMidX + off, abovePointerY - pos, miniatureMidX + off, abovePointerY - pos - linesEven, greenColor );
                        }
                }

                if ( ! roomBelow.empty() )
                {
                        // draw middle line
                        allegro::drawLine( where, miniatureMidX, belowPointerY + linesEven - 1, miniatureMidX, belowPointerY + ( linesEven << 1 ), greenColor );

                        int pos = 0;
                        for ( unsigned int off = linesEven ; off > 0 ; off -- , pos ++ )
                        {
                                allegro::drawLine( where, miniatureMidX - off, belowPointerY + pos, miniatureMidX - off, belowPointerY + pos + linesEven, greenColor );
                                allegro::drawLine( where, miniatureMidX + off, belowPointerY + pos, miniatureMidX + off, belowPointerY + pos + linesEven, greenColor );
                        }
                }
        }

        // paint final room

        if ( room.getNameOfFileWithDataAboutRoom() == "finalroom.xml" )
        {
                for ( unsigned int x = 0 ; x < tilesX ; ++ x )
                {
                        for ( unsigned int y = 0 ; y < tilesY ; ++ y )
                        {
                                drawIsoTile( where, iniX, iniY, x, y, Color::greenColor(), true, true, true, true ) ;
                                fillIsoTileInside( where, iniX, iniY, x, y, Color::yellowColor(), true ) ;
                        }
                }
        }
}

void Miniature::drawEastDoorOnMiniature( const allegro::Pict& where, int x0, int y0, unsigned int tilesX, unsigned int tilesY, const Color& color )
{
        if ( color.isKeyColor() ) return ;

        drawIsoTile( where, x0, y0, tilesX - 1, tilesY, color, false, true, false, false );

        {
                int x = x0 + ( ( tilesX - 1 ) - ( tilesY + 1 ) ) * ( sizeOfTile << 1 ) ;
                int y = y0 + ( ( tilesY + 1 ) + ( tilesX - 1 ) ) * sizeOfTile ;
                where.drawPixelAt( x, y, color.toAllegroColor() );
                where.drawPixelAt( x + 1, y, color.toAllegroColor() );
        }

        drawIsoTile( where, x0, y0, tilesX, tilesY, color, false, false, false, true );

        {
                int x = x0 + ( ( tilesX + 1 ) - ( tilesY + 1 ) ) * ( sizeOfTile << 1 ) ;
                int y = y0 + ( ( tilesY + 1 ) + ( tilesX + 1 ) ) * sizeOfTile ;
                where.drawPixelAt( x - 2, y - 1, color.toAllegroColor() );
                where.drawPixelAt( x - 1, y - 1, color.toAllegroColor() );
        }
}

void Miniature::drawNorthDoorOnMiniature( const allegro::Pict& where, int x0, int y0, unsigned int tilesX, unsigned int tilesY, const Color& color )
{
        if ( color.isKeyColor() ) return ;

        drawIsoTile( where, x0, y0, tilesX, tilesY - 1, color, true, false, false, false );

        {
                int x = x0 + ( ( tilesX + 1 ) - ( tilesY - 1 ) ) * ( sizeOfTile << 1 ) ;
                int y = y0 + ( ( tilesY - 1 ) + ( tilesX + 1 ) ) * sizeOfTile ;
                where.drawPixelAt( x, y, color.toAllegroColor() );
                where.drawPixelAt( x + 1, y, color.toAllegroColor() );
        }

        drawIsoTile( where, x0, y0, tilesX, tilesY, color, false, false, true, false );

        {
                int x = x0 + ( ( tilesX + 1 ) - ( tilesY + 1 ) ) * ( sizeOfTile << 1 ) ;
                int y = y0 + ( ( tilesY + 1 ) + ( tilesX + 1 ) ) * sizeOfTile ;
                where.drawPixelAt( x + 2, y - 1, color.toAllegroColor() );
                where.drawPixelAt( x + 3, y - 1, color.toAllegroColor() );
        }
}

void Miniature::drawWestDoorOnMiniature( const allegro::Pict& where, int x0, int y0, unsigned int tilesX, unsigned int tilesY, const Color& color )
{
        if ( color.isKeyColor() ) return ;

        drawIsoTile( where, x0, y0, tilesX - 1, tilesY, color, false, true, false, false );

        {
                int x = x0 + ( ( tilesX - 1 ) - tilesY ) * ( sizeOfTile << 1 ) ;
                int y = y0 + ( tilesY + ( tilesX - 1 ) ) * sizeOfTile ;
                where.drawPixelAt( x + 2, y - 1, color.toAllegroColor() );
                where.drawPixelAt( x + 3, y - 1, color.toAllegroColor() );
        }

        drawIsoTile( where, x0, y0, tilesX, tilesY, color, false, false, false, true );

        {
                int x = x0 + ( ( tilesX + 1 ) - tilesY ) * ( sizeOfTile << 1 ) ;
                int y = y0 + ( tilesY + ( tilesX + 1 ) ) * sizeOfTile ;
                where.drawPixelAt( x, y - 2, color.toAllegroColor() );
                where.drawPixelAt( x + 1, y - 2, color.toAllegroColor() );
        }
}

void Miniature::drawSouthDoorOnMiniature( const allegro::Pict& where, int x0, int y0, unsigned int tilesX, unsigned int tilesY, const Color& color )
{
        if ( color.isKeyColor() ) return ;

        drawIsoTile( where, x0, y0, tilesX, tilesY - 1, color, true, false, false, false );

        {
                int x = x0 + ( tilesX - ( tilesY - 1 ) ) * ( sizeOfTile << 1 ) ;
                int y = y0 + ( ( tilesY - 1 ) + tilesX ) * sizeOfTile ;
                where.drawPixelAt( x - 2, y - 1, color.toAllegroColor() );
                where.drawPixelAt( x - 1, y - 1, color.toAllegroColor() );
        }

        drawIsoTile( where, x0, y0, tilesX, tilesY, color, false, false, true, false );

        {
                int x = x0 + ( tilesX - ( tilesY + 1 ) ) * ( sizeOfTile << 1 ) ;
                int y = y0 + ( ( tilesY + 1 ) + tilesX ) * sizeOfTile ;
                where.drawPixelAt( x, y - 2, color.toAllegroColor() );
                where.drawPixelAt( x + 1, y - 2, color.toAllegroColor() );
        }
}

void Miniature::drawIsoSquare( const allegro::Pict& where, int x0, int y0, unsigned int tilesX, unsigned int tilesY, const Color& color )
{
        if ( color.isKeyColor() ) return ;

        unsigned int posX = x0 ;
        unsigned int posY = y0 ;

        for ( unsigned int tile = 0 ; tile < tilesX ; tile ++ )
        {
                for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ )
                {
                        where.drawPixelAt( posX++, posY, color.toAllegroColor() );
                        where.drawPixelAt( posX++, posY++, color.toAllegroColor() );
                }
        }

        posX--; posY--;

        for ( unsigned int tile = 0 ; tile < tilesY ; tile ++ )
        {
                for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ )
                {
                        where.drawPixelAt( posX--, posY, color.toAllegroColor() );
                        where.drawPixelAt( posX--, posY++, color.toAllegroColor() );
                }
        }

        posX = x0 + 1 ;
        posY = y0 ;

        for ( unsigned int tile = 0 ; tile < tilesY ; tile ++ )
        {
                for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ )
                {
                        where.drawPixelAt( posX--, posY, color.toAllegroColor() );
                        where.drawPixelAt( posX--, posY++, color.toAllegroColor() );
                }
        }

        posX++; posY--;

        for ( unsigned int tile = 0 ; tile < tilesX ; tile ++ )
        {
                for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ )
                {
                        where.drawPixelAt( posX++, posY, color.toAllegroColor() );
                        where.drawPixelAt( posX++, posY++, color.toAllegroColor() );
                }
        }
}

void Miniature::drawIsoTile( const allegro::Pict& where, int x0, int y0, int tileX, int tileY, const Color& color, bool loX, bool loY, bool hiX, bool hiY )
{
        if ( color.isKeyColor() ) return ;

        int x = 0 ;
        int y = 0 ;

        if ( loX )
        {
                x = x0 + ( tileX - tileY ) * ( sizeOfTile << 1 ) ;
                y = y0 + ( tileY + tileX ) * sizeOfTile ;

                for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ )
                {
                        where.drawPixelAt( x++, y, color.toAllegroColor() );
                        where.drawPixelAt( x++, y++, color.toAllegroColor() );
                }
        }

        if ( loY )
        {
                x = x0 + ( tileX - tileY ) * ( sizeOfTile << 1 ) + 1 ;
                y = y0 + ( tileY + tileX ) * sizeOfTile ;

                for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ )
                {
                        where.drawPixelAt( x--, y, color.toAllegroColor() );
                        where.drawPixelAt( x--, y++, color.toAllegroColor() );
                }
        }

        if ( hiX )
        {
                x = x0 + ( tileX - tileY ) * ( sizeOfTile << 1 ) - ( ( sizeOfTile - 1 ) << 1 ) ;
                y = y0 + ( tileY + tileX ) * sizeOfTile + ( sizeOfTile - 1 ) ;

                for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ )
                {
                        where.drawPixelAt( x++, y, color.toAllegroColor() );
                        where.drawPixelAt( x++, y++, color.toAllegroColor() );
                }
        }

        if ( hiY )
        {
                x = x0 + ( tileX - tileY ) * ( sizeOfTile << 1 ) + ( ( sizeOfTile - 1 ) << 1 ) + 1 ;
                y = y0 + ( tileY + tileX ) * sizeOfTile + ( sizeOfTile - 1 ) ;

                for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ )
                {
                        where.drawPixelAt( x--, y, color.toAllegroColor() );
                        where.drawPixelAt( x--, y++, color.toAllegroColor() );
                }
        }
}

void Miniature::fillIsoTile( const allegro::Pict& where, int x0, int y0, int tileX, int tileY, const Color& color )
{
        if ( color.isKeyColor() ) return ;

        for ( unsigned int piw = 0 ; piw < sizeOfTile ; piw ++ )
        {
                int x = x0 + ( tileX - tileY ) * ( sizeOfTile << 1 ) - ( piw << 1 ) ;
                int y = y0 + ( tileY + tileX ) * sizeOfTile + piw ;

                for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ )
                {
                        where.drawPixelAt( x++, y, color.toAllegroColor() );
                        where.drawPixelAt( x++, y++, color.toAllegroColor() );
                }
        }
}

void Miniature::fillIsoTileInside( const allegro::Pict& where, int x0, int y0, int tileX, int tileY, const Color& color, bool fullFill )
{
        if ( color.isKeyColor() ) return ;

        if ( sizeOfTile == 2 )
        {
                int x = x0 + ( tileX - tileY ) * ( sizeOfTile << 1 ) ;
                int y = y0 + ( tileY + tileX ) * sizeOfTile + 1 ;

                where.drawPixelAt( x++, y, color.toAllegroColor() );
                where.drawPixelAt( x++, y++, color.toAllegroColor() );
        }
        else
        {
                for ( unsigned int piw = 1 ; piw < ( sizeOfTile - 1 ) ; piw ++ )
                {
                        int x = x0 + ( tileX - tileY ) * ( sizeOfTile << 1 ) - ( piw << 1 ) + 2 ;
                        int y = y0 + ( tileY + tileX ) * sizeOfTile + piw + 1 ;

                        for ( unsigned int pix = 1 ; pix < ( sizeOfTile - 1 ) ; pix ++ )
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

}
