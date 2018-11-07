
#include "Room.hpp"
#include "Color.hpp"
#include "FloorTile.hpp"
#include "Wall.hpp"
#include "ItemData.hpp"
#include "Mediator.hpp"
#include "Camera.hpp"
#include "GameManager.hpp"
#include "Behavior.hpp"
#include "Elevator.hpp"

#include <tinyxml2.h>

#include <algorithm> // std::for_each


namespace iso
{

Room::Room( const std::string& roomFile, const std::string& scenery, unsigned int xTiles, unsigned int yTiles, unsigned int tileSize, const std::string& floorKind )
: Mediated( )
        , visited( false )
        , connections( nilPointer )
        , nameOfFileWithDataAboutRoom( roomFile )
        , scenery( scenery )
        , tileSize( tileSize )
        , kindOfFloor( floorKind )
        , active( false )
        , whereToDraw( nilPointer )
{
        this->howManyTiles.first = xTiles;
        this->howManyTiles.second = yTiles;
        this->mediator = new Mediator( this );
        this->camera = new Camera( this );

        ///bounds.clear ();
        bounds[ "north" ] = 64000;
        bounds[ "south" ] = 64000;
        bounds[ "east" ] = 64000;
        bounds[ "west" ] = 64000;
        bounds[ "northeast" ] = 64000;
        bounds[ "southeast" ] = 64000;
        bounds[ "southwest" ] = 64000;
        bounds[ "northwest" ] = 64000;
        bounds[ "eastnorth" ] = 64000;
        bounds[ "eastsouth" ] = 64000;
        bounds[ "westnorth" ] = 64000;
        bounds[ "westsouth" ] = 64000;

        ///doors.clear ();
        doors[ "north" ] = nilPointer;
        doors[ "south" ] = nilPointer;
        doors[ "east" ] = nilPointer;
        doors[ "west" ] = nilPointer;
        doors[ "northeast" ] = nilPointer;
        doors[ "southeast" ] = nilPointer;
        doors[ "southwest" ] = nilPointer;
        doors[ "northwest" ] = nilPointer;
        doors[ "eastnorth" ] = nilPointer;
        doors[ "eastsouth" ] = nilPointer;
        doors[ "westnorth" ] = nilPointer;
        doors[ "westsouth" ] = nilPointer;

        // create empty floor
        for ( unsigned int i = 0 ; i < xTiles * yTiles /* + 1 */ ; i++ )
        {
                this->floorTiles.push_back( nilPointer );
        }

        // create sequence of drawing, as example for 8 x 8 grid
        //
        //                00
        //              02  01
        //            05  04  03
        //          09  08  07  06
        //        14  13  12  11  10
        //      20  19  18  17  16  15
        //    27  26  25  24  23  22  21
        //  35  34  33  32  31  30  29  28
        //    42  41  40  39  38  37  36
        //      48  47  46  45  44  43
        //        53  52  51  50  49
        //          57  56  55  54
        //            60  59  58
        //              62  61
        //                63
        //
        // sequence of drawing is
        //
        //                00
        //              01  08
        //            02  09  16
        //          03  10  17  24
        //        04  11  18  25  32
        //      05  12  19  26  33  40
        //    06  13  20  27  34  41  48
        //  07  14  21  28  35  42  49  56
        //    15  22  29  36  43  50  57
        //      23  30  37  44  51  58
        //        31  38  45  52  59
        //          39  46  53  60
        //            47  54  61
        //              55  62
        //                63

        this->drawSequence = new unsigned int[ xTiles * yTiles ];
        size_t pos = 0;

        for ( unsigned int f = 0 ; f < xTiles + yTiles ; f++ )
        {
                for ( int n = yTiles - 1 ; n >= 0 ; n-- )
                {
                        unsigned int index = n * ( xTiles - 1 ) + f;
                        if ( ( index >= n * xTiles ) && ( index < ( n + 1 ) * xTiles ) )
                        {
                                this->drawSequence[ pos++ ] = index;
                        }
                }
        }

        // crea la imagen with suitable dimensions to draw this room

        int roomW = ScreenWidth();
        int roomH = ScreenHeight();

        if ( isSingleRoom() )
        {
                // single room ( id est up to 10 x 10 tiles ) just fits to the screen
                // but image of double or triple room is larger
        }
        else if ( xTiles > maxTilesOfSingleRoom && yTiles > maxTilesOfSingleRoom )
        {
                roomW += 20 * ( tileSize << 1 );
                roomH += 20 * tileSize;
        }
        else if ( xTiles > maxTilesOfSingleRoom || yTiles > maxTilesOfSingleRoom )
        {
                roomW += ( xTiles > maxTilesOfSingleRoom ? ( xTiles - maxTilesOfSingleRoom ) * ( tileSize << 1 ) : 0 )
                                        + ( yTiles > maxTilesOfSingleRoom ? ( yTiles - maxTilesOfSingleRoom ) * ( tileSize << 1 ) : 0 );
                roomH += ( xTiles > maxTilesOfSingleRoom ? ( xTiles - maxTilesOfSingleRoom ) * tileSize : 0 )
                                        + ( yTiles > maxTilesOfSingleRoom ? ( yTiles - maxTilesOfSingleRoom ) * tileSize : 0 );
        }

        whereToDraw = new Picture( roomW, roomH );

        // 0 for pure black shadows, 128 for 50% opacity of shadows, 256 for no shadows
        shadingOpacity = iso::GameManager::getInstance().getDrawShadows () ? 128 /* 0 */ : 256 ;

        std::cout << "created room \"" << nameOfFileWithDataAboutRoom << "\"" << std::endl ;
}

Room::~Room()
{
        // bin doors
        for ( std::map< std::string, Door * >::const_iterator mi = doors.begin (); mi != doors.end (); ++mi )
        {
                delete mi->second ;
        }

        // bin floor
        std::for_each( floorTiles.begin (), floorTiles.end (), DeleteIt() );

        // bin walls
        std::for_each( wallX.begin (), wallX.end (), DeleteIt() );
        std::for_each( wallY.begin (), wallY.end (), DeleteIt() );

        // bin sequence of drawing
        delete drawSequence;

        delete camera;

        delete mediator;

        // bin items

        for ( size_t i = 0; i < gridItems.size(); i++ )
        {
                gridItems[ i ].clear();
        }
        gridItems.clear() ;

        freeItems.clear() ;

        // characters too

        playersYetInRoom.clear() ;
        playersWhoEnteredRoom.clear() ;
}

std::string Room::whichRoom() const
{
        if ( getTilesX() <= maxTilesOfSingleRoom && getTilesY() <= maxTilesOfSingleRoom )
        {
                return "single" ;
        }
        // is it double room along Y
        else if ( getTilesX() <= maxTilesOfSingleRoom && getTilesY() > maxTilesOfSingleRoom )
        {
                return "double along Y" ;
        }
        // is it double room along X
        else if ( getTilesX() > maxTilesOfSingleRoom && getTilesY() <= maxTilesOfSingleRoom )
        {
                return "double along X" ;
        }
        else
        {
                return "triple" ;
        }
}

bool Room::saveAsXML( const std::string& file )
{
        std::cout << "writing room as XML file \"" << file << "\"" << std::endl ;

        tinyxml2::XMLDocument roomXml;

        tinyxml2::XMLNode * root = roomXml.NewElement( "room" );
        roomXml.InsertFirstChild( root );

        tinyxml2::XMLElement* scenery = roomXml.NewElement( "scenery" ) ;
        scenery->SetText( getScenery().c_str () );
        root->InsertEndChild( scenery );

        tinyxml2::XMLElement* xTiles = roomXml.NewElement( "xTiles" ) ;
        xTiles->SetText( getTilesX() );
        root->InsertEndChild( xTiles );

        tinyxml2::XMLElement* yTiles = roomXml.NewElement( "yTiles" ) ;
        yTiles->SetText( getTilesY() );
        root->InsertEndChild( yTiles );

        tinyxml2::XMLElement* tileSize = roomXml.NewElement( "tileSize" ) ;
        tileSize->SetText( getSizeOfOneTile() );
        root->InsertEndChild( tileSize );

        if ( whichRoom() == "triple" )
        {
                tinyxml2::XMLElement* tripleRoomData = roomXml.NewElement( "triple-room-data" ) ;

                for ( std::list< TripleRoomInitialPoint >::iterator it = listOfInitialPointsForTripleRoom.begin () ;
                        it != listOfInitialPointsForTripleRoom.end () ; ++ it )
                {
                        TripleRoomInitialPoint point = *it ;

                        std::string wayOfEntry = point.getWayOfEntry().toString();
                        if ( wayOfEntry != "nowhere" )
                        {
                                tinyxml2::XMLElement* initialPoint = roomXml.NewElement( wayOfEntry.c_str () ) ;
                                initialPoint->SetAttribute( "x", point.getX () );
                                initialPoint->SetAttribute( "y", point.getY () );

                                tripleRoomData->InsertEndChild( initialPoint );
                        }
                }

                tinyxml2::XMLElement* boundX = roomXml.NewElement( "bound-x" ) ;
                boundX->SetAttribute( "minimum", tripleRoomBoundX.first );
                boundX->SetAttribute( "maximum", tripleRoomBoundX.second );
                tripleRoomData->InsertEndChild( boundX );

                tinyxml2::XMLElement* boundY = roomXml.NewElement( "bound-y" ) ;
                boundY->SetAttribute( "minimum", tripleRoomBoundY.first );
                boundY->SetAttribute( "maximum", tripleRoomBoundY.second );
                tripleRoomData->InsertEndChild( boundY );

                root->InsertEndChild( tripleRoomData );
        }

        tinyxml2::XMLElement* floorKind = roomXml.NewElement( "floorKind" ) ;
        floorKind->SetText( getKindOfFloor().c_str () );
        root->InsertEndChild( floorKind );

        // write tiles of floor

        if ( floorTiles.size() > 0 )
        {
                tinyxml2::XMLElement* floor = roomXml.NewElement( "floor" );

                for ( std::vector< FloorTile * >::const_iterator it = floorTiles.begin () ; it != floorTiles.end () ; ++ it )
                {
                        FloorTile* theTile = *it ;
                        if ( theTile != nilPointer )
                        {
                                tinyxml2::XMLElement* tile = roomXml.NewElement( "tile" );

                                tinyxml2::XMLElement* tileX = roomXml.NewElement( "x" );
                                tinyxml2::XMLElement* tileY = roomXml.NewElement( "y" );
                                tileX->SetText( theTile->getCellX() );
                                tileY->SetText( theTile->getCellY() );
                                tinyxml2::XMLElement* tilePicture = roomXml.NewElement( "picture" );
                                tilePicture->SetText( theTile->getRawImage()->getName().c_str () );

                                tile->InsertEndChild( tileX );
                                tile->InsertEndChild( tileY );
                                tile->InsertEndChild( tilePicture );

                                floor->InsertEndChild( tile );
                        }
                }

                root->InsertEndChild( floor );
        }

        // write walls

        if ( wallX.size() + wallY.size() > 0 )
        {
                tinyxml2::XMLElement* walls = roomXml.NewElement( "walls" );

                for ( std::vector< Wall * >::const_iterator it = wallX.begin () ; it != wallX.end () ; ++ it )
                {
                        Wall* segment = *it ;
                        if ( segment != nilPointer )
                        {
                                tinyxml2::XMLElement* wall = roomXml.NewElement( "wall" );

                                wall->SetAttribute( "on", segment->isOnX() ? "x" : "y" );

                                tinyxml2::XMLElement* wallPosition = roomXml.NewElement( "position" );
                                wallPosition->SetText( segment->getPosition() );
                                wall->InsertEndChild( wallPosition );

                                tinyxml2::XMLElement* wallPicture = roomXml.NewElement( "picture" );
                                wallPicture->SetText( segment->getImage()->getName().c_str () );
                                wall->InsertEndChild( wallPicture );

                                walls->InsertEndChild( wall );
                        }
                }

                for ( std::vector< Wall * >::const_iterator it = wallY.begin () ; it != wallY.end () ; ++ it )
                {
                        Wall* segment = *it ;
                        if ( segment != nilPointer )
                        {
                                tinyxml2::XMLElement* wall = roomXml.NewElement( "wall" );

                                wall->SetAttribute( "on", segment->isOnX() ? "x" : "y" );

                                tinyxml2::XMLElement* wallPosition = roomXml.NewElement( "position" );
                                wallPosition->SetText( segment->getPosition() );
                                wall->InsertEndChild( wallPosition );

                                tinyxml2::XMLElement* wallPicture = roomXml.NewElement( "picture" );
                                wallPicture->SetText( segment->getImage()->getName().c_str () );
                                wall->InsertEndChild( wallPicture );

                                walls->InsertEndChild( wall );
                        }
                }

                root->InsertEndChild( walls );
        }

        // write items

        if ( gridItems.size() + freeItems.size() + doors.size() > 0 )
        {
                tinyxml2::XMLElement* items = roomXml.NewElement( "items" );

                // grid items

                for ( unsigned int column = 0; column < gridItems.size (); column ++ )
                {
                        for ( std::vector< GridItemPtr >::const_iterator gi = gridItems[ column ].begin (); gi != gridItems[ column ].end (); ++ gi )
                        {
                                GridItemPtr theItem = *gi ;
                                if ( theItem != nilPointer )
                                {
                                        tinyxml2::XMLElement* item = roomXml.NewElement( "item" );

                                        item->SetAttribute( "x", theItem->getCellX() );
                                        item->SetAttribute( "y", theItem->getCellY() );
                                        int z = theItem->getZ();
                                        z = ( z > Top ) ? z / LayerHeight : Top ;
                                        item->SetAttribute( "z", z );

                                        tinyxml2::XMLElement* itemLabel = roomXml.NewElement( "label" );
                                        itemLabel->SetText( theItem->getLabel().c_str () );
                                        item->InsertEndChild( itemLabel );

                                        tinyxml2::XMLElement* itemOrientation = roomXml.NewElement( "orientation" );
                                        std::string orientation = theItem->getOrientation().toString();
                                        if ( orientation == "nowhere" ) orientation = "none" ;
                                        itemOrientation->SetText( orientation.c_str () );
                                        item->InsertEndChild( itemOrientation );

                                        if ( theItem->getBehavior() != nilPointer )
                                        {
                                                tinyxml2::XMLElement* itemBehavior = roomXml.NewElement( "behavior" );
                                                itemBehavior->SetText( theItem->getBehavior()->getNameOfBehavior().c_str () );
                                                item->InsertEndChild( itemBehavior );
                                        }

                                        tinyxml2::XMLElement* kindOfItem = roomXml.NewElement( "kind" );
                                        kindOfItem->SetText( "griditem" );
                                        item->InsertEndChild( kindOfItem );

                                        items->InsertEndChild( item );
                                }
                        }
                }

                // free items

                for ( std::vector< FreeItemPtr >::const_iterator fi = freeItems.begin (); fi != freeItems.end (); ++ fi )
                {
                        FreeItemPtr theItem = *fi ;
                        if ( theItem != nilPointer && ! theItem->isPartOfDoor() && theItem->whichKindOfItem() != "player item" )
                        {
                                tinyxml2::XMLElement* item = roomXml.NewElement( "item" );

                                item->SetAttribute( "x", theItem->getOriginalCellX() );
                                item->SetAttribute( "y", theItem->getOriginalCellY() );
                                item->SetAttribute( "z", theItem->getOriginalCellZ() );

                                tinyxml2::XMLElement* itemLabel = roomXml.NewElement( "label" );
                                itemLabel->SetText( theItem->getLabel().c_str () );
                                item->InsertEndChild( itemLabel );

                                tinyxml2::XMLElement* itemOrientation = roomXml.NewElement( "orientation" );
                                std::string orientation = theItem->getOrientation().toString();
                                if ( orientation == "nowhere" ) orientation = "none" ;
                                itemOrientation->SetText( orientation.c_str () );
                                item->InsertEndChild( itemOrientation );

                                if ( theItem->getBehavior() != nilPointer )
                                {
                                        std::string behavior = theItem->getBehavior()->getNameOfBehavior();

                                        tinyxml2::XMLElement* itemBehavior = roomXml.NewElement( "behavior" );
                                        itemBehavior->SetText( behavior.c_str () );
                                        item->InsertEndChild( itemBehavior );

                                        if ( behavior == "behavior of elevator" )
                                        {
                                                Elevator* elevatorBehavior = dynamic_cast< Elevator* >( theItem->getBehavior() );

                                                tinyxml2::XMLElement* top = roomXml.NewElement( "top" );
                                                tinyxml2::XMLElement* bottom = roomXml.NewElement( "bottom" );
                                                tinyxml2::XMLElement* ascent = roomXml.NewElement( "ascent" );

                                                top->SetText( elevatorBehavior->getTop() );
                                                bottom->SetText( elevatorBehavior->getBottom() );
                                                ascent->SetText( elevatorBehavior->getAscent() ? "true" : "false" ); // is first movement of elevator ascending

                                                item->InsertEndChild( top );
                                                item->InsertEndChild( bottom );
                                                item->InsertEndChild( ascent );
                                        }
                                }

                                tinyxml2::XMLElement* kindOfItem = roomXml.NewElement( "kind" );
                                kindOfItem->SetText( "freeitem" );
                                item->InsertEndChild( kindOfItem );

                                items->InsertEndChild( item );
                        }
                }

                // doors

                for ( std::map< std::string, Door* >::const_iterator di = doors.begin (); di != doors.end (); ++ di )
                {
                        Door* theDoor = di->second ;
                        if ( theDoor != nilPointer )
                        {
                                tinyxml2::XMLElement* item = roomXml.NewElement( "item" );

                                item->SetAttribute( "x", theDoor->getCellX() );
                                item->SetAttribute( "y", theDoor->getCellY() );
                                int z = theDoor->getZ();
                                z = ( z > Top ) ? z / LayerHeight : Top ;
                                item->SetAttribute( "z", z );

                                tinyxml2::XMLElement* itemLabel = roomXml.NewElement( "label" );
                                itemLabel->SetText( theDoor->getLabel().c_str () );
                                item->InsertEndChild( itemLabel );

                                tinyxml2::XMLElement* itemOrientation = roomXml.NewElement( "orientation" );
                                itemOrientation->SetText( theDoor->getWhereIsDoor().c_str () );
                                item->InsertEndChild( itemOrientation );

                                tinyxml2::XMLElement* kindOfItem = roomXml.NewElement( "kind" );
                                kindOfItem->SetText( "door" );
                                item->InsertEndChild( kindOfItem );

                                items->InsertEndChild( item );
                        }
                }

                root->InsertEndChild( items );
        }

        tinyxml2::XMLError result = roomXml.SaveFile( file.c_str () );
        if ( result != tinyxml2::XML_SUCCESS )
        {
                std::cerr << "can’t save room as \"" << file << "\"" << std::endl ;
                return false;
        }

        return true;
}

std::vector < PlayerItemPtr > Room::getPlayersYetInRoom () const
{
        return playersYetInRoom ;
}

std::vector < PlayerItemPtr > Room::getPlayersWhoEnteredRoom () const
{
        return playersWhoEnteredRoom ;
}

void Room::addFloor( FloorTile * floorTile )
{
        floorTile->setMediator( mediator );
        floorTile->calculateOffset();

        this->floorTiles[ floorTile->getColumn() ] = floorTile;
}

void Room::addWall( Wall * wall )
{
        wall->setMediator( mediator );
        wall->calculateOffset();

        if ( wall->isOnX() )
        {
                this->wallX.push_back( wall );
        }
        else
        {
                this->wallY.push_back( wall );
        }
}

void Room::addDoor( Door * door )
{
        door->setMediator( mediator );

        this->doors[ door->getWhereIsDoor() ] = door;

        // each door is actually three free items
        FreeItemPtr leftJamb = door->getLeftJamb() ;
        FreeItemPtr rightJamb = door->getRightJamb() ;
        FreeItemPtr lintel = door->getLintel() ;

        leftJamb->setPartOfDoor( true );
        rightJamb->setPartOfDoor( true );
        lintel->setPartOfDoor( true );

        addFreeItem( leftJamb );
        addFreeItem( rightJamb );
        addFreeItem( lintel );
}

void Room::updateWallsWithDoors ()
{
        // update positions of walls
        for ( std::vector< Wall * >::iterator wx = this->wallX.begin (); wx != this->wallX.end (); ++wx )
        {
                ( *wx )->calculateOffset();
        }
        for ( std::vector< Wall * >::iterator wy = this->wallY.begin (); wy != this->wallY.end (); ++wy )
        {
                ( *wy )->calculateOffset();
        }
}

void Room::addGridItemToContainer( const GridItemPtr& gridItem )
{
        int column = gridItem->getColumnOfGrid();
        gridItems[ column ].push_back( gridItem );
        // don’t sort here
}

void Room::addGridItem( const GridItemPtr& gridItem )
{
        if ( gridItem == nilPointer ) return;

        if ( ( gridItem->getCellX() < 0 || gridItem->getCellY() < 0 ) ||
                ( gridItem->getCellX() >= static_cast< int >( this->getTilesX() ) ||
                        gridItem->getCellY() >= static_cast< int >( this->getTilesY() ) ) )
        {
                std::cerr << "coordinates for " << gridItem->whichKindOfItem() << " are out of limits" << std::endl ;
                return;
        }

        if ( gridItem->getHeight() < 1 )
        {
                std::cerr << "can’t add " << gridItem->whichKindOfItem() << " which height is zero" << std::endl ;
                return;
        }

        gridItem->setMediator( mediator );

        mediator->clearStackOfCollisions ();

        std::string labelOfItem = gridItem->getLabel() ;
        unsigned int uniqueNumberOfItem = nextNumbers[ labelOfItem ] ;
        if ( uniqueNumberOfItem == 0 ) // is it new label
        {
                 uniqueNumberOfItem = 1;
        }
        nextNumbers[ labelOfItem ] = uniqueNumberOfItem + 1;

        std::ostringstream name;
        name << labelOfItem << " " << util::toStringWithOrdinalSuffix( uniqueNumberOfItem ) <<
                " @" << gridItem->getX() << "," << gridItem->getY() << "," << gridItem->getZ() ;

        gridItem->setUniqueName( name.str() );

        addGridItemToContainer( gridItem );

        if ( gridItem->getZ() != Top )
        {
                // when item goes lower than top, look for collisions
                mediator->lookForCollisionsOf( gridItem->getUniqueName() );
        }
        else
        {
                // whem item goes to top, modify its position on Z
                gridItem->setZ( mediator->findHighestZ( *gridItem ) );
        }

        if ( ! mediator->isStackOfCollisionsEmpty () )
        {
                // can’t add item when there’s some collision
                std::cerr << "there’s collision with " << gridItem->whichKindOfItem() << std::endl ;
                return;
        }

        // calculate offset of item’s image from origin of room
        if ( gridItem->getRawImage() != nilPointer )
        {
                std::pair< int, int > offset (
                        ( ( this->tileSize * ( gridItem->getCellX() - gridItem->getCellY() ) ) << 1 ) - ( gridItem->getRawImage()->getWidth() >> 1 ) + 1,
                        this->tileSize * ( gridItem->getCellX() + gridItem->getCellY() + 2 ) - gridItem->getRawImage()->getHeight() - gridItem->getZ() - 1
                ) ;
                gridItem->setOffset( offset );
        }

        mediator->reshadeWithGridItem( *gridItem );
        mediator->remaskWithGridItem( *gridItem );

#if defined( DEBUG ) && DEBUG
        std::cout << gridItem->whichKindOfItem() << " \"" << gridItem->getUniqueName() << "\" is yet part of room \"" << getNameOfFileWithDataAboutRoom() << "\"" << std::endl ;
#endif
}

void Room::addFreeItemToContainer( const FreeItemPtr& freeItem )
{
        freeItems.push_back( freeItem );
        // no sorting here
}

void Room::addFreeItem( const FreeItemPtr& freeItem )
{
        if ( freeItem == nilPointer ) return;

        if ( freeItem->getX() < 0 || freeItem->getY() < 1 || freeItem->getZ() < Top )
        {
                std::cerr << "coordinates for " << freeItem->whichKindOfItem() << " are out of limits" << std::endl ;
                dumpItemInsideThisRoom( *freeItem );
                return;
        }

        if ( freeItem->getHeight() < 1 || freeItem->getWidthX() < 1 || freeItem->getWidthY() < 1 )
        {
                std::cerr << "can’t add " << freeItem->whichKindOfItem() << " which dimension is zero" << std::endl ;
                return;
        }

        if ( ( freeItem->getX() + static_cast< int >( freeItem->getWidthX() ) > static_cast< int >( this->getTilesX() * this->tileSize ) )
                || ( freeItem->getY() - static_cast< int >( freeItem->getWidthY() ) + 1 < 0 )
                || ( freeItem->getY() > static_cast< int >( this->getTilesY() * this->tileSize ) - 1 ) )
        {
                std::cerr << "coordinates for " << freeItem->whichKindOfItem() << " are out of room" << std::endl ;
                dumpItemInsideThisRoom( *freeItem );
                return;
        }

        freeItem->setMediator( mediator );

        mediator->clearStackOfCollisions ();

        std::string labelOfItem = freeItem->getLabel() ;
        unsigned int uniqueNumberOfItem = nextNumbers[ labelOfItem ] ;
        if ( uniqueNumberOfItem == 0 ) // is it new label
        {
                 uniqueNumberOfItem = 1;
        }
        nextNumbers[ labelOfItem ] = uniqueNumberOfItem + 1;

        freeItem->setUniqueName( labelOfItem + " " + util::toStringWithOrdinalSuffix( uniqueNumberOfItem ) );

        addFreeItemToContainer( freeItem );

        // for item which is placed at some height, look for collisions
        if ( freeItem->getZ() > Top )
        {
                mediator->lookForCollisionsOf( freeItem->getUniqueName() );
        }
        // for item at the top of column
        else
        {
                freeItem->setZ( mediator->findHighestZ( *freeItem ) );
        }

        // collision is found, so can’t add this item
        if ( ! mediator->isStackOfCollisionsEmpty () )
        {
                std::cerr << "there’s collision with " << freeItem->whichKindOfItem() << std::endl ;
                dumpItemInsideThisRoom( *freeItem );
                return;
        }

        // calculate offset of item’s image from origin of room
        if ( freeItem->getRawImage () != nilPointer )
        {
                std::pair< int, int > offset (
                        ( ( freeItem->getX() - freeItem->getY() ) << 1 ) + freeItem->getWidthX() + freeItem->getWidthY() - ( freeItem->getRawImage()->getWidth() >> 1 ) - 1,
                        freeItem->getX() + freeItem->getY() + freeItem->getWidthX() - freeItem->getRawImage()->getHeight() - freeItem->getZ()
                ) ;
                freeItem->setOffset( offset );
        }

        mediator->reshadeWithFreeItem( *freeItem );
        mediator->remaskWithFreeItem( *freeItem );

#if defined( DEBUG ) && DEBUG
        std::cout << freeItem->whichKindOfItem() << " \"" << freeItem->getUniqueName() << "\" is yet in room \"" << getNameOfFileWithDataAboutRoom() << "\"" << std::endl ;
#endif
}

bool Room::addPlayerToRoom( const PlayerItemPtr& playerItem, bool playerEntersRoom )
{
        if ( playerItem == nilPointer ) return false;

        for ( std::vector< PlayerItemPtr >::const_iterator pi = playersYetInRoom.begin (); pi != playersYetInRoom.end (); ++pi )
        {
                if ( playerItem == *pi )
                {
                        // player is in room already
                        return false;
                }
        }

        if ( playerEntersRoom )
        {
                for ( std::vector< PlayerItemPtr >::iterator epi = playersWhoEnteredRoom.begin (); epi != playersWhoEnteredRoom.end (); ++epi )
                {
                        PlayerItemPtr enteredPlayer = *epi ;

                        if ( enteredPlayer->getOriginalLabel() == "headoverheels" && playerItem->getOriginalLabel() != "headoverheels" )
                        {
                                // case when joined character enters room, splits in this room, and one of characters exits & re~enters
                                std::cout << "character \"" << playerItem->getOriginalLabel() << "\" enters but joined \"headoverheels\" entered the same room before" << std::endl ;

                                // bin joined character
                                playersWhoEnteredRoom.erase( epi );
                                /*  epi-- ;  */

                                // add copy of another character as entered
                                copyAnotherCharacterAsEntered( playerItem->getOriginalLabel() );

                                break;
                        }

                        if ( enteredPlayer->getOriginalLabel() == playerItem->getOriginalLabel() )
                        {
                                // case when character returns back to this room, maybe via different way
                                std::cout << "character \"" << playerItem->getOriginalLabel() << "\" already entered this room some time ago" << std::endl ;

                                // bin previous entry
                                playersWhoEnteredRoom.erase( epi );
                                /*  epi-- ;  */

                                break;
                        }
                }
        }

        if ( playerItem->getX() < 0 || playerItem->getY() < 1 || playerItem->getZ() < Top )
        {
                std::cerr << "coordinates for " << playerItem->whichKindOfItem() << " are out of limits" << std::endl ;
                dumpItemInsideThisRoom( *playerItem );
                return false;
        }

        if ( playerItem->getHeight() < 1 || playerItem->getWidthX() < 1 || playerItem->getWidthY() < 1 )
        {
                std::cerr << "can’t add " << playerItem->whichKindOfItem() << " which dimension is zero" << std::endl ;
                return false;
        }

        if ( ( playerItem->getX() + static_cast< int >( playerItem->getWidthX() ) > static_cast< int >( this->getTilesX() * this->tileSize ) )
                || ( playerItem->getY() - static_cast< int >( playerItem->getWidthY() ) + 1 < 0 )
                || ( playerItem->getY() > static_cast< int >( this->getTilesY() * this->tileSize ) - 1 ) )
        {
                std::cerr << "coordinates for " << playerItem->whichKindOfItem() << " are out of room" << std::endl ;
                dumpItemInsideThisRoom( *playerItem );
                return false;
        }

        playerItem->setMediator( mediator );

        mediator->clearStackOfCollisions ();

        std::string labelOfItem = "character " + playerItem->getOriginalLabel() ;
        unsigned int uniqueNumberOfItem = nextNumbers[ labelOfItem ] ;
        if ( uniqueNumberOfItem > 0 ) // is there some player with the same label
        {
                 std::cerr << "oops, can’t add the second character \"" << labelOfItem << "\" to this room" << std::endl ;
                 return false;
        }
        nextNumbers[ labelOfItem ] = uniqueNumberOfItem + 1;

        playerItem->setUniqueName( labelOfItem + " @ " + getNameOfFileWithDataAboutRoom() );

        std::cout << "adding character \"" << playerItem->getOriginalLabel() << "\" to room \"" << getNameOfFileWithDataAboutRoom() << "\"" << std::endl ;

        addFreeItemToContainer( playerItem );

        // for item which is placed at some height, look for collisions
        if ( playerItem->getZ() > Top )
        {
                mediator->lookForCollisionsOf( playerItem->getUniqueName() );
                while ( ! mediator->isStackOfCollisionsEmpty () )
                {
                        playerItem->setZ( playerItem->getZ() + LayerHeight );
                        mediator->clearStackOfCollisions ();
                        mediator->lookForCollisionsOf( playerItem->getUniqueName() );
                }
        }
        // for item at the top of column
        else
        {
                playerItem->setZ( mediator->findHighestZ( *playerItem ) );
        }

        // collision is found, so can’t add this item
        if ( ! mediator->isStackOfCollisionsEmpty () )
        {
                std::cerr << "there’s collision with " << playerItem->whichKindOfItem() << std::endl ;
                dumpItemInsideThisRoom( *playerItem );
                return false;
        }

        // set offset of player’s image from origin of room
        if ( playerItem->getRawImage () != nilPointer )
        {
                std::pair< int, int > offset (
                        ( ( playerItem->getX() - playerItem->getY() ) << 1 ) + playerItem->getWidthX() + playerItem->getWidthY() - ( playerItem->getRawImage()->getWidth() >> 1 ) - 1,
                        playerItem->getX() + playerItem->getY() + playerItem->getWidthX() - playerItem->getRawImage()->getHeight() - playerItem->getZ()
                ) ;
                playerItem->setOffset( offset );
        }

        if ( mediator->getActiveCharacter() == nilPointer )
        {
                mediator->setActiveCharacter( playerItem );
        }

        mediator->reshadeWithFreeItem( *playerItem );
        mediator->remaskWithFreeItem( *playerItem );

        // add player item to room
        this->playersYetInRoom.push_back( playerItem );

        if ( playerEntersRoom )
        {
                PlayerItemPtr copyOfPlayer( new PlayerItem( *playerItem ) );
                copyOfPlayer->setBehaviorOf( playerItem->getBehavior()->getNameOfBehavior() );
                this->playersWhoEnteredRoom.push_back( copyOfPlayer );

                std::cout << "copy of character \"" << copyOfPlayer->getOriginalLabel() << "\""
                                << " with behavior \"" << copyOfPlayer->getBehavior()->getNameOfBehavior() << "\""
                                << " is created to rebuild this room" << std::endl ;
        }

        return true;
}

void Room::dumpItemInsideThisRoom( const Item& item )
{
        std::cout << "   " << item.whichKindOfItem()
                        << " at " << item.getX() << " " << item.getY() << " " << item.getZ()
                        << " with dimensions " << item.getWidthX() << " x " << item.getWidthY() << " x " << item.getHeight()
                        << std::endl
                        << "   inside room \"" << this->nameOfFileWithDataAboutRoom << "\""
                        << " of " << this->getTilesX() << " x " << this->getTilesY() << " tiles"
                        << " each tile of " << this->tileSize << " pixels"
                        << std::endl ;
}

void Room::copyAnotherCharacterAsEntered( const std::string& name )
{
        for ( std::vector< PlayerItemPtr >::const_iterator pi = playersYetInRoom.begin (); pi != playersYetInRoom.end (); ++pi )
        {
                if ( ( *pi )->getOriginalLabel() != name )
                {
                        bool alreadyThere = false;

                        for ( std::vector< PlayerItemPtr >::const_iterator epi = playersWhoEnteredRoom.begin (); epi != playersWhoEnteredRoom.end (); ++epi )
                        {
                                if ( ( *epi )->getOriginalLabel() == ( *pi )->getOriginalLabel() )
                                {
                                        alreadyThere = true;
                                        break;
                                }
                        }

                        if ( ! alreadyThere )
                        {
                                PlayerItemPtr copy( new PlayerItem( *( *pi ) ) );
                                copy->setBehaviorOf( ( *pi )->getBehavior()->getNameOfBehavior() );
                                copy->setWayOfEntry( "just wait" );

                                playersWhoEnteredRoom.push_back( copy );
                        }
                }
        }
}

void Room::removeFloor( FloorTile * floorTile )
{
        this->floorTiles[ floorTile->getColumn () ] = nilPointer;
        delete floorTile ;
}

void Room::removeGridItemByUniqueName( const std::string& uniqueName )
{
        GridItemPtr gridItem ;
        bool found = false ;

        for ( unsigned int column = 0 ; column < gridItems.size() ; column ++ )
        {
                for ( std::vector< GridItemPtr >::iterator g = gridItems[ column ].begin (); g != gridItems[ column ].end (); ++ g )
                {
                        if ( *g != nilPointer && ( *g )->getUniqueName() == uniqueName )
                        {
                                gridItem = *g ;
                                found = true ;

                                std::cout << "removing " << ( *g )->whichKindOfItem() << " \"" << uniqueName <<
                                        "\" from room \"" << getNameOfFileWithDataAboutRoom() << "\"" << std::endl ;

                                gridItems[ column ].erase( g );

                                break ;
                        }
                }

                if ( found ) break ;
        }

        if ( found )
        {
                mediator->reshadeWithGridItem( *gridItem );
                mediator->remaskWithGridItem( *gridItem );
        }
}

void Room::removeFreeItemByUniqueName( const std::string& uniqueName )
{
        FreeItemPtr freeItem ;
        bool found = false ;

        for ( std::vector< FreeItemPtr >::iterator f = freeItems.begin (); f != freeItems.end (); ++ f )
        {
                if ( *f != nilPointer && ( *f )->getUniqueName() == uniqueName )
                {
                        freeItem = *f ;
                        found = true ;

                        std::cout << "removing " << ( *f )->whichKindOfItem() << " \"" << uniqueName <<
                                "\" from room \"" << getNameOfFileWithDataAboutRoom() << "\"" << std::endl ;

                        freeItems.erase( f );

                        break ;
                }
        }

        if ( found )
        {
                mediator->reshadeWithFreeItem( *freeItem );
                mediator->remaskWithFreeItem( *freeItem );
        }
}

bool Room::removePlayerFromRoom( const PlayerItem & playerItem, bool playerExitsRoom )
{
        for ( std::vector< PlayerItemPtr >::iterator pi = playersYetInRoom.begin (); pi != playersYetInRoom.end (); ++pi )
        {
                if ( playerItem.getUniqueName() == ( *pi )->getUniqueName() )
                {
                        std::string character = playerItem.getOriginalLabel();
                        bool wasActive = playerItem.isActiveCharacter() ;

                        removeFreeItemByUniqueName( playerItem.getUniqueName() );

                        nextNumbers[ "character " + character ] -- ;

                        if ( wasActive )
                        {
                                // activate other character, or set it to nil when there’s no other character in room
                                mediator->setActiveCharacter( mediator->getWaitingCharacter() );
                        }

                        playersYetInRoom.erase( pi );
                        /// pi --; // not needed, this iteration is last one due to "return" below

                        // when player leaves room, bin its copy on entry
                        if ( playerExitsRoom )
                        {
                                for ( std::vector< PlayerItemPtr >::iterator epi = playersWhoEnteredRoom.begin (); epi != playersWhoEnteredRoom.end (); ++epi )
                                {
                                        if ( ( *epi )->getOriginalLabel() == character )
                                        {
                                                std::cout << "and removing copy of character \"" << character << "\" created on entry to this room" << std::endl ;

                                                playersWhoEnteredRoom.erase( epi );
                                                /// epi-- ; // not needed because of "break"

                                                break;
                                        }
                                }
                        }

                        return true;
                }
        }

        return false;
}

bool Room::isAnyPlayerStillInRoom () const
{
        return ! playersYetInRoom.empty () ;
}

unsigned int Room::removeBars ()
{
        unsigned int howManyBars = 0;

        mediator->lockGridItemsMutex();
        mediator->lockFreeItemsMutex();

        std::vector< std::string > gridItemsToRemove ;

        for ( unsigned int column = 0; column < gridItems.size(); column ++ )
        {
                std::vector< GridItemPtr > columnOfItems = gridItems[ column ];

                for ( std::vector< GridItemPtr >::const_iterator gi = columnOfItems.begin (); gi != columnOfItems.end (); ++ gi )
                {
                        GridItemPtr gridItem = *gi ;
                        if ( gridItem == nilPointer ) continue ;

                        if ( gridItem->getLabel().find( "bars" ) != std::string::npos )
                        {
                                gridItemsToRemove.push_back( gridItem->getUniqueName() );
                                howManyBars ++;
                        }
                }
        }

        for ( std::vector< std::string >::const_iterator gi = gridItemsToRemove.begin (); gi != gridItemsToRemove.end (); ++ gi )
        {
                removeGridItemByUniqueName( *gi );
        }

        std::vector< std::string > freeItemsToRemove ;

        for ( std::vector< FreeItemPtr >::const_iterator fi = freeItems.begin (); fi != freeItems.end (); ++ fi )
        {
                FreeItemPtr freeItem = *fi ;
                if ( freeItem == nilPointer ) continue ;

                if ( freeItem->getLabel().find( "bars" ) != std::string::npos )
                {
                        freeItemsToRemove.push_back( freeItem->getUniqueName() );
                        howManyBars ++;
                }
        }

        for ( std::vector< std::string >::const_iterator fi = freeItemsToRemove.begin (); fi != freeItemsToRemove.end (); ++ fi )
        {
                removeFreeItemByUniqueName( *fi );
        }

        mediator->unlockFreeItemsMutex();
        mediator->unlockGridItemsMutex();

        return howManyBars;
}

void Room::dontDisappearOnJump ()
{
        for ( unsigned int column = 0; column < gridItems.size(); column ++ )
        {
                std::vector< GridItemPtr > columnOfItems = gridItems[ column ];

                for ( std::vector< GridItemPtr >::const_iterator gi = columnOfItems.begin (); gi != columnOfItems.end (); ++ gi )
                {
                        GridItemPtr gridItem = *gi;
                        if ( gridItem->getBehavior() != nilPointer )
                        {
                                std::string behavior = gridItem->getBehavior()->getNameOfBehavior();
                                if ( behavior == "behavior of disappearance on jump into" ||
                                                behavior == "behavior of slow disappearance on jump into" )
                                {
                                        gridItem->setBehaviorOf( "still" );

                                        const Picture* original = gridItem->getRawImage();
                                        Picture* copy = new Picture( *original );

                                        Color::multiplyWithColor(
                                                copy,
                                                ( behavior == "behavior of slow disappearance on jump into" ) ?
                                                        Color::magentaColor() : Color::redColor() );

                                        gridItem->changeImage( copy );
                                }
                        }
                }
        }

        for ( std::vector< FreeItemPtr >::const_iterator fi = freeItems.begin (); fi != freeItems.end (); ++fi )
        {
                FreeItemPtr freeItem = *fi;
                if ( freeItem->getBehavior() != nilPointer )
                {
                        std::string behavior = freeItem->getBehavior()->getNameOfBehavior();
                        if ( behavior == "behavior of disappearance on jump into" ||
                                        behavior == "behavior of slow disappearance on jump into" )
                        {
                                freeItem->setBehaviorOf( "still" );

                                const Picture* original = freeItem->getRawImage();
                                Picture* copy = new Picture( *original );

                                Color::multiplyWithColor(
                                        copy,
                                        ( behavior == "behavior of slow disappearance on jump into" ) ?
                                                Color::magentaColor() : Color::redColor() );

                                freeItem->changeImage( copy );
                        }
                }
        }
}

void Room::draw( const allegro::Pict& where )
{
        // draw room when it is active and image to draw it isn’t nil
        if ( active && where.isNotNil() )
        {
                // acquire picture before drawing
                allegro::acquirePict( where );

                // clear image of room
                if ( GameManager::getInstance().charactersFly() )
                        where.clearToColor( Color::darkBlueColor().toAllegroColor() );
                else
                        where.clearToColor( Color::blackColor().toAllegroColor() );

                // adjust position of camera
                if ( getTilesX() > maxTilesOfSingleRoom || getTilesY() > maxTilesOfSingleRoom )
                {
                        camera->centerOn( * mediator->getActiveCharacter () );
                }

                // draw tiles of floor
                for ( unsigned int xCell = 0; xCell < getTilesX(); xCell++ )
                {
                        for ( unsigned int yCell = 0; yCell < getTilesY(); yCell++ )
                        {
                                unsigned int column = getTilesX() * yCell + xCell;
                                FloorTile* tile = floorTiles[ column ];

                                if ( tile != nilPointer )  // if there is tile of floor here
                                {
                                        // shade this tile when shadows are on
                                        if ( shadingOpacity < 256 && tile->getRawImage() != nilPointer )
                                        {
                                                mediator->lockGridItemsMutex();
                                                mediator->lockFreeItemsMutex();

                                                if ( tile->getWantShadow() )
                                                {
                                                        mediator->castShadowOnFloor( *tile );
                                                }

                                                mediator->unlockGridItemsMutex();
                                                mediator->unlockFreeItemsMutex();
                                        }

                                        // draw this tile o’floor
                                        tile->draw( where );
                                }
                        }
                }

                // draw walls without doors
                for ( std::vector< Wall * >::iterator wx = this->wallX.begin (); wx != this->wallX.end (); ++wx )
                {
                        ( *wx )->draw( where );
                }
                for ( std::vector< Wall * >::iterator wy = this->wallY.begin (); wy != this->wallY.end (); ++wy )
                {
                        ( *wy )->draw( where );
                }

                mediator->lockGridItemsMutex();
                mediator->lockFreeItemsMutex();

                // draw grid items

                for ( unsigned int i = 0 ; i < getTilesX() * getTilesY() ; i ++ )
                {
                        for ( std::vector< GridItemPtr >::const_iterator gi = gridItems[ drawSequence[ i ] ].begin () ;
                                gi != gridItems[ drawSequence[ i ] ].end () ; ++ gi )
                        {
                                GridItemPtr gridItem = *gi ;

                                if ( shadingOpacity < 256 && gridItem->getRawImage() != nilPointer )
                                {
                                        // cast shadow
                                        if ( gridItem->getWantShadow() )
                                        {
                                                mediator->castShadowOnGridItem( * gridItem );
                                        }
                                }

                                gridItem->draw( where );
                        }
                }

                // for free items there’re two steps before drawing

                // at first shade every free item with grid items and other free items
                for ( std::vector< FreeItemPtr >::const_iterator fi = freeItems.begin (); fi != freeItems.end (); ++ fi )
                {
                        if ( ( *fi )->getRawImage() != nilPointer )
                        {
                                // shade an item when shadows are on
                                if ( shadingOpacity < 256 )
                                {
                                        ( *fi )->requestShadow();
                                }
                        }
                }

                // then mask it and finally draw it
                for ( std::vector< FreeItemPtr >::const_iterator fi = freeItems.begin (); fi != freeItems.end (); ++ fi )
                {
                        if ( ( *fi )->getRawImage() != nilPointer )
                        {
                                ( *fi )->requestMask();
                                ( *fi )->draw( where );
                        }
                }

                mediator->unlockGridItemsMutex();
                mediator->unlockFreeItemsMutex();

                // release picture after drawing
                allegro::releasePict( where );
        }
}

void Room::calculateBounds()
{
        bounds[ "north" ] = doors[ "north" ] || doors[ "northeast" ] || doors[ "northwest" ] || ! hasFloor() ? tileSize : 0;
        bounds[ "east" ] = doors[ "east" ] || doors[ "eastnorth" ] || doors[ "eastsouth" ] || ! hasFloor() ? tileSize : 0;
        bounds[ "south" ] = tileSize * getTilesX() - ( doors[ "south" ] || doors[ "southeast" ] || doors[ "southwest" ]  ? tileSize : 0 );
        bounds[ "west" ] = tileSize * getTilesY() - ( doors[ "west" ] || doors[ "westnorth" ] || doors[ "westsouth" ]  ? tileSize : 0 );

        // door limits of triple room
        bounds[ "northeast" ] = doors[ "northeast" ] ? doors[ "northeast" ]->getLintel()->getX() + doors[ "northeast" ]->getLintel()->getWidthX() - tileSize : bounds[ "north" ];
        bounds[ "northwest" ] = doors[ "northwest" ] ? doors[ "northwest" ]->getLintel()->getX() + doors[ "northwest" ]->getLintel()->getWidthX() - tileSize : bounds[ "north" ];
        bounds[ "southeast" ] = doors[ "southeast" ] ? doors[ "southeast" ]->getLintel()->getX() + tileSize : bounds[ "south" ];
        bounds[ "southwest" ] = doors[ "southwest" ] ? doors[ "southwest" ]->getLintel()->getX() + tileSize : bounds[ "south" ];
        bounds[ "eastnorth" ] = doors[ "eastnorth" ] ? doors[ "eastnorth" ]->getLintel()->getY() + doors[ "eastnorth" ]->getLintel()->getWidthY() - tileSize : bounds[ "east" ];
        bounds[ "eastsouth" ] = doors[ "eastsouth" ] ? doors[ "eastsouth" ]->getLintel()->getY() + doors[ "eastsouth" ]->getLintel()->getWidthY() - tileSize : bounds[ "east" ];
        bounds[ "westnorth" ] = doors[ "westnorth" ] ? doors[ "westnorth" ]->getLintel()->getY() + tileSize : bounds[ "west" ];
        bounds[ "westsouth" ] = doors[ "westsouth" ] ? doors[ "westsouth" ]->getLintel()->getY() + tileSize : bounds[ "west" ];
}

void Room::calculateCoordinates( bool hasNorthDoor, bool hasEastDoor )
{
        bool hasNoFloor = ! hasFloor() ;

        // don’t count tiles taken by doors
        int xGrid = hasNorthDoor || hasNoFloor ? getTilesX() - 1 : getTilesX() ;
        int yGrid = hasEastDoor || hasNoFloor ? getTilesY() - 1 : getTilesY() ;

        // if there’s south or west door then variable is odd, in such case subtract one more 1
        xGrid += ( xGrid & 1 ? -1 : 0 );
        yGrid += ( yGrid & 1 ? -1 : 0 );

        // the origin for 8 x 8, that’s without doors, room is at ( width / 2, height / 3 )
        // for smaller room the origin moves to center on 8 x 8 grid, as example for 6 x 8 room X moves one tile
        int middlePointX = ( xGrid > 8 || yGrid > 8 ? getWhereToDraw()->getWidth() : ScreenWidth() ) >> 1 ;
        int middlePointY = ScreenHeight() / 3 ;
        this->coordinates.first = middlePointX
                                - ( hasNorthDoor || hasNoFloor ? ( tileSize << 1 ) : 0)
                                + ( hasEastDoor || hasNoFloor ? ( tileSize << 1 ) : 0 );
        this->coordinates.second = middlePointY
                                 - ( hasNorthDoor || hasNoFloor ? tileSize : 0 )
                                 - ( hasEastDoor || hasNoFloor ? tileSize : 0 );

        if ( xGrid <= 8 && yGrid <= 8 )
        {
                this->coordinates.first += ( yGrid - xGrid ) * tileSize;
                this->coordinates.second += ( ( 16 - xGrid - yGrid ) >> 1 ) * tileSize;
        }
        else
        {
                this->coordinates.first = ( getWhereToDraw()->getWidth() - ( ( xGrid + yGrid ) * ( tileSize << 1 ) ) ) / 2
                                        + ( yGrid * ( tileSize << 1 ) )
                                        - ( hasNorthDoor ? ( tileSize << 1 ) : 0 )
                                        + ( hasEastDoor ? ( tileSize << 1 ) : 0 );
        }
}

bool Room::activateCharacterByLabel( const std::string& player )
{
        for ( std::vector< PlayerItemPtr >::const_iterator pi = playersYetInRoom.begin (); pi != playersYetInRoom.end (); ++pi )
        {
                if ( player == ( *pi )->getLabel() )
                {
                        mediator->setActiveCharacter( *pi );
                        return true;
                }
        }

        return false;
}

void Room::activate()
{
        this->mediator->beginUpdate();
        this->active = true;
}

void Room::deactivate()
{
        this->mediator->endUpdate();
        this->active = false;
}

bool Room::swapCharactersInRoom ()
{
        return mediator->pickNextCharacter();
}

bool Room::continueWithAlivePlayer ()
{
        PlayerItemPtr previouslyAlivePlayer = mediator->getActiveCharacter();

        if ( previouslyAlivePlayer->getLives() == 0 )
        {
                // look for next player
                std::vector< PlayerItemPtr >::iterator i = playersYetInRoom.begin () ;
                while ( i != playersYetInRoom.end () )
                {
                        if ( *i != nilPointer && ( *i )->getLabel() == previouslyAlivePlayer->getLabel() ) break;
                        ++ i ;
                }
                ++i ;
                mediator->setActiveCharacter( i != this->playersYetInRoom.end () ? ( *i ) : *this->playersYetInRoom.begin () );

                // is there other alive player in room
                if ( previouslyAlivePlayer != mediator->getActiveCharacter() )
                {
                        removePlayerFromRoom( *previouslyAlivePlayer, true );

                        this->activate();
                        return true;
                }

                return false;
        }

        return true;
}

bool Room::calculateEntryCoordinates( const Way& wayOfEntry, int widthX, int widthY, int northBound, int eastBound, int southBound, int westBound, int* x, int* y, int* z )
{
        bool result = false;
        int differentSizeDeltaX = 0;
        int differentSizeDeltaY = 0;

        if ( wayOfEntry.toString() == "up" || wayOfEntry.toString() == "down" ||
                        wayOfEntry.toString() == "via teleport" || wayOfEntry.toString() == "via second teleport" )
        {
                const int limitOfSingleRoom = maxTilesOfSingleRoom * tileSize ;

                // calculate the difference on X axis when moving from single room to double room or vice versa
                if ( ( bounds[ "south" ] - bounds[ "north" ] > limitOfSingleRoom && southBound - northBound <= limitOfSingleRoom ) ||
                        ( bounds[ "south" ] - bounds[ "north" ] <= limitOfSingleRoom && southBound - northBound > limitOfSingleRoom ) )
                {
                        differentSizeDeltaX = ( bounds[ "south" ] - bounds[ "north" ] - southBound + northBound ) >> 1;
                }

                // calculate the difference on Y axis when moving from single room to double room or vice versa
                if ( ( bounds[ "west" ] - bounds[ "east" ] > limitOfSingleRoom && westBound - eastBound <= limitOfSingleRoom ) ||
                        ( bounds[ "west" ] - bounds[ "east" ] <= limitOfSingleRoom && westBound - eastBound > limitOfSingleRoom ) )
                {
                        differentSizeDeltaY = ( bounds[ "west" ] - bounds[ "east" ] - westBound + eastBound ) >> 1;
                }
        }

        // calculate coordinates according to way of entry
        switch ( wayOfEntry.getIntegerOfWay () )
        {
                case Way::North:
                        *x = bounds[ "north" ] - tileSize + 1;
                        *y = doors[ "north" ]->getLeftJamb()->getY() - doors[ "north" ]->getLeftJamb()->getWidthY();
                        *z = doors[ "north" ]->getLeftJamb()->getZ();
                        result = true;
                        break;

                case Way::South:
                        *x = bounds[ "south" ] + tileSize - widthX;
                        *y = doors[ "south" ]->getLeftJamb()->getY() - doors[ "south" ]->getLeftJamb()->getWidthY();
                        *z = doors[ "south" ]->getLeftJamb()->getZ();
                        result = true;
                        break;

                case Way::East:
                        *x = doors[ "east" ]->getLeftJamb()->getX() + doors[ "east" ]->getLeftJamb()->getWidthX();
                        *y = bounds[ "east" ] - tileSize + widthY;
                        *z = doors[ "east" ]->getLeftJamb()->getZ();
                        result = true;
                        break;

                case Way::West:
                        *x = doors[ "west" ]->getLeftJamb()->getX() + doors[ "west" ]->getLeftJamb()->getWidthX();
                        *y = bounds[ "west" ] + tileSize - 1;
                        *z = doors[ "west" ]->getLeftJamb()->getZ();
                        result = true;
                        break;

                case Way::Northeast:
                        *x = bounds[ "northeast" ];
                        *y = doors[ "northeast" ]->getLeftJamb()->getY() - doors[ "northeast" ]->getLeftJamb()->getWidthY();
                        *z = doors[ "northeast" ]->getLeftJamb()->getZ();
                        result = true;
                        break;

                case Way::Northwest:
                        *x = bounds[ "northwest" ];
                        *y = doors[ "northwest" ]->getLeftJamb()->getY() - doors[ "northwest" ]->getLeftJamb()->getWidthY();
                        *z = doors[ "northwest" ]->getLeftJamb()->getZ();
                        result = true;
                        break;

                case Way::Southeast:
                        *x = bounds[ "southeast" ] - widthX;
                        *y = doors[ "southeast" ]->getLeftJamb()->getY() - doors[ "southeast" ]->getLeftJamb()->getWidthY();
                        *z = doors[ "southeast" ]->getLeftJamb()->getZ();
                        result = true;
                        break;

                case Way::Southwest:
                        *x = bounds[ "southwest" ] - widthX;
                        *y = doors[ "southwest" ]->getLeftJamb()->getY() - doors[ "southwest" ]->getLeftJamb()->getWidthY();
                        *z = doors[ "southwest" ]->getLeftJamb()->getZ();
                        result = true;
                        break;

                case Way::Eastnorth:
                        *x = doors[ "eastnorth" ]->getLeftJamb()->getX() + doors[ "eastnorth" ]->getLeftJamb()->getWidthX();
                        *y = bounds[ "eastnorth" ] + widthY;
                        *z = doors[ "eastnorth" ]->getLeftJamb()->getZ();
                        result = true;
                        break;

                case Way::Eastsouth:
                        *x = doors[ "eastsouth" ]->getLeftJamb()->getX() + doors[ "eastsouth" ]->getLeftJamb()->getWidthX();
                        *y = bounds[ "eastsouth" ] + widthY;
                        *z = doors[ "eastsouth" ]->getLeftJamb()->getZ();
                        result = true;
                        break;

                case Way::Westnorth:
                        *x = doors[ "westnorth" ]->getLeftJamb()->getX() + doors[ "westnorth" ]->getLeftJamb()->getWidthX();
                        *y = bounds[ "westnorth" ] - widthY;
                        *z = doors[ "westnorth" ]->getLeftJamb()->getZ();
                        result = true;
                        break;

                case Way::Westsouth:
                        *x = doors[ "westsouth" ]->getLeftJamb()->getX() + doors[ "westsouth" ]->getLeftJamb()->getWidthX();
                        *y = bounds[ "westsouth" ] - widthY;
                        *z = doors[ "westsouth" ]->getLeftJamb()->getZ();
                        result = true;
                        break;

                case Way::Up:
                        *x += bounds[ "north" ] - northBound + ( ( bounds[ "south" ] - bounds[ "north" ] - southBound + northBound ) >> 1 );
                        *x += ( *x < ( ( bounds[ "south" ] - bounds[ "north" ] ) >> 1 ) ? -differentSizeDeltaX : differentSizeDeltaX );
                        *y += bounds[ "east" ] - eastBound + ( ( bounds[ "west" ] - bounds[ "east" ] - westBound + eastBound ) >> 1 );
                        *y += ( *y < ( ( bounds[ "west" ] - bounds[ "east" ] ) >> 1 ) ? -differentSizeDeltaY : differentSizeDeltaY );
                        *z = MaxLayers * LayerHeight;
                        result = true;
                        break;

                case Way::Down:
                        *x += bounds[ "north" ] - northBound + ( ( bounds[ "south" ] - bounds[ "north" ] - southBound + northBound ) >> 1 );
                        *x += ( *x < ( ( bounds[ "south" ] - bounds[ "north" ] ) >> 1 ) ? -differentSizeDeltaX : differentSizeDeltaX );
                        *y += bounds[ "east" ] - eastBound + ( ( bounds[ "west" ] - bounds[ "east" ] - westBound + eastBound) >> 1 );
                        *y += ( *y < ( ( bounds[ "west" ] - bounds[ "east" ]) >> 1 ) ? -differentSizeDeltaY : differentSizeDeltaY );
                        *z = LayerHeight;
                        result = true;
                        break;

                case Way::ByTeleport:
                case Way::ByTeleportToo:
                        *x += bounds[ "north" ] - northBound + ( ( bounds[ "south" ] - bounds[ "north" ] - southBound + northBound ) >> 1 );
                        *x += ( *x < ( ( bounds[ "south" ] - bounds[ "north" ] ) >> 1 ) ? -differentSizeDeltaX : differentSizeDeltaX );
                        *y += differentSizeDeltaY + bounds[ "east" ] - eastBound + ( ( bounds[ "west" ] - bounds[ "east" ] - westBound + eastBound ) >> 1 );
                        *y += ( *y < ( ( bounds[ "west" ] - bounds[ "east" ] ) >> 1 ) ? -differentSizeDeltaY : differentSizeDeltaY );
                        result = true;
                        break;

                default:
                        ;
        }

        return result;
}

int Room::getXCenterForItem( const ItemData* data )
{
        return
                ( ( getLimitAt( "south" ) - getLimitAt( "north" ) + data->getWidthX() ) >> 1 )
                        + ( hasDoorAt( "north" ) ? getSizeOfOneTile() >> 1 : 0 )
                                - ( hasDoorAt( "south" ) ? getSizeOfOneTile() >> 1 : 0 ) ;
}

int Room::getYCenterForItem( const ItemData* data )
{
        return
                ( ( getLimitAt( "west" ) - getLimitAt( "east" ) + data->getWidthY() ) >> 1 )
                        + ( hasDoorAt( "east" ) ? getSizeOfOneTile() >> 1 : 0 )
                                - ( hasDoorAt( "west" ) ? getSizeOfOneTile() >> 1 : 0 )
                                        - 1 ;
}

void Room::addTripleRoomInitialPoint( const Way& way, int x, int y )
{
        listOfInitialPointsForTripleRoom.push_back( TripleRoomInitialPoint( way, x, y ) );
}

void Room::assignTripleRoomBounds( int minX, int maxX, int minY, int maxY )
{
        tripleRoomBoundX.first = minX;
        tripleRoomBoundX.second = maxX;
        tripleRoomBoundY.first = minY;
        tripleRoomBoundY.second = maxY;
}

TripleRoomInitialPoint Room::findInitialPointOfEntryToTripleRoom( const Way& way )
{
        for ( std::list< TripleRoomInitialPoint >::iterator it = listOfInitialPointsForTripleRoom.begin () ;
                it != listOfInitialPointsForTripleRoom.end () ; ++ it )
        {
                if ( ( *it ).getWayOfEntry().getIntegerOfWay() == way.getIntegerOfWay() )
                {
                        return ( *it ) ;
                }
        }

        return TripleRoomInitialPoint( Way( "nowhere" ), 0, 0 ) ;
}

}
