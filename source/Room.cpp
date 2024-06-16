
#include "Room.hpp"

#include "Color.hpp"
#include "FloorTile.hpp"
#include "WallPiece.hpp"
#include "DescriptionOfItem.hpp"
#include "ItemDescriptions.hpp"
#include "Mediator.hpp"
#include "Camera.hpp"
#include "GameManager.hpp"
#include "Behavior.hpp"
#include "Elevator.hpp"

# define SHOW_ORIGIN_OF_ROOM    0

# if defined( SHOW_ORIGIN_OF_ROOM ) && SHOW_ORIGIN_OF_ROOM
  # include "FlickeringColor.hpp"
# endif

#include <tinyxml2.h>

#include <algorithm> // std::for_each

/* static */
const std::string Room::Sides_Of_Room[ Room::Sides ] =
                        {  "northeast", "northwest", "north", "southeast", "southwest", "south",
                                "eastnorth", "eastsouth", "east", "westnorth", "westsouth", "west"  };

Room::Room( const std::string & roomFile,
                unsigned short xTiles, unsigned short yTiles,
                        const std::string & roomScenery, const std::string & floorKind )
        : Mediated( )
        , nameOfRoomDescriptionFile( roomFile )
        , howManyTilesOnX( xTiles )
        , howManyTilesOnY( yTiles )
        , scenery( roomScenery )
        , kindOfFloor( floorKind )
        , color( "white" )
        , connections( nilPointer )
        , drawingSequence( new unsigned int[ xTiles * yTiles ] )
        , camera( new Camera( this ) )
        , whereToDraw( )
{
        setMediator( new Mediator( this ) );

        for ( unsigned int i = 0 ; i < Room::Sides ; ++ i ) {
                bounds[ Room::Sides_Of_Room[ i ] ] = -1 ;
                doors[ Room::Sides_Of_Room[ i ] ] = nilPointer ;
        }

        // make nil floor
        for ( unsigned int i = 0 ; i < xTiles * yTiles /* + 1 */ ; ++ i )
        {
                this->floorTiles.push_back( nilPointer );
        }

        // make the room’s grid structure
        for ( unsigned int i = 0 ; i < xTiles * yTiles /* + 1 */ ; ++ i )
        {
                this->gridItems.push_back( std::vector< GridItemPtr > () );
        }

        // create the sequence of drawing, as example for the 8 x 8 grid
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
        // the sequence of drawing is
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

        size_t pos = 0;

        for ( unsigned int f = 0 ; f < xTiles + yTiles ; ++ f ) {
                for ( unsigned int g = yTiles ; g > 0 ; -- g )
                {
                        unsigned int n = g - 1 ; // g = n + 1
                        unsigned int index = n * ( xTiles - 1 ) + f ;

                        if ( ( index >= n * xTiles ) && ( index < g * xTiles ) )
                                drawingSequence[ pos ++ ] = index ;
                }
        }

        // 0 for black shadows, 128 for 50% opacity, 256 for no shadows
        this->shadingTransparency = GameManager::getInstance().getCastShadows () ? 128 /* 0 */ : 256 ;

        IF_DEBUG( std::cout << "created room \"" << getNameOfRoomDescriptionFile() << "\"" << std::endl )
}

Room::~Room()
{
        delete getMediator() ;

        // bin doors
        for ( std::map< std::string, Door * >::const_iterator mi = doors.begin (); mi != doors.end (); ++mi )
        {
                delete mi->second ;
        }

        // bin floor
        std::for_each( floorTiles.begin (), floorTiles.end (), DeleteIt() );

        // bin walls
        std::for_each( wallPieces.begin (), wallPieces.end (), DeleteIt() );

        // bin the sequence of drawing
        delete [] drawingSequence ;

        delete this->camera ;

        // bin items

        for ( size_t i = 0; i < gridItems.size(); i++ )
        {
                gridItems[ i ].clear();
        }
        gridItems.clear() ;

        freeItems.clear() ;

        // characters too

        charactersYetInRoom.clear() ;
        charactersWhoEnteredRoom.clear() ;
}

