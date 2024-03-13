
#include "Room.hpp"

#include "Color.hpp"
#include "FloorTile.hpp"
#include "Wall.hpp"
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

using behaviors::Elevator ;

/* static */
const std::string Room::Sides_Of_Room[ Room::Sides ] =
                        {  "northeast", "northwest", "north", "southeast", "southwest", "south",
                                "eastnorth", "eastsouth", "east", "westnorth", "westsouth", "west"  };

Room::Room( const std::string & roomFile, const std::string & scenery,
                unsigned int xTiles, unsigned int yTiles,
                const std::string & floorKind )
        : Mediated( )
        , connections( nilPointer )
        , nameOfFileWithDataAboutRoom( roomFile )
        , scenery( scenery )
        , color( "white" )
        , howManyTiles( std::pair< unsigned int, unsigned int >( xTiles, yTiles ) )
        , kindOfFloor( floorKind )
        , drawSequence( new unsigned int[ xTiles * yTiles ] )
        , camera( new Camera( this ) )
        , whereToDraw( )
{
        this->mediator = new Mediator( this );

        for ( int i = 0 ; i < Room::Sides ; ++ i ) {
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

        for ( unsigned int f = 0 ; f < xTiles + yTiles ; f++ )
        {
                for ( int n = yTiles - 1 ; n >= 0 ; n-- )
                {
                        unsigned int index = n * ( xTiles - 1 ) + f;
                        if ( ( index >= n * xTiles ) && ( index < ( n + 1 ) * xTiles ) )
                        {
                                drawSequence[ pos++ ] = index;
                        }
                }
        }

        // 0 for black shadows, 128 for 50% opacity, 256 for no shadows
        this->shadingTransparency = GameManager::getInstance().getCastShadows () ? 128 /* 0 */ : 256 ;

#if defined( DEBUG ) && DEBUG
        std::cout << "created room \"" << nameOfFileWithDataAboutRoom << "\"" << std::endl ;
#endif
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

        // bin the sequence of drawing
        delete [] drawSequence ;

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
        xTiles->SetText( getTilesX() );
        root->InsertEndChild( xTiles );

        tinyxml2::XMLElement* yTiles = roomXml.NewElement( "yTiles" ) ;
        yTiles->SetText( getTilesY() );
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
                for ( std::vector< std::pair< int, int > >::const_iterator it = tilesWithoutFloor.begin () ; it != tilesWithoutFloor.end () ; ++ it )
                {
                        tinyxml2::XMLElement* nofloor = roomXml.NewElement( "nofloor" );

                        nofloor->SetAttribute( "x", it->first );
                        nofloor->SetAttribute( "y", it->second );

                        root->InsertEndChild( nofloor );
                }
        }

        // write tiles of the floor

        if ( getScenery() == "" )
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

        tinyxml2::XMLElement* walls = roomXml.NewElement( "walls" );

        if ( wallX.size() + wallY.size() > 0 )
        {
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
        }

        root->InsertEndChild( walls );

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
                                        if ( ( theItem->isSegmentOfWallOnX () && theItem->getCellY() == 0 ) ||
                                                ( theItem->isSegmentOfWallOnY () && theItem->getCellX() == 0 ) )
                                        {
                                                tinyxml2::XMLElement* wall = roomXml.NewElement( "wall" );

                                                wall->SetAttribute( "on", theItem->isSegmentOfWallOnX () ? "x" : "y" );

                                                tinyxml2::XMLElement* wallPosition = roomXml.NewElement( "position" );
                                                wallPosition->SetText( theItem->isSegmentOfWallOnX () ? theItem->getCellX() : theItem->getCellY() );
                                                wall->InsertEndChild( wallPosition );

                                                tinyxml2::XMLElement* wallPicture = roomXml.NewElement( "picture" );
                                                wallPicture->SetText( ( theItem->getOriginalKind() + ".png" ).c_str () );
                                                wall->InsertEndChild( wallPicture );

                                                walls->InsertEndChild( wall );

                                                continue ;
                                        }

                                        tinyxml2::XMLElement* item = roomXml.NewElement( "item" );

                                        item->SetAttribute( "x", theItem->getCellX() );
                                        item->SetAttribute( "y", theItem->getCellY() );
                                        int z = theItem->getZ();
                                        z = ( z > Room::FloorZ ) ? ( z / Room::LayerHeight ) : Room::FloorZ ;
                                        item->SetAttribute( "z", z );

                                        tinyxml2::XMLElement* kindOfItem = roomXml.NewElement( "kind" );
                                        kindOfItem->SetText( theItem->getKind().c_str () );
                                        item->InsertEndChild( kindOfItem );

                                        tinyxml2::XMLElement* itemOrientation = roomXml.NewElement( "orientation" );
                                        const std::string & heading = theItem->getHeading ();
                                        const std::string orientation = ( heading.empty() || heading == "nowhere" ) ? "none" : heading ;
                                        itemOrientation->SetText( orientation.c_str () );
                                        item->InsertEndChild( itemOrientation );

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
                }

                // free items

                for ( std::vector< FreeItemPtr >::const_iterator fi = freeItems.begin (); fi != freeItems.end (); ++ fi )
                {
                        FreeItemPtr theItem = *fi ;
                        if ( theItem != nilPointer && ! theItem->isPartOfDoor() && theItem->whichItemClass() != "avatar item" )
                        {
                                tinyxml2::XMLElement* item = roomXml.NewElement( "item" );

                                item->SetAttribute( "x", theItem->getInitialCellX () );
                                item->SetAttribute( "y", theItem->getInitialCellY () );
                                item->SetAttribute( "z", theItem->getInitialCellZ () );

                                tinyxml2::XMLElement* kindOfItem = roomXml.NewElement( "kind" );
                                kindOfItem->SetText( theItem->getKind().c_str () );
                                item->InsertEndChild( kindOfItem );

                                tinyxml2::XMLElement* itemOrientation = roomXml.NewElement( "orientation" );
                                const std::string & heading = theItem->getHeading ();
                                const std::string orientation = ( heading.empty() || heading == "nowhere" ) ? "none" : heading ;
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
                                                Elevator* elevatorBehavior = dynamic_cast< Elevator* >( theItem->getBehavior().get () );

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

                                tinyxml2::XMLElement* itemClass = roomXml.NewElement( "class" );
                                itemClass->SetText( "freeitem" );
                                item->InsertEndChild( itemClass );

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
                                z = ( z > Room::FloorZ ) ? ( z / Room::LayerHeight ) : Room::FloorZ ;
                                item->SetAttribute( "z", z );

                                tinyxml2::XMLElement* kindOfItem = roomXml.NewElement( "kind" );
                                kindOfItem->SetText( theDoor->getKind().c_str () );
                                item->InsertEndChild( kindOfItem );

                                tinyxml2::XMLElement* itemOrientation = roomXml.NewElement( "orientation" );
                                itemOrientation->SetText( theDoor->getWhereIsDoor().c_str () );
                                item->InsertEndChild( itemOrientation );

                                tinyxml2::XMLElement* itemClass = roomXml.NewElement( "class" );
                                itemClass->SetText( "door" );
                                item->InsertEndChild( itemClass );

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

void Room::addFloorTile( FloorTile * floorTile )
{
        if ( floorTile == nilPointer ) return ;

        floorTile->setMediator( mediator );
        floorTile->calculateOffset();

        // bin old tile, if any
        removeFloorAt( floorTile->getCellX(), floorTile->getCellY() );

        this->floorTiles[ floorTile->getIndexOfColumn() ] = floorTile;
}

void Room::addWall( Wall * wall )
{
        if ( wall == nilPointer ) return ;

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
        if ( door == nilPointer ) return ;

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

        camera->recenterRoom () ;
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

        // convert walls near doors to grid items to draw them after doors

        if ( hasDoorAt( "north" ) || hasDoorAt( "northeast" ) || hasDoorAt( "northwest" ) )
        {
                Door* northDoor = getDoorAt( "north" );
                Door* northeastDoor = getDoorAt( "northeast" );
                Door* northwestDoor = getDoorAt( "northwest" );

                std::vector< Wall* > wallsToBin ;

                for ( std::vector< Wall * >::iterator wy = wallY.begin (); wy != wallY.end (); ++ wy )
                {
                        Wall* segment = *wy ;

                        if ( ( northDoor != nilPointer && segment->getPosition() == northDoor->getCellY() + 2 ) ||
                                ( northeastDoor != nilPointer && segment->getPosition() == northeastDoor->getCellY() + 2 ) ||
                                ( northwestDoor != nilPointer && segment->getPosition() == northwestDoor->getCellY() + 2 ) )
                        {
                                std::string imageName = segment->getImage()->getName() ;
                                imageName = imageName.substr( 0, imageName.find_last_of( "." ) );

                                const DescriptionOfItem* descriptionOfWall = ItemDescriptions::descriptions().getDescriptionByKind( imageName );
                                if ( descriptionOfWall != nilPointer ) {
                                        addGridItem( GridItemPtr( new GridItem( *descriptionOfWall, 0, segment->getPosition(), 0 ) ) );
                                        wallsToBin.push_back( segment );
                                }
                        }
                }

                if ( ! wallsToBin.empty() )
                {
                        Wall* sayBye = wallsToBin.back ();
                        wallsToBin.pop_back( );
                        removeWallOnY( sayBye );
                }
        }
        if ( hasDoorAt( "east" ) || hasDoorAt( "eastnorth" ) || hasDoorAt( "eastsouth" ) )
        {
                Door* eastDoor = getDoorAt( "east" );
                Door* eastnorthDoor = getDoorAt( "eastnorth" );
                Door* eastsouthDoor = getDoorAt( "eastsouth" );

                std::vector< Wall* > wallsToBin ;

                for ( std::vector< Wall * >::iterator wx = wallX.begin (); wx != wallX.end (); ++ wx )
                {
                        Wall* segment = *wx ;

                        if ( ( eastDoor != nilPointer && segment->getPosition() == eastDoor->getCellX() + 2 ) ||
                                ( eastnorthDoor != nilPointer && segment->getPosition() == eastnorthDoor->getCellX() + 2 ) ||
                                ( eastsouthDoor != nilPointer && segment->getPosition() == eastsouthDoor->getCellX() + 2 ) )
                        {
                                std::string imageName = segment->getImage()->getName() ;
                                imageName = imageName.substr( 0, imageName.find_last_of( "." ) );

                                const DescriptionOfItem* descriptionOfWall = ItemDescriptions::descriptions().getDescriptionByKind( imageName );
                                if ( descriptionOfWall != nilPointer ) {
                                        addGridItem( GridItemPtr( new GridItem( *descriptionOfWall, segment->getPosition(), 0, 0 ) ) );
                                        wallsToBin.push_back( segment );
                                }
                        }
                }

                if ( ! wallsToBin.empty() )
                {
                        Wall* sayBye = wallsToBin.back ();
                        wallsToBin.pop_back( );
                        removeWallOnX( sayBye );
                }
        }

        // sort walls

        std::sort( wallX.begin (), wallX.end (), Wall::comparePointersToWall );
        std::sort( wallY.begin (), wallY.end (), Wall::comparePointersToWall );
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
                std::cerr << "coordinates for " << gridItem->whichItemClass() << " are out of limits" << std::endl ;
                return;
        }

        if ( gridItem->getUnsignedHeight() == 0 )
        {
                std::cerr << "can’t add " << gridItem->whichItemClass() << " which height is zero" << std::endl ;
                return;
        }

        gridItem->setMediator( mediator );

        mediator->clearCollisions ();

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
                mediator->collectCollisionsWith( gridItem->getUniqueName() );
        } else
        {       // an item with z = FloorZ goes above the previously added ones in the column with the same x and y
                // and its position on Z is calculated
                gridItem->setZ( mediator->findHighestZ( *gridItem ) );
                std::cout << "for " << gridItem->whichItemClass() << " \"" << gridItem->getUniqueName()
                                        << "\" the calculated “Z above others” is " << gridItem->getZ() << std::endl ;
        }

        if ( mediator->isThereAnyCollision () )
        {
                // can’t add item when there’s some collision
                std::cerr << "there’s collision with " << gridItem->whichItemClass() << std::endl ;
                return ;
        }

        // update the offset of the item’s image from the room’s origin
        gridItem->updateImageOffset() ;

        mediator->wantShadowFromGridItem( *gridItem );
        mediator->wantToMaskWithGridItem( *gridItem );

#if defined( DEBUG ) && DEBUG
        std::cout << gridItem->whichItemClass() << " \"" << gridItem->getUniqueName() << "\" is yet part of room \"" << getNameOfRoomDescriptionFile() << "\"" << std::endl ;
#endif
}

