
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

#if defined( DEBUG ) && DEBUG
# define DEBUG_ORIGIN_OF_ROOM   0

# if defined( DEBUG_ORIGIN_OF_ROOM ) && DEBUG_ORIGIN_OF_ROOM
  # include "FlickeringColor.hpp"
# endif
#endif

#include <tinyxml2.h>

#include <algorithm> // std::for_each

using behaviors::Elevator ;


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
        for ( unsigned int i = 0 ; i < xTiles * yTiles /* + 1 */ ; i ++ )
        {
                this->floorTiles.push_back( nilPointer );
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

        // 0 for pure black shadows, 128 for 50% opacity of shadows, 256 for no shadows
        shadingOpacity = GameManager::getInstance().getCastShadows () ? 128 /* 0 */ : 256 ;

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

std::string Room::whichRoom () const
{
        if ( isSingleRoom () ) return "single" ;

        // is it the double room along Y
        if ( getTilesX() <= maxTilesOfSingleRoom && getTilesY() > maxTilesOfSingleRoom )
        {
                return "double along Y" ;
        }
        // is it the double room along X
        else if ( getTilesX() > maxTilesOfSingleRoom && getTilesY() <= maxTilesOfSingleRoom )
        {
                return "double along X" ;
        }

        return "triple" ;
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
                                        z = ( z > Isomot::FloorZ ) ? ( z / Isomot::LayerHeight ) : Isomot::FloorZ ;
                                        item->SetAttribute( "z", z );

                                        tinyxml2::XMLElement* kindOfItem = roomXml.NewElement( "kind" );
                                        kindOfItem->SetText( theItem->getKind().c_str () );
                                        item->InsertEndChild( kindOfItem );

                                        tinyxml2::XMLElement* itemOrientation = roomXml.NewElement( "orientation" );
                                        std::string orientation = theItem->getOrientation();
                                        if ( orientation == "nowhere" ) orientation = "none" ;
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

                                item->SetAttribute( "x", theItem->getOriginalCellX() );
                                item->SetAttribute( "y", theItem->getOriginalCellY() );
                                item->SetAttribute( "z", theItem->getOriginalCellZ() );

                                tinyxml2::XMLElement* kindOfItem = roomXml.NewElement( "kind" );
                                kindOfItem->SetText( theItem->getKind().c_str () );
                                item->InsertEndChild( kindOfItem );

                                tinyxml2::XMLElement* itemOrientation = roomXml.NewElement( "orientation" );
                                std::string orientation = theItem->getOrientation();
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
                                z = ( z > Isomot::FloorZ ) ? ( z / Isomot::LayerHeight ) : Isomot::FloorZ ;
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

        floorTiles[ floorTile->getColumn() ] = floorTile;
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

                                addGridItem( GridItemPtr( new GridItem( descriptionOfWall, 0, segment->getPosition(), 0, "nowhere" ) ) );

                                wallsToBin.push_back( segment );
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

                                addGridItem( GridItemPtr( new GridItem( descriptionOfWall, segment->getPosition(), 0, 0, "nowhere" ) ) );

                                wallsToBin.push_back( segment );
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

        if ( gridItem->getHeight() < 1 )
        {
                std::cerr << "can’t add " << gridItem->whichItemClass() << " which height is zero" << std::endl ;
                return;
        }

        gridItem->setMediator( mediator );

        mediator->clearStackOfCollisions ();

        const std::string & kindOfItem = gridItem->getKind () ;
        unsigned int uniqueNumberOfItem = nextNumbers[ kindOfItem ] ;
        if ( uniqueNumberOfItem == 0 ) // is it new kind
        {
                 uniqueNumberOfItem = 1;
        }
        nextNumbers[ kindOfItem ] = uniqueNumberOfItem + 1;

        std::ostringstream name;
        name << kindOfItem << " " << util::toStringWithOrdinalSuffix( uniqueNumberOfItem ) <<
                " @" << gridItem->getX() << "," << gridItem->getY() << "," << gridItem->getZ() ;

        gridItem->setUniqueName( name.str() );

        addGridItemToContainer( gridItem );

        if ( gridItem->getZ() != Isomot::FloorZ )
        {
                // when item goes lower than top, look for collisions
                mediator->lookForCollisionsOf( gridItem->getUniqueName() );
        }
        else
        {
                // whem item goes to top, modify its position on Z
                gridItem->setZ( mediator->findHighestZ( *gridItem ) );
                std::cout << "for " << gridItem->whichItemClass() << " \"" << gridItem->getUniqueName() << "\" on top Z is " << gridItem->getZ() << std::endl ;
        }

        if ( ! mediator->isStackOfCollisionsEmpty () )
        {
                // can’t add item when there’s some collision
                std::cerr << "there’s collision with " << gridItem->whichItemClass() << std::endl ;
                return ;
        }

        // calculate the offset of the item’s image from the origin of room
        std::pair< int, int > offset (
                ( ( getSizeOfOneTile () * ( gridItem->getCellX() - gridItem->getCellY() ) ) << 1 ) - ( gridItem->getRawImage().getWidth() >> 1 ) + 1,
                getSizeOfOneTile () * ( gridItem->getCellX() + gridItem->getCellY() + 2 ) - gridItem->getRawImage().getHeight() - gridItem->getZ() - 1
        ) ;
        gridItem->setOffset( offset );

        mediator->wantShadowFromGridItem( *gridItem );
        mediator->wantToMaskWithGridItem( *gridItem );

#if defined( DEBUG ) && DEBUG
        std::cout << gridItem->whichItemClass() << " \"" << gridItem->getUniqueName() << "\" is yet part of room \"" << getNameOfRoomDescriptionFile() << "\"" << std::endl ;
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

        if ( freeItem->getX() < 0 || freeItem->getY() < 1 || freeItem->getZ() < Isomot::FloorZ )
        {
                std::cerr << "coordinates for " << freeItem->whichItemClass() << " are out of limits" << std::endl ;
                dumpItemInsideThisRoom( *freeItem );
                return;
        }

        if ( freeItem->getHeight() < 1 || freeItem->getWidthX() < 1 || freeItem->getWidthY() < 1 )
        {
                std::cerr << "can’t add " << freeItem->whichItemClass() << " which dimension is zero" << std::endl ;
                return;
        }

        if ( ( freeItem->getX() + static_cast< int >( freeItem->getWidthX() ) > static_cast< int >( this->getTilesX() * getSizeOfOneTile() ) )
                || ( freeItem->getY() - static_cast< int >( freeItem->getWidthY() ) + 1 < 0 )
                || ( freeItem->getY() > static_cast< int >( this->getTilesY() * getSizeOfOneTile() ) - 1 ) )
        {
                std::cerr << "coordinates for " << freeItem->whichItemClass() << " are out of room" << std::endl ;
                dumpItemInsideThisRoom( *freeItem );
                return;
        }

        freeItem->setMediator( mediator );

        mediator->clearStackOfCollisions ();

        const std::string & kindOfItem = freeItem->getKind () ;
        unsigned int uniqueNumberOfItem = nextNumbers[ kindOfItem ] ;
        if ( uniqueNumberOfItem == 0 ) // is it new kind
        {
                 uniqueNumberOfItem = 1;
        }
        nextNumbers[ kindOfItem ] = uniqueNumberOfItem + 1;

        freeItem->setUniqueName( kindOfItem + " " + util::toStringWithOrdinalSuffix( uniqueNumberOfItem ) );

        addFreeItemToContainer( freeItem );

        // for item which is placed at some height, look for collisions
        if ( freeItem->getZ() > Isomot::FloorZ )
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
                std::cerr << "there’s collision with " << freeItem->whichItemClass() << std::endl ;
                dumpItemInsideThisRoom( *freeItem );
                return;
        }

        // calculate the offset of the item’s image from the origin of room
        std::pair< int, int > offset (
                ( ( freeItem->getX() - freeItem->getY() ) << 1 ) + freeItem->getWidthX() + freeItem->getWidthY() - ( freeItem->getRawImage().getWidth() >> 1 ) - 1,
                freeItem->getX() + freeItem->getY() + freeItem->getWidthX() - freeItem->getRawImage().getHeight() - freeItem->getZ()
        ) ;
        freeItem->setOffset( offset );

        mediator->wantShadowFromFreeItem( *freeItem );
        mediator->wantToMaskWithFreeItem( *freeItem );

#if defined( DEBUG ) && DEBUG
        std::cout << freeItem->whichItemClass() << " \"" << freeItem->getUniqueName() << "\" is yet in room \"" << getNameOfRoomDescriptionFile() << "\"" << std::endl ;
#endif
}