bool Room::saveAsXML( const std::string & file )
{
        tinyxml2::XMLDocument roomXml ; // make new XML document

        tinyxml2::XMLElement * root = roomXml.NewElement( "room" );
        roomXml.InsertFirstChild( root );

        if ( ! getScenery().empty () )
                root->SetAttribute( "scenery", getScenery().c_str () );

        tinyxml2::XMLElement* xTiles = roomXml.NewElement( "xTiles" ) ;
        xTiles->SetText( getTilesOnX() );
        root->InsertEndChild( xTiles );

        tinyxml2::XMLElement* yTiles = roomXml.NewElement( "yTiles" ) ;
        yTiles->SetText( getTilesOnY() );
        root->InsertEndChild( yTiles );

        tinyxml2::XMLElement* roomColor = roomXml.NewElement( "color" ) ;
        roomColor->SetText( getColor().c_str () );
        root->InsertEndChild( roomColor );

        tinyxml2::XMLElement* floorKind = roomXml.NewElement( "floorKind" ) ;
        floorKind->SetText( getKindOfFloor().c_str () );
        root->InsertEndChild( floorKind );

        // write tiles without floor

        if ( tilesWithoutFloor.size() > 0 )
        {
                for ( std::set< std::pair< int, int > >::const_iterator it = tilesWithoutFloor.begin () ; it != tilesWithoutFloor.end () ; ++ it )
                {
                        tinyxml2::XMLElement* nofloor = roomXml.NewElement( "nofloor" );

                        nofloor->SetAttribute( "x", it->first );
                        nofloor->SetAttribute( "y", it->second );

                        root->InsertEndChild( nofloor );
                }
        }

        // write tiles of the floor

        if ( getScenery().empty () )
        {
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
                                        tilePicture->SetText( theTile->getRawImage().getName().c_str () );

                                        tile->InsertEndChild( tileX );
                                        tile->InsertEndChild( tileY );
                                        tile->InsertEndChild( tilePicture );

                                        floor->InsertEndChild( tile );
                                }
                        }

                        root->InsertEndChild( floor );
                }
        }

        // write walls

        tinyxml2::XMLElement* walls = nilPointer ;

        if ( wallPieces.size() > 0 )
        {
                walls = roomXml.NewElement( "walls" );

                for ( std::vector< WallPiece * >::const_iterator it = wallPieces.begin () ; it != wallPieces.end () ; ++ it )
                {
                        WallPiece* piece = *it ;
                        if ( piece != nilPointer )
                        {
                                tinyxml2::XMLElement* wall = roomXml.NewElement( "wall" );

                                wall->SetAttribute( "along", piece->isAlongX() ? "x" : "y" );

                                tinyxml2::XMLElement* wallPosition = roomXml.NewElement( "position" );
                                wallPosition->SetText( piece->getPosition() );
                                wall->InsertEndChild( wallPosition );

                                tinyxml2::XMLElement* wallPicture = roomXml.NewElement( "picture" );
                                wallPicture->SetText( piece->getNameOfImage().c_str () );
                                wall->InsertEndChild( wallPicture );

                                walls->InsertEndChild( wall );
                        }
                }
        }

        if ( walls != nilPointer )
                root->InsertEndChild( walls );

        // write items

        if ( gridItems.size() + freeItems.size() + doors.size() > 0 )
        {
                tinyxml2::XMLElement* items = roomXml.NewElement( "items" );

                // the grid items

                for ( unsigned int column = 0; column < gridItems.size (); column ++ ) {
                        for ( std::vector< GridItemPtr >::const_iterator gi = gridItems[ column ].begin (); gi != gridItems[ column ].end (); ++ gi )
                        {
                                GridItemPtr theItem = *gi ;
                                if ( theItem == nilPointer ) continue ;

                                const std::string & kindOfItem = theItem->getOriginalKind() ;

                                if ( kindOfItem.find( "wall" ) != std::string::npos
                                                && kindOfItem.find( "invisible-wall" ) == std::string::npos
                                                        && walls != nilPointer )
                                {
                                        bool isAlongX = ( theItem->getCellY() == 0 && kindOfItem.find( "-x" ) != std::string::npos );
                                        bool isAlongY = ( theItem->getCellX() == 0 && kindOfItem.find( "-y" ) != std::string::npos );
                                        if ( isAlongX || isAlongY )
                                        {
                                                // write a wall grid item as a wall piece

                                                tinyxml2::XMLElement* wall = roomXml.NewElement( "wall" );

                                                wall->SetAttribute( "along", isAlongX ? "x" : "y" );

                                                tinyxml2::XMLElement* wallPosition = roomXml.NewElement( "position" );
                                                wallPosition->SetText( isAlongX ? theItem->getCellX() : theItem->getCellY() );
                                                wall->InsertEndChild( wallPosition );

                                                tinyxml2::XMLElement* wallPicture = roomXml.NewElement( "picture" );
                                                wallPicture->SetText( ( kindOfItem + ".png" ).c_str () );
                                                wall->InsertEndChild( wallPicture );

                                                walls->InsertEndChild( wall );

                                                continue ;
                                        }
                                }

                                tinyxml2::XMLElement* item = roomXml.NewElement( "item" );

                                item->SetAttribute( "x", theItem->getCellX() );
                                item->SetAttribute( "y", theItem->getCellY() );
                                int z = theItem->getZ();
                                z = ( z > Room::FloorZ ) ? ( z / Room::LayerHeight ) : Room::FloorZ ;
                                item->SetAttribute( "z", z );

                                tinyxml2::XMLElement* kind = roomXml.NewElement( "kind" );
                                kind->SetText( kindOfItem.c_str () );
                                item->InsertEndChild( kind );

                                const std::string & heading = theItem->getHeading ();
                                const std::string orientation = ( heading.empty() || heading == "none" ) ? "" : heading ;
                                if ( ! orientation.empty() ) {
                                        tinyxml2::XMLElement* itemOrientation = roomXml.NewElement( "orientation" );
                                        itemOrientation->SetText( orientation.c_str () );
                                        item->InsertEndChild( itemOrientation );
                                }

                                if ( theItem->getBehavior() != nilPointer )
                                {
                                        tinyxml2::XMLElement* itemBehavior = roomXml.NewElement( "behavior" );
                                        itemBehavior->SetText( theItem->getBehavior()->getNameOfBehavior().c_str () );
                                        item->InsertEndChild( itemBehavior );
                                }

                                tinyxml2::XMLElement* itemClass = roomXml.NewElement( "class" );
                                itemClass->SetText( "griditem" );
                                item->InsertEndChild( itemClass );

                                items->InsertEndChild( item );
                        }
                }

                // the free items

                for ( std::vector< FreeItemPtr >::const_iterator fi = freeItems.begin (); fi != freeItems.end (); ++ fi )
                {
                        FreeItemPtr theItem = *fi ;
                        if ( theItem != nilPointer && ! theItem->isPartOfDoor() && theItem->whichItemClass() != "avatar item" )
                        {
                                tinyxml2::XMLElement* item = roomXml.NewElement( "item" );

                                item->SetAttribute( "x", theItem->getInitialCellX () );
                                item->SetAttribute( "y", theItem->getInitialCellY () );
                                item->SetAttribute( "z", theItem->getInitialCellZ () );

                                tinyxml2::XMLElement* kind = roomXml.NewElement( "kind" );
                                kind->SetText( theItem->getOriginalKind().c_str () );
                                item->InsertEndChild( kind );

                                const std::string & heading = theItem->getHeading ();
                                const std::string orientation = ( heading.empty() || heading == "none" ) ? "" : heading ;
                                if ( ! orientation.empty() ) {
                                        tinyxml2::XMLElement* itemOrientation = roomXml.NewElement( "orientation" );
                                        itemOrientation->SetText( orientation.c_str () );
                                        item->InsertEndChild( itemOrientation );
                                }

                                if ( theItem->getBehavior() != nilPointer )
                                {
                                        const behaviors::Behavior & behavior = * theItem->getBehavior() ;
                                        std::string whichBehavior = behavior.getNameOfBehavior();

                                        tinyxml2::XMLElement* itemBehavior = roomXml.NewElement( "behavior" );
                                        itemBehavior->SetText( whichBehavior.c_str () );
                                        item->InsertEndChild( itemBehavior );

                                        if ( whichBehavior == "behavior of elevator" )
                                        {
                                                const behaviors::Elevator & behaviorOfElevator = dynamic_cast< const behaviors::Elevator & >( behavior );

                                                tinyxml2::XMLElement* top = roomXml.NewElement( "top" );
                                                tinyxml2::XMLElement* bottom = roomXml.NewElement( "bottom" );
                                                tinyxml2::XMLElement* ascent = roomXml.NewElement( "ascent" );

                                                top->SetText( behaviorOfElevator.getTop() );
                                                bottom->SetText( behaviorOfElevator.getBottom() );
                                                ascent->SetText( behaviorOfElevator.getAscent() ? "true" : "false" ); // does it go up first or down?

                                                item->InsertEndChild( top );
                                                item->InsertEndChild( bottom );
                                                item->InsertEndChild( ascent );
                                        }
                                }

                                tinyxml2::XMLElement* itemClass = roomXml.NewElement( "class" );
                                itemClass->SetText( "freeitem" );
                                item->InsertEndChild( itemClass );

                                items->InsertEndChild( item );
                        }
                }

                // the doors

                for ( std::map< std::string, Door* >::const_iterator di = doors.begin (); di != doors.end (); ++ di )
                {
                        Door* theDoor = di->second ;
                        if ( theDoor != nilPointer )
                        {
                                tinyxml2::XMLElement* item = roomXml.NewElement( "item" );

                                item->SetAttribute( "x", theDoor->getCellX() );
                                item->SetAttribute( "y", theDoor->getCellY() );
                                int z = theDoor->getZ();
                                z = ( z > Room::FloorZ ) ? ( z / Room::LayerHeight ) : Room::FloorZ ;
                                item->SetAttribute( "z", z );

                                tinyxml2::XMLElement* kind = roomXml.NewElement( "kind" );
                                kind->SetText( theDoor->getKind().c_str () );
                                item->InsertEndChild( kind );

                                tinyxml2::XMLElement* whereIsDoor = roomXml.NewElement( "where" );
                                whereIsDoor->SetText( theDoor->getWhereIsDoor().c_str () );
                                item->InsertEndChild( whereIsDoor );

                                tinyxml2::XMLElement* itemClass = roomXml.NewElement( "class" );
                                itemClass->SetText( "door" );
                                item->InsertEndChild( itemClass );

                                items->InsertEndChild( item );
                        }
                }

                root->InsertEndChild( items );
        }

        tinyxml2::XMLError result = roomXml.SaveFile( file.c_str () );
        if ( result != tinyxml2::XML_SUCCESS ) {
                std::cerr << "can’t save room as \"" << file << "\"" << std::endl ;
                return false ;
        }

        return true ;
}

