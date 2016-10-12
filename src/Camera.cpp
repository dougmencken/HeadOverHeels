#include "Camera.hpp"
#include "Room.hpp"
#include "PlayerItem.hpp"
#include "Ism.hpp"

# include "RoomBuilder.hpp"

namespace isomot
{

Camera::Camera(Room* room)
: room(room),
  reference(std::pair<int, int>(0, 0)),
  delta(std::pair<int, int>(0, 0))
{
}

Camera::~Camera()
{
}

void Camera::turnOn( PlayerItem* player, const Direction& entry )
{
  if( delta.first == 0 && delta.second == 0 && player != 0 && ( room->getTilesX() > 10 || room->getTilesY() > 10 ) )
  {
    // Sala doble a lo largo del eje Y
    if( room->getTilesX() <= 10 && room->getTilesY() > 10 )
    {
      if( abs( player->getY() ) < room->getTilesY() * room->getTileSize() / 2 )
      {
        delta.first = room->getDestination()->w - ScreenWidth;
        delta.second = 0;
      }
      else
      {
        delta.first = 0;
        delta.second = room->getDestination()->h - ScreenHeight;
      }
    }
    // Sala doble a lo largo del eje X
    else if( room->getTilesX() > 10 && room->getTilesY() <= 10 )
    {
      if( abs( player->getX() ) < room->getTilesX() * room->getTileSize() / 2 )
      {
        delta.first = 0;
        delta.second = 0;
      }
      else
      {
        delta.first = room->getDestination ()->w - ScreenWidth;
        delta.second = room->getDestination ()->h - ScreenHeight;
      }
    }
    // Sala triple
    else
    {
      // Posición inicial
      TripleRoomStartPoint* startPoint = room->findTripleRoomStartPoint( entry );
      if( startPoint != 0 )
      {
          delta.first = startPoint->getX();
          delta.second = startPoint->getY();
      }
      else
      { // exempli gratia on restore of a game which was saved in triple room
          int midX = RoomBuilder::getXCenterOfRoom( player->getItemData(), room );
          int midY = RoomBuilder::getYCenterOfRoom( player->getItemData(), room );
          delta.first = midX + ( 12 * room->getTileSize() );
          delta.second = midY + ( 6 * room->getTileSize() );
      }
    }

    reference.first = player->getX ();
    reference.second = player->getY ();
  }
}

bool Camera::center( PlayerItem* player )
{
  // Indica si la cámara se ha desplazado
  bool changed = false;

  // Sala triple
  if((room->getTilesX() > 10 && room->getTilesY() > 10) && player != 0)
  {
    // Debe haber cambio en el eje X para realizar los cálculos
    if(reference.first - player->getX() != 0 && room->getTilesX() > 10)
    {
      // Diferencia en X respecto al último movimiento
      int offsetX = player->getX() - reference.first;

      // Límites de la sala para efectuar el desplazamiento de la cámara
      int minX = room->getTripleRoomBoundX().first;//96;
      int maxX = room->getTripleRoomBoundX().second;//144;

      // Hay desplazamiento al norte
      if(offsetX < 0 && player->getX() > minX && player->getX() <= maxX)
      {
        delta.first += (offsetX << 1);
        delta.second += offsetX;
        changed = true;
      }
      // Hay desplazamiento al sur
      else if(offsetX > 0 && player->getX() > minX && player->getX() <= maxX)
      {
        delta.first += (offsetX << 1);
        delta.second += offsetX;
        changed = true;
      }
    }

    // Debe haber cambio en el eje Y para realizar los cálculos
    if(reference.second - player->getY() != 0 && room->getTilesY() > 10)
    {
      // Diferencia en Y respecto al último movimiento
      int offsetY = player->getY() - reference.second;

      // Límites de la sala para efectuar el desplazamiento de la cámara
      int minY = room->getTripleRoomBoundY().first;//80;
      int maxY = room->getTripleRoomBoundY().second;//128;

      // Hay desplazamiento al este
      if(offsetY < 0 && player->getY() > minY && player->getY() <= maxY)
      {
        delta.first -= (offsetY << 1);
        delta.second += offsetY;
        changed = true;
      }
      // Hay desplazamiento al oeste
      else if(offsetY > 0 && player->getY() > minY && player->getY() <= maxY)
      {
        delta.first -= (offsetY << 1);
        delta.second += offsetY;
        changed = true;
      }
    }
  }
  // Sala doble
  else if((room->getTilesX() > 10 || room->getTilesY() > 10) && player != 0)
  {
    // Debe haber cambio en el eje X para realizar los cálculos
    if(reference.first - player->getX() != 0 && room->getTilesX() > 10)
    {
      // Diferencia en X respecto al último movimiento
      int offsetX = player->getX() - reference.first;

      // Límites de la sala para efectuar el desplazamiento de la cámara
      int minX = (room->getTilesX() * room->getTileSize()) / 4;
      int maxX = (room->getTilesX() * room->getTileSize() * 3) / 4;

      // Hay desplazamiento al norte
      if(offsetX < 0 && player->getX() <= maxX - 1 && player->getX() >= minX - 1)
      {
        // Si no se han alcanzado los límites de la sala el desplazamiento al norte es posible
        if(delta.first >= 0 && delta.second >= 0)
        {
          delta.first += (offsetX << 1);
          delta.second += offsetX;
          changed = true;
        }
      }
      // Hay desplazamiento al sur
      else if(offsetX > 0 && player->getX() <= maxX && player->getX() >= minX)
      {
        // Si no se han alcanzado los límites de la sala el desplazamiento al sur es posible
        if(delta.first <= (room->getDestination()->w - ScreenWidth) && delta.second <= (room->getDestination()->h - ScreenHeight))
        {
          delta.first += (offsetX << 1);
          delta.second += offsetX;
          changed = true;
        }
      }
    }

    // Debe haber cambio en el eje Y para realizar los cálculos
    if(reference.second - player->getY() != 0 && room->getTilesY() > 10)
    {
      // Diferencia en Y respecto al último movimiento
      int offsetY = player->getY() - reference.second;

      // Límites de la sala para efectuar el desplazamiento de la cámara
      int minY = (room->getTilesY() * room->getTileSize()) / 4;
      int maxY = (room->getTilesY() * room->getTileSize() * 3) / 4;

      // Hay desplazamiento al este
      if(offsetY < 0 && player->getY() <= maxY - 1 && player->getY() >= minY - 1)
      {
        // Si no se han alcanzado los límites de la sala el desplazamiento al este es posible
        if(delta.first <= room->getDestination()->w - ScreenWidth && delta.second >= 0)
        {
          delta.first -= (offsetY << 1);
          delta.second += offsetY;
          changed = true;
        }
      }
      // Hay desplazamiento al oeste
      else if(offsetY > 0 && player->getY() <= maxY && player->getY() >= minY)
      {
        // Si no se han alcanzado los límites de la sala el desplazamiento al oeste es posible
        if(delta.first >= 0 && delta.second <= room->getDestination()->h - ScreenHeight)
        {
          delta.first -= (offsetY << 1);
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
