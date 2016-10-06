#include "RoomBuilder.hpp"
#include "Exception.hpp"
#include "ItemDataManager.hpp"
#include "Room.hpp"
#include "FloorTile.hpp"
#include "Wall.hpp"
#include "GridItem.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "Door.hpp"
#include "BonusManager.hpp"
#include "GameManager.hpp"
#include <loadpng.h>

namespace isomot
{

RoomBuilder::RoomBuilder(ItemDataManager* itemDataManager, const std::string& fileName)
{
  // Nombre del archivo
  this->fileName = fileName;

  // El gestor de datos
  this->itemDataManager = itemDataManager;
  // La sala en construcción
  this->room = 0;
}

RoomBuilder::RoomBuilder(ItemDataManager* itemDataManager)
{
  // El gestor de datos
  this->itemDataManager = itemDataManager;
  // La sala en construcción
  this->room = 0;
}

RoomBuilder::~RoomBuilder()
{

}

Room* RoomBuilder::buildRoom()
{
  // Procesa el archivo XML especificado para construir la sala
  try
  {
    std::auto_ptr<rxml::RoomXML> roomXML(rxml::room(fileName.c_str()));

    // Crea la sala con los parámetros básicos: identificador, dimensiones y existencia o no de suelo
    this->room = new Room(roomXML->name(), roomXML->scenery(), roomXML->xTiles(), roomXML->yTiles(), roomXML->width(), FloorId(int(roomXML->floorType())));
    if(this->room == 0) throw "Room creation error: " + fileName + " has bad atributtes.";

    // Para calcular las coordenadas de pantalla donde se situará el origen del espacio isométrico,
    // se necesita saber si la sala tiene puerta al norte y/o al este
    bool hasNorthDoor = false;
    bool hasEastDoor = false;

    // Recorre la lista de elementos buscando las puertas
    for(rxml::items::item_const_iterator i = roomXML->items().item().begin(); i != roomXML->items().item().end(); ++i)
    {
      if((*i).type() == rxml::type::door)
      {
        if((*i).direction() == rxml::direction::north || (*i).direction() == rxml::direction::northeast || (*i).direction() == rxml::direction::northwest)
        {
          hasNorthDoor = true;
        }
        else if((*i).direction() == rxml::direction::east || (*i).direction() == rxml::direction::eastnorth || (*i).direction() == rxml::direction::eastsouth)
        {
          hasEastDoor = true;
        }
      }
    }

    // Una vez que se sabe de la existencia o no de las puertas, se calculan las coordenadas
    room->calculateCoordinates(hasNorthDoor, hasEastDoor,
                               roomXML->deltaX().present() ? roomXML->deltaX().get() : 0,
                               roomXML->deltaY().present() ? roomXML->deltaY().get() : 0);

    // Si la sala es triple se almacenan los datos constantes para situar inicialmente la cámara
    // y las coordenadas de la sala donde se realizará el desplazamiento de la cámara
    if(roomXML->triple_room_data().present())
    {
      if(roomXML->triple_room_data().get().northeast().present())
      {
        rxml::northeast d = roomXML->triple_room_data().get().northeast().get();
        room->addTripleRoomStartPoint(Northeast, d.x(), d.y());
      }
      if(roomXML->triple_room_data().get().southeast().present())
      {
        rxml::southeast d = roomXML->triple_room_data().get().southeast().get();
        room->addTripleRoomStartPoint(Southeast, d.x(), d.y());
      }
      if(roomXML->triple_room_data().get().northwest().present())
      {
        rxml::northwest d = roomXML->triple_room_data().get().northwest().get();
        room->addTripleRoomStartPoint(Northwest, d.x(), d.y());
      }
      if(roomXML->triple_room_data().get().southwest().present())
      {
        rxml::southwest d = roomXML->triple_room_data().get().southwest().get();
        room->addTripleRoomStartPoint(Southwest, d.x(), d.y());
      }
      if(roomXML->triple_room_data().get().eastnorth().present())
      {
        rxml::eastnorth d = roomXML->triple_room_data().get().eastnorth().get();
        room->addTripleRoomStartPoint(Eastnorth, d.x(), d.y());
      }
      if(roomXML->triple_room_data().get().eastsouth().present())
      {
        rxml::eastsouth d = roomXML->triple_room_data().get().eastsouth().get();
        room->addTripleRoomStartPoint(Eastsouth, d.x(), d.y());
      }
      if(roomXML->triple_room_data().get().westnorth().present())
      {
        rxml::westnorth d = roomXML->triple_room_data().get().westnorth().get();
        room->addTripleRoomStartPoint(Westnorth, d.x(), d.y());
      }
      if(roomXML->triple_room_data().get().westsouth().present())
      {
        rxml::westsouth d = roomXML->triple_room_data().get().westsouth().get();
        room->addTripleRoomStartPoint(Westsouth, d.x(), d.y());
      }

      room->assignTripleRoomBounds(roomXML->triple_room_data().get().bound_x().minimum(),
                                   roomXML->triple_room_data().get().bound_x().maximum(),
                                   roomXML->triple_room_data().get().bound_y().minimum(),
                                   roomXML->triple_room_data().get().bound_y().maximum());
    }

    // Crea el suelo a base de losetas
    for(rxml::floor::tile_const_iterator i = roomXML->floor().tile().begin(); i != roomXML->floor().tile().end (); ++i)
    {
      FloorTile* floorTile = this->buildFloorTile(*i);
      room->addComponent(floorTile);
    }

    // Crea las paredes sin puertas
    if(roomXML->walls().present())
    {
      for(rxml::walls::wall_const_iterator i = roomXML->walls().get().wall().begin(); i != roomXML->walls().get().wall().end(); ++i)
      {
        Wall* wall = this->buildWall(*i);
        room->addComponent(wall);
      }
    }

    // Sitúa los elementos en la sala. Un elemento puede ser: un muro, una puerta, uno rejilla o uno libre
    for(rxml::items::item_const_iterator i = roomXML->items().item().begin(); i != roomXML->items().item().end (); ++i)
    {
      // Es una pared. Las paredes formadas por elementos son aquellas que tienen puertas
      if((*i).type() == rxml::type::wall)
      {

      }
      // Es una puerta
      else if((*i).type() == rxml::type::door)
      {
        Door* door = this->buildDoor(*i);

        if(door == 0)
        {
          std::ostringstream oss;
          oss << "Cannot build door with coordinates " << (*i).x() << ", " << (*i).y() << ", " << (*i).z();
          throw oss.str();
        }
        room->addComponent(door);
      }
      // Es un elemento rejilla
      else if((*i).type() == rxml::type::griditem)
      {
        GridItem* gridItem = this->buildGridItem(*i);

        if(gridItem == 0)
        {
          std::ostringstream oss;
          oss << "Cannot build grid-item with coordinates " << (*i).x() << ", " << (*i).y() << ", " << (*i).z();
          throw oss.str();
        }
        room->addComponent(gridItem);
      }
      // Es un elemento libre
      else if((*i).type() == rxml::type::freeitem)
      {
        FreeItem* freeItem = this->buildFreeItem(*i);

        if(freeItem == 0)
        {
          // En este caso no se lanza ninguna excepción porque hay
          // elementos -los bonus- que pueden no crearse
          std::ostringstream oss;
          oss << "Cannot build free-item with coordinates " << (*i).x() << ", " << (*i).y() << ", " << (*i).z();
          std::cout << oss.str() << std::endl;
        }
        else
        {
          room->addComponent(freeItem);
        }
      }
    }

    // Calcula los límites de la sala
    room->calculateBounds();
  }
  catch(const xml_schema::exception& e)
  {
    std::cout << e << std::endl;
  }
  catch(const Exception& e)
  {
    std::cout << e.what() << std::endl;
  }

  return this->room;
}

PlayerItem* RoomBuilder::buildPlayerItem(const PlayerId& playerId, const BehaviorId& behaviorId, int x, int y, int z, const Direction& direction)
{
  return buildPlayerItem(this->room, playerId, behaviorId, x, y, z, direction);
}

PlayerItem* RoomBuilder::buildPlayerItem(Room* room, const PlayerId& playerId, const BehaviorId& behaviorId, int x, int y, int z, const Direction& direction, bool hasItem)
{
  PlayerItem* playerItem = 0;
  GameManager* gameManager = GameManager::getInstance();
  PlayerId newPlayerId = playerId;
  BehaviorId newBehaviorId = behaviorId;

  // Si el jugador compuesto se ha quedado sin vidas, se comprueba si alguno de
  // los jugadores conseva alguna, para crearlo en su lugar
  if(gameManager->getLives(playerId) == 0)
  {
    if(playerId == HeadAndHeels)
    {
      // Jugador Superviviente
      if(gameManager->getLives(Head) > 0)
      {
        newPlayerId = Head;
        newBehaviorId = HeadBehavior;
      }
      else if(gameManager->getLives(Heels) > 0)
      {
        newPlayerId = Heels;
        newBehaviorId = HeelsBehavior;
      }
      else
      {
        newPlayerId = NoPlayer;
      }
    }
    // Es posible que los dos jugadores se unieran en la sala y se hayan quedado sin vidas
    else
    {
      if(gameManager->getLives(Head) == 0 && gameManager->getLives(Heels) == 0)
      {
        newPlayerId = NoPlayer;
      }
    }
  }

  // Se buscan los datos del elemento
  ItemData* itemData = this->itemDataManager->find(short(newPlayerId));

  // Si se han encontrado y al jugador le quedan vidas, se coloca el elemento en la sala
  if(newPlayerId != NoPlayer && itemData != 0)
  {
    // Para poder crear el jugador le deben quedar vidas
    if(gameManager->getLives(newPlayerId) > 0)
    {
      playerItem = new PlayerItem(itemData, x, y, z, direction);

      // No se pueden llevar elementos a otras salas
      if(hasItem)
      {
        gameManager->setItemTaken(0);
      }

      // Asigna las vidas
      playerItem->setLives(gameManager->getLives(newPlayerId));

      // Asigna la posesión de sus objetos
      playerItem->setTools(gameManager->hasTool(newPlayerId));

      // Asigna la cantidad de munición disponible
      playerItem->setAmmo(gameManager->getDonuts(newPlayerId));

      // Asigna (si hay) la cantidad de grandes saltos disponibles
      playerItem->setHighJumps(gameManager->getHighJumps());

      // Asigna (si hay) el tiempo restante de doble velocidad
      playerItem->setHighSpeed(gameManager->getHighSpeed());

      // Asigna (si hay) el tiempo restante de inmunidad
      playerItem->setShieldTime(gameManager->getShield(newPlayerId));

      // Un jugador necesita acceso a los datos de otros elementos dado que los
      // necesita para modelar su comportamiento. Por ejemplo: el disparo y el elemento
      // de transición entre los telepuertos
      playerItem->assignBehavior(newBehaviorId, reinterpret_cast<void*>(itemDataManager));

      // Añade el jugador a la sala
      room->addComponent(playerItem);
    }
  }

  return playerItem;
}

FloorTile* RoomBuilder::buildFloorTile(const rxml::tile& tile)
{
  FloorTile* floorTile = 0;

  try
  {
    BITMAP* bitmap = load_png( ( isomot::sharePath() + tile.bitmap() ).c_str(), 0 );
    if(bitmap == 0) throw "Bitmap " + tile.bitmap() + " not found.";
    int column = room->getTilesX() * tile.y() + tile.x();
    floorTile = new FloorTile(column, tile.x(), tile.y(), bitmap);
    floorTile->setOffset(tile.offsetX(), tile.offsetY());
  }
  catch(const Exception& e)
  {
    std::cout << e.what() << std::endl;
  }

  return floorTile;
}

Wall* RoomBuilder::buildWall(const rxml::wall& wall)
{
  Wall* roomWall = 0;

  try
  {
    BITMAP* bitmap = load_png( ( isomot::sharePath() + wall.bitmap() ).c_str(), 0 );
    if(bitmap == 0) throw "Bitmap " + wall.bitmap() + " not found.";
    roomWall = new Wall(wall.axis() == rxml::axis::x ? AxisX : AxisY, wall.index(), bitmap);
  }
  catch(const Exception& e)
  {
    std::cout << e.what() << std::endl;
  }

  return roomWall;
}

GridItem* RoomBuilder::buildGridItem(const rxml::item& item)
{
  GridItem* gridItem = 0;

  // Se buscan los datos del elemento
  ItemData* itemData = this->itemDataManager->find(item.label());

  // Si se han encontrado, se coloca el elemento en la sala
  if(itemData != 0)
  {
    // Hay diferencia entre los enumerados de dirección manejados por el archivo y por las salas
    gridItem = new GridItem(itemData, item.x(), item.y(), (item.z() > Top ? item.z() * LayerHeight : Top),
                            item.direction() == rxml::direction::none ? NoDirection : Direction(item.direction() - 1));

    // Datos extra necesarios para el comportamiento del elemento
    if(item.behavior() == VolatileTimeBehavior || item.behavior() == VolatileTouchBehavior ||
       item.behavior() == VolatileWeightBehavior || item.behavior() == VolatileHeavyBehavior ||
       item.behavior() == VolatilePuppyBehavior)
    {
      gridItem->assignBehavior(BehaviorId(item.behavior()), reinterpret_cast<void*>(this->itemDataManager->find(BubblesLabel)));
    }
    else
    {
      gridItem->assignBehavior(BehaviorId(item.behavior()), 0);
    }
  }

  return gridItem;
}

FreeItem* RoomBuilder::buildFreeItem(const rxml::item& item)
{
  FreeItem* freeItem = 0;
  int foundData = 0;
  int* data = 0;

  // Se buscan los datos del elemento
  ItemData* itemData = this->itemDataManager->find(item.label());

  // Si se han encontrado, se coloca el elemento en la sala
  if(itemData != 0)
  {
    // Coordenadas libres
    int fx = item.x() * room->getTileSize() + ((room->getTileSize() - itemData->widthX) >> 1);
    int fy = (item.y() + 1) * room->getTileSize() - ((room->getTileSize() - itemData->widthY) >> 1) - 1;
    int fz = item.z() != Top ? item.z() * LayerHeight : Top;

    // La única excepción para colocar un elemento es que sea un bonus y ya se haya cogido
    if(BonusManager::getInstance()->isPresent(room->getIdentifier(), itemData->label))
    {
      // Hay diferencia entre los enumerados de dirección manejados por el archivo y por las salas
      freeItem = new FreeItem(itemData, fx, fy, fz,
                              item.direction() == rxml::direction::none ? NoDirection : Direction(item.direction() - 1));

      // Datos extra necesarios para el comportamiento del elemento
      switch(item.behavior())
      {
        case ElevatorBehavior:
          data = new int[3];

          // Extrae los datos extra asociados al ascensor
          for(rxml::item::extra_const_iterator i = item.extra().begin(); i != item.extra().end (); ++i)
          {
            data[foundData++] = (*i);
          }

          // Si los tres datos necesarios constan entonces se asigna el comportamiento
          if(foundData == 3)
          {
            freeItem->assignBehavior(BehaviorId(item.behavior()), reinterpret_cast<void*>(data));
            delete[] data;
          }
          break;

        case HunterWaiting4Behavior:
          freeItem->assignBehavior(BehaviorId(item.behavior()), reinterpret_cast<void*>(this->itemDataManager->find(ImperialGuardLabel)));
          break;

        case SpecialBehavior:
        case VolatileTimeBehavior:
        case VolatileTouchBehavior:
        case VolatileWeightBehavior:
        case CannonBallBehavior:
          freeItem->assignBehavior(BehaviorId(item.behavior()), reinterpret_cast<void*>(this->itemDataManager->find(BubblesLabel)));
          break;

        default:
          freeItem->assignBehavior(BehaviorId(item.behavior()), 0);
      }
    }
  }

  return freeItem;
}

Door* RoomBuilder::buildDoor(const rxml::item& item)
{
  return new Door(this->itemDataManager, item.label(), item.x(), item.y(), (item.z() > Top ? item.z() * LayerHeight : Top), Direction(item.direction() - 1));
}

}