void Room::addFloorTile( FloorTile * floorTile )
{
        if ( floorTile == nilPointer ) return ;

        floorTile->setMediator( getMediator() );

        // bin old tile, if any
        removeFloorAt( floorTile->getCellX(), floorTile->getCellY() );

        this->floorTiles[ floorTile->getIndexOfColumn() ] = floorTile;
}

void Room::addWallPiece( WallPiece * segment )
{
        if ( segment == nilPointer ) return ;

        segment->setMediator( getMediator() );
        this->wallPieces.push_back( segment );
}

void Room::addDoor( Door * door )
{
        if ( door == nilPointer ) return ;

        door->setMediator( getMediator() );

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

        camera->recenterRoom () ;
}

void Room::convertWallsNearDoors ()
{
        // convert walls near doors to grid items to draw them after doors

        std::vector< WallPiece * > wallsToBin ;

        if ( hasDoorOn( "north" ) || hasDoorOn( "northeast" ) || hasDoorOn( "northwest" ) )
        {
                Door* northDoor = getDoorOn( "north" );
                Door* northeastDoor = getDoorOn( "northeast" );
                Door* northwestDoor = getDoorOn( "northwest" );

                for ( std::vector< WallPiece * >::iterator wy = wallPieces.begin (); wy != wallPieces.end (); ++ wy )
                {
                        if ( ( *wy )->isAlongX() ) continue ;

                        WallPiece* segment = *wy ;

                        if ( ( northDoor != nilPointer && segment->getPosition() == northDoor->getCellY() + 2 ) ||
                                ( northeastDoor != nilPointer && segment->getPosition() == northeastDoor->getCellY() + 2 ) ||
                                ( northwestDoor != nilPointer && segment->getPosition() == northwestDoor->getCellY() + 2 ) )
                        {
                                std::string imageName = segment->getNameOfImage() ;
                                imageName = imageName.substr( 0, imageName.find_last_of( "." ) );

                                const DescriptionOfItem* descriptionOfWall = ItemDescriptions::descriptions().getDescriptionByKind( imageName );
                                if ( descriptionOfWall != nilPointer ) {
                                        addGridItem( GridItemPtr( new GridItem( *descriptionOfWall, 0, segment->getPosition(), 0 ) ) );
                                        wallsToBin.push_back( segment );
                                }
                        }
                }
        }
        if ( hasDoorOn( "east" ) || hasDoorOn( "eastnorth" ) || hasDoorOn( "eastsouth" ) )
        {
                Door* eastDoor = getDoorOn( "east" );
                Door* eastnorthDoor = getDoorOn( "eastnorth" );
                Door* eastsouthDoor = getDoorOn( "eastsouth" );

                for ( std::vector< WallPiece * >::iterator wx = wallPieces.begin (); wx != wallPieces.end (); ++ wx )
                {
                        if ( ( *wx )->isAlongY() ) continue ;

                        WallPiece * segment = *wx ;

                        if ( ( eastDoor != nilPointer && segment->getPosition() == eastDoor->getCellX() + 2 ) ||
                                ( eastnorthDoor != nilPointer && segment->getPosition() == eastnorthDoor->getCellX() + 2 ) ||
                                ( eastsouthDoor != nilPointer && segment->getPosition() == eastsouthDoor->getCellX() + 2 ) )
                        {
                                std::string imageName = segment->getNameOfImage() ;
                                imageName = imageName.substr( 0, imageName.find_last_of( "." ) );

                                const DescriptionOfItem* descriptionOfWall = ItemDescriptions::descriptions().getDescriptionByKind( imageName );
                                if ( descriptionOfWall != nilPointer ) {
                                        addGridItem( GridItemPtr( new GridItem( *descriptionOfWall, segment->getPosition(), 0, 0 ) ) );
                                        wallsToBin.push_back( segment );
                                }
                        }
                }
        }

        bool resort = ! wallsToBin.empty() ;
        while ( ! wallsToBin.empty() ) {
                WallPiece* sayBye = wallsToBin.back ();
                wallsToBin.pop_back() ;
                removeWallPiece( sayBye );
        }

        if ( resort )
                // sort the pieces
                std::sort( wallPieces.begin (), wallPieces.end (), WallPiece::compareWallPointers );
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
                ( gridItem->getCellX() >= static_cast< int >( this->getTilesOnX() ) ||
                        gridItem->getCellY() >= static_cast< int >( this->getTilesOnY() ) ) )
        {
                std::cerr << "coordinates for " << gridItem->whichItemClass() << " are out of limits" << std::endl ;
                return;
        }

        if ( gridItem->getUnsignedHeight() == 0 )
        {
                std::cerr << "can’t add " << gridItem->whichItemClass() << " which height is zero" << std::endl ;
                return;
        }

        gridItem->setMediator( getMediator() );

        getMediator()->clearCollisions ();

        const std::string & kindOfItem = gridItem->getKind () ;
        unsigned int uniqueNumberOfItem = nextNumbers[ kindOfItem ] ;
        if ( uniqueNumberOfItem == 0 ) // is it the new kind
        {
                 uniqueNumberOfItem = 1;
        }
        nextNumbers[ kindOfItem ] = uniqueNumberOfItem + 1;

        std::ostringstream name;
        name << kindOfItem << " " << util::toStringWithOrdinalSuffix( uniqueNumberOfItem ) <<
                " @" << gridItem->getX() << "," << gridItem->getY() << "," << gridItem->getZ() ;

        gridItem->setUniqueName( name.str() );

        addGridItemToContainer( gridItem );

        if ( gridItem->getZ() != Room::FloorZ ) {
                // when the position on Z is set, look for collisions there
                getMediator()->collectCollisionsWith( gridItem->getUniqueName() );
        } else
        {       // an item with z = FloorZ goes above the previously added ones in the column with the same x and y
                // and its position on Z is calculated
                gridItem->setZ( getMediator()->findHighestZ( *gridItem ) );
                std::cout << "for " << gridItem->whichItemClass() << " \"" << gridItem->getUniqueName()
                                        << "\" the calculated “Z above others” is " << gridItem->getZ() << std::endl ;
        }

        if ( getMediator()->isThereAnyCollision () )
        {
                // can’t add item when there’s some collision
                std::cerr << "there’s collision with " << gridItem->whichItemClass() << std::endl ;
                return ;
        }

        // update the offset of the item’s image from the room’s origin
        gridItem->updateImageOffset() ;

        getMediator()->wantShadowFromGridItem( *gridItem );
        getMediator()->wantToMaskWithGridItem( *gridItem );

#if defined( DEBUG ) && DEBUG
        std::cout << gridItem->whichItemClass() << " \"" << gridItem->getUniqueName() << "\" is yet part of room \"" << getNameOfRoomDescriptionFile() << "\"" << std::endl ;
#endif
}

void Room::sortGridItems ()
{
        getMediator()->lockGridItemsMutex ();

        for ( unsigned int column = 0; column < this->gridItems.size(); ++ column )
        {
                if ( ! this->gridItems[ column ].empty() )
                        std::sort( this->gridItems[ column ].begin (), this->gridItems[ column ].end () );
        }

        getMediator()->unlockGridItemsMutex ();
}

void Room::addFreeItemToContainer( const FreeItemPtr& freeItem )
{
        freeItems.push_back( freeItem );
        // no sorting here
}

