
#include "Door.hpp"
#include "ItemDataManager.hpp"
#include "FreeItem.hpp"
#include "Mediator.hpp"


namespace isomot
{

Door::Door( ItemDataManager * itemData, const std::string& label, int cx, int cy, int z, const Direction& direction )
        : itemDataManager( itemData )
        , labelOfDoor( label )
        , cx( cx )
        , cy( cy )
        , z( z )
        , direction( direction )
        , leftJamb( 0 )
        , rightJamb( 0 )
        , lintel( 0 )
{
        /* std::cout << "creation of door \"" << labelOfDoor << "\"" << std::endl ; */
}

Door::~Door()
{

}

FreeItem* Door::getLeftJamb()
{
        if ( leftJamb == 0 )
        {
                int x( 0 ), y( 0 );
                int tileSize = mediator->getSizeOfOneTile();

                ItemData* leftJambData = itemDataManager->findItemByLabel( labelOfDoor + "~leftjamb" );

                if ( leftJambData != 0 )
                {
                        switch ( direction )
                        {
                                case North:
                                case Northeast:
                                case Northwest:
                                        x = cx * tileSize + leftJambData->widthX - 2;
                                        y = ( cy + 2 ) * tileSize - 2;
                                        leftLimit = y + leftJambData->widthY;
                                        break;

                                case South:
                                case Southeast:
                                case Southwest:
                                        x = cx * tileSize;
                                        y = ( cy + 2 ) * tileSize - 2;
                                        leftLimit = y + leftJambData->widthY;
                                        break;

                                case East:
                                case Eastnorth:
                                case Eastsouth:
                                        x = cx * tileSize;
                                        y = ( cy + 1 ) * tileSize - 1;
                                        leftLimit = x + leftJambData->widthX;
                                        break;

                                case West:
                                case Westnorth:
                                case Westsouth:
                                        x = cx * tileSize;
                                        y = ( cy + 1 ) * tileSize - leftJambData->widthY + 1;
                                        leftLimit = x + leftJambData->widthX;
                                        break;

                                default:
                                        ;
                        }

                        leftJamb = new FreeItem( leftJambData, x, y, Top, NoDirection );
                }
        }

        return leftJamb;
}

FreeItem* Door::getRightJamb()
{
        if ( rightJamb == 0 )
        {
                int x( 0 ), y( 0 );
                int tileSize = mediator->getSizeOfOneTile();

                ItemData* rightJambData = itemDataManager->findItemByLabel( labelOfDoor + "~rightjamb" );

                if ( rightJambData != 0 )
                {
                        switch ( direction )
                        {
                                case North:
                                case Northeast:
                                case Northwest:
                                        x = cx * tileSize + rightJambData->widthX - 2;
                                        y = cy * tileSize + rightJambData->widthY - 1;
                                        rightLimit = y;
                                        break;

                                case South:
                                case Southeast:
                                case Southwest:
                                        x = cx * tileSize;
                                        y = cy * tileSize + rightJambData->widthY - 1;
                                        rightLimit = y;
                                        break;

                                case East:
                                case Eastnorth:
                                case Eastsouth:
                                        x = ( cx + 2 ) * tileSize - rightJambData->widthX - 2;
                                        y = ( cy + 1 ) * tileSize - 1;
                                        rightLimit = x;
                                        break;

                                case West:
                                case Westnorth:
                                case Westsouth:
                                        x = ( cx + 2 ) * tileSize - rightJambData->widthX - 2;
                                        y = ( cy + 1 ) * tileSize - rightJambData->widthY + 1;
                                        rightLimit = x;
                                        break;

                                default:
                                        ;
                        }

                        rightJamb = new FreeItem( rightJambData, x, y, Top, NoDirection );
                }
        }

        return rightJamb;
}

FreeItem* Door::getLintel()
{
        if ( lintel == 0 )
        {
                int x( 0 ), y( 0 );
                int tileSize = mediator->getSizeOfOneTile();

                ItemData* lintelData = itemDataManager->findItemByLabel( labelOfDoor + "~lintel" );

                if ( lintelData != 0 )
                {
                        switch ( direction )
                        {
                                case North:
                                case Northeast:
                                case Northwest:
                                        x = cx * tileSize + lintelData->widthX - 2;
                                        y = ( cy + 2 ) * tileSize - 1;
                                        break;

                                case South:
                                case Southeast:
                                case Southwest:
                                        x = cx * tileSize;
                                        y = ( cy + 2 ) * tileSize - 1;
                                        break;

                                case East:
                                case Eastnorth:
                                case Eastsouth:
                                        x = cx * tileSize;
                                        y = ( cy + 1 ) * tileSize - 1;
                                        break;

                                case West:
                                case Westnorth:
                                case Westsouth:
                                        x = cx * tileSize;
                                        y = ( cy + 1 ) * tileSize - lintelData->widthY + 1;
                                        break;

                                default:
                                        ;
                        }

                        lintel = new FreeItem( lintelData, x, y, Top, NoDirection );
                }
        }

        return lintel;
}

bool Door::isUnderDoor( int x, int y, int z )
{
        bool result = false;

        if ( z < 0 ) z = 0;

        switch ( direction )
        {
                case North:
                case Northeast:
                case Northwest:
                case South:
                case Southeast:
                case Southwest:
                        result = ( y >= rightLimit && y <= leftLimit && z >= leftJamb->getZ () && z <= lintel->getZ () );
                        break;

                case East:
                case Eastnorth:
                case Eastsouth:
                case West:
                case Westnorth:
                case Westsouth:
                        result = ( x <= rightLimit && x >= leftLimit && z >= leftJamb->getZ () && z <= lintel->getZ () );
                        break;

                default:
                        ;
        }

        return result;
}

}
