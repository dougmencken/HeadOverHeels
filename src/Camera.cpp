
#include "Camera.hpp"
#include "Room.hpp"
#include "PlayerItem.hpp"
#include "Ism.hpp"

# include "RoomBuilder.hpp"


namespace isomot
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

void Camera::turnOn( PlayerItem* player, const Way& wayOfEntry )
{
        const unsigned int maxTilesOfSingleRoom = 10 ;

        if ( delta.first == 0 && delta.second == 0 && player != 0 &&
                        ( room->getTilesX() > maxTilesOfSingleRoom || room->getTilesY() > maxTilesOfSingleRoom ) )
        {
                // is it double room along Y
                if ( room->getTilesX() <= maxTilesOfSingleRoom && room->getTilesY() > maxTilesOfSingleRoom )
                {
                        int playerY = player->getY ();
                        unsigned int absoluteValueOfY = ( playerY >= 0 ? playerY : -playerY );

                        if ( absoluteValueOfY < room->getTilesY() * room->getSizeOfOneTile() / 2 )
                        {
                                delta.first = room->getPicture()->w - ScreenWidth;
                                delta.second = 0;
                        }
                        else
                        {
                                delta.first = 0;
                                delta.second = room->getPicture()->h - ScreenHeight;
                        }
                }
                // is it double room along X
                else if ( room->getTilesX() > maxTilesOfSingleRoom && room->getTilesY() <= maxTilesOfSingleRoom )
                {
                        int playerX = player->getX ();
                        unsigned int absoluteValueOfX = ( playerX >= 0 ? playerX : -playerX );

                        if ( absoluteValueOfX < room->getTilesX() * room->getSizeOfOneTile() / 2 )
                        {
                                delta.first = 0;
                                delta.second = 0;
                        }
                        else
                        {
                                delta.first = room->getPicture()->w - ScreenWidth;
                                delta.second = room->getPicture()->h - ScreenHeight;
                        }
                }
                // it’s triple room then
                else
                {
                        TripleRoomInitialPoint* initialPoint = room->findInitialPointOfEntryToTripleRoom( wayOfEntry );
                        if ( initialPoint != 0 )
                        {
                                delta.first = initialPoint->getX();
                                delta.second = initialPoint->getY();
                        }
                        else
                        { // for example it’s resume of some game which was saved in triple room
                                int midX = RoomBuilder::getXCenterOfRoom( player->getDataOfItem(), room );
                                int midY = RoomBuilder::getYCenterOfRoom( player->getDataOfItem(), room );
                                delta.first = midX + ( 12 * room->getSizeOfOneTile() );  // yeah, 12 is just some magic number
                                delta.second = midY + ( 6 * room->getSizeOfOneTile() );  // as well as 6
                        }
                }

                reference.first = player->getX ();
                reference.second = player->getY ();
        }
}