void Room::addFreeItem( const FreeItemPtr& freeItem )
{
        if ( freeItem == nilPointer ) return;

        if ( freeItem->getX() < 0 || freeItem->getY() < 1 || freeItem->getZ() < Room::FloorZ )
        {
                std::cerr << "coordinates for " << freeItem->whichItemClass() << " are out of limits" << std::endl ;
                dumpItemInsideThisRoom( *freeItem );
                return;
        }

        if ( freeItem->getUnsignedHeight() == 0 || freeItem->getUnsignedWidthX() == 0 || freeItem->getUnsignedWidthY() == 0 )
        {
                std::cerr << "can’t add " << freeItem->whichItemClass() << " which dimension is zero" << std::endl ;
                return;
        }

        if ( ( freeItem->getX() + freeItem->getWidthX() > static_cast< int >( this->getTilesOnX() * getSizeOfOneTile() ) )
                || ( freeItem->getY() - freeItem->getWidthY() + 1 < 0 )
                        || ( freeItem->getY() > static_cast< int >( this->getTilesOnY() * getSizeOfOneTile() ) - 1 ) )
        {
                std::cerr << "coordinates for " << freeItem->whichItemClass() << " are out of room" << std::endl ;
                dumpItemInsideThisRoom( *freeItem );
                return;
        }

        freeItem->setMediator( getMediator() );

        getMediator()->clearCollisions ();

        const std::string & kindOfItem = freeItem->getKind () ;
        unsigned int uniqueNumberOfItem = nextNumbers[ kindOfItem ] ;
        if ( uniqueNumberOfItem == 0 ) // is it the new kind
        {
                 uniqueNumberOfItem = 1;
        }
        nextNumbers[ kindOfItem ] = uniqueNumberOfItem + 1;

        freeItem->setUniqueName( kindOfItem + " " + util::toStringWithOrdinalSuffix( uniqueNumberOfItem ) );

        addFreeItemToContainer( freeItem );

        // for item which is placed at some height, look for collisions
        if ( freeItem->getZ() > Room::FloorZ )
        {
                getMediator()->collectCollisionsWith( freeItem->getUniqueName() );
        }
        // for item at the top of column
        else
        {
                freeItem->setZ( getMediator()->findHighestZ( *freeItem ) );
        }

        // collision is found, so can’t add this item
        if ( getMediator()->isThereAnyCollision () )
        {
                std::cerr << "there’s collision with " << freeItem->whichItemClass() << std::endl ;
                dumpItemInsideThisRoom( *freeItem );
                return;
        }

        getMediator()->wantShadowFromFreeItem( *freeItem );
        getMediator()->wantToMaskWithFreeItem( *freeItem );

#if defined( DEBUG ) && DEBUG
        std::cout << freeItem->whichItemClass() << " \"" << freeItem->getUniqueName() << "\" is yet in room \"" << getNameOfRoomDescriptionFile() << "\"" << std::endl ;
#endif
}

void Room::sortFreeItems ()
{
        getMediator()->lockFreeItemsMutex ();

        std::sort( this->freeItems.begin (), this->freeItems.end () );

        // remask items after sorting because overlappings may change
        for ( std::vector< FreeItemPtr >::iterator f = this->freeItems.begin (); f != this->freeItems.end (); ++ f )
                ( *f )->setWantMaskTrue ();

        getMediator()->unlockFreeItemsMutex ();
}

bool Room::placeCharacterInRoom( const std::string & name,
                                        bool justEntered,
                                        int x, int y, int z,
                                        const std::string & heading, const std::string & wayOfEntry )
{
        GameInfo & gameInfo = GameManager::getInstance().getGameInfo () ;

        std::string nameOfCharacter( "none" );

        if ( gameInfo.getLivesByName( name ) > 0 )
        {
                nameOfCharacter = name ;
        }
        else
        if ( name == "headoverheels" )
        {
                // when the composite character ran out of lives, check if any of the simple characters survived
                if ( gameInfo.getLivesByName( "head" ) > 0 )
                        nameOfCharacter = "head" ;
                else
                if ( gameInfo.getLivesByName( "heels" ) > 0 )
                        nameOfCharacter = "heels" ;
        }
        else
        {
                if ( gameInfo.getLivesByName( "head" ) == 0 && gameInfo.getLivesByName( "heels" ) == 0 )
                        nameOfCharacter = "game over" ;
        }

        const DescriptionOfItem * itemDescription = ItemDescriptions::descriptions().getDescriptionByKind( nameOfCharacter );

        if ( ( nameOfCharacter == "headoverheels" || nameOfCharacter == "head" || nameOfCharacter == "heels" )
                        && itemDescription != nilPointer )
        {
                if ( gameInfo.getLivesByName( nameOfCharacter ) > 0 ) {
                        // there are lives left, create the character
                        AvatarItemPtr character( new AvatarItem( *itemDescription, x, y, z, heading ) );
                        assert( character != nilPointer );

                        // automove on entry
                        if ( wayOfEntry.empty() ) {
                                // perhaps the character walks through a door
                                for ( std::map< std::string, Door* >::const_iterator di = this->doors.begin() ; di != this->doors.end() ; ++ di ) {
                                        Door* door = di->second ;
                                        if ( door != nilPointer && door->isUnderDoor( *character ) ) {
                                                character->setWayOfEntry( di->first );
                                                break ;
                                        }
                                }
                        } else
                                character->setWayOfEntry( wayOfEntry );

                        return placeCharacterInRoom( character, justEntered );
                }
        }

        return false ;
}

