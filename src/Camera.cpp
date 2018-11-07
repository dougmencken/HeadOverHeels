
#include "Camera.hpp"
#include "Room.hpp"
#include "PlayerItem.hpp"
#include "Ism.hpp"

# include "RoomBuilder.hpp"


namespace iso
{

Camera::Camera( Room * room )
        : room( room )
        , reference( std::pair< int, int >( 0, 0 ) )
        , delta( std::pair< int, int >( 0, 0 ) )
{
}

Camera::~Camera()
{
}

void Camera::turnOn( const PlayerItem& player, const Way& wayOfEntry )
{
        if ( delta.first == 0 && delta.second == 0 )
        {
                if ( room->whichRoom() == "double along Y" )
                {
                        int playerY = player.getY ();
                        unsigned int absoluteValueOfY = ( playerY >= 0 ? playerY : -playerY );

                        if ( absoluteValueOfY < room->getTilesY() * room->getSizeOfOneTile() / 2 )
                        {
                                delta.first = static_cast< int >( room->getWhereToDraw()->getWidth() ) - static_cast< int >( ScreenWidth() );
                                delta.second = 0;
                        }
                        else
                        {
                                delta.first = 0;
                                delta.second = static_cast< int >( room->getWhereToDraw()->getHeight() ) - static_cast< int >( ScreenHeight() );
                        }
                }
                else if ( room->whichRoom() == "double along X" )
                {
                        int playerX = player.getX ();
                        unsigned int absoluteValueOfX = ( playerX >= 0 ? playerX : -playerX );

                        if ( absoluteValueOfX < room->getTilesX() * room->getSizeOfOneTile() / 2 )
                        {
                                delta.first = 0;
                                delta.second = 0;
                        }
                        else
                        {
                                delta.first = static_cast< int >( room->getWhereToDraw()->getWidth() ) - static_cast< int >( ScreenWidth() );
                                delta.second = static_cast< int >( room->getWhereToDraw()->getHeight() ) - static_cast< int >( ScreenHeight() );
                        }
                }
                else if ( room->whichRoom() == "triple" )
                {
                        TripleRoomInitialPoint initialPoint = room->findInitialPointOfEntryToTripleRoom( wayOfEntry );
                        if ( initialPoint.getWayOfEntry().toString() != "nowhere" )
                        {
                                delta.first = initialPoint.getX();
                                delta.second = initialPoint.getY();
                        }
                        else
                        { // as example it’s resume of game which was saved in triple room
                                int midX = room->getXCenterForItem( player );
                                int midY = room->getYCenterForItem( player );
                                delta.first = midX + ( 12 * room->getSizeOfOneTile() );  // yeah, 12 is just some magic number
                                delta.second = midY + ( 6 * room->getSizeOfOneTile() );  // as well as 6
                        }
                }

                reference.first = player.getX ();
                reference.second = player.getY ();
        }
}

bool Camera::centerOn( const PlayerItem& player )
{
        // whether camera has moved
        bool changed = false;

        if ( room->whichRoom() == "triple" )
        {
                // is there change on X
                if ( reference.first - player.getX() != 0 && room->getTilesX() > Room::maxTilesOfSingleRoom )
                {
                        int offsetX = player.getX() - reference.first;

                        // limits of room to carry out camera’s movement
                        int minX = room->getTripleRoomBoundX().first; //96;
                        int maxX = room->getTripleRoomBoundX().second; //144;

                        if ( ( offsetX < 0 && player.getX() > minX && player.getX() <= maxX ) ||
                                ( offsetX > 0 && player.getX() > minX && player.getX() <= maxX ) )
                        {
                                delta.first += ( offsetX << 1 );
                                delta.second += offsetX;
                                changed = true;
                        }
                }

                // is there change on Y
                if ( reference.second - player.getY() != 0 && room->getTilesY() > Room::maxTilesOfSingleRoom )
                {
                        int offsetY = player.getY() - reference.second;

                        // limits of room to carry out camera’s movement
                        int minY = room->getTripleRoomBoundY().first; //80;
                        int maxY = room->getTripleRoomBoundY().second; //128;

                        if ( ( offsetY < 0 && player.getY() > minY && player.getY() <= maxY ) ||
                                ( offsetY > 0 && player.getY() > minY && player.getY() <= maxY ) )
                        {
                                delta.first -= ( offsetY << 1 );
                                delta.second += offsetY;
                                changed = true;
                        }
                }
        }
        else if ( room->whichRoom() == "double along X" || room->whichRoom() == "double along Y" )
        {
                // is there change on X
                if ( reference.first - player.getX() != 0 && room->getTilesX() > Room::maxTilesOfSingleRoom )
                {
                        int offsetX = player.getX() - reference.first;

                        // limits of room to carry out camera’s movement
                        int minX = ( room->getTilesX() * room->getSizeOfOneTile() ) / 4;
                        int maxX = ( room->getTilesX() * room->getSizeOfOneTile() * 3 ) / 4;

                        if ( offsetX < 0 && player.getX() <= maxX - 1 && player.getX() >= minX - 1 )
                        {
                                if ( delta.first >= 0 && delta.second >= 0 )
                                {
                                        delta.first += ( offsetX << 1 );
                                        delta.second += offsetX;
                                        changed = true;
                                }
                        }
                        else if ( offsetX > 0 && player.getX() <= maxX && player.getX() >= minX )
                        {
                                if ( delta.first <= ( static_cast< int >( room->getWhereToDraw()->getWidth() ) - static_cast< int >( ScreenWidth() ) ) &&
                                        delta.second <= ( static_cast< int >( room->getWhereToDraw()->getHeight() ) - static_cast< int >( ScreenHeight() ) ) )
                                {
                                        delta.first += ( offsetX << 1 );
                                        delta.second += offsetX;
                                        changed = true;
                                }
                        }
                }

                // is there change on Y
                if ( reference.second - player.getY() != 0 && room->getTilesY() > Room::maxTilesOfSingleRoom )
                {
                        int offsetY = player.getY() - reference.second;

                        // limits of room to carry out camera’s movement
                        int minY = ( room->getTilesY() * room->getSizeOfOneTile() ) / 4;
                        int maxY = ( room->getTilesY() * room->getSizeOfOneTile() * 3 ) / 4;

                        if ( offsetY < 0 && player.getY() <= maxY - 1 && player.getY() >= minY - 1 )
                        {
                                if ( delta.first <= static_cast< int >( room->getWhereToDraw()->getWidth() ) - static_cast< int >( ScreenWidth() ) && delta.second >= 0 )
                                {
                                        delta.first -= ( offsetY << 1 );
                                        delta.second += offsetY;
                                        changed = true;
                                }
                        }
                        else if ( offsetY > 0 && player.getY() <= maxY && player.getY() >= minY )
                        {
                                if ( delta.first >= 0 && delta.second <= static_cast< int >( room->getWhereToDraw()->getHeight() ) - static_cast< int >( ScreenHeight() ) )
                                {
                                        delta.first -= ( offsetY << 1 );
                                        delta.second += offsetY;
                                        changed = true;
                                }
                        }
                }
        }

        // refresh point of reference
        reference.first = player.getX();
        reference.second = player.getY();

        return changed;
}

}