void Room::sortGridItems ()
{
        mediator->lockGridItemsMutex ();

        for ( unsigned int column = 0; column < this->gridItems.size(); ++ column )
        {
                if ( ! this->gridItems[ column ].empty() )
                        std::sort( this->gridItems[ column ].begin (), this->gridItems[ column ].end () );
        }

        mediator->unlockGridItemsMutex ();
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

        if ( ( freeItem->getX() + freeItem->getWidthX() > static_cast< int >( this->getTilesX() * getSizeOfOneTile() ) )
                || ( freeItem->getY() - freeItem->getWidthY() + 1 < 0 )
                        || ( freeItem->getY() > static_cast< int >( this->getTilesY() * getSizeOfOneTile() ) - 1 ) )
        {
                std::cerr << "coordinates for " << freeItem->whichItemClass() << " are out of room" << std::endl ;
                dumpItemInsideThisRoom( *freeItem );
                return;
        }

        freeItem->setMediator( mediator );

        mediator->clearCollisions ();

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
                mediator->collectCollisionsWith( freeItem->getUniqueName() );
        }
        // for item at the top of column
        else
        {
                freeItem->setZ( mediator->findHighestZ( *freeItem ) );
        }

        // collision is found, so can’t add this item
        if ( mediator->isThereAnyCollision () )
        {
                std::cerr << "there’s collision with " << freeItem->whichItemClass() << std::endl ;
                dumpItemInsideThisRoom( *freeItem );
                return;
        }

        mediator->wantShadowFromFreeItem( *freeItem );
        mediator->wantToMaskWithFreeItem( *freeItem );