bool Room::placeCharacterInRoom( const AvatarItemPtr & character, bool justEntered )
{
        for ( std::vector< AvatarItemPtr >::const_iterator pi = charactersYetInRoom.begin (); pi != charactersYetInRoom.end (); ++pi )
        {
                if ( character == *pi )
                        // this character is already in room
                        return true ;
        }

        if ( justEntered )
        {
                for ( std::vector< AvatarItemPtr >::iterator it = charactersWhoEnteredRoom.begin (); it != charactersWhoEnteredRoom.end (); ++ it )
                {
                        AvatarItemPtr characterEntered = *it ;

                        if ( characterEntered->getOriginalKind() == "headoverheels" && character->getOriginalKind() != "headoverheels" )
                        {
                                // case when joined character enters room, splits in this room, and one of characters exits & re~enters
                                std::cout << "character \"" << character->getOriginalKind() << "\" enters but joined \"headoverheels\" entered the same room before" << std::endl ;

                                // bin joined character
                                charactersWhoEnteredRoom.erase( it );
                                /*  -- it ;  */

                                // add copy the other character as entered
                                copyAnotherCharacterAsEntered( character->getOriginalKind() );

                                break;
                        }

                        if ( characterEntered->getOriginalKind() == character->getOriginalKind() )
                        {
                                // case when character returns back to this room, maybe via different way
                                std::cout << "character \"" << character->getOriginalKind() << "\" already entered this room some time ago" << std::endl ;

                                // bin previous entry
                                charactersWhoEnteredRoom.erase( it );
                                /*  -- it ;  */

                                break;
                        }
                }
        }

        if ( character->getX() < 0 || character->getY() < 1 || character->getZ() < Room::FloorZ )
        {
                std::cerr << "coordinates for " << character->whichItemClass() << " are out of limits" << std::endl ;
                dumpItemInsideThisRoom( *character );
                return false;
        }

        if ( character->getUnsignedHeight() == 0 || character->getUnsignedWidthX() == 0 || character->getUnsignedWidthY() == 0 )
        {
                std::cerr << "can’t add " << character->whichItemClass() << " which dimension is zero" << std::endl ;
                return false;
        }

        if ( ( character->getX() + character->getWidthX() > static_cast< int >( this->getTilesOnX() * getSizeOfOneTile() ) )
                || ( character->getY() - character->getWidthY() + 1 < 0 )
                        || ( character->getY() > static_cast< int >( this->getTilesOnY() * getSizeOfOneTile() ) - 1 ) )
        {
                std::cerr << "coordinates for " << character->whichItemClass() << " are out of room" << std::endl ;
                dumpItemInsideThisRoom( *character );
                return false;
        }

        character->setMediator( getMediator() );

        getMediator()->clearCollisions ();

        std::string kindOfItem = "character " + character->getOriginalKind() ;
        unsigned int uniqueNumberOfItem = nextNumbers[ kindOfItem ] ;
        if ( uniqueNumberOfItem > 0 ) // is there some character with the same name
        {
                 std::cerr << "oops, can’t add the second character \"" << kindOfItem << "\" to this room" << std::endl ;
                 return false;
        }
        nextNumbers[ kindOfItem ] = uniqueNumberOfItem + 1;

        character->setUniqueName( kindOfItem + " @ " + getNameOfRoomDescriptionFile() );

        std::cout << "adding character \"" << character->getOriginalKind() << "\" to room \"" << getNameOfRoomDescriptionFile() << "\"" << std::endl ;

        addFreeItemToContainer( character );

        // if character is placed at the given height, look for collisions
        if ( character->getZ() > Room::FloorZ )
        {
                getMediator()->collectCollisionsWith( character->getUniqueName() );
                while ( getMediator()->isThereAnyCollision () )
                {
                        character->setZ( character->getZ() + Room::LayerHeight );
                        getMediator()->clearCollisions ();
                        getMediator()->collectCollisionsWith( character->getUniqueName() );
                }
        }
        else    // for character at the top of column
                character->setZ( getMediator()->findHighestZ( *character ) );

        // collision is found, so can’t add this item
        if ( getMediator()->isThereAnyCollision () )
        {
                std::cerr << "there’s collision with " << character->whichItemClass() << std::endl ;
                dumpItemInsideThisRoom( *character );
                return false;
        }

        if ( getMediator()->getActiveCharacter() == nilPointer )
                getMediator()->activateCharacterByName( character->getOriginalKind() );

        getMediator()->wantShadowFromFreeItem( *character );
        getMediator()->wantToMaskWithFreeItem( *character );

        // add avatar item to room
        this->charactersYetInRoom.push_back( character );

        if ( justEntered )
        {
                AvatarItemPtr copyOfCharacter( new AvatarItem( *character ) );
                copyOfCharacter->setBehaviorOf( character->getBehavior()->getNameOfBehavior() );
                this->charactersWhoEnteredRoom.push_back( copyOfCharacter );

                std::cout << "character \"" << copyOfCharacter->getOriginalKind() << "\""
                                << " was copied to restart this room" << std::endl ;
        }

        // perhaps the character appeared in the room by means of teleportation
        if ( character->getWayOfEntry().empty () ) {
                character->canAdvanceTo( 0, 0, -1 ); // is there a teletransportation device under the character
                if ( getMediator()->collisionWithBehavingAs( "behavior of teletransport" ) != nilPointer )
                        character->setWayOfEntry( "via teleport" );
        }

        return true ;
}

void Room::dumpItemInsideThisRoom( const Item & item )
{
        std::cout << "   " << item.whichItemClass() << " \"" << item.getUniqueName() << "\""
                        << " at " << item.getX() << " " << item.getY() << " " << item.getZ()
                        << " with dimensions " << item.getWidthX() << " x " << item.getWidthY() << " x " << item.getHeight()
                        << std::endl
                        << "   inside room \"" << getNameOfRoomDescriptionFile() << "\""
                        << " of " << getTilesOnX () << " x " << getTilesOnY () << " tiles"
                        << " each tile of " << getSizeOfOneTile () << " pixels"
                        << std::endl ;
}

void Room::copyAnotherCharacterAsEntered( const std::string & name )
{
        for ( std::vector< AvatarItemPtr >::const_iterator pi = charactersYetInRoom.begin (); pi != charactersYetInRoom.end (); ++pi )
        {
                if ( ( *pi )->getOriginalKind() != name )
                {
                        bool alreadyThere = false;

                        for ( std::vector< AvatarItemPtr >::const_iterator it = charactersWhoEnteredRoom.begin (); it != charactersWhoEnteredRoom.end (); ++ it )
                        {
                                if ( ( *it )->getOriginalKind() == ( *pi )->getOriginalKind() )
                                {
                                        alreadyThere = true;
                                        break;
                                }
                        }

                        if ( ! alreadyThere )
                        {
                                AvatarItemPtr copy( new AvatarItem( *( *pi ) ) );
                                copy->setBehaviorOf( ( *pi )->getBehavior()->getNameOfBehavior() );
                                copy->setWayOfEntry( "" );

                                charactersWhoEnteredRoom.push_back( copy );
                        }
                }
        }
}

void Room::removeFloorAt( int tileX, int tileY )
{
        FloorTile* tile = nilPointer ;

        for ( std::vector< FloorTile* >::const_iterator i = floorTiles.begin (); i != floorTiles.end (); ++ i )
        {
                if ( *i == nilPointer ) continue ;

                if ( ( *i )->getCellX() == tileX && ( *i )->getCellY() == tileY )
                {
                        tile = *i ;
                        break ;
                }
        }

        if ( tile != nilPointer ) removeFloorTile( tile );

        floorTiles[ tileX + ( getTilesOnX() * tileY ) ] = nilPointer ;
}

void Room::removeFloorTile( FloorTile * floorTile )
{
        if ( floorTile != nilPointer )
                this->floorTiles[ floorTile->getIndexOfColumn () ] = nilPointer ;

        delete floorTile ;
}

void Room::removeWallPiece( WallPiece * segment )
{
        if ( segment == nilPointer ) return ;

        std::vector< WallPiece * >::iterator si = std::find( wallPieces.begin (), wallPieces.end (), segment );
        if ( si != wallPieces.end () ) wallPieces.erase( si );

        delete segment ;
}

void Room::removeGridItemByUniqueName( const std::string & uniqueName )
{
        GridItemPtr foundGridItem ;

        for ( unsigned int column = 0 ; column < gridItems.size() ; column ++ ) {
                for ( std::vector< GridItemPtr >::iterator g = gridItems[ column ].begin (); g != gridItems[ column ].end (); ++ g )
                {
                        if ( *g != nilPointer && ( *g )->getUniqueName() == uniqueName )
                        {
                                foundGridItem = *g ;

                                std::cout << "removing " << ( *g )->whichItemClass() << " \"" << uniqueName <<
                                        "\" from room \"" << getNameOfRoomDescriptionFile() << "\"" << std::endl ;

                                gridItems[ column ].erase( g );

                                break ;
                        }
                }

                if ( foundGridItem != nilPointer ) break ;
        }

        if ( foundGridItem != nilPointer ) {
                getMediator()->wantShadowFromGridItem( * foundGridItem );
                getMediator()->wantToMaskWithGridItem( * foundGridItem );
        }
}

void Room::removeFreeItemByUniqueName( const std::string & uniqueName )
{
        FreeItemPtr foundFreeItem ;

        for ( std::vector< FreeItemPtr >::iterator f = freeItems.begin (); f != freeItems.end (); ++ f )
        {
                if ( *f != nilPointer && ( *f )->getUniqueName() == uniqueName )
                {
                        foundFreeItem = *f ;

                        std::cout << "removing " << ( *f )->whichItemClass() << " \"" << uniqueName <<
                                "\" from room \"" << getNameOfRoomDescriptionFile() << "\"" << std::endl ;

                        freeItems.erase( f );

                        break ;
                }
        }

        if ( foundFreeItem != nilPointer ) {
                getMediator()->wantShadowFromFreeItem( * foundFreeItem );
                getMediator()->wantToMaskWithFreeItem( * foundFreeItem );
        }
}