bool Room::addCharacterToRoom( const AvatarItemPtr & character, bool characterEntersRoom )
{
        for ( std::vector< AvatarItemPtr >::const_iterator pi = charactersYetInRoom.begin (); pi != charactersYetInRoom.end (); ++pi )
        {
                if ( character == *pi )
                {
                        // this character is already in room
                        return false;
                }
        }

        if ( characterEntersRoom )
        {
                for ( std::vector< AvatarItemPtr >::iterator epi = charactersWhoEnteredRoom.begin (); epi != charactersWhoEnteredRoom.end (); ++epi )
                {
                        AvatarItemPtr characterEntered = *epi ;

                        if ( characterEntered->getOriginalKind() == "headoverheels" && character->getOriginalKind() != "headoverheels" )
                        {
                                // case when joined character enters room, splits in this room, and one of characters exits & re~enters
                                std::cout << "character \"" << character->getOriginalKind() << "\" enters but joined \"headoverheels\" entered the same room before" << std::endl ;

                                // bin joined character
                                charactersWhoEnteredRoom.erase( epi );
                                /*  epi-- ;  */

                                // add copy of another character as entered
                                copyAnotherCharacterAsEntered( character->getOriginalKind() );

                                break;
                        }

                        if ( characterEntered->getOriginalKind() == character->getOriginalKind() )
                        {
                                // case when character returns back to this room, maybe via different way
                                std::cout << "character \"" << character->getOriginalKind() << "\" already entered this room some time ago" << std::endl ;

                                // bin previous entry
                                charactersWhoEnteredRoom.erase( epi );
                                /*  epi-- ;  */

                                break;
                        }
                }
        }

        if ( character->getX() < 0 || character->getY() < 1 || character->getZ() < Isomot::FloorZ )
        {
                std::cerr << "coordinates for " << character->whichItemClass() << " are out of limits" << std::endl ;
                dumpItemInsideThisRoom( *character );
                return false;
        }

        if ( character->getHeight() < 1 || character->getWidthX() < 1 || character->getWidthY() < 1 )
        {
                std::cerr << "can’t add " << character->whichItemClass() << " which dimension is zero" << std::endl ;
                return false;
        }

        if ( ( character->getX() + static_cast< int >( character->getWidthX() ) > static_cast< int >( this->getTilesX() * getSizeOfOneTile() ) )
                || ( character->getY() - static_cast< int >( character->getWidthY() ) + 1 < 0 )
                || ( character->getY() > static_cast< int >( this->getTilesY() * getSizeOfOneTile() ) - 1 ) )
        {
                std::cerr << "coordinates for " << character->whichItemClass() << " are out of room" << std::endl ;
                dumpItemInsideThisRoom( *character );
                return false;
        }

        character->setMediator( mediator );

        mediator->clearStackOfCollisions ();

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
        if ( character->getZ() > Isomot::FloorZ )
        {
                mediator->lookForCollisionsOf( character->getUniqueName() );
                while ( ! mediator->isStackOfCollisionsEmpty () )
                {
                        character->setZ( character->getZ() + Isomot::LayerHeight );
                        mediator->clearStackOfCollisions ();
                        mediator->lookForCollisionsOf( character->getUniqueName() );
                }
        }
        // for item at the top of column
        else
        {
                character->setZ( mediator->findHighestZ( *character ) );
        }

        // collision is found, so can’t add this item
        if ( ! mediator->isStackOfCollisionsEmpty () )
        {
                std::cerr << "there’s collision with " << character->whichItemClass() << std::endl ;
                dumpItemInsideThisRoom( *character );
                return false;
        }

        // the offset of the character’s image from the room's origin
        std::pair< int, int > offset (
                ( ( character->getX() - character->getY() ) << 1 ) + character->getWidthX() + character->getWidthY() - ( character->getRawImage().getWidth() >> 1 ) - 1,
                character->getX() + character->getY() + character->getWidthX() - character->getRawImage().getHeight() - character->getZ()
        ) ;
        character->setOffset( offset );

        if ( mediator->getActiveCharacter() == nilPointer )
        {
                mediator->setActiveCharacter( character );
        }

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

        return true;
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

                        for ( std::vector< AvatarItemPtr >::const_iterator epi = charactersWhoEnteredRoom.begin (); epi != charactersWhoEnteredRoom.end (); ++epi )
                        {
                                if ( ( *epi )->getOriginalKind() == ( *pi )->getOriginalKind() )
                                {
                                        alreadyThere = true;
                                        break;
                                }
                        }

                        if ( ! alreadyThere )
                        {
                                AvatarItemPtr copy( new AvatarItem( *( *pi ) ) );
                                copy->setBehaviorOf( ( *pi )->getBehavior()->getNameOfBehavior() );
                                copy->setWayOfEntry( "just wait" );

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
                floorTiles[ floorTile->getColumn () ] = nilPointer ;

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

                                std::cout << "removing " << ( *g )->whichItemClass() << " \"" << uniqueName <<
                                        "\" from room \"" << getNameOfRoomDescriptionFile() << "\"" << std::endl ;

                                gridItems[ column ].erase( g );

                                break ;
                        }
                }

                if ( found ) break ;
        }

        if ( found )
        {
                mediator->wantShadowFromGridItem( *gridItem );
                mediator->wantToMaskWithGridItem( *gridItem );
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

                        std::cout << "removing " << ( *f )->whichItemClass() << " \"" << uniqueName <<
                                "\" from room \"" << getNameOfRoomDescriptionFile() << "\"" << std::endl ;

                        freeItems.erase( f );

                        break ;
                }
        }

        if ( found )
        {
                mediator->wantShadowFromFreeItem( *freeItem );
                mediator->wantToMaskWithFreeItem( *freeItem );
        }
}