#if defined( DEBUG ) && DEBUG
        std::cout << freeItem->whichItemClass() << " \"" << freeItem->getUniqueName() << "\" is yet in room \"" << getNameOfRoomDescriptionFile() << "\"" << std::endl ;
#endif
}

void Room::sortFreeItems ()
{
        mediator->lockFreeItemsMutex ();

        std::sort( this->freeItems.begin (), this->freeItems.end () );

        // remask items after sorting because overlappings may change
        for ( std::vector< FreeItemPtr >::iterator f = this->freeItems.begin (); f != this->freeItems.end (); ++ f )
                ( *f )->setWantMaskTrue ();

        mediator->unlockFreeItemsMutex ();
}

bool Room::addCharacterToRoom( const AvatarItemPtr & character, bool characterEntersRoom )
{
        for ( std::vector< AvatarItemPtr >::const_iterator pi = charactersYetInRoom.begin (); pi != charactersYetInRoom.end (); ++pi )
        {
                if ( character == *pi )
                        // this character is already in room
                        return false;
        }

        if ( characterEntersRoom )
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

        if ( ( character->getX() + character->getWidthX() > static_cast< int >( this->getTilesX() * getSizeOfOneTile() ) )
                || ( character->getY() - character->getWidthY() + 1 < 0 )
                        || ( character->getY() > static_cast< int >( this->getTilesY() * getSizeOfOneTile() ) - 1 ) )
        {
                std::cerr << "coordinates for " << character->whichItemClass() << " are out of room" << std::endl ;
                dumpItemInsideThisRoom( *character );
                return false;
        }

        character->setMediator( mediator );

        mediator->clearCollisions ();

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

        // for item which is placed at some height, look for collisions
        if ( character->getZ() > Room::FloorZ )
        {
                mediator->collectCollisionsWith( character->getUniqueName() );
                while ( mediator->isThereAnyCollision () )
                {
                        character->setZ( character->getZ() + Room::LayerHeight );
                        mediator->clearCollisions ();
                        mediator->collectCollisionsWith( character->getUniqueName() );
                }
        }
        // for item at the top of column
        else
        {
                character->setZ( mediator->findHighestZ( *character ) );
        }

        // collision is found, so can’t add this item
        if ( mediator->isThereAnyCollision () )
        {
                std::cerr << "there’s collision with " << character->whichItemClass() << std::endl ;
                dumpItemInsideThisRoom( *character );
                return false;
        }

        if ( mediator->getActiveCharacter() == nilPointer )
                mediator->setActiveCharacter( character );

        mediator->wantShadowFromFreeItem( *character );
        mediator->wantToMaskWithFreeItem( *character );

        // add avatar item to room
        this->charactersYetInRoom.push_back( character );

        if ( characterEntersRoom )
        {
                AvatarItemPtr copyOfCharacter( new AvatarItem( *character ) );
                copyOfCharacter->setBehaviorOf( character->getBehavior()->getNameOfBehavior() );
                this->charactersWhoEnteredRoom.push_back( copyOfCharacter );

                std::cout << "copy of character \"" << copyOfCharacter->getOriginalKind() << "\""
                                << " is created to rebuild this room" << std::endl ;
        }

        // perhaps the character appeared in the room by means of teleportation
        if ( character->getWayOfEntry().empty () ) {
                character->canAdvanceTo( 0, 0, -1 ); // is there a teletransportation device under the character
                if ( mediator->collisionWithBehavingAs( "behavior of teletransport" ) != nilPointer )
                        character->setWayOfEntry( "via teleport" );
        }

        return true ;
}