bool Room::removeCharacterFromRoom( const AvatarItem & character, bool characterExitsRoom )
{
        for ( std::vector< AvatarItemPtr >::iterator pi = charactersYetInRoom.begin (); pi != charactersYetInRoom.end (); ++ pi )
        {
                if ( character.getUniqueName() == ( *pi )->getUniqueName() )
                {
                        const std::string & characterName = character.getOriginalKind() ;
                        bool wasActive = character.isActiveCharacter() ;

                        removeFreeItemByUniqueName( character.getUniqueName() );

                        nextNumbers[ "character " + characterName ] -- ;

                        if ( wasActive ) {
                                if ( getMediator()->getWaitingCharacter() != nilPointer )
                                        // activate the waiting character
                                        getMediator()->activateCharacterByName( getMediator()->getWaitingCharacter()->getOriginalKind() );
                                else
                                        getMediator()->deactivateAnyCharacter() ;
                        }

                        charactersYetInRoom.erase( pi );
                        /// pi --; // not needed, this iteration is the last one due to "return" below

                        // when a character leaves room, bin its copy on entry
                        if ( characterExitsRoom )
                        {
                                for ( std::vector< AvatarItemPtr >::iterator it = charactersWhoEnteredRoom.begin (); it != charactersWhoEnteredRoom.end (); ++ it )
                                {
                                        if ( ( *it )->getOriginalKind() == characterName )
                                        {
                                                std::cout << "and removing the copy of character \"" << characterName << "\" created on entry to this room" << std::endl ;

                                                charactersWhoEnteredRoom.erase( it );
                                                /// -- it ; // not needed due to break
                                                break;
                                        }
                                }
                        }

                        return true;
                }
        }

        return false;
}

bool Room::isAnyCharacterStillInRoom () const
{
        return ! charactersYetInRoom.empty () ;
}

unsigned int Room::removeItemsOfKind ( const std::string & kind )
{
        unsigned int howManyItemsWereRemoved = 0 ;

        getMediator()->lockGridItemsMutex();

        std::vector< std::string > gridItemsToRemove ;

        for ( unsigned int column = 0; column < gridItems.size(); column ++ )
        {
                for ( std::vector< GridItemPtr >::const_iterator gi = gridItems[ column ].begin (); gi != gridItems[ column ].end (); ++ gi )
                {
                        GridItemPtr gridItem = *gi ;
                        if ( gridItem == nilPointer ) continue ;

                        if ( gridItem->getKind() == kind )
                                gridItemsToRemove.push_back( gridItem->getUniqueName() );
                }
        }

        for ( std::vector< std::string >::const_iterator gi = gridItemsToRemove.begin (); gi != gridItemsToRemove.end (); ++ gi )
        {
                removeGridItemByUniqueName( *gi );
                howManyItemsWereRemoved ++ ;
        }

        getMediator()->unlockGridItemsMutex();
        getMediator()->lockFreeItemsMutex();

        std::vector< std::string > freeItemsToRemove ;

        for ( std::vector< FreeItemPtr >::const_iterator fi = freeItems.begin (); fi != freeItems.end (); ++ fi )
        {
                FreeItemPtr freeItem = *fi ;
                if ( freeItem == nilPointer ) continue ;

                if ( freeItem->getKind() == kind )
                        freeItemsToRemove.push_back( freeItem->getUniqueName() );
        }

        for ( std::vector< std::string >::const_iterator fi = freeItemsToRemove.begin (); fi != freeItemsToRemove.end (); ++ fi )
        {
                removeFreeItemByUniqueName( *fi );
                howManyItemsWereRemoved ++ ;
        }

        getMediator()->unlockFreeItemsMutex();

        return howManyItemsWereRemoved ;
}

void Room::dontDisappearOnJump ()
{
        for ( unsigned int column = 0; column < gridItems.size(); column ++ ) {
                for ( std::vector< GridItemPtr >::iterator gi = gridItems[ column ].begin (); gi != gridItems[ column ].end (); ++ gi )
                {
                        GridItem & gridItem = *( *gi );
                        if ( gridItem.getBehavior() != nilPointer )
                        {
                                std::string behavior = gridItem.getBehavior()->getNameOfBehavior();
                                if ( behavior == "behavior of disappearance on jump into" ||
                                                behavior == "behavior of slow disappearance on jump into" )
                                {
                                        gridItem.setBehaviorOf( "still" );

                                        if ( ! GameManager::getInstance().isSimpleGraphicsSet() )
                                                Color::multiplyWithColor(
                                                        gridItem.getRawImageToChangeIt (),
                                                        ( behavior == "behavior of slow disappearance on jump into" ) ?
                                                                Color::byName( "magenta" ) : Color::byName( "red" ) );
                                        else
                                                Color::invertColors( gridItem.getRawImageToChangeIt () );

                                        gridItem.freshProcessedImage();
                                        gridItem.setWantShadow( true );
                                }
                        }
                }
        }

        for ( std::vector< FreeItemPtr >::iterator fi = freeItems.begin (); fi != freeItems.end (); ++fi )
        {
                FreeItem & freeItem = *( *fi );
                if ( freeItem.getBehavior() != nilPointer )
                {
                        std::string behavior = freeItem.getBehavior()->getNameOfBehavior();
                        if ( behavior == "behavior of disappearance on jump into" ||
                                        behavior == "behavior of slow disappearance on jump into" )
                        {
                                freeItem.setBehaviorOf( "still" );

                                if ( ! GameManager::getInstance().isSimpleGraphicsSet() )
                                        Color::multiplyWithColor(
                                                freeItem.getRawImageToChangeIt (),
                                                ( behavior == "behavior of slow disappearance on jump into" ) ?
                                                        Color::byName( "magenta" ) : Color::byName( "red" ) );
                                else
                                        Color::invertColors( freeItem.getRawImageToChangeIt () );

                                freeItem.freshBothProcessedImages();
                                freeItem.setWantShadow( true );
                                freeItem.setWantMaskTrue();
                        }
                }
        }
}

unsigned int Room::getWidthOfRoomImage () const
{
        unsigned int roomW = ( getTilesOnX () + getTilesOnY () ) * ( getSizeOfOneTile () << 1 ) ;

        if ( ( ! hasFloor() || ( ! hasDoorOn( "north" ) && ! hasDoorOn( "northeast" ) && ! hasDoorOn( "northwest" ) ) )
                && ! hasDoorOn( "west" ) && ! hasDoorOn( "westnorth" ) && ! hasDoorOn( "westsouth" ) )
        {
                roomW += getSizeOfOneTile () ;
        }
        if ( ( ! hasFloor() || ( ! hasDoorOn( "east" ) && ! hasDoorOn( "eastnorth" ) && ! hasDoorOn( "eastsouth" ) ) )
                && ! hasDoorOn( "south" ) && ! hasDoorOn( "southeast" ) && ! hasDoorOn( "southwest" ) )
        {
                roomW += getSizeOfOneTile () ;
        }

        return roomW ;
}

unsigned int Room::getHeightOfRoomImage () const
{
        unsigned int roomH = /* height of plane */ ( getTilesOnX () + getTilesOnY () ) * getSizeOfOneTile ();

        roomH += /* room’s height in 3D */ ( Room::MaxLayers + 2 ) * Room::LayerHeight ;
        roomH += /* height of floor */ 8 ;

        if ( ! hasDoorOn( "south" ) && ! hasDoorOn( "southwest" ) && ! hasDoorOn( "southeast" ) &&
                ! hasDoorOn( "west" ) && ! hasDoorOn( "westsouth" ) && ! hasDoorOn( "westnorth" ) )
        {
                roomH += ( getSizeOfOneTile () >> 1 ) ;
        }

        return roomH ;
}

