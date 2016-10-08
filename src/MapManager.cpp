
#include "MapManager.hpp"
#include "Ism.hpp"
#include "Isomot.hpp"
#include "RoomBuilder.hpp"
#include "ItemDataManager.hpp"
#include "PlayerItem.hpp"
#include "Door.hpp"
#include "Behavior.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "Camera.hpp"
#include "Exception.hpp"
#include "SoundManager.hpp"

# include <xercesc/util/PlatformUtils.hpp>

namespace isomot
{

MapManager::MapManager(Isomot* isomot, const std::string& fileName)
: isomot(isomot),
  activeRoom(0),
  fileName(fileName)
{
  path = isomot::sharePath() + "map/";
}

MapManager::~MapManager()
{

}

void MapManager::load()
{
  xercesc::XMLPlatformUtils::Initialize ();

  // Carga el archivo XML especificado y almacena los datos XML en la lista
  try
  {
    std::auto_ptr<mxml::MapXML> mapXML(mxml::map((path + fileName).c_str()));

    for(mxml::MapXML::room_const_iterator i = mapXML->room().begin(); i != mapXML->room().end (); ++i)
    {
      // Se crea la sala a partir del nombre de su archivo XML
      std::auto_ptr<MapRoomData> roomData(new MapRoomData((*i).file()));

      // Conexión norte
      if((*i).north().present())
      {
        roomData->setNorth((*i).north().get());
      }

      // Conexión sur
      if((*i).south().present())
      {
        roomData->setSouth((*i).south().get());
      }

      // Conexión este
      if((*i).east().present())
      {
        roomData->setEast((*i).east().get());
      }

      // Conexión oeste
      if((*i).west().present())
      {
        roomData->setWest((*i).west().get());
      }

      // Conexión inferior
      if((*i).floor().present())
      {
        roomData->setFloor((*i).floor().get());
      }

      // Conexión superior
      if((*i).roof().present())
      {
        roomData->setRoof((*i).roof().get());
      }

      // Conexión por telepuerto
      if((*i).teleport().present())
      {
        roomData->setTeleport((*i).teleport().get());
      }

      // Conexión por un segundo telepuerto
      if((*i).teleport2().present())
      {
        roomData->setTeleport2((*i).teleport2().get());
      }

      // Conexión norte-este
      if((*i).north_east().present())
      {
        roomData->setNorthEast((*i).north_east().get());
      }

      // Conexión norte-oeste
      if((*i).north_west().present())
      {
        roomData->setNorthWest((*i).north_west().get());
      }

      // Conexión sur-este
      if((*i).south_east().present())
      {
        roomData->setSouthEast((*i).south_east().get());
      }

      // Conexión norte-oeste
      if((*i).south_west().present())
      {
        roomData->setSouthWest((*i).south_west().get());
      }

      // Conexión este-norte
      if((*i).east_north().present())
      {
        roomData->setEastNorth((*i).east_north().get());
      }

      // Conexión este-sur
      if((*i).east_south().present())
      {
        roomData->setEastSouth((*i).east_south().get());
      }

      // Conexión oeste-norte
      if((*i).west_north().present())
      {
        roomData->setWestNorth((*i).west_north().get());
      }

      // Conexión oeste-sur
      if((*i).west_south().present())
      {
        roomData->setWestSouth((*i).west_south().get());
      }

      // Los datos de la sala se incorporan a la lista
      this->mapData.push_back(*roomData.get());
    }
  }
  catch(const xml_schema::exception& e)
  {
    std::cerr << e << std::endl;
  }
  catch(const Exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  xercesc::XMLPlatformUtils::Terminate ();
}

void MapManager::start(const std::string& firstRoomFileName, const std::string& secondRoomFileName)
{
  // Datos de la primera sala en el mapa
  MapRoomData* firstRoomData = findRoomData(firstRoomFileName);

  // Creación de la primera sala
  if(firstRoomData != 0)
  {
    std::auto_ptr<RoomBuilder> roomBuilder(new RoomBuilder(isomot->getItemDataManager(), path + firstRoomFileName));
    Room* firstRoom = roomBuilder->buildRoom();

    // Si se ha creado la sala se coloca al primer personaje en el centro
    if(firstRoom != 0)
    {
      // Posición inicial del jugador
      ItemData* firstPlayerData = isomot->getItemDataManager()->find(Head);

      int centerX = ((firstRoom->getBound(South) - firstRoom->getBound(North) + firstPlayerData->widthX) >>1) +
                     (firstRoom->getDoor(North) != 0 ? firstRoom->getTileSize() >> 1 : 0) -
                     (firstRoom->getDoor(South) != 0 ? firstRoom->getTileSize() >> 1 : 0);
      int centerY = ((firstRoom->getBound(West) - firstRoom->getBound(East) + firstPlayerData->widthY) >> 1) +
                     (firstRoom->getDoor(East) != 0 ? firstRoom->getTileSize() >> 1 : 0) -
                     (firstRoom->getDoor(West) != 0 ? firstRoom->getTileSize() >> 1 : 0) - 1;

      // Almacena en los datos de la sala en el mapa la posición inicial del jugador
      PlayerStartPosition firstPlayerPosition(Head);
      firstPlayerPosition.assignPosition(Wait, centerX, centerY, 0, West);
      firstRoomData->addPlayerPosition(firstPlayerPosition);

      // Crea al jugador
      roomBuilder->buildPlayerItem(Head, HeadBehavior, centerX, centerY, 0, West);

      // La primera sala es la sala activa
      firstRoom->activatePlayer(Head);
      firstRoom->getCamera()->turnOn(firstRoom->getMediator()->getActivePlayer(), Wait);
      activeRoom = firstRoom;
      firstRoomData->setActivePlayer(Head);
      rooms.push_back(firstRoom);
    }
  }

  // Datos de la segunda sala en el mapa
  MapRoomData* secondRoomData = findRoomData(secondRoomFileName);

  // Creación de la segunda sala
  if(secondRoomData != 0)
  {
    std::auto_ptr<RoomBuilder> roomBuilder(new RoomBuilder(isomot->getItemDataManager(), path + secondRoomFileName));
    Room* secondRoom = roomBuilder->buildRoom();

    // Si se ha creado la sala se coloca al primer personaje en el centro
    if(secondRoom != 0)
    {
      // Posición inicial del jugador
      ItemData* secondPlayerData = isomot->getItemDataManager()->find(Heels);

      int centerX = ((secondRoom->getBound(South) - secondRoom->getBound(North) + secondPlayerData->widthX) >> 1) +
                     (secondRoom->getDoor(North) != 0 ? secondRoom->getTileSize() >> 1 : 0) -
                     (secondRoom->getDoor(South) != 0 ? secondRoom->getTileSize() >> 1 : 0);
      int centerY = ((secondRoom->getBound(West) - secondRoom->getBound(East) + secondPlayerData->widthY) >> 1) +
                     (secondRoom->getDoor(East) != 0 ? secondRoom->getTileSize() >> 1 : 0) -
                     (secondRoom->getDoor(West) != 0 ? secondRoom->getTileSize() >> 1 : 0) - 1;

      // Almacena en los datos de la sala en el mapa la posición inicial del jugador
      PlayerStartPosition secondPlayerPosition(Heels);
      secondPlayerPosition.assignPosition(Wait, centerX, centerY, 0, South);
      secondRoomData->addPlayerPosition(secondPlayerPosition);

      // Crea al jugador
      roomBuilder->buildPlayerItem(Heels, HeelsBehavior, centerX, centerY, 0, South);

      // Activa al jugador
      secondRoom->activatePlayer(Heels);
      secondRoom->getCamera()->turnOn(secondRoom->getMediator()->getActivePlayer(), Wait);
      secondRoomData->setActivePlayer(Heels);
      rooms.push_back(secondRoom);
    }
  }
}

void MapManager::startOldGame(const sgxml::player& data)
{
  // Datos de la sala en el mapa
  MapRoomData* roomData = findRoomData(data.roomFilename());
  // La sala a crear
  Room* room = 0;

  // Creación de la sala
  if(roomData != 0)
  {
    // Si ya hay una sala creada existe la posibilidad de que la sala del segundo jugador sea la misma
    if(this->activeRoom != 0 && this->activeRoom->getIdentifier().compare(roomData->getRoom()) == 0)
    {
      room = this->activeRoom;
    }
    else
    {
      std::auto_ptr<RoomBuilder> roomBuilder(new RoomBuilder(isomot->getItemDataManager(), path + data.roomFilename()));
      room = roomBuilder->buildRoom();
    }

    // Si se ha creado la sala se coloca al personaje
    if(room != 0)
    {
      PlayerId playerId;
      BehaviorId behaviorId;

      switch(data.label())
      {
        case Head:
          playerId = Head;
          behaviorId = HeadBehavior;
          break;

        case Heels:
          playerId = Heels;
          behaviorId = HeelsBehavior;
          break;

        case HeadAndHeels:
          playerId = HeadAndHeels;
          behaviorId = HeadAndHeelsBehavior;
          break;
      }

      // Almacena en los datos de la sala en el mapa la posición inicial del jugador
      PlayerStartPosition playerPosition(playerId);
      playerPosition.assignPosition(Direction(data.entry()), data.x(), data.y(), data.z(), Direction(data.direction()));
      roomData->addPlayerPosition(playerPosition);

      // Crea al jugador en la sala
      std::auto_ptr<RoomBuilder> roomBuilder(new RoomBuilder(isomot->getItemDataManager()));
      PlayerItem* player = roomBuilder->buildPlayerItem(room, playerId, behaviorId, data.x(), data.y(), data.z(), Direction(data.direction()));

      // Se cambia el estado del jugador en función de la vía de entrada
      switch(Direction(data.entry()))
      {
        case North:
        case Northeast:
        case Northwest:
          player->getBehavior()->changeStateId(StateAutoMoveSouth);
          break;

        case South:
        case Southeast:
        case Southwest:
          player->getBehavior()->changeStateId(StateAutoMoveNorth);
          break;

        case East:
        case Eastnorth:
        case Eastsouth:
          player->getBehavior()->changeStateId(StateAutoMoveWest);
          break;

        case West:
        case Westnorth:
        case Westsouth:
          player->getBehavior()->changeStateId(StateAutoMoveEast);
          break;

        case ByTeleport:
        case ByTeleport2:
        case Wait:
          player->getBehavior()->changeStateId(StateStartWayInTeletransport);
          break;

        default:
          ;
      }

      // Si se está creando al jugador activo entonces su sala es la sala activa
      if(data.active())
      {
        room->activatePlayer(playerId);
        room->getCamera()->turnOn(room->getMediator()->getActivePlayer(), Direction(data.entry()));
        this->activeRoom = room;
        roomData->setActivePlayer(playerId);
        this->rooms.push_back(room);
      }
      // Si el otro jugador se crea en otra sala entonces se establece como el jugador
      // que el usuario controla en dicha sala. Si se crea en la misma sala que el
      // jugador activo entonces no hace falta hacer nada más
      else if(this->activeRoom != room)
      {
        // Activa al jugador
        room->activatePlayer(playerId);
        room->getCamera()->turnOn(room->getMediator()->getActivePlayer(), Direction(data.entry()));
        roomData->setActivePlayer(playerId);
        this->rooms.push_back(room);
      }
    }
  }
}

void MapManager::reset()
{
  // Elimina las salas
  if(this->rooms.size() == 2 && this->rooms[0]->getIdentifier().compare(this->rooms[1]->getIdentifier()) == 0)
  {
    delete this->rooms[0];
  }
  else
  {
    std::for_each(this->rooms.begin(), this->rooms.end(), DeleteObject());
  }
  this->rooms.clear();
  this->activeRoom = 0;

  // Reestablece los datos de las salas
  std::for_each(this->mapData.begin(), this->mapData.end(), std::mem_fun_ref(&MapRoomData::reset));
}

Room* MapManager::changeRoom(const Direction& exit)
{
  Room* newRoom = 0;
  PlayerItem* player = 0;

  // Desactiva la sala activa
  activeRoom->deactivate();

  // Detiene todos los sonidos
  SoundManager::getInstance()->stopAllSounds();

  // Busca en el mapa los datos de la sala activa
  MapRoomData* activeRoomData = findRoomData(activeRoom->getIdentifier());

  // Borra los datos de entrada del jugador activo de la sala
  PlayerId activePlayer = activeRoomData->getActivePlayer();
  activeRoomData->removePlayerPosition(activePlayer);

  // Recupera la posición de salida del jugador activo
  player = static_cast<PlayerItem*>(activeRoom->getMediator()->findItem(short(activePlayer)));
  PlayerStartPosition exitPosition(activePlayer);
  exitPosition.assignPosition(player->getExit(), player->getX(), player->getY(), player->getZ(), player->getOrientation());
  BehaviorId playerBehavior = player->getBehavior()->getId();
  // Averigua si el jugador porta un elemento
  bool hasItem = player->consultTakenItemImage() != 0;

  // Almacena los límites sala para normalizar las coordenadas de salida o entrada a la nueva
  // sala en en caso de que el jugador acceda a través del suelo, el techo o un telepuerto
  int northBound = activeRoom->getBound(North);
  int eastBound = activeRoom->getBound(East);
  int southBound = activeRoom->getBound(South);
  int westBound = activeRoom->getBound(West);

  // Si no quedan jugadores en la sala entonces la sala se destruye
  if(!activeRoomData->remainPlayers() || activePlayer == HeadAndHeels)
  {
    rooms.erase(std::remove_if(rooms.begin(), rooms.end(), std::bind2nd(EqualRoom(), activeRoom->getIdentifier())), rooms.end());
    activeRoomData->clearPlayersPosition();
    delete activeRoom;
  }
  // En caso contrario se elimina al jugador activo y se selecciona el nuevo
  // jugador activo en la sala que se abandona
  else
  {
    activeRoom->removeComponent(player);
    activeRoomData->removePlayerPosition(PlayerId(player->getLabel()));
  }

  // Busca en el mapa los datos de la sala destino y obtiene la vía de entrada
  Direction entry;
  MapRoomData* destinationRoomData = findRoomData(activeRoomData->findDestinationRoom(exit, &entry));
  assert(destinationRoomData != 0);
  destinationRoomData->adjustEntry(&entry, activeRoomData->getRoom());

  // Si la sala existe entonces habrá un jugador en ella
  bool playerPresent = false;

  // Si la sala no está creada entonces se crea
  if(!destinationRoomData->remainPlayers())
  {
    std::auto_ptr<RoomBuilder> roomBuilder(new RoomBuilder(isomot->getItemDataManager(), path + destinationRoomData->getRoom()));
    newRoom = roomBuilder->buildRoom();
    rooms.push_back(newRoom);
  }
  // Si ya existe se busca en el vector de salas
  else
  {
    newRoom = findRoom(destinationRoomData->getRoom());
    newRoom->setActive(true);
    playerPresent = true;
  }

  // Datos del jugador activo
  ItemData* playerData = isomot->getItemDataManager()->find(activePlayer);

  // Posición inicial del jugador
  int x = exitPosition.getX();
  int y = exitPosition.getY();
  int z = exitPosition.getZ();
  newRoom->calculateEntryCoordinates(entry, playerData->widthX, playerData->widthY, northBound, eastBound, southBound, westBound, &x, &y, &z);

  // Almacena en los datos de la sala en el mapa la posición inicial del jugador
  PlayerStartPosition playerPosition(activePlayer);
  playerPosition.assignPosition(entry, x, y, z, exitPosition.getOrientation());

  // Si ya hay un jugador presente recupera sus datos por si se da la circunstancia de que
  // la vía de entrada de ambos jugadores sea la misma
  if(playerPresent)
  {
    PlayerItem* player = newRoom->getMediator()->getActivePlayer();
    PlayerStartPosition playerPresentPosition(PlayerId(player->getLabel()));
    playerPresentPosition.assignPosition(player->getExit(), player->getX(), player->getY(), player->getZ(), player->getOrientation());
    destinationRoomData->addPlayerPosition(playerPosition, playerPresentPosition);
  }
  else
  {
    destinationRoomData->addPlayerPosition(playerPosition);
  }

  // Crea al jugador
  std::auto_ptr<RoomBuilder> roomBuilder(new RoomBuilder(isomot->getItemDataManager()));
  if(entry == ByTeleport || entry == ByTeleport2)
  {
    z = Top;
  }
  player = roomBuilder->buildPlayerItem(newRoom, activePlayer, playerBehavior, x, y, z, exitPosition.getOrientation(), hasItem);

  // Se cambia el estado del jugador en función de la vía de entrada
  switch(entry)
  {
    case North:
    case Northeast:
    case Northwest:
      player->getBehavior()->changeStateId(StateAutoMoveSouth);
      break;

    case South:
    case Southeast:
    case Southwest:
      player->getBehavior()->changeStateId(StateAutoMoveNorth);
      break;

    case East:
    case Eastnorth:
    case Eastsouth:
      player->getBehavior()->changeStateId(StateAutoMoveWest);
      break;

    case West:
    case Westnorth:
    case Westsouth:
      player->getBehavior()->changeStateId(StateAutoMoveEast);
      break;

    case ByTeleport:
    case ByTeleport2:
      player->getBehavior()->changeStateId(StateStartWayInTeletransport);
      break;

    case Up:
      player->getBehavior()->changeStateId(StateFall);
      break;

    default:
      ;
  }

  // La sala destino es la sala activa
  destinationRoomData->setActivePlayer(activePlayer);
  newRoom->activatePlayer(activePlayer);
  newRoom->getCamera()->turnOn(newRoom->getMediator()->getActivePlayer(), entry);
  activeRoom = newRoom;
  // Activa la nueva sala
  activeRoom->activate();

  return newRoom;
}

Room* MapManager::restartRoom()
{
  Room* newRoom = 0;
  PlayerItem* player = 0;
  PlayerId activePlayer = NoPlayer;
  Direction entry = NoDirection;

  // Desactiva la sala activa
  activeRoom->deactivate();

  // Busca en el mapa los datos de la sala activa
  MapRoomData* activeRoomData = findRoomData(activeRoom->getIdentifier());

  // Se vuelve a crear la sala
  std::auto_ptr<RoomBuilder> roomBuilder(new RoomBuilder(isomot->getItemDataManager(), path + activeRoomData->getRoom()));
  newRoom = roomBuilder->buildRoom();

  // Posición inicial de todos los jugadores presentes en la sala
  std::list<PlayerStartPosition> playersPosition = activeRoomData->getPlayersPosition();

  BehaviorId playerBehavior = NoBehavior;

  // Para cada jugador presente en la sala:
  for(std::list<PlayerStartPosition>::iterator i = playersPosition.begin(); i != playersPosition.end(); ++i)
  {
    // Recupera el comportamiento del jugador
    player = dynamic_cast<PlayerItem*>(activeRoom->getMediator()->findItem(short((*i).getPlayer())));

    // Si hay un jugador presente en la sala, se recupera su comportamiento
    if(player != 0)
    {
      playerBehavior = player->getBehavior()->getId();
    }
    // Si no hay jugadores en la sala pero se sabe que entraron entonces estamos en el caso en
    // el que entró un jugador simple y formó el compuesto con otro ya presente. En tales circunstancias
    // se crean los dos jugadores simples a partir de sus datos de posición inicial
    else
    {
      switch((*i).getPlayer())
      {
        case Head:
          playerBehavior = HeadBehavior;
          break;

        case Heels:
          playerBehavior = HeelsBehavior;
          break;

        case HeadAndHeels:
          playerBehavior = HeadAndHeelsBehavior;
          break;

        default:
          ;
      }
    }

    // Se comprueba si éste es el jugador activo. En tal caso se recuperan sus datos de posición inicial
    if(activeRoomData->getActivePlayer() == (*i).getPlayer() || player == 0)
    {
      // Si no hay jugador activo se asigna a partir de los datos de posición
      activePlayer = player != 0 ? activeRoomData->getActivePlayer() : (*i).getPlayer();

      // Posición inicial del jugador
      int x = (*i).getX();
      int y = (*i).getY();
      int z = (*i).getZ();

      // Crea al jugador
      player = roomBuilder->buildPlayerItem(newRoom, (*i).getPlayer(), playerBehavior, x, y, z, (*i).getOrientation());

      // Si se ha podido crear al jugador, la partida continua. De lo contrario quiere decir que ha terminado
      if(player != 0)
      {
        // Vía de entrada del jugador
        entry = (*i).getEntry();

        // Si el jugador ha cambiado (se ha pasado del jugador compuesto a uno simple), se actualiza
        if(PlayerId(player->getLabel()) != activePlayer)
        {
          activePlayer = PlayerId(player->getLabel());
          // Se debe actualizar la lista de posiciones iniciales porque el jugador ha cambiado
          PlayerStartPosition newPlayerStartPosition(activePlayer);
          newPlayerStartPosition.assignPosition(entry, (*i).getX(), (*i).getY(), (*i).getZ(), (*i).getOrientation());
          i = playersPosition.erase(i);
          i = playersPosition.insert(i, newPlayerStartPosition);
        }

        // Se cambia el estado del jugador en función de la vía de entrada
        switch(entry)
        {
          case North:
          case Northeast:
          case Northwest:
            player->getBehavior()->changeStateId(StateAutoMoveSouth);
            break;

          case South:
          case Southeast:
          case Southwest:
            player->getBehavior()->changeStateId(StateAutoMoveNorth);
            break;

          case East:
          case Eastnorth:
          case Eastsouth:
            player->getBehavior()->changeStateId(StateAutoMoveWest);
            break;

          case West:
          case Westnorth:
          case Westsouth:
            player->getBehavior()->changeStateId(StateAutoMoveEast);
            break;

          case ByTeleport:
          case ByTeleport2:
            player->getBehavior()->changeStateId(StateStartWayInTeletransport);
            break;

          default:
            ;
        }
      }
    }
    // Si no es el jugador activo entonces el jugador reaparecerá en la posición que tenía en el
    // momento que entró en la sala
    else
    {
      // Crea al jugador
      PlayerStartPosition* position = activeRoomData->findPlayerPosition((*i).getPlayer());
      player = roomBuilder->buildPlayerItem(newRoom, (*i).getPlayer(), playerBehavior, position->getX(), position->getY(), position->getZ(), position->getOrientation());

      // Se cambia el estado del jugador en función de la vía de entrada
      switch((*i).getEntry())
      {
        case North:
        case Northeast:
        case Northwest:
          player->getBehavior()->changeStateId(StateAutoMoveSouth);
          break;

        case South:
        case Southeast:
        case Southwest:
          player->getBehavior()->changeStateId(StateAutoMoveNorth);
          break;

        case East:
        case Eastnorth:
        case Eastsouth:
          player->getBehavior()->changeStateId(StateAutoMoveWest);
          break;

        case West:
        case Westnorth:
        case Westsouth:
          player->getBehavior()->changeStateId(StateAutoMoveEast);
          break;

        case ByTeleport:
        case ByTeleport2:
          player->getBehavior()->changeStateId(StateStartWayInTeletransport);
          break;

        default:
          ;
      }
    }
  }

  // Se destruye la sala
  rooms.erase(std::remove_if(rooms.begin(), rooms.end(), std::bind2nd(EqualRoom(), activeRoom->getIdentifier())), rooms.end());
  delete activeRoom;

  // Hay jugador
  if(player != 0)
  {
    // Se añade la sala recién creada a la lista
    rooms.push_back(newRoom);

    // La primera sala es la sala activa
    activeRoomData->setActivePlayer(activePlayer);
    newRoom->activatePlayer(activePlayer);
    newRoom->getCamera()->turnOn(newRoom->getMediator()->getActivePlayer(), entry);
    activeRoom = newRoom;
    // Activa la nueva sala
    activeRoom->activate();
  }
  // No se ha podido crear el jugador, la partida termina
  else
  {
    delete newRoom;
    activeRoom = newRoom = 0;
  }

  return newRoom;
}

Room* MapManager::createRoom(const std::string& fileName)
{
  std::auto_ptr<RoomBuilder> roomBuilder(new RoomBuilder(isomot->getItemDataManager(), path + fileName));
  return roomBuilder->buildRoom();
}

Room* MapManager::swapRoom()
{
  // Tiene que haber más de una sala para que se pueda hacer el cambio
  if(rooms.size() > 1)
  {
    // Detiene todos los sonidos
    SoundManager::getInstance()->stopAllSounds();

    // Desactiva la sala actual
    activeRoom->deactivate();
    // Busca la sala
    std::vector<Room*>::iterator i = std::find_if(rooms.begin(), rooms.end(), std::bind2nd(EqualRoom(), activeRoom->getIdentifier()));
    // Se pasa a la siguiente
    ++i;
    // Si se llegó al final se asigna la primera, sino la siguiente
    activeRoom = (i != rooms.end() ? (*i) : *rooms.begin());
    // Activa la nueva sala
    activeRoom->activate();
  }

  return activeRoom;
}

Room* MapManager::destroyAndSwapRoom()
{
  // Desactiva la sala actual
  activeRoom->deactivate();

  // Busca en el mapa los datos de la sala activa para borrar de la sala
  // los datos de entrada del jugador activo
  MapRoomData* activeRoomData = findRoomData(activeRoom->getIdentifier());
  activeRoomData->removePlayerPosition(activeRoomData->getActivePlayer());

  // Busca la nueva sala
  std::vector<Room*>::iterator i = std::find_if(rooms.begin(), rooms.end(), std::bind2nd(EqualRoom(), activeRoom->getIdentifier()));
  // Se pasa a la siguiente
  ++i;
  // Si se llegó al final se asigna la primera, sino la siguiente
  Room* inactiveRoom = activeRoom;
  activeRoom = (i != rooms.end() ? (*i) : *rooms.begin());

  // Si la sala inactiva es igual a la nueva sala activa quiere decir que sólo hay una
  // sala activa, por tanto sólo hay un jugador con vidas y las acaba de perder todas:
  // fin de la partida
  if(inactiveRoom == activeRoom)
  {
    activeRoom = 0;
  }

  // Elimina la sala inactiva
  rooms.erase(std::remove_if(rooms.begin(), rooms.end(), std::bind2nd(EqualRoom(), inactiveRoom->getIdentifier())), rooms.end());
  delete inactiveRoom;

  // Activa la nueva sala
  if(activeRoom != 0)
  {
    activeRoom->activate();
  }

  return activeRoom;
}

void MapManager::updateActivePlayer()
{
  // Busca en el mapa los datos de la sala activa
  MapRoomData* activeRoomData = findRoomData(activeRoom->getIdentifier());
  // Cambia el jugador
  activeRoomData->setActivePlayer(PlayerId(activeRoom->getMediator()->getActivePlayer()->getLabel()));
}

void MapManager::load(sgxml::exploredRooms::visited_sequence& visitedSequence)
{
  for(sgxml::exploredRooms::visited_const_iterator i = visitedSequence.begin(); i != visitedSequence.end(); ++i)
  {
    std::list<MapRoomData>::iterator m = std::find_if(mapData.begin(), mapData.end(), std::bind2nd(EqualMapRoomData(), (*i).filename()));
    if(m != mapData.end())
    {
      static_cast<MapRoomData*>(&(*m))->setVisited(true);
    }
  }
}

void MapManager::save(sgxml::exploredRooms::visited_sequence& visitedSequence)
{
  for(std::list<MapRoomData>::iterator i = this->mapData.begin(); i != this->mapData.end(); ++i)
  {
    if((*i).isVisited())
    {
      visitedSequence.push_back(sgxml::visited((*i).getRoom()));
    }
  }
}

unsigned short MapManager::countVisitedRooms()
{
  unsigned short number = 0;

  for(std::list<MapRoomData>::iterator i = this->mapData.begin(); i != this->mapData.end(); ++i)
  {
    number += (*i).isVisited() ? 1 : 0;
  }

  return number;
}

MapRoomData* MapManager::findRoomData(const std::string& room)
{
  std::list<MapRoomData>::iterator i = std::find_if(mapData.begin(), mapData.end(), std::bind2nd(EqualMapRoomData(), room));
  MapRoomData* data = (i != mapData.end() ? static_cast<MapRoomData*>(&(*i)) : 0);

  return data;
}

Room* MapManager::findRoom(const std::string& room)
{
  std::vector<Room*>::iterator i = std::find_if(rooms.begin(), rooms.end(), std::bind2nd(EqualRoom(), room));
  return (i != rooms.end() ? (*i) : 0);
}

PlayerStartPosition* MapManager::findPlayerPosition(const std::string& room, const PlayerId& playerId)
{
  return this->findRoomData(room)->findPlayerPosition(playerId);
}

Room* MapManager::getActiveRoom()
{
  return activeRoom;
}

Room* MapManager::getHideRoom()
{
  Room* room = 0;

  for(size_t i = 0; i < this->rooms.size(); ++i)
  {
    if(this->rooms[i] != this->activeRoom)
    {
      room = this->rooms[i];
    }
  }

  return room;
}

bool EqualMapRoomData::operator()(const MapRoomData& mapData, const std::string& room) const
{
  return (mapData.getRoom().compare(room) == 0);
}

bool EqualRoom::operator()(Room* room, const std::string& identifier) const
{
  return (room->getIdentifier().compare(identifier) == 0);
}

}