bool Camera::centerOn( PlayerItem* player )
{
        const unsigned int maxTilesOfSingleRoom = 10 ;

        // whether camera has moved
        bool changed = false;

        // Sala triple
        if ( ( room->getTilesX() > maxTilesOfSingleRoom && room->getTilesY() > maxTilesOfSingleRoom ) && player != 0 )
        {
                // Debe haber cambio en el eje X para realizar los cálculos
                if ( reference.first - player->getX() != 0 && room->getTilesX() > maxTilesOfSingleRoom )
                {
                        // Diferencia en X respecto al último movimiento
                        int offsetX = player->getX() - reference.first;

                        // Límites de la sala para efectuar el desplazamiento de la cámara
                        int minX = room->getTripleRoomBoundX().first; //96;
                        int maxX = room->getTripleRoomBoundX().second; //144;

                        // Hay desplazamiento al norte
                        if ( offsetX < 0 && player->getX() > minX && player->getX() <= maxX )
                        {
                                delta.first += ( offsetX << 1 );
                                delta.second += offsetX;
                                changed = true;
                        }
                        // Hay desplazamiento al sur
                        else if ( offsetX > 0 && player->getX() > minX && player->getX() <= maxX )
                        {
                                delta.first += ( offsetX << 1 );
                                delta.second += offsetX;
                                changed = true;
                        }
                }

                // Debe haber cambio en el eje Y para realizar los cálculos
                if ( reference.second - player->getY() != 0 && room->getTilesY() > maxTilesOfSingleRoom )
                {
                        // Diferencia en Y respecto al último movimiento
                        int offsetY = player->getY() - reference.second;

                        // Límites de la sala para efectuar el desplazamiento de la cámara
                        int minY = room->getTripleRoomBoundY().first; //80;
                        int maxY = room->getTripleRoomBoundY().second; //128;

                        // Hay desplazamiento al este
                        if ( offsetY < 0 && player->getY() > minY && player->getY() <= maxY )
                        {
                                delta.first -= ( offsetY << 1 );
                                delta.second += offsetY;
                                changed = true;
                        }
                        // Hay desplazamiento al oeste
                        else if ( offsetY > 0 && player->getY() > minY && player->getY() <= maxY )
                        {
                                delta.first -= ( offsetY << 1 );
                                delta.second += offsetY;
                                changed = true;
                        }
                }
        }
        // Sala doble
        else if ( ( room->getTilesX() > maxTilesOfSingleRoom || room->getTilesY() > maxTilesOfSingleRoom ) && player != 0 )
        {
                // Debe haber cambio en el eje X para realizar los cálculos
                if ( reference.first - player->getX() != 0 && room->getTilesX() > maxTilesOfSingleRoom )
                {
                        // Diferencia en X respecto al último movimiento
                        int offsetX = player->getX() - reference.first;

                        // Límites de la sala para efectuar el desplazamiento de la cámara
                        int minX = ( room->getTilesX() * room->getSizeOfOneTile() ) / 4;
                        int maxX = ( room->getTilesX() * room->getSizeOfOneTile() * 3 ) / 4;

                        // Hay desplazamiento al norte
                        if ( offsetX < 0 && player->getX() <= maxX - 1 && player->getX() >= minX - 1 )
                        {
                                // Si no se han alcanzado los límites de la sala el desplazamiento al norte es posible
                                if ( delta.first >= 0 && delta.second >= 0 )
                                {
                                        delta.first += ( offsetX << 1 );
                                        delta.second += offsetX;
                                        changed = true;
                                }
                        }
                        // Hay desplazamiento al sur
                        else if ( offsetX > 0 && player->getX() <= maxX && player->getX() >= minX )
                        {
                                // Si no se han alcanzado los límites de la sala el desplazamiento al sur es posible
                                if ( delta.first <= ( room->getPicture()->w - ScreenWidth ) && delta.second <= ( room->getPicture()->h - ScreenHeight ) )
                                {
                                        delta.first += ( offsetX << 1 );
                                        delta.second += offsetX;
                                        changed = true;
                                }
                        }
                }

                // Debe haber cambio en el eje Y para realizar los cálculos
                if ( reference.second - player->getY() != 0 && room->getTilesY() > maxTilesOfSingleRoom )
                {
                        // Diferencia en Y respecto al último movimiento
                        int offsetY = player->getY() - reference.second;

                        // Límites de la sala para efectuar el desplazamiento de la cámara
                        int minY = ( room->getTilesY() * room->getSizeOfOneTile() ) / 4;
                        int maxY = ( room->getTilesY() * room->getSizeOfOneTile() * 3 ) / 4;

                        // Hay desplazamiento al este
                        if ( offsetY < 0 && player->getY() <= maxY - 1 && player->getY() >= minY - 1 )
                        {
                                // Si no se han alcanzado los límites de la sala el desplazamiento al este es posible
                                if ( delta.first <= room->getPicture()->w - ScreenWidth && delta.second >= 0 )
                                {
                                        delta.first -= ( offsetY << 1 );
                                        delta.second += offsetY;
                                        changed = true;
                                }
                        }
                        // Hay desplazamiento al oeste
                        else if ( offsetY > 0 && player->getY() <= maxY && player->getY() >= minY )
                        {
                                // Si no se han alcanzado los límites de la sala el desplazamiento al oeste es posible
                                if ( delta.first >= 0 && delta.second <= room->getPicture()->h - ScreenHeight )
                                {
                                        delta.first -= ( offsetY << 1 );
                                        delta.second += offsetY;
                                        changed = true;
                                }
                        }
                }
        }

        // Actualización del punto de referencia
        reference.first = player->getX();
        reference.second = player->getY();

        return changed;
}

}