void Room::drawRoom ()
{
        if ( whereToDraw == nilPointer )
        {
                // crea la imagen with suitable dimensions to draw this room
                whereToDraw = PicturePtr( new Picture( getWidthOfRoomImage (), getHeightOfRoomImage () ) );
        }

        drawOn( whereToDraw->getAllegroPict() ) ;
}

void Room::draw ()
{
        const allegro::Pict& where = allegro::Pict::getWhereToDraw() ;

        // begin with a blank canvas
        where.clearToColor( AllegroColor::keyColor() );

        // draw tiles o’floor

        for ( unsigned int xCell = 0; xCell < getTilesOnX(); xCell ++ ) {
                for ( unsigned int yCell = 0; yCell < getTilesOnY(); yCell ++ )
                {
                        unsigned int column = getTilesOnX() * yCell + xCell ;
                        FloorTile* tile = floorTiles[ column ];

                        if ( tile != nilPointer ) // there’s tile of floor here
                        {
                                if ( shadingTransparency < 256 ) // visible shadows
                                        if ( tile->getWantShadow() )
                                                getMediator()->castShadowOnFloor( *tile );

                                tile->draw ();
                        }
                }
        }

        // draw walls
        if ( GameManager::getInstance().getDrawingOfWalls () )
        {
                for ( std::vector< WallPiece * >::iterator wi = this->wallPieces.begin (); wi != this->wallPieces.end (); ++ wi )
                        ( *wi )->draw ();
        }

        getMediator()->lockGridItemsMutex();
        getMediator()->lockFreeItemsMutex();

        // draw grid items

        for ( unsigned int i = 0 ; i < getTilesOnX() * getTilesOnY() ; i ++ )
        {
                std::vector < GridItemPtr > columnToDraw = this->gridItems[ this->drawingSequence[ i ] ];
                for ( std::vector< GridItemPtr >::iterator gi = columnToDraw.begin () ; gi != columnToDraw.end () ; ++ gi )
                {
                        GridItem & gridItem = *( *gi );

                        if ( ! GameManager::getInstance().getDrawingOfWalls()
                                        && gridItem.getKind().find( "wall" ) != std::string::npos ) {
                                ///gridItem.setTransparency( 80 );
                                continue ;
                        }

                        if ( shadingTransparency < 256 ) {
                                // cast shadow
                                if ( gridItem.getWantShadow() )
                                        getMediator()->castShadowOnGridItem( gridItem );
                        }

                        gridItem.draw ();
                }
        }

        // for free items there’re two steps before drawing

        // at first shade every free item with grid items and other free items
        for ( std::vector< FreeItemPtr >::iterator fi = freeItems.begin (); fi != freeItems.end (); ++ fi )
        {
                FreeItem & freeItem = *( *fi );

                // shade an item when shadows are on
                if ( shadingTransparency < 256 ) freeItem.requestShadow();
        }

        // then mask it and finally draw it
        for ( std::vector< FreeItemPtr >::iterator fi = freeItems.begin (); fi != freeItems.end (); ++ fi )
        {
                FreeItem & freeItem = *( *fi );

                freeItem.requestMask();
                freeItem.draw ();
        }

        getMediator()->unlockGridItemsMutex();
        getMediator()->unlockFreeItemsMutex();


#if defined( SHOW_ORIGIN_OF_ROOM ) && SHOW_ORIGIN_OF_ROOM
        // draw point of the room’s origin
        if ( FlickeringColor::flickerWhiteAndTransparent() != Color::whiteColor() ) {
                allegro::fillCircle( getX0(), getY0(), 3, Color::blackColor().toAllegroColor() );
                where.drawPixelAt( getX0(), getY0(), Color::whiteColor().toAllegroColor() );
        } else {
                allegro::fillCircle( getX0(), getY0(), 3, Color::whiteColor().toAllegroColor() );
                where.drawPixelAt( getX0(), getY0(), Color::blackColor().toAllegroColor() );
        }
#endif
}

void Room::calculateBounds()
{
        unsigned int oneTileLong = getSizeOfOneTile () ;

        bounds[ "north" ] = hasDoorOn( "north" ) || hasDoorOn( "northeast" ) || hasDoorOn( "northwest" ) || ! hasFloor() ? oneTileLong : 0 ;
        bounds[ "east" ] = hasDoorOn( "east" ) || hasDoorOn( "eastnorth" ) || hasDoorOn( "eastsouth" ) || ! hasFloor() ? oneTileLong : 0 ;
        bounds[ "south" ] = oneTileLong * getTilesOnX() - ( hasDoorOn( "south" ) || hasDoorOn( "southeast" ) || hasDoorOn( "southwest" )  ? oneTileLong : 0 );
        bounds[ "west" ] = oneTileLong * getTilesOnY() - ( hasDoorOn( "west" ) || hasDoorOn( "westnorth" ) || hasDoorOn( "westsouth" )  ? oneTileLong : 0 );

        if ( this->isTripleRoom () ) {
                // limits for a triple room
                bounds[ "northeast" ] = hasDoorOn( "northeast" ) ? doors[ "northeast" ]->getLintel()->getX() + doors[ "northeast" ]->getLintel()->getWidthX() - oneTileLong : bounds[ "north" ];
                bounds[ "northwest" ] = hasDoorOn( "northwest" ) ? doors[ "northwest" ]->getLintel()->getX() + doors[ "northwest" ]->getLintel()->getWidthX() - oneTileLong : bounds[ "north" ];
                bounds[ "southeast" ] = hasDoorOn( "southeast" ) ? doors[ "southeast" ]->getLintel()->getX() + oneTileLong : bounds[ "south" ];
                bounds[ "southwest" ] = hasDoorOn( "southwest" ) ? doors[ "southwest" ]->getLintel()->getX() + oneTileLong : bounds[ "south" ];
                bounds[ "eastnorth" ] = hasDoorOn( "eastnorth" ) ? doors[ "eastnorth" ]->getLintel()->getY() + doors[ "eastnorth" ]->getLintel()->getWidthY() - oneTileLong : bounds[ "east" ];
                bounds[ "eastsouth" ] = hasDoorOn( "eastsouth" ) ? doors[ "eastsouth" ]->getLintel()->getY() + doors[ "eastsouth" ]->getLintel()->getWidthY() - oneTileLong : bounds[ "east" ];
                bounds[ "westnorth" ] = hasDoorOn( "westnorth" ) ? doors[ "westnorth" ]->getLintel()->getY() + oneTileLong : bounds[ "west" ];
                bounds[ "westsouth" ] = hasDoorOn( "westsouth" ) ? doors[ "westsouth" ]->getLintel()->getY() + oneTileLong : bounds[ "west" ];
        }
}

void Room::activate()
{
        getMediator()->beginUpdating ();
}

void Room::deactivate()
{
        getMediator()->endUpdating ();
}

bool Room::isActive() const
{
        return getMediator()->isThreadRunning() ;
}

bool Room::swapCharactersInRoom ()
{
        return getMediator()->pickNextCharacter () ;
}

