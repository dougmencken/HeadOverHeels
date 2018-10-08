
#include "Room.hpp"
#include "FloorTile.hpp"
#include "Wall.hpp"
#include "Item.hpp"
#include "GridItem.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "Mediator.hpp"
#include "Camera.hpp"
#include "GameManager.hpp"
#include "Behavior.hpp"
#include "Color.hpp"

#include <algorithm>


namespace isomot
{

Room::Room( const std::string& roomFile, const std::string& scenery, int xTiles, int yTiles, int tileSize, const std::string& floor )
: Mediated( )
        , nameOfFileWithDataAboutRoom( roomFile )
        , scenery( scenery )
        , tileSize( tileSize )
        , kindOfFloor( floor )
        , wayOfExit( "no exit" )
        , whereToDraw( nilPointer )
{
        playersYetInRoom.clear ();
        playersWhoEnteredRoom.clear ();

        this->numberOfTiles.first = xTiles;
        this->numberOfTiles.second = yTiles;
        this->mediator = new Mediator( this );
        this->camera = new Camera( this );

        nextNumbers.clear ();

        bounds.clear ();
        bounds[ "north" ] = 64000;
        bounds[ "south" ] = 64000;
        bounds[ "rast" ] = 64000;
        bounds[ "west" ] = 64000;
        bounds[ "northeast" ] = 64000;
        bounds[ "southeast" ] = 64000;
        bounds[ "southwest" ] = 64000;
        bounds[ "northwest" ] = 64000;
        bounds[ "eastnorth" ] = 64000;
        bounds[ "eastsouth" ] = 64000;
        bounds[ "westnorth" ] = 64000;
        bounds[ "westsouth" ] = 64000;

        doors.clear ();
        doors[ "north" ] = 0;
        doors[ "south" ] = 0;
        doors[ "east" ] = 0;
        doors[ "west" ] = 0;
        doors[ "northeast" ] = 0;
        doors[ "southeast" ] = 0;
        doors[ "southwest" ] = 0;
        doors[ "northwest" ] = 0;
        doors[ "eastnorth" ] = 0;
        doors[ "eastsouth" ] = 0;
        doors[ "westnorth" ] = 0;
        doors[ "westsouth" ] = 0;

        // create empty floor
        for ( int i = 0; i < xTiles * yTiles + 1; i++ )
        {
                this->floor.push_back( nilPointer );
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

        int pos = 0;
        this->drawSequence = new int[ xTiles * yTiles ];

        for ( int f = 0; f <= ( xTiles + yTiles - 1 ); f++ )
        {
                for ( int n = yTiles - 1; n >= 0; n-- )
                {
                        int index = n * ( xTiles - 1 ) + f;
                        if ( ( index >= n * xTiles ) && ( index < ( n + 1 ) * xTiles ) )
                        {
                                this->drawSequence[ pos++ ] = index;
                        }
                }
        }

        // crea la imagen with suitable dimensions to draw the active room

        int roomW = ScreenWidth();
        int roomH = ScreenHeight();

        if ( isSingleRoom() )
        {
                // single room ( id est up to 10 x 10 tiles ) just fits to the screen
                // but image of double or triple room is larger
        }
        else if ( xTiles > 10 && yTiles > 10 )
        {
                roomW += 20 * ( tileSize << 1 );
                roomH += 20 * tileSize;
        }
        else if ( xTiles > 10 || yTiles > 10 )
        {
                roomW += ( xTiles > 10 ? ( ( xTiles - 10 ) * ( tileSize << 1 ) ) : 0 )
                                        + ( yTiles > 10 ? ( ( yTiles - 10 ) * ( tileSize << 1 ) ) : 0 );
                roomH += ( xTiles > 10 ? ( ( xTiles - 10 ) * tileSize) : 0 )
                                        + ( yTiles > 10 ? ( ( yTiles - 10 ) * tileSize) : 0 );
        }

        whereToDraw = new Picture( roomW, roomH );

        // 0 for pure black shadows, 128 for 50% opacity of shadows, 256 for no shadows
        shadingOpacity = isomot::GameManager::getInstance()->getDrawShadows () ? 128 /* 0 */ : 256 ;

        // since yet room is active
        this->active = true;

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
        std::for_each( floor.begin (), floor.end (), DeleteIt() );

        // bin walls
        std::for_each( wallX.begin (), wallX.end (), DeleteIt() );
        std::for_each( wallY.begin (), wallY.end (), DeleteIt() );

        // bin sequence of drawing
        delete drawSequence;

        delete camera;

        delete mediator;

        // bin players

        while ( ! this->playersYetInRoom.empty () )
        {
                PlayerItem* player = *( playersYetInRoom.begin () );
                playersYetInRoom.remove( player );
                delete player;
        }

        while ( ! this->playersWhoEnteredRoom.empty () )
        {
                const PlayerItem* player = *( playersWhoEnteredRoom.begin () );
                playersWhoEnteredRoom.remove( player );
                delete player;
        }
}

std::list < PlayerItem * > Room::getPlayersYetInRoom () const
{
        return playersYetInRoom ;
}

std::list < const PlayerItem * > Room::getPlayersWhoEnteredRoom () const
{
        return playersWhoEnteredRoom ;
}

void Room::addFloor( FloorTile * floorTile )
{
        floorTile->setMediator( mediator );
        floorTile->calculateOffset();

        this->floor[ floorTile->getColumn() ] = floorTile;
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
        FreeItem* leftJamb = door->getLeftJamb() ;
        FreeItem* rightJamb = door->getRightJamb() ;
        FreeItem* lintel = door->getLintel() ;

        leftJamb->setPartOfDoor( true );
        rightJamb->setPartOfDoor( true );
        lintel->setPartOfDoor( true );

        addFreeItem( leftJamb );
        addFreeItem( rightJamb );
        addFreeItem( lintel );
}

void Room::updateWallsWithDoors ()
{
        // update position of walls
        for ( std::vector< Wall * >::iterator wx = this->wallX.begin (); wx != this->wallX.end (); ++wx )
        {
                ( *wx )->calculateOffset();
        }
        for ( std::vector< Wall * >::iterator wy = this->wallY.begin (); wy != this->wallY.end (); ++wy )
        {
                ( *wy )->calculateOffset();
        }
}

void Room::addGridItem( GridItem * gridItem )
{
        if ( gridItem == nilPointer ) return;

        if ( ( gridItem->getCellX() < 0 || gridItem->getCellY() < 0 ) ||
                ( gridItem->getCellX() >= static_cast< int >( this->numberOfTiles.first ) ||
                        gridItem->getCellY() >= static_cast< int >( this->numberOfTiles.second ) ) )
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
        name << labelOfItem << " " << toStringWithOrdinalSuffix( uniqueNumberOfItem ) <<
                " @" << gridItem->getX() << "," << gridItem->getY() << "," << gridItem->getZ() ;

        gridItem->setUniqueName( name.str() );

        if ( gridItem->getZ() != Top )
        {
                // when item goes lower than top, look for collisions
                mediator->findCollisionWithItem( gridItem );
        }
        else
        {
                // whem item goes to top, modify its position on Z
                gridItem->setZ( mediator->findHighestZ( gridItem ) );
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

        mediator->addGridItemToList( gridItem );

        if ( shadingOpacity < 256 && gridItem->getImageOfShadow() != nilPointer )
        {
                mediator->reshadeWithGridItem( gridItem );
        }

        mediator->remaskWithGridItem( gridItem );

#if defined( DEBUG ) && DEBUG
        std::cout << gridItem->whichKindOfItem() << " \"" << gridItem->getUniqueName() << "\" is yet part of room \"" << getNameOfFileWithDataAboutRoom() << "\"" << std::endl ;
#endif
}

void Room::addFreeItem( FreeItem * freeItem )
{
        if ( freeItem == nilPointer ) return;

        if ( freeItem->getX() < 0 || freeItem->getY() < 1 || freeItem->getZ() < Top )
        {
                std::cerr << "coordinates for " << freeItem->whichKindOfItem() << " are out of limits" << std::endl ;
                dumpItemInsideThisRoom( freeItem );
                return;
        }

        if ( freeItem->getHeight() < 1 || freeItem->getWidthX() < 1 || freeItem->getWidthY() < 1 )
        {
                std::cerr << "can’t add " << freeItem->whichKindOfItem() << " which dimension is zero" << std::endl ;
                return;
        }

        if ( ( freeItem->getX() + static_cast< int >( freeItem->getWidthX() ) > static_cast< int >( this->numberOfTiles.first * this->tileSize ) )
                || ( freeItem->getY() - static_cast< int >( freeItem->getWidthY() ) + 1 < 0 )
                || ( freeItem->getY() > static_cast< int >( this->numberOfTiles.second * this->tileSize ) - 1 ) )
        {
                std::cerr << "coordinates for " << freeItem->whichKindOfItem() << " are out of room" << std::endl ;
                dumpItemInsideThisRoom( freeItem );
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

        freeItem->setUniqueName( labelOfItem + " " + toStringWithOrdinalSuffix( uniqueNumberOfItem ) );

        // for item which is placed at some height, look for collisions
        if ( freeItem->getZ() > Top )
        {
                mediator->findCollisionWithItem( freeItem );
        }
        // for item at the top of column
        else
        {
                freeItem->setZ( mediator->findHighestZ( freeItem ) );
        }

        // collision is found, so can’t add this item
        if ( ! mediator->isStackOfCollisionsEmpty () )
        {
                std::cerr << "there’s collision with " << freeItem->whichKindOfItem() << std::endl ;
                dumpItemInsideThisRoom( freeItem );
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

        // add free item to room
        mediator->addFreeItemToList( freeItem );

        if ( shadingOpacity < 256 && freeItem->getImageOfShadow() != nilPointer )
        {
                mediator->reshadeWithFreeItem( freeItem );
        }

        mediator->remaskWithFreeItem( freeItem );

#if defined( DEBUG ) && DEBUG
        std::cout << freeItem->whichKindOfItem() << " \"" << freeItem->getUniqueName() << "\" is yet in room \"" << getNameOfFileWithDataAboutRoom() << "\"" << std::endl ;
#endif
}

bool Room::addPlayerToRoom( PlayerItem* playerItem, bool playerEntersRoom )
{
        if ( playerItem == nilPointer ) return false;

        for ( std::list< PlayerItem * >::const_iterator pi = playersYetInRoom.begin (); pi != playersYetInRoom.end (); ++pi )
        {
                if ( playerItem == *pi )
                {
                        // player is in room already
                        return false;
                }
        }

        if ( playerEntersRoom )
        {
                for ( std::list< const PlayerItem * >::const_iterator epi = playersWhoEnteredRoom.begin (); epi != playersWhoEnteredRoom.end (); ++epi )
                {
                        const PlayerItem* enteredPlayer = *epi ;

                        if ( enteredPlayer->getLabel() == "headoverheels" && playerItem->getLabel() != "headoverheels" )
                        {
                                // case when joined character enters room, splits in this room, and one of characters exits & re~enters
                                std::cout << "character \"" << playerItem->getLabel() << "\" enters but joined \"headoverheels\" entered the same room before" << std::endl ;

                                // bin joined character
                                this->playersWhoEnteredRoom.remove( enteredPlayer );
                                /*  epi-- ;  */
                                delete enteredPlayer ;

                                // add copy of another character as entered
                                copyAnotherCharacterAsEntered( playerItem->getLabel() );

                                break;
                        }

                        if ( enteredPlayer->getLabel() == playerItem->getLabel() )
                        {
                                // case when character returns back to this room, maybe via different way
                                std::cout << "character \"" << playerItem->getLabel() << "\" already entered this room some time ago" << std::endl ;

                                // bin previous entry
                                this->playersWhoEnteredRoom.remove( enteredPlayer );
                                /*  epi-- ;  */
                                delete enteredPlayer ;
                                break;
                        }
                }
        }

        if ( playerItem->getX() < 0 || playerItem->getY() < 1 || playerItem->getZ() < Top )
        {
                std::cerr << "coordinates for " << playerItem->whichKindOfItem() << " are out of limits" << std::endl ;
                dumpItemInsideThisRoom( playerItem );
                return false;
        }

        if ( playerItem->getHeight() < 1 || playerItem->getWidthX() < 1 || playerItem->getWidthY() < 1 )
        {
                std::cerr << "can’t add " << playerItem->whichKindOfItem() << " which dimension is zero" << std::endl ;
                return false;
        }

        if ( ( playerItem->getX() + static_cast< int >( playerItem->getWidthX() ) > static_cast< int >( this->numberOfTiles.first * this->tileSize ) )
                || ( playerItem->getY() - static_cast< int >( playerItem->getWidthY() ) + 1 < 0 )
                || ( playerItem->getY() > static_cast< int >( this->numberOfTiles.second * this->tileSize ) - 1 ) )
        {
                std::cerr << "coordinates for " << playerItem->whichKindOfItem() << " are out of room" << std::endl ;
                dumpItemInsideThisRoom( playerItem );
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

        // for item which is placed at some height, look for collisions
        if ( playerItem->getZ() > Top )
        {
                mediator->findCollisionWithItem( playerItem );
                while ( ! mediator->isStackOfCollisionsEmpty () )
                {
                        playerItem->setZ( playerItem->getZ() + LayerHeight );
                        mediator->clearStackOfCollisions ();
                        mediator->findCollisionWithItem( playerItem );
                }
        }
        // for item at the top of column
        else
        {
                playerItem->setZ( mediator->findHighestZ( playerItem ) );
        }

        // collision is found, so can’t add this item
        if ( ! mediator->isStackOfCollisionsEmpty () )
        {
                std::cerr << "there’s collision with " << playerItem->whichKindOfItem() << std::endl ;
                dumpItemInsideThisRoom( playerItem );
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

        std::cout << "adding character \"" << playerItem->getLabel() << "\" to room \"" << getNameOfFileWithDataAboutRoom() << "\"" << std::endl ;

        mediator->addFreeItemToList( playerItem );

        if ( mediator->getActiveCharacter() == nilPointer )
        {
                mediator->setActiveCharacter( playerItem );
        }

        if ( shadingOpacity < 256 && playerItem->getImageOfShadow() != nilPointer )
        {
                mediator->reshadeWithFreeItem( playerItem );
        }

        mediator->remaskWithFreeItem( playerItem );

        // add player item to room
        this->playersYetInRoom.push_back( playerItem );

        if ( playerEntersRoom )
        {
                PlayerItem* copyOfPlayer = new PlayerItem( *playerItem );
                copyOfPlayer->assignBehavior( playerItem->getBehavior()->getNameOfBehavior(), playerItem->getDataOfItem() );
                this->playersWhoEnteredRoom.push_back( copyOfPlayer );

                std::cout << "copy of character \"" << copyOfPlayer->getLabel() << "\""
                                << " with behavior \"" << copyOfPlayer->getBehavior()->getNameOfBehavior() << "\""
                                << " is created to rebuild this room" << std::endl ;
        }

        return true;
}

void Room::dumpItemInsideThisRoom( const Item* item )
{
        std::cout << "   " << item->whichKindOfItem()
                        << " at " << item->getX() << " " << item->getY() << " " << item->getZ()
                        << " with dimensions " << item->getWidthX() << " x " << item->getWidthY() << " x " << item->getHeight()
                        << std::endl
                        << "   inside room \"" << this->nameOfFileWithDataAboutRoom << "\""
                        << " of " << this->numberOfTiles.first << " x " << this->numberOfTiles.second << " tiles"
                        << " each tile of " << this->tileSize << " pixels"
                        << std::endl ;
}

void Room::copyAnotherCharacterAsEntered( const std::string& name )
{
        for ( std::list< PlayerItem * >::const_iterator pi = playersYetInRoom.begin (); pi != playersYetInRoom.end (); ++pi )
        {
                if ( ( *pi )->getLabel() != name )
                {
                        bool alreadyThere = false;

                        for ( std::list< const PlayerItem * >::const_iterator epi = playersWhoEnteredRoom.begin (); epi != playersWhoEnteredRoom.end (); ++epi )
                        {
                                if ( ( *epi )->getLabel() == ( *pi )->getLabel() )
                                {
                                        alreadyThere = true;
                                        break;
                                }
                        }

                        if ( ! alreadyThere )
                        {
                                PlayerItem* copy = new PlayerItem( *( *pi ) );
                                copy->assignBehavior( ( *pi )->getBehavior()->getNameOfBehavior(), ( *pi )->getDataOfItem() );
                                copy->setWayOfEntry( "just wait" );

                                playersWhoEnteredRoom.push_back( copy );
                        }
                }
        }
}

void Room::removeFloor( FloorTile * floorTile )
{
        this->floor[ floorTile->getColumn () ] = nilPointer;
        delete floorTile;
}

void Room::removeGridItem( GridItem * gridItem )
{
        std::cout << "removing " << gridItem->whichKindOfItem() << " \"" << gridItem->getUniqueName() <<
                        "\" from room \"" << getNameOfFileWithDataAboutRoom() << "\"" << std::endl ;

        mediator->removeGridItemFromList( gridItem );

        if ( shadingOpacity < 256 && gridItem->getImageOfShadow() != nilPointer )
        {
                mediator->reshadeWithGridItem( gridItem );
        }

        mediator->remaskWithGridItem( gridItem );

        delete gridItem;
}

void Room::removeFreeItem( FreeItem * freeItem )
{
        std::cout << "removing " << freeItem->whichKindOfItem() << " \"" << freeItem->getUniqueName() <<
                        "\" from room \"" << getNameOfFileWithDataAboutRoom() << "\"" << std::endl ;

        mediator->removeFreeItemFromList( freeItem );

        if ( shadingOpacity < 256 && freeItem->getImageOfShadow() != nilPointer )
        {
                mediator->reshadeWithFreeItem( freeItem );
        }

        mediator->remaskWithFreeItem( freeItem );

        delete freeItem;
}

bool Room::removePlayerFromRoom( PlayerItem* playerItem, bool playerExitsRoom )
{
        if ( playerItem == nilPointer ) return false ;

        for ( std::list< PlayerItem * >::const_iterator pi = playersYetInRoom.begin (); pi != playersYetInRoom.end (); ++pi )
        {
                if ( playerItem == *pi )
                {
                        std::cout << "removing " << playerItem->whichKindOfItem() << " \"" << playerItem->getUniqueName() <<
                                        "\" from room \"" << getNameOfFileWithDataAboutRoom() << "\"" << std::endl ;

                        mediator->removeFreeItemFromList( playerItem );
                        nextNumbers[ "character " + playerItem->getOriginalLabel() ] -- ;

                        if ( shadingOpacity < 256 && playerItem->getImageOfShadow() != nilPointer )
                        {
                                mediator->reshadeWithFreeItem( playerItem );
                        }

                        mediator->remaskWithFreeItem( playerItem );

                        if ( playerItem->isActiveCharacter() )
                        {
                                // activate other character, or set it to nil when there’s no other character in room
                                mediator->setActiveCharacter( mediator->getWaitingCharacter() );
                        }

                        std::string nameOfPlayer = playerItem->getLabel();

                        this->playersYetInRoom.remove( playerItem );
                        /// pi --; // not needed, this iteration is last one due to "return" below

                        delete playerItem;

                        // when player leaves room, bin its copy on entry
                        if ( playerExitsRoom )
                        {
                                for ( std::list< const PlayerItem * >::const_iterator epi = playersWhoEnteredRoom.begin (); epi != playersWhoEnteredRoom.end (); ++epi )
                                {
                                        const PlayerItem* enteredPlayer = *epi ;

                                        if ( enteredPlayer->getLabel() == nameOfPlayer )
                                        {
                                                std::cout << "also removing copy of character \"" << nameOfPlayer << "\" created on entry to this room" << std::endl ;

                                                this->playersWhoEnteredRoom.remove( enteredPlayer );
                                                /// epi-- ; // not needed because of "break"
                                                delete enteredPlayer ;
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
        return ! this->playersYetInRoom.empty () ;
}

unsigned int Room::removeBars ()
{
        unsigned int howManyBars = 0;

        std::vector < std::list < GridItem * > > gridItems = mediator->getGridItems ();
        std::list< FreeItem * > freeItems = mediator->getFreeItems ();

        for ( unsigned int column = 0; column < gridItems.size(); column ++ )
        {
                std::list < GridItem * > columnOfItems = gridItems[ column ];

                for ( std::list< GridItem * >::const_iterator gi = columnOfItems.begin (); gi != columnOfItems.end (); ++ gi )
                {
                        if ( ( *gi )->getLabel() == "bars-ns" || ( *gi )->getLabel() == "bars-ew" )
                        {
                                this->removeGridItem( *gi );
                                howManyBars ++;
                        }
                }
        }

        for ( std::list< FreeItem * >::const_iterator fi = freeItems.begin (); fi != freeItems.end (); ++fi )
        {
                if ( ( *fi )->getLabel() == "bars-ns" || ( *fi )->getLabel() == "bars-ew" )
                {
                        this->removeFreeItem( *fi );
                        howManyBars ++;
                }
        }

        return howManyBars;
}

void Room::dontDisappearOnJump ()
{
        std::vector < std::list < GridItem * > > gridItems = mediator->getGridItems ();
        std::list< FreeItem * > freeItems = mediator->getFreeItems ();

        for ( unsigned int column = 0; column < gridItems.size(); column ++ )
        {
                std::list < GridItem * > columnOfItems = gridItems[ column ];

                for ( std::list< GridItem * >::const_iterator gi = columnOfItems.begin (); gi != columnOfItems.end (); ++ gi )
                {
                        GridItem* gridItem = *gi;
                        if ( gridItem->getBehavior() != nilPointer )
                        {
                                std::string behavior = gridItem->getBehavior()->getNameOfBehavior();
                                if ( behavior == "behavior of disappearance on jump into" ||
                                                behavior == "behavior of slow disappearance on jump into" )
                                {
                                        Behavior* thatBehavior = gridItem->getBehavior();
                                        gridItem->setBehavior( nilPointer );
                                        delete thatBehavior ;

                                        Picture* original = gridItem->getRawImage();
                                        Picture* copy = new Picture( *original );

                                        gridItem->changeImage( Color::multiplyWithColor(
                                                copy,
                                                ( behavior == "behavior of slow disappearance on jump into" ) ?
                                                        Color::magentaColor() :
                                                        Color::redColor() )
                                        );
                                }
                        }
                }
        }

        for ( std::list< FreeItem * >::const_iterator fi = freeItems.begin (); fi != freeItems.end (); ++fi )
        {
                FreeItem* freeItem = *fi;
                if ( freeItem->getBehavior() != nilPointer )
                {
                        std::string behavior = freeItem->getBehavior()->getNameOfBehavior();
                        if ( behavior == "behavior of disappearance on jump into" ||
                                        behavior == "behavior of slow disappearance on jump into" )
                        {
                                Behavior* thatBehavior = freeItem->getBehavior();
                                freeItem->setBehavior( nilPointer );
                                delete thatBehavior ;

                                Picture* original = freeItem->getRawImage();
                                Picture* copy = new Picture( *original );

                                freeItem->changeImage( Color::multiplyWithColor(
                                        copy,
                                        ( behavior == "behavior of slow disappearance on jump into" ) ?
                                                Color::magentaColor() :
                                                Color::redColor() )
                                );
                        }
                }
        }
}

void Room::draw( const allegro::Pict& where )
{
        const unsigned int maxTilesOfSingleRoom = 10 ;

        // draw room when it is active and image to draw it isn’t nil
        if ( active && where.isNotNil() )
        {
                // acquire video memory before drawing
                if ( where.isInVideoMemory() )
                {
                        allegro::acquirePict( where );
                }

                // clear image of room
                if ( GameManager::getInstance()->charactersFly() )
                        where.clearToColor( Color::darkBlueColor().toAllegroColor() );
                else
                        where.clearToColor( Color::blackColor().toAllegroColor() );

                // adjust position of camera
                if ( numberOfTiles.first > maxTilesOfSingleRoom || numberOfTiles.second > maxTilesOfSingleRoom )
                {
                        camera->centerOn( mediator->getActiveCharacter () );
                }

                // draw tiles of floor
                for ( unsigned int xCell = 0; xCell < this->numberOfTiles.first; xCell++ )
                {
                        for ( unsigned int yCell = 0; yCell < this->numberOfTiles.second; yCell++ )
                        {
                                unsigned int column = this->numberOfTiles.first * yCell + xCell;
                                FloorTile* tile = floor[ column ];

                                if ( tile != nilPointer )  // if there is tile of floor here
                                {
                                        // shade this tile when shadows are on
                                        if ( shadingOpacity < 256 && tile->getRawImage() != nilPointer )
                                        {
                                                mediator->lockGridItemMutex();
                                                mediator->lockFreeItemMutex();

                                                if ( tile->getWantShadow() )
                                                {
                                                        mediator->castShadowOnFloor( tile );
                                                }

                                                mediator->unlockGridItemMutex();
                                                mediator->unlockFreeItemMutex();
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

                mediator->lockGridItemMutex();
                mediator->lockFreeItemMutex();

                // draw grid items
                std::vector < std::list < GridItem * > > gridItems = mediator->getGridItems ();

                for ( unsigned int i = 0; i < numberOfTiles.first * numberOfTiles.second; i ++ )
                {
                        for ( std::list< GridItem * >::const_iterator gi = gridItems[ drawSequence[ i ] ].begin () ;
                                gi != gridItems[ drawSequence[ i ] ].end () ; ++ gi )
                        {
                                GridItem* gridItem = *gi ;

                                if ( shadingOpacity < 256 && gridItem->getRawImage() != nilPointer )
                                {
                                        // cast shadow
                                        if ( gridItem->getWantShadow() )
                                        {
                                                mediator->castShadowOnGridItem( gridItem );
                                        }
                                }

                                gridItem->draw( where );
                        }
                }

                // for free items there’re two steps before drawing
                std::list< FreeItem * > freeItems = mediator->getFreeItems ();

                // at first shade every free item with grid items and other free items
                for ( std::list< FreeItem * >::const_iterator fi = freeItems.begin (); fi != freeItems.end (); ++ fi )
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
                for ( std::list< FreeItem * >::const_iterator fi = freeItems.begin (); fi != freeItems.end (); ++ fi )
                {
                        if ( ( *fi )->getRawImage() != nilPointer )
                        {
                                ( *fi )->requestMask();
                                ( *fi )->draw( where );
                        }
                }

                mediator->unlockGridItemMutex();
                mediator->unlockFreeItemMutex();

                // release video memory after drawing
                if ( where.isInVideoMemory() )
                {
                        allegro::releasePict( where );
                }
        }
}

void Room::calculateBounds()
{
        bounds[ "north" ] = doors[ "north" ] || doors[ "northeast" ] || doors[ "northwest" ] || this->kindOfFloor == "none" ? tileSize : 0;
        bounds[ "east" ] = doors[ "east" ] || doors[ "eastnorth" ] || doors[ "eastsouth" ] || this->kindOfFloor == "none" ? tileSize : 0;
        bounds[ "south" ] = tileSize * numberOfTiles.first - ( doors[ "south" ] || doors[ "southeast" ] || doors[ "southwest" ]  ? tileSize : 0 );
        bounds[ "west" ] = tileSize * numberOfTiles.second - ( doors[ "west" ] || doors[ "westnorth" ] || doors[ "westsouth" ]  ? tileSize : 0 );

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
        bool hasNoFloor = ( this->kindOfFloor == "none" );

        // don’t count tiles taken by doors
        int xGrid = hasNorthDoor || hasNoFloor ? numberOfTiles.first - 1 : numberOfTiles.first;
        int yGrid = hasEastDoor || hasNoFloor ? numberOfTiles.second - 1 : numberOfTiles.second;

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
        for ( std::list< PlayerItem * >::const_iterator pi = playersYetInRoom.begin (); pi != playersYetInRoom.end (); ++pi )
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

bool Room::swapCharactersInRoom ( ItemDataManager * itemDataManager )
{
        return mediator->pickNextCharacter( itemDataManager );
}

bool Room::continueWithAlivePlayer ( )
{
        PlayerItem* previouslyAlivePlayer = mediator->getActiveCharacter();

        if ( previouslyAlivePlayer->getLives() == 0 )
        {
                // look for next player
                std::list< PlayerItem* >::iterator i =
                                std::find_if( this->playersYetInRoom.begin (), this->playersYetInRoom.end (),
                                                std::bind2nd( EqualLabelOfItem(), previouslyAlivePlayer->getLabel() ) );
                ++i ;
                mediator->setActiveCharacter( i != this->playersYetInRoom.end () ? ( *i ) : *this->playersYetInRoom.begin () );

                // is there other alive player in room
                if ( previouslyAlivePlayer != mediator->getActiveCharacter() )
                {
                        removePlayerFromRoom( previouslyAlivePlayer, true );

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
                const unsigned int maxTilesOfSingleRoom = 10 ;
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

void Room::addTripleRoomInitialPoint( const Way& way, int x, int y )
{
        this->listOfInitialPointsForTripleRoom.push_back( TripleRoomInitialPoint( way, x, y ) );
}

void Room::assignTripleRoomBounds( int minX, int maxX, int minY, int maxY )
{
        this->tripleRoomBoundX.first = minX;
        this->tripleRoomBoundX.second = maxX;
        this->tripleRoomBoundY.first = minY;
        this->tripleRoomBoundY.second = maxY;
}

TripleRoomInitialPoint* Room::findInitialPointOfEntryToTripleRoom( const Way& way )
{
        std::list< TripleRoomInitialPoint >::iterator i = std::find_if(
                                                                listOfInitialPointsForTripleRoom.begin (),
                                                                listOfInitialPointsForTripleRoom.end (),
                                                                std::bind2nd( EqualTripleRoomInitialPoint(), way ) );

        return ( i != listOfInitialPointsForTripleRoom.end () ? ( &( *i ) ) : nilPointer );
}

TripleRoomInitialPoint::TripleRoomInitialPoint( const Way& way, int x, int y )
        : wayOfEntry( way )
        , x( x )
        , y( y )
{
}

bool EqualTripleRoomInitialPoint::operator()( TripleRoomInitialPoint point, const Way& wayOfEntry ) const
{
        return ( point.getWayOfEntry().getIntegerOfWay() == wayOfEntry.getIntegerOfWay() );
}

}