void Room::dumpItemInsideThisRoom( const Item & item )
{
        std::cout << "   " << item.whichItemClass()
                        << " at " << item.getX() << " " << item.getY() << " " << item.getZ()
                        << " with dimensions " << item.getWidthX() << " x " << item.getWidthY() << " x " << item.getHeight()
                        << std::endl
                        << "   inside room \"" << this->nameOfFileWithDataAboutRoom << "\""
                        << " of " << getTilesX () << " x " << getTilesY () << " tiles"
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

        floorTiles[ tileX + ( getTilesX() * tileY ) ] = nilPointer ;
}

void Room::removeFloorTile( FloorTile * floorTile )
{
        if ( floorTile != nilPointer )
                this->floorTiles[ floorTile->getIndexOfColumn () ] = nilPointer ;

        delete floorTile ;
}

void Room::removeWallOnX( Wall * segment )
{
        if ( segment == nilPointer ) return ;

        std::vector< Wall* >::iterator si = std::find( wallX.begin (), wallX.end (), segment );
        if ( si != wallX.end () ) wallX.erase( si );

        delete segment ;
}

void Room::removeWallOnY( Wall * segment )
{
        if ( segment == nilPointer ) return ;

        std::vector< Wall* >::iterator si = std::find( wallY.begin (), wallY.end (), segment );
        if ( si != wallY.end () ) wallY.erase( si );

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
                mediator->wantShadowFromGridItem( * foundGridItem );
                mediator->wantToMaskWithGridItem( * foundGridItem );
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
                mediator->wantShadowFromFreeItem( * foundFreeItem );
                mediator->wantToMaskWithFreeItem( * foundFreeItem );
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
                                // activate the waiting character
                                // it is nil when there’s no other character in this room
                                mediator->setActiveCharacter( mediator->getWaitingCharacter() );
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
                                                std::cout << "and removing copy of character \"" << characterName << "\" created on entry to this room" << std::endl ;

                                                charactersWhoEnteredRoom.erase( it );
                                                /// -- it ; // not needed because of "break"

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

        mediator->lockGridItemsMutex();
        mediator->lockFreeItemsMutex();

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

        mediator->unlockFreeItemsMutex();
        mediator->unlockGridItemsMutex();

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
        unsigned int roomW = ( getTilesX () + getTilesY () ) * ( getSizeOfOneTile () << 1 ) ;

        if ( ( ! hasFloor() || ( ! hasDoorAt( "north" ) && ! hasDoorAt( "northeast" ) && ! hasDoorAt( "northwest" ) ) )
                && ! hasDoorAt( "west" ) && ! hasDoorAt( "westnorth" ) && ! hasDoorAt( "westsouth" ) )
        {
                roomW += getSizeOfOneTile () ;
        }
        if ( ( ! hasFloor() || ( ! hasDoorAt( "east" ) && ! hasDoorAt( "eastnorth" ) && ! hasDoorAt( "eastsouth" ) ) )
                && ! hasDoorAt( "south" ) && ! hasDoorAt( "southeast" ) && ! hasDoorAt( "southwest" ) )
        {
                roomW += getSizeOfOneTile () ;
        }

        return roomW ;
}