bool Room::continueWithAliveCharacter ()
{
        AvatarItemPtr previouslyAliveCharacter = getMediator()->getActiveCharacter() ;
        assert( previouslyAliveCharacter != nilPointer );

        if ( previouslyAliveCharacter->getLives() > 0 ) return true ;

        // that character has no more lives, look for the other one in this room
        AvatarItemPtr nextCharacter ( nilPointer );
        for ( unsigned int i = 0 ; i < charactersYetInRoom.size () ; ++ i )
        {
                AvatarItemPtr character = charactersYetInRoom[ i ];
                if ( character != nilPointer && character->getOriginalKind() != previouslyAliveCharacter->getOriginalKind() ) {
                        nextCharacter = character ;
                        break ;
                }
        }

        if ( nextCharacter != nilPointer && nextCharacter->getLives() > 0 )
        {
                getMediator()->activateCharacterByName( nextCharacter->getOriginalKind() );

                removeCharacterFromRoom( *previouslyAliveCharacter, /* leaving room */ true );

                ///this->activate();
                return true ;
        }

        return false ;
}

bool Room::calculateEntryCoordinates( const std::string & way,
                                        int widthX, int widthY,
                                        int previousNorthBound, int previousEastBound, int previousSouthBound, int previousWestBound,
                                        int* x, int* y, int* z )
{
        int northToSouth = bounds[ "south" ] - bounds[ "north" ] ;           //  N + N2S = S     *------>(N)----->(S)
        int previousSouth2North = previousNorthBound - previousSouthBound ;  //  N' = S' + S2N'  *-->(N')<-------------(S')
        int previousNorth2South = - previousSouth2North ;                    //  N2S' = - S2N'   *-->(N')------------->(S')

        int eastToWest = bounds[ "west" ] - bounds[ "east" ] ;
        int previousWest2East = previousEastBound - previousWestBound ;
        int previousEast2West = - previousWest2East ;

        int dX = ( northToSouth + previousSouth2North ) >> 1 ;
        int dY = ( eastToWest + previousWest2East ) >> 1 ;

        // when moving from a single room to a double room or vice versa
        int differenceOnX = 0 ;
        int differenceOnY = 0 ;

        if ( way == "above" || way == "below" || way == "via teleport" || way == "via second teleport" )
        {
                const int limitOfSingleRoom = maxTilesOfSingleRoom * getSizeOfOneTile ();

                if ( ( northToSouth > limitOfSingleRoom && previousNorth2South <= limitOfSingleRoom )
                        || ( northToSouth <= limitOfSingleRoom && previousNorth2South > limitOfSingleRoom ) )
                {
                        differenceOnX = dX ;
                }

                if ( ( eastToWest > limitOfSingleRoom && previousEast2West <= limitOfSingleRoom )
                        || ( eastToWest <= limitOfSingleRoom && previousEast2West > limitOfSingleRoom ) )
                {
                        differenceOnY = dY ;
                }
        }

        const FreeItemPtr & leftJamb = hasDoorOn( way ) ? doors[ way ]->getLeftJamb() : FreeItemPtr () ;

        if ( leftJamb != nilPointer ) *z = leftJamb->getZ ();
        else if ( way == "above" ) *z = Room::MaxLayers * Room::LayerHeight ;
        else if ( way == "below" ) *z = Room::LayerHeight ;

        bool okay = false ;
        unsigned int oneTileSize = getSizeOfOneTile ();

        // calculate coordinates by the way of entry
        switch ( Way( way ).getIntegerOfWay () )
        {
                case Way::North:
                        *x = bounds[ way ] - oneTileSize + 1 ;
                        *y = leftJamb->getY() - leftJamb->getWidthY() ;
                        okay = true ;
                        break;

                case Way::South:
                        *x = bounds[ way ] + oneTileSize - widthX ;
                        *y = leftJamb->getY() - leftJamb->getWidthY() ;
                        okay = true ;
                        break;

                case Way::East:
                        *x = leftJamb->getX() + leftJamb->getWidthX() ;
                        *y = bounds[ way ] - oneTileSize + widthY ;
                        okay = true ;
                        break;

                case Way::West:
                        *x = leftJamb->getX() + leftJamb->getWidthX() ;
                        *y = bounds[ way ] + oneTileSize - 1 ;
                        okay = true ;
                        break;

                case Way::Northeast:
                        *x = bounds[ way ];
                        *y = leftJamb->getY() - leftJamb->getWidthY() ;
                        okay = true ;
                        break;

                case Way::Northwest:
                        *x = bounds[ way ];
                        *y = leftJamb->getY() - leftJamb->getWidthY() ;
                        okay = true ;
                        break;

                case Way::Southeast:
                        *x = bounds[ way ] - widthX ;
                        *y = leftJamb->getY() - leftJamb->getWidthY() ;
                        okay = true ;
                        break;

                case Way::Southwest:
                        *x = bounds[ way ] - widthX ;
                        *y = leftJamb->getY() - leftJamb->getWidthY() ;
                        okay = true ;
                        break;

                case Way::Eastnorth:
                        *x = leftJamb->getX() + leftJamb->getWidthX() ;
                        *y = bounds[ way ] + widthY ;
                        okay = true ;
                        break;

                case Way::Eastsouth:
                        *x = leftJamb->getX() + leftJamb->getWidthX() ;
                        *y = bounds[ way ] + widthY ;
                        okay = true ;
                        break;

                case Way::Westnorth:
                        *x = leftJamb->getX() + leftJamb->getWidthX() ;
                        *y = bounds[ way ] - widthY ;
                        okay = true ;
                        break;

                case Way::Westsouth:
                        *x = leftJamb->getX() + leftJamb->getWidthX() ;
                        *y = bounds[ way ] - widthY ;
                        okay = true ;
                        break;

                case Way::Above:
                        *x += bounds[ "north" ] - previousNorthBound + dX ;
                        *x += ( *x < ( northToSouth >> 1 ) ) ? - differenceOnX : differenceOnX ;
                        *y += bounds[ "east" ] - previousEastBound + dY ;
                        *y += ( *y < ( eastToWest >> 1 ) ) ? - differenceOnY : differenceOnY ;
                        okay = true ;
                        break;

                case Way::Below:
                        *x += bounds[ "north" ] - previousNorthBound + dX ;
                        *x += ( *x < ( northToSouth >> 1 ) ) ? - differenceOnX : differenceOnX ;
                        *y += bounds[ "east" ] - previousEastBound + dY ;
                        *y += ( *y < ( eastToWest >> 1 ) ) ? - differenceOnY : differenceOnY ;
                        okay = true ;
                        break;

                case Way::ByTeleport:
                case Way::ByTeleportToo:
                        *x += bounds[ "north" ] - previousNorthBound + dX ;
                        *x += ( *x < ( northToSouth >> 1 ) ) ? - differenceOnX : differenceOnX ;
                        *y += differenceOnY + bounds[ "east" ] - previousEastBound + dY ;
                        *y += ( *y < ( eastToWest >> 1 ) ) ? - differenceOnY : differenceOnY ;
                        okay = true ;
                        break;

                default:
                        ;
        }

        return okay ;
}

int Room::getXCenterForItem( int widthX )
{
        return
                ( ( getLimitAt( "south" ) - getLimitAt( "north" ) + widthX ) >> 1 )
                        + ( hasDoorOn( "north" ) ? getSizeOfOneTile() >> 1 : 0 )
                                - ( hasDoorOn( "south" ) ? getSizeOfOneTile() >> 1 : 0 ) ;
}

int Room::getYCenterForItem( int widthY )
{
        return
                ( ( getLimitAt( "west" ) - getLimitAt( "east" ) + widthY ) >> 1 )
                        + ( hasDoorOn( "east" ) ? getSizeOfOneTile() >> 1 : 0 )
                                - ( hasDoorOn( "west" ) ? getSizeOfOneTile() >> 1 : 0 )
                                        - 1 ;
}
