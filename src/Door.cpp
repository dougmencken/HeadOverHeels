
#include "Door.hpp"
#include "ItemDataManager.hpp"
#include "FreeItem.hpp"
#include "Mediator.hpp"


namespace isomot
{

Door::Door( ItemDataManager * itemData, const std::string& label, int cx, int cy, int z, const Way& way )
        : itemDataManager( itemData )
        , labelOfDoor( label )
        , cellX( cx )
        , cellY( cy )
        , z( z )
        , positionOfDoor( way )
        , leftJamb( nilPointer )
        , rightJamb( nilPointer )
        , lintel( nilPointer )
{
        /* std::cout << "creation of door \"" << labelOfDoor << "\"" << std::endl ; */
}

Door::~Door()
{

}

FreeItem* Door::getLeftJamb()
{
        if ( leftJamb == nilPointer )
        {
                int x( 0 ), y( 0 );
                int tileSize = mediator->getRoom()->getSizeOfOneTile();

                ItemData* leftJambData = itemDataManager->findDataByLabel( labelOfDoor + "~leftjamb" );

                if ( leftJambData != nilPointer )
                {
                        switch ( getWhereIsDoor().getIntegerOfWay() )
                        {
                                case North:
                                case Northeast:
                                case Northwest:
                                        x = cellX * tileSize + leftJambData->getWidthX() - 2;
                                        y = ( cellY + 2 ) * tileSize - 2;
                                        leftLimit = y + leftJambData->getWidthY();
                                        break;

                                case South:
                                case Southeast:
                                case Southwest:
                                        x = cellX * tileSize;
                                        y = ( cellY + 2 ) * tileSize - 2;
                                        leftLimit = y + leftJambData->getWidthY();
                                        break;

                                case East:
                                case Eastnorth:
                                case Eastsouth:
                                        x = cellX * tileSize;
                                        y = ( cellY + 1 ) * tileSize - 1;
                                        leftLimit = x + leftJambData->getWidthX();
                                        break;

                                case West:
                                case Westnorth:
                                case Westsouth:
                                        x = cellX * tileSize;
                                        y = ( cellY + 1 ) * tileSize - leftJambData->getWidthY() + 1;
                                        leftLimit = x + leftJambData->getWidthX();
                                        break;

                                default:
                                        ;
                        }

                        leftJamb = new FreeItem( leftJambData, x, y, Top, Nowhere );
                        leftJamb->setUniqueName( leftJamb->getLabel() + " " + makeRandomString( 8 ) );
                }
        }

        return leftJamb;
}

FreeItem* Door::getRightJamb()
{
        if ( rightJamb == nilPointer )
        {
                int x( 0 ), y( 0 );
                int tileSize = mediator->getRoom()->getSizeOfOneTile();

                ItemData* rightJambData = itemDataManager->findDataByLabel( labelOfDoor + "~rightjamb" );

                if ( rightJambData != nilPointer )
                {
                        switch ( getWhereIsDoor().getIntegerOfWay() )
                        {
                                case North:
                                case Northeast:
                                case Northwest:
                                        x = cellX * tileSize + rightJambData->getWidthX() - 2;
                                        y = cellY * tileSize + rightJambData->getWidthY() - 1;
                                        rightLimit = y;
                                        break;

                                case South:
                                case Southeast:
                                case Southwest:
                                        x = cellX * tileSize;
                                        y = cellY * tileSize + rightJambData->getWidthY() - 1;
                                        rightLimit = y;
                                        break;

                                case East:
                                case Eastnorth:
                                case Eastsouth:
                                        x = ( cellX + 2 ) * tileSize - rightJambData->getWidthX() - 2;
                                        y = ( cellY + 1 ) * tileSize - 1;
                                        rightLimit = x;
                                        break;

                                case West:
                                case Westnorth:
                                case Westsouth:
                                        x = ( cellX + 2 ) * tileSize - rightJambData->getWidthX() - 2;
                                        y = ( cellY + 1 ) * tileSize - rightJambData->getWidthY() + 1;
                                        rightLimit = x;
                                        break;

                                default:
                                        ;
                        }

                        rightJamb = new FreeItem( rightJambData, x, y, Top, Nowhere );
                        rightJamb->setUniqueName( rightJamb->getLabel() + " " + makeRandomString( 8 ) );
                }
        }

        return rightJamb;
}

FreeItem* Door::getLintel()
{
        if ( lintel == nilPointer )
        {
                int x( 0 ), y( 0 );
                int tileSize = mediator->getRoom()->getSizeOfOneTile();

                ItemData* lintelData = itemDataManager->findDataByLabel( labelOfDoor + "~lintel" );

                if ( lintelData != nilPointer )
                {
                        switch ( getWhereIsDoor().getIntegerOfWay() )
                        {
                                case North:
                                case Northeast:
                                case Northwest:
                                        x = cellX * tileSize + lintelData->getWidthX() - 2;
                                        y = ( cellY + 2 ) * tileSize - 1;
                                        break;

                                case South:
                                case Southeast:
                                case Southwest:
                                        x = cellX * tileSize;
                                        y = ( cellY + 2 ) * tileSize - 1;
                                        break;

                                case East:
                                case Eastnorth:
                                case Eastsouth:
                                        x = cellX * tileSize;
                                        y = ( cellY + 1 ) * tileSize - 1;
                                        break;

                                case West:
                                case Westnorth:
                                case Westsouth:
                                        x = cellX * tileSize;
                                        y = ( cellY + 1 ) * tileSize - lintelData->getWidthY() + 1;
                                        break;

                                default:
                                        ;
                        }

                        lintel = new FreeItem( lintelData, x, y, Top, Nowhere );
                        lintel->setUniqueName( lintel->getLabel() + " " + makeRandomString( 8 ) );
                }
        }

        return lintel;
}

bool Door::isUnderDoor( int x, int y, int z )
{
        bool result = false;

        if ( z < 0 ) z = 0;

        switch ( getWhereIsDoor().getIntegerOfWay() )
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