unsigned int Room::getHeightOfRoomImage () const
{
        unsigned int roomH = /* height of plane */ ( getTilesX () + getTilesY () ) * getSizeOfOneTile ();

        roomH += /* room’s height in 3D */ ( Room::MaxLayers + 2 ) * Room::LayerHeight ;
        roomH += /* height of floor */ 8 ;

        if ( ! hasDoorAt( "south" ) && ! hasDoorAt( "southwest" ) && ! hasDoorAt( "southeast" ) &&
                ! hasDoorAt( "west" ) && ! hasDoorAt( "westsouth" ) && ! hasDoorAt( "westnorth" ) )
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

        // clear image of room
        where.clearToColor( AllegroColor::keyColor() );

        /* if ( GameManager::getInstance().charactersFly() )
                where.clearToColor( Color::byName( "dark blue" ).toAllegroColor() );
        else
                where.clearToColor( Color::blackColor().toAllegroColor() ); */

        // draw tiles o’floor

        for ( unsigned int xCell = 0; xCell < getTilesX(); xCell ++ ) {
                for ( unsigned int yCell = 0; yCell < getTilesY(); yCell ++ )
                {
                        unsigned int column = getTilesX() * yCell + xCell ;
                        FloorTile* tile = floorTiles[ column ];

                        if ( tile != nilPointer ) // there’s tile of floor here
                        {
                                if ( shadingTransparency < 256 ) // visible shadows
                                        if ( tile->getWantShadow() )
                                                mediator->castShadowOnFloor( *tile );

                                tile->draw ();
                        }
                }
        }

        // draw walls without doors

        for ( std::vector< Wall * >::iterator wx = this->wallX.begin (); wx != this->wallX.end (); ++wx ) {
                ( *wx )->draw ();
        }
        for ( std::vector< Wall * >::iterator wy = this->wallY.begin (); wy != this->wallY.end (); ++wy ) {
                ( *wy )->draw ();
        }

        mediator->lockGridItemsMutex();
        mediator->lockFreeItemsMutex();

        // draw grid items

        for ( unsigned int i = 0 ; i < getTilesX() * getTilesY() ; i ++ )
        {
                for ( std::vector< GridItemPtr >::iterator gi = gridItems[ drawSequence[ i ] ].begin () ;
                        gi != gridItems[ drawSequence[ i ] ].end () ; ++ gi )
                {
                        GridItem& gridItem = *( *gi );

                        if ( shadingTransparency < 256 )
                        {
                                // cast shadow
                                if ( gridItem.getWantShadow() )
                                {
                                        mediator->castShadowOnGridItem( gridItem );
                                }
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

        mediator->unlockGridItemsMutex();
        mediator->unlockFreeItemsMutex();


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

        bounds[ "north" ] = hasDoorAt( "north" ) || hasDoorAt( "northeast" ) || hasDoorAt( "northwest" ) || ! hasFloor() ? oneTileLong : 0 ;
        bounds[ "east" ] = hasDoorAt( "east" ) || hasDoorAt( "eastnorth" ) || hasDoorAt( "eastsouth" ) || ! hasFloor() ? oneTileLong : 0 ;
        bounds[ "south" ] = oneTileLong * getTilesX() - ( hasDoorAt( "south" ) || hasDoorAt( "southeast" ) || hasDoorAt( "southwest" )  ? oneTileLong : 0 );
        bounds[ "west" ] = oneTileLong * getTilesY() - ( hasDoorAt( "west" ) || hasDoorAt( "westnorth" ) || hasDoorAt( "westsouth" )  ? oneTileLong : 0 );

        if ( this->isTripleRoom () ) {
                // limits for a triple room
                bounds[ "northeast" ] = hasDoorAt( "northeast" ) ? doors[ "northeast" ]->getLintel()->getX() + doors[ "northeast" ]->getLintel()->getWidthX() - oneTileLong : bounds[ "north" ];
                bounds[ "northwest" ] = hasDoorAt( "northwest" ) ? doors[ "northwest" ]->getLintel()->getX() + doors[ "northwest" ]->getLintel()->getWidthX() - oneTileLong : bounds[ "north" ];
                bounds[ "southeast" ] = hasDoorAt( "southeast" ) ? doors[ "southeast" ]->getLintel()->getX() + oneTileLong : bounds[ "south" ];
                bounds[ "southwest" ] = hasDoorAt( "southwest" ) ? doors[ "southwest" ]->getLintel()->getX() + oneTileLong : bounds[ "south" ];
                bounds[ "eastnorth" ] = hasDoorAt( "eastnorth" ) ? doors[ "eastnorth" ]->getLintel()->getY() + doors[ "eastnorth" ]->getLintel()->getWidthY() - oneTileLong : bounds[ "east" ];
                bounds[ "eastsouth" ] = hasDoorAt( "eastsouth" ) ? doors[ "eastsouth" ]->getLintel()->getY() + doors[ "eastsouth" ]->getLintel()->getWidthY() - oneTileLong : bounds[ "east" ];
                bounds[ "westnorth" ] = hasDoorAt( "westnorth" ) ? doors[ "westnorth" ]->getLintel()->getY() + oneTileLong : bounds[ "west" ];
                bounds[ "westsouth" ] = hasDoorAt( "westsouth" ) ? doors[ "westsouth" ]->getLintel()->getY() + oneTileLong : bounds[ "west" ];
        }
}

bool Room::activateCharacterByName( const std::string & name )
{
        for ( unsigned int i = 0 ; i < charactersYetInRoom.size () ; ++ i )
        {
                AvatarItemPtr character = charactersYetInRoom[ i ];
                if ( name == character->getOriginalKind () )
                {
                        mediator->setActiveCharacter( character );
                        return true ;
                }
        }

        return false ;
}

void Room::activate()
{
        mediator->beginUpdate();
}

void Room::deactivate()
{
        mediator->endUpdate();
}

bool Room::isActive() const
{
        return mediator->isThreadRunning() ;
}

bool Room::swapCharactersInRoom ()
{
        return mediator->pickNextCharacter () ;
}

bool Room::continueWithAliveCharacter ()
{
        AvatarItemPtr previouslyAliveCharacter = mediator->getActiveCharacter() ;
        assert( previouslyAliveCharacter != nilPointer );

        if ( previouslyAliveCharacter->getLives() > 0 ) return true ;

        // that character has no more lives, look for the other one in this room
        AvatarItemPtr nextCharacter ( nilPointer );
        for ( unsigned int i = 0 ; i < charactersYetInRoom.size () ; ++ i )
        {
                AvatarItemPtr character = charactersYetInRoom[ i ];
                if ( character != nilPointer && character->getKind() != previouslyAliveCharacter->getKind() ) {
                        nextCharacter = character ;
                        break ;
                }
        }

        if ( nextCharacter != nilPointer && nextCharacter->getLives() > 0 )
        {
                mediator->setActiveCharacter( nextCharacter );

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

        const FreeItemPtr & leftJamb = hasDoorAt( way ) ? doors[ way ]->getLeftJamb() : FreeItemPtr () ;

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
                        + ( hasDoorAt( "north" ) ? getSizeOfOneTile() >> 1 : 0 )
                                - ( hasDoorAt( "south" ) ? getSizeOfOneTile() >> 1 : 0 ) ;
}

int Room::getYCenterForItem( int widthY )
{
        return
                ( ( getLimitAt( "west" ) - getLimitAt( "east" ) + widthY ) >> 1 )
                        + ( hasDoorAt( "east" ) ? getSizeOfOneTile() >> 1 : 0 )
                                - ( hasDoorAt( "west" ) ? getSizeOfOneTile() >> 1 : 0 )
                                        - 1 ;
}