bool Room::removeCharacterFromRoom( const AvatarItem & character, bool characterExitsRoom )
{
        for ( std::vector< AvatarItemPtr >::iterator pi = charactersYetInRoom.begin (); pi != charactersYetInRoom.end (); ++pi )
        {
                if ( character.getUniqueName() == ( *pi )->getUniqueName() )
                {
                        const std::string & characterName = character.getOriginalKind() ;
                        bool wasActive = character.isActiveCharacter() ;

                        removeFreeItemByUniqueName( character.getUniqueName() );

                        nextNumbers[ "character " + characterName ] -- ;

                        if ( wasActive )
                        {
                                // activate other character, or set it to nil when there’s no other character in room
                                mediator->setActiveCharacter( mediator->getWaitingCharacter() );
                        }

                        charactersYetInRoom.erase( pi );
                        /// pi --; // not needed, this iteration is the last one due to "return" below

                        // when a character leaves room, bin its copy on entry
                        if ( characterExitsRoom )
                        {
                                for ( std::vector< AvatarItemPtr >::iterator epi = charactersWhoEnteredRoom.begin (); epi != charactersWhoEnteredRoom.end (); ++epi )
                                {
                                        if ( ( *epi )->getOriginalKind() == characterName )
                                        {
                                                std::cout << "and removing copy of character \"" << characterName << "\" created on entry to this room" << std::endl ;

                                                charactersWhoEnteredRoom.erase( epi );
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

bool Room::isAnyCharacterStillInRoom () const
{
        return ! charactersYetInRoom.empty () ;
}

unsigned int Room::removeBars ()
{
        unsigned int howManyBars = 0;

        mediator->lockGridItemsMutex();
        mediator->lockFreeItemsMutex();

        std::vector< std::string > gridItemsToRemove ;

        for ( unsigned int column = 0; column < gridItems.size(); column ++ )
        {
                for ( std::vector< GridItemPtr >::const_iterator gi = gridItems[ column ].begin (); gi != gridItems[ column ].end (); ++ gi )
                {
                        GridItemPtr gridItem = *gi ;
                        if ( gridItem == nilPointer ) continue ;

                        if ( gridItem->getKind().find( "bars" ) != std::string::npos )
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

                if ( freeItem->getKind().find( "bars" ) != std::string::npos )
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
                for ( std::vector< GridItemPtr >::iterator gi = gridItems[ column ].begin (); gi != gridItems[ column ].end (); ++ gi )
                {
                        GridItem& gridItem = *( *gi );
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
                FreeItem& freeItem = *( *fi );
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

        roomH += /* room’s height in 3D */ ( Isomot::MaxLayers + 2 ) * Isomot::LayerHeight ;
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

        for ( unsigned int xCell = 0; xCell < getTilesX(); xCell++ )
        {
                for ( unsigned int yCell = 0; yCell < getTilesY(); yCell++ )
                {
                        unsigned int column = getTilesX() * yCell + xCell;
                        FloorTile* tile = floorTiles[ column ];

                        if ( tile != nilPointer )  // if there is tile of floor here
                        {
                                // shade this tile when shadows are on
                                if ( shadingOpacity < 256 )
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

                                tile->draw ();
                        }
                }
        }

        // draw walls without doors

        for ( std::vector< Wall * >::iterator wx = this->wallX.begin (); wx != this->wallX.end (); ++wx )
        {
                ( *wx )->draw ();
        }
        for ( std::vector< Wall * >::iterator wy = this->wallY.begin (); wy != this->wallY.end (); ++wy )
        {
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

                        if ( shadingOpacity < 256 )
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
                FreeItem& freeItem = *( *fi );

                // shade an item when shadows are on
                if ( shadingOpacity < 256 )
                {
                        freeItem.requestShadow();
                }
        }

        // then mask it and finally draw it
        for ( std::vector< FreeItemPtr >::iterator fi = freeItems.begin (); fi != freeItems.end (); ++ fi )
        {
                FreeItem& freeItem = *( *fi );

                freeItem.requestMask();
                freeItem.draw ();
        }

        mediator->unlockGridItemsMutex();
        mediator->unlockFreeItemsMutex();



#if defined( DEBUG_ORIGIN_OF_ROOM ) && DEBUG_ORIGIN_OF_ROOM
        // draw point of room’s origin
        if ( FlickeringColor::flickerWhiteAndTransparent() != Color::whiteColor() )
        {
                allegro::fillCircle( getX0(), getY0(), 4, Color::blackColor().toAllegroColor() );
                where.drawPixelAt( getX0(), getY0(), Color::whiteColor().toAllegroColor() );
        }
        else
        {
                allegro::fillCircle( getX0(), getY0(), 4, Color::whiteColor().toAllegroColor() );
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

        // door limits of triple room
        bounds[ "northeast" ] = hasDoorAt( "northeast" ) ? doors[ "northeast" ]->getLintel()->getX() + doors[ "northeast" ]->getLintel()->getWidthX() - oneTileLong : bounds[ "north" ];
        bounds[ "northwest" ] = hasDoorAt( "northwest" ) ? doors[ "northwest" ]->getLintel()->getX() + doors[ "northwest" ]->getLintel()->getWidthX() - oneTileLong : bounds[ "north" ];
        bounds[ "southeast" ] = hasDoorAt( "southeast" ) ? doors[ "southeast" ]->getLintel()->getX() + oneTileLong : bounds[ "south" ];
        bounds[ "southwest" ] = hasDoorAt( "southwest" ) ? doors[ "southwest" ]->getLintel()->getX() + oneTileLong : bounds[ "south" ];
        bounds[ "eastnorth" ] = hasDoorAt( "eastnorth" ) ? doors[ "eastnorth" ]->getLintel()->getY() + doors[ "eastnorth" ]->getLintel()->getWidthY() - oneTileLong : bounds[ "east" ];
        bounds[ "eastsouth" ] = hasDoorAt( "eastsouth" ) ? doors[ "eastsouth" ]->getLintel()->getY() + doors[ "eastsouth" ]->getLintel()->getWidthY() - oneTileLong : bounds[ "east" ];
        bounds[ "westnorth" ] = hasDoorAt( "westnorth" ) ? doors[ "westnorth" ]->getLintel()->getY() + oneTileLong : bounds[ "west" ];
        bounds[ "westsouth" ] = hasDoorAt( "westsouth" ) ? doors[ "westsouth" ]->getLintel()->getY() + oneTileLong : bounds[ "west" ];
}

void Room::calculateCoordinatesOfOrigin( bool hasNorthDoor, bool hasEastDoor, bool hasSouthDoor, bool hasWestDoor )
{
        coordinatesOfOrigin.second = ( Isomot::MaxLayers + 2 ) * Isomot::LayerHeight ;
        coordinatesOfOrigin.first = getTilesY () * ( getSizeOfOneTile () << 1 ) ;

        if ( ! hasNorthDoor && ! hasWestDoor && hasFloor() ) coordinatesOfOrigin.first += getSizeOfOneTile () ;
        ( void ) hasEastDoor ; ( void ) hasSouthDoor ;

#if defined( DEBUG_ORIGIN_OF_ROOM ) && DEBUG_ORIGIN_OF_ROOM
        std::cout << "origin of room \"" << getNameOfRoomDescriptionFile() << "\" is at " <<
                        "( " << coordinatesOfOrigin.first << ", " << coordinatesOfOrigin.second << " )" <<
                        std::endl ;
#endif
}

bool Room::activateCharacterByName( const std::string & character )
{
        for ( std::vector< AvatarItemPtr >::const_iterator pi = charactersYetInRoom.begin (); pi != charactersYetInRoom.end (); ++pi )
        {
                if ( character == ( *pi )->getOriginalKind() )
                {
                        mediator->setActiveCharacter( *pi );
                        return true;
                }
        }

        return false;
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

        if ( previouslyAliveCharacter->getLives() == 0 )
        {
                // look for the next character
                std::vector< AvatarItemPtr >::iterator i = charactersYetInRoom.begin () ;
                while ( i != charactersYetInRoom.end () )
                {
                        if ( *i != nilPointer && ( *i )->getKind () == previouslyAliveCharacter->getKind () ) break;
                        ++ i ;
                }
                ++i ;
                mediator->setActiveCharacter( i != this->charactersYetInRoom.end () ? ( *i ) : *this->charactersYetInRoom.begin () );

                // is there other alive characters in room
                if ( previouslyAliveCharacter != mediator->getActiveCharacter() )
                {
                        removeCharacterFromRoom( *previouslyAliveCharacter, true );

                        this->activate();
                        return true;
                }

                return false;
        }

        return true;
}

bool Room::calculateEntryCoordinates( const Way & wayOfEntry,
                                        int widthX, int widthY,
                                        int northBound, int eastBound, int southBound, int westBound,
                                        int* x, int* y, int* z )
{
        int differenceOnX = 0 ;
        int differenceOnY = 0 ;

        if ( wayOfEntry.toString() == "above" || wayOfEntry.toString() == "below" ||
                        wayOfEntry.toString() == "via teleport" || wayOfEntry.toString() == "via second teleport" )
        {
                const int limitOfSingleRoom = maxTilesOfSingleRoom * getSizeOfOneTile ();

                // calculate the difference on X axis when moving from a single room to a double room or vice versa
                if ( ( bounds[ "south" ] - bounds[ "north" ] > limitOfSingleRoom && southBound - northBound <= limitOfSingleRoom ) ||
                        ( bounds[ "south" ] - bounds[ "north" ] <= limitOfSingleRoom && southBound - northBound > limitOfSingleRoom ) )
                {
                        differenceOnX = ( bounds[ "south" ] - bounds[ "north" ] - southBound + northBound ) >> 1;
                }

                // calculate the difference on Y axis when moving from a single room to a double room or vice versa
                if ( ( bounds[ "west" ] - bounds[ "east" ] > limitOfSingleRoom && westBound - eastBound <= limitOfSingleRoom ) ||
                        ( bounds[ "west" ] - bounds[ "east" ] <= limitOfSingleRoom && westBound - eastBound > limitOfSingleRoom ) )
                {
                        differenceOnY = ( bounds[ "west" ] - bounds[ "east" ] - westBound + eastBound ) >> 1;
                }
        }

        bool okay = false ;
        unsigned int oneTileSize = getSizeOfOneTile ();

        // calculate coordinates by the way of entry
        switch ( wayOfEntry.getIntegerOfWay () )
        {
                case Way::North:
                        *x = bounds[ "north" ] - oneTileSize + 1;
                        *y = doors[ "north" ]->getLeftJamb()->getY() - doors[ "north" ]->getLeftJamb()->getWidthY();
                        *z = doors[ "north" ]->getLeftJamb()->getZ();
                        okay = true ;
                        break;

                case Way::South:
                        *x = bounds[ "south" ] + oneTileSize - widthX;
                        *y = doors[ "south" ]->getLeftJamb()->getY() - doors[ "south" ]->getLeftJamb()->getWidthY();
                        *z = doors[ "south" ]->getLeftJamb()->getZ();
                        okay = true ;
                        break;

                case Way::East:
                        *x = doors[ "east" ]->getLeftJamb()->getX() + doors[ "east" ]->getLeftJamb()->getWidthX();
                        *y = bounds[ "east" ] - oneTileSize + widthY;
                        *z = doors[ "east" ]->getLeftJamb()->getZ();
                        okay = true ;
                        break;

                case Way::West:
                        *x = doors[ "west" ]->getLeftJamb()->getX() + doors[ "west" ]->getLeftJamb()->getWidthX();
                        *y = bounds[ "west" ] + oneTileSize - 1;
                        *z = doors[ "west" ]->getLeftJamb()->getZ();
                        okay = true ;
                        break;

                case Way::Northeast:
                        *x = bounds[ "northeast" ];
                        *y = doors[ "northeast" ]->getLeftJamb()->getY() - doors[ "northeast" ]->getLeftJamb()->getWidthY();
                        *z = doors[ "northeast" ]->getLeftJamb()->getZ();
                        okay = true ;
                        break;

                case Way::Northwest:
                        *x = bounds[ "northwest" ];
                        *y = doors[ "northwest" ]->getLeftJamb()->getY() - doors[ "northwest" ]->getLeftJamb()->getWidthY();
                        *z = doors[ "northwest" ]->getLeftJamb()->getZ();
                        okay = true ;
                        break;

                case Way::Southeast:
                        *x = bounds[ "southeast" ] - widthX;
                        *y = doors[ "southeast" ]->getLeftJamb()->getY() - doors[ "southeast" ]->getLeftJamb()->getWidthY();
                        *z = doors[ "southeast" ]->getLeftJamb()->getZ();
                        okay = true ;
                        break;

                case Way::Southwest:
                        *x = bounds[ "southwest" ] - widthX;
                        *y = doors[ "southwest" ]->getLeftJamb()->getY() - doors[ "southwest" ]->getLeftJamb()->getWidthY();
                        *z = doors[ "southwest" ]->getLeftJamb()->getZ();
                        okay = true ;
                        break;

                case Way::Eastnorth:
                        *x = doors[ "eastnorth" ]->getLeftJamb()->getX() + doors[ "eastnorth" ]->getLeftJamb()->getWidthX();
                        *y = bounds[ "eastnorth" ] + widthY;
                        *z = doors[ "eastnorth" ]->getLeftJamb()->getZ();
                        okay = true ;
                        break;

                case Way::Eastsouth:
                        *x = doors[ "eastsouth" ]->getLeftJamb()->getX() + doors[ "eastsouth" ]->getLeftJamb()->getWidthX();
                        *y = bounds[ "eastsouth" ] + widthY;
                        *z = doors[ "eastsouth" ]->getLeftJamb()->getZ();
                        okay = true ;
                        break;

                case Way::Westnorth:
                        *x = doors[ "westnorth" ]->getLeftJamb()->getX() + doors[ "westnorth" ]->getLeftJamb()->getWidthX();
                        *y = bounds[ "westnorth" ] - widthY;
                        *z = doors[ "westnorth" ]->getLeftJamb()->getZ();
                        okay = true ;
                        break;

                case Way::Westsouth:
                        *x = doors[ "westsouth" ]->getLeftJamb()->getX() + doors[ "westsouth" ]->getLeftJamb()->getWidthX();
                        *y = bounds[ "westsouth" ] - widthY;
                        *z = doors[ "westsouth" ]->getLeftJamb()->getZ();
                        okay = true ;
                        break;

                case Way::Above:
                        *x += bounds[ "north" ] - northBound + ( ( bounds[ "south" ] - bounds[ "north" ] - southBound + northBound ) >> 1 );
                        *x += ( *x < ( ( bounds[ "south" ] - bounds[ "north" ] ) >> 1 ) ? -differenceOnX : differenceOnX );
                        *y += bounds[ "east" ] - eastBound + ( ( bounds[ "west" ] - bounds[ "east" ] - westBound + eastBound ) >> 1 );
                        *y += ( *y < ( ( bounds[ "west" ] - bounds[ "east" ] ) >> 1 ) ? -differenceOnY : differenceOnY );
                        *z = Isomot::MaxLayers * Isomot::LayerHeight ;
                        okay = true ;
                        break;

                case Way::Below:
                        *x += bounds[ "north" ] - northBound + ( ( bounds[ "south" ] - bounds[ "north" ] - southBound + northBound ) >> 1 );
                        *x += ( *x < ( ( bounds[ "south" ] - bounds[ "north" ] ) >> 1 ) ? -differenceOnX : differenceOnX );
                        *y += bounds[ "east" ] - eastBound + ( ( bounds[ "west" ] - bounds[ "east" ] - westBound + eastBound) >> 1 );
                        *y += ( *y < ( ( bounds[ "west" ] - bounds[ "east" ]) >> 1 ) ? -differenceOnY : differenceOnY );
                        *z = Isomot::LayerHeight ;
                        okay = true ;
                        break;

                case Way::ByTeleport:
                case Way::ByTeleportToo:
                        *x += bounds[ "north" ] - northBound + ( ( bounds[ "south" ] - bounds[ "north" ] - southBound + northBound ) >> 1 );
                        *x += ( *x < ( ( bounds[ "south" ] - bounds[ "north" ] ) >> 1 ) ? -differenceOnX : differenceOnX );
                        *y += differenceOnY + bounds[ "east" ] - eastBound + ( ( bounds[ "west" ] - bounds[ "east" ] - westBound + eastBound ) >> 1 );
                        *y += ( *y < ( ( bounds[ "west" ] - bounds[ "east" ] ) >> 1 ) ? -differenceOnY : differenceOnY );
                        okay = true ;
                        break;

                default:
                        ;
        }

        return okay ;
}

int Room::getXCenterForItem( const DescriptionOfItem* data )
{
        return
                ( ( getLimitAt( "south" ) - getLimitAt( "north" ) + data->getWidthX() ) >> 1 )
                        + ( hasDoorAt( "north" ) ? getSizeOfOneTile() >> 1 : 0 )
                                - ( hasDoorAt( "south" ) ? getSizeOfOneTile() >> 1 : 0 ) ;
}

int Room::getYCenterForItem( const DescriptionOfItem* data )
{
        return
                ( ( getLimitAt( "west" ) - getLimitAt( "east" ) + data->getWidthY() ) >> 1 )
                        + ( hasDoorAt( "east" ) ? getSizeOfOneTile() >> 1 : 0 )
                                - ( hasDoorAt( "west" ) ? getSizeOfOneTile() >> 1 : 0 )
                                        - 1 ;
}
