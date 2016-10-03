#include "Door.hpp"
#include "ItemDataManager.hpp"
#include "FreeItem.hpp"
#include "Mediator.hpp"

namespace isomot
{

Door::Door(ItemDataManager* itemDataManager, short label, int cx, int cy, int z, const Direction& direction)
{
  this->itemDataManager = itemDataManager;
  this->label = label;
  this->cx = cx;
  this->cy = cy;
  this->z = z;
  this->direction = direction;
  this->leftJamb = 0;
  this->rightJamb = 0;
  this->lintel = 0;
}

Door::~Door()
{
  // La destrucción de las partes de la puerta la gestiona el mediador
}

FreeItem* Door::getLeftJamb()
{
  if(leftJamb == 0)
  {
    // Coordenadas libres
    int x(0), y(0);
    // Tamaño de la loseta
    int tileSize = mediator->getTileSize();

    // Se buscan los datos del elemento
    ItemData* itemData = itemDataManager->find(label);

    // Si se han encontrado, se calculan sus coordenadas libres
    if(itemData != 0)
    {
      switch(direction)
      {
        case North:
        case Northeast:
        case Northwest:
          x = cx * tileSize + itemData->widthX - 2;
          y = (cy + 2) * tileSize - 2;
          leftLimit = y + itemData->widthY;
          break;

        case South:
        case Southeast:
        case Southwest:
          x = cx * tileSize;
          y = (cy + 2) * tileSize - 2;
          leftLimit = y + itemData->widthY;
          break;

        case East:
        case Eastnorth:
        case Eastsouth:
          x = cx * tileSize;
          y = (cy + 1) * tileSize - 1;
          leftLimit = x + itemData->widthX;
          break;

        case West:
        case Westnorth:
        case Westsouth:
          x = cx * tileSize;
          y = (cy + 1) * tileSize - itemData->widthY + 1;
          leftLimit = x + itemData->widthX;
          break;

        default:
          ;
      }

      // Creación de la jamba como elemento libre
      leftJamb = new FreeItem(itemData, x, y, Top, NoDirection);
    }
  }

  return leftJamb;
}

FreeItem* Door::getRightJamb()
{
  if(rightJamb == 0)
  {
    // Coordenadas libres
    int x(0), y(0);
    // Tamaño de la loseta
    int tileSize = mediator->getTileSize();

    // Se buscan los datos del elemento
    ItemData* itemData = itemDataManager->find(label + 1);

    // Si se han encontrado, se calculan sus coordenadas libres
    if(itemData != 0)
    {
      switch(direction)
      {
        case North:
        case Northeast:
        case Northwest:
          x = cx * tileSize + itemData->widthX - 2;
          y = cy * tileSize + itemData->widthY - 1;
          rightLimit = y;
          break;

        case South:
        case Southeast:
        case Southwest:
          x = cx * tileSize;
          y = cy * tileSize + itemData->widthY - 1;
          rightLimit = y;
          break;

        case East:
        case Eastnorth:
        case Eastsouth:
          x = (cx + 2) * tileSize - itemData->widthX - 2;
          y = (cy + 1) * tileSize - 1;
          rightLimit = x;
          break;

        case West:
        case Westnorth:
        case Westsouth:
          x = (cx + 2) * tileSize - itemData->widthX - 2;
          y = (cy + 1) * tileSize - itemData->widthY + 1;
          rightLimit = x;
          break;

        default:
          ;
      }

      // Creación de la jamba como elemento libre
      rightJamb = new FreeItem(itemData, x, y, Top, NoDirection);
    }
  }

  return rightJamb;
}

FreeItem* Door::getLintel()
{
  if(lintel == 0)
  {
    // Coordenadas libres
    int x(0), y(0);
    // Tamaño de la loseta
    int tileSize = mediator->getTileSize();

    // Se buscan los datos del elemento
    ItemData* itemData = itemDataManager->find(label + 2);

    // Si se han encontrado, se calculan sus coordenadas libres
    if(itemData != 0)
    {
      switch(direction)
      {
        case North:
        case Northeast:
        case Northwest:
          x = cx * tileSize + itemData->widthX - 2;
          y = (cy + 2) * tileSize - 1;
          break;

        case South:
        case Southeast:
        case Southwest:
          x = cx * tileSize;
          y = (cy + 2) * tileSize - 1;
          break;

        case East:
        case Eastnorth:
        case Eastsouth:
          x = cx * tileSize;
          y = (cy + 1) * tileSize - 1;
          break;

        case West:
        case Westnorth:
        case Westsouth:
          x = cx * tileSize;
          y = (cy + 1) * tileSize - itemData->widthY + 1;
          break;

        default:
          ;
      }

      // Creación del dintel como elemento libre
      lintel = new FreeItem(itemData, x, y, Top, NoDirection);
    }
  }

  return lintel;
}

bool Door::isUnderDoor(int x, int y, int z)
{
  bool result = false;
  // La comprobación puede realizarse si el elemento está a ras del suelo
  z = (z < 0 ? 0 : z);

  switch(direction)
  {
    case North:
    case Northeast:
    case Northwest:
    case South:
    case Southeast:
    case Southwest:
      result = (y >= rightLimit && y <= leftLimit && z >= leftJamb->getZ() && z <= lintel->getZ());
      break;

    case East:
    case Eastnorth:
    case Eastsouth:
    case West:
    case Westnorth:
    case Westsouth:
      result = (x <= rightLimit && x >= leftLimit && z >= leftJamb->getZ() && z <= lintel->getZ());
      break;

    default:
      ;
  }

  return result;
}

}
