// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef MapRoomData_hpp_
#define MapRoomData_hpp_

#include <string>
#include <list>
#include <algorithm>
#include <functional>
#include "Ism.hpp"

namespace isomot
{

class PlayerStartPosition;

/**
 * Registro que almacena la información de una sala del mapa leída de un archivo del disco
 */
class MapRoomData
{

public:

        /**
         * Constructor
         * @param Nombre completo del archivo que contiene los datos de esta sala
         */
        MapRoomData( const std::string& room ) ;

        virtual ~MapRoomData( ) ;

        /**
         * Asigna la posición inicial de un jugador en la sala
         * @param playerPosition Datos de posición
         * @param playerPresent Indica si ya hay un jugador presente en esta sala. En tal caso puede
         * darse la circunstancia de que la vía de entrada de ambos jugadores sea la misma. Para solventar
         * este conflicto se cambian los datos del jugador presente por sus datos de posición actuales
         */
        void addPlayerPosition ( const PlayerStartPosition& playerPosition ) ;

        /**
         * Asigna la posición inicial de un jugador en la sala
         * @param playerPosition Datos de posición
         * @param playerPresentPosition Datos de un jugador que ya está presente en esta sala. Puede
         * darse la circunstancia de que la vía de entrada de ambos jugadores sea la misma. Para solventar
         * este conflicto se cambian los datos del jugador presente por sus datos de posición actuales
         */
        void addPlayerPosition ( PlayerStartPosition& playerPosition, PlayerStartPosition& playerPresentPosition ) ;

        /**
         * Elimina la posición inicial de un jugador de la sala
         * @param player Identificador del jugador
         */
        void removePlayerPosition ( const PlayerId& player ) ;

        /**
         * Busca la posición inicial de un jugador en la sala
         * @param player Identificador del jugador
         * @return Datos con la posición inicial del jugador ó 0 si no existen datos para el jugador
         */
        PlayerStartPosition * findPlayerPosition ( const PlayerId& player ) ;

        /**
         * Indica si hay jugadores en la sala
         * @return true si quedan jugadores o false en caso contrario
         */
        bool remainPlayers () ;

        /**
         * Busca una sala conectada con ésta
         * @param exit Salida de la sala actual que comunica con la sala destino
         * @param entry Devuelve la entrada por la que se accederá a la sala
         * @return El nombre del archivo de la sala destino o una cadena vacía si
         * la salida no conduce a alguna parte
         */
        std::string findConnectedRoom ( const Direction& exit, Direction* entry ) const ;

        /**
         * Comprueba que la entrada a la sala es correcta y la corrige si no lo es
         * Cuando se accede a una sala triple o cuádruple desde una simple o doble
         * la entrada no es correcta porque por la estructura del mapa se devuelve
         * un punto cardinal simple y las puertas de las salas triple o cuádruples
         * no están situadas en esos puntos cardinales
         * @param entry Dirección de entrada a comprobar
         * @param previousRoom Nombre de la sala desde la cual se ha entrado a ésta
         */
        void adjustEntry ( Direction* entry, const std::string& previousRoom ) ;

        /**
         * Elimina la posición inicial de los jugadores en la sala
         */
        void clearPlayersPosition () ;

        /**
         * Reestablece los datos de la sala para una nueva partida
         */
        void reset () ;

private:

        /**
         * Nombre completo del archivo que contiene los datos de esta sala
         */
        std::string room ;

        /**
         * Indica si la sala ha sido visitada por alguno de los jugadores
         */
        bool visited ;

        /**
         * El jugador activo, el que controla el usuario, cuando se crea la sala
         */
        PlayerId activePlayer ;

        /**
         * Posición inicial de los jugadores que pudieran estar presentes en la sala cuando ésta se crea
         */
        std::list < PlayerStartPosition > playersPosition ;

        /**
         * Nombre completo del archivo de la sala situada al norte
         * Una cadena vacía indica que no hay sala en esa dirección
         */
        std::string north ;

        /**
         * Nombre completo del archivo de la sala situada al sur
         * Una cadena vacía indica que no hay sala en esa dirección
         */
        std::string south ;

        /**
         * Nombre completo del archivo de la sala situada al este
         * Una cadena vacía indica que no hay sala en esa dirección
         */
        std::string east ;

        /**
         * Nombre completo del archivo de la sala situada al oeste
         * Una cadena vacía indica que no hay sala en esa dirección
         */
        std::string west ;

        /**
         * Nombre completo del archivo de la sala situada debajo
         * La sala no puede tener suelo
         * Una cadena vacía indica que no hay sala en esa dirección
         */
        std::string floor ;

        /**
         * Nombre completo del archivo de la sala situada encima
         * La sala superior no puede tener suelo
         * Una cadena vacía indica que no hay sala en esa dirección
         */
        std::string roof ;

        /**
         * Nombre completo del archivo de la sala a la que lleva el telepuerto
         * La sala destino debe tener un telepuerto en la misma posición que la actual
         * Una cadena vacía indica que no hay sala en esa dirección
         */
        std::string teleport ;

        /**
         * Nombre completo del archivo de la sala a la que lleva el segundo telepuerto
         * La sala destino debe tener un telepuerto en la misma posición que la actual
         * Una cadena vacía indica que no hay sala en esa dirección
         */
        std::string teleport2 ;

        /**
         * Nombre completo del archivo de la sala situada en la posición norte-este
         * Sólo tiene sentido en sala triples o cuádruples. Una cadena vacía indica que no hay sala en esa dirección
         */
        std::string northEast ;

        /**
         * Nombre completo del archivo de la sala situada en la posición norte-oeste
         * Sólo tiene sentido en sala triples o cuádruples. Una cadena vacía indica que no hay sala en esa dirección
         */
        std::string northWest ;

        /**
         * Nombre completo del archivo de la sala situada en la posición sur-este
         * Sólo tiene sentido en sala triples o cuádruples. Una cadena vacía indica que no hay sala en esa dirección
         */
        std::string southEast ;

        /**
         * Nombre completo del archivo de la sala situada en la posición sur-oeste
         * Sólo tiene sentido en sala triples o cuádruples. Una cadena vacía indica que no hay sala en esa dirección
         */
        std::string southWest ;

        /**
         * Nombre completo del archivo de la sala situada en la posición este-norte
         * Sólo tiene sentido en sala triples o cuádruples. Una cadena vacía indica que no hay sala en esa dirección
         */
        std::string eastNorth ;

        /**
         * Nombre completo del archivo de la sala situada en la posición este-sur
         * Sólo tiene sentido en sala triples o cuádruples. Una cadena vacía indica que no hay sala en esa dirección
         */
        std::string eastSouth ;

        /**
         * Nombre completo del archivo de la sala situada en la posición oeste-norte
         * Sólo tiene sentido en sala triples o cuádruples. Una cadena vacía indica que no hay sala en esa dirección
         */
        std::string westNorth ;

        /**
         * Nombre completo del archivo de la sala situada en la posición oeste-sur
         * Sólo tiene sentido en sala triples o cuádruples. Una cadena vacía indica que no hay sala en esa dirección
         */
        std::string westSouth ;

public: // Operaciones de consulta y actualización

        /**
         * Nombre completo del archivo que contiene los datos de esta sala
         * @return Una cadena con el nombre del archivo XML, extensión incluida
         */
        std::string getRoom () const {  return this->room ;  }

        /**
         * Establece si la sala ha sido visitada por alguno de los jugadores
         * @param true si ha sido visitada o false en caso contrario
         */
        void setVisited ( bool visited ) {  this->visited = visited ;  }

        /**
         * Indica si la sala ha sido visitada por alguno de los jugadores
         */
        bool isVisited () const {  return this->visited ;  }

        /**
         * Establece el jugador activo, el que controla el usuario, cuando se crea la sala
         */
        void setActivePlayer ( const PlayerId& activePlayer ) {  this->activePlayer = activePlayer ;  }

        /**
         * El jugador activo, el que controla el usuario, cuando se crea la sala
         * @return El identificador de un jugador
         */
        PlayerId getActivePlayer () const {  return activePlayer ;  }

        /**
         * Posición inicial de los jugadores que pudieran estar presentes en la sala cuando ésta se crea
         * @return Una lista con las posiciones iniciales de los jugadores
         */
        std::list < PlayerStartPosition > & getPlayersPosition () {  return playersPosition ;  }

        /**
         * Establece la sala situada al norte
         * @param room El nombre completo del archivo de la sala
         */
        void setNorth ( const std::string& room ) {  this->north = room ;  }

        /**
         * Establece la sala situada al sur
         * @param room El nombre completo del archivo de la sala
         */
        void setSouth ( const std::string& room ) {  this->south = room ;  }

        /**
         * Establece la sala situada al este
         * @param room El nombre completo del archivo de la sala
         */
        void setEast ( const std::string& room ) {  this->east = room ;  }

        /**
         * Establece la sala situada al oeste
         * @param room El nombre completo del archivo de la sala
         */
        void setWest ( const std::string& room ) {  this->west = room ;  }

        /**
         * Establece la sala situada debajo. La sala no puede tener suelo
         * @param room El nombre completo del archivo de la sala
         */
        void setFloor ( const std::string& room ) {  this->floor = room ;  }

        /**
         * Establece la sala situada encima. Dicha sala no puede tener suelo
         * @param room El nombre completo del archivo de la sala
         */
        void setRoof ( const std::string& room ) {  this->roof = room ;  }

        /**
         * Establece la sala a la que conduce el telepuerto. La sala destino debe
         * tener un telepuerto en la misma posición que la actual
         * @param room El nombre completo del archivo de la sala
         */
        void setTeleport ( const std::string& room ) {  this->teleport = room ;  }

        /**
         * Establece la sala a la que conduce el segundo telepuerto. La sala destino debe
         * tener un telepuerto en la misma posición que la actual
         * @param room El nombre completo del archivo de la sala
         */
        void setTeleportToo ( const std::string& room ) {  this->teleport2 = room ;  }

        /**
         * Establece la sala situada en la posición norte-este
         * @param room El nombre completo del archivo de la sala
         */
        void setNorthEast ( const std::string& room ) {  this->northEast = room ;  }

        /**
         * Establece la sala situada en la posición norte-oeste
         * @param room El nombre completo del archivo de la sala
         */
        void setNorthWest ( const std::string& room ) {  this->northWest = room ;  }

        /**
         * Establece la sala situada en la posición sur-este
         * @param room El nombre completo del archivo de la sala
         */
        void setSouthEast ( const std::string& room ) {  this->southEast = room ;  }

        /**
         * Establece la sala situada en la posición sur-oeste
         * @param room El nombre completo del archivo de la sala
         */
        void setSouthWest ( const std::string& room ) {  this->southWest = room ;  }

        /**
         * Establece la sala situada en la posición este-norte
         * @param room El nombre completo del archivo de la sala
         */
        void setEastNorth ( const std::string& room ) {  this->eastNorth = room ;  }

        /**
         * Establece la sala situada en la posición este-sur
         * @param room El nombre completo del archivo de la sala
         */
        void setEastSouth ( const std::string& room ) {  this->eastSouth = room ;  }

        /**
         * Establece la sala situada en la posición oeste-norte
         * @param room El nombre completo del archivo de la sala
         */
        void setWestNorth ( const std::string& room ) {  this->westNorth = room ;  }

        /**
         * Establece la sala situada en la posición oeste-sur
         * @param room El nombre completo del archivo de la sala
         */
        void setWestSouth ( const std::string& room ) {  this->westSouth = room ;  }

};


/**
 * Posición inicial de un jugador cuando se crea la sala
 */
class PlayerStartPosition
{

public:

        /**
         * Constructor
         * @param player Identificador del jugador
         */
        PlayerStartPosition( const PlayerId& player ) ;

        virtual ~PlayerStartPosition( ) ;

        /**
         * Establece la puerta por la que entra el jugador a la sala
         * @param door Identificador de la posición de una puerta
         */
        void assignDoor ( const Direction& door ) ;

        /**
         * Establece la posición espacial por la que entra el jugador a la sala
         * @param entry Entrada por el suelo, por el techo o por un telepuerto
         * @param x Coordenada X donde aparece el jugador
         * @param y Coordenada Y donde aparece el jugador
         * @param z Coordenada Z donde aparece el jugador
         * @param orientation Orientación del jugador
         */
        void assignPosition ( const Direction& entry, int x, int y, int z, const Direction& orientation ) ;

private:

        /**
         * Jugador que está presente en la sala cuando la sala se crea
         */
        PlayerId player ;

        /**
         * Camino de entrada del jugador: puerta, telepuerto, por el suelo o por el techo
         */
        Direction entry ;

        /**
         * Coordenada X del jugador si no entra por una puerta
         */
        int x ;

        /**
         * Coordenada Y del jugador si no entra por una puerta
         */
        int y ;

        /**
         * Coordenada Z del jugador si no entra por una puerta
         */
        int z ;

        /**
         * Orientación del jugador si no entra por una puerta
         */
        Direction orientation ;

public: // Operaciones de consulta y actualización

        /**
         * Jugador que está presente en la sala cuando la sala se crea
         */
        PlayerId getPlayer () const {  return player ;  }

        /**
         * Camino de entrada del jugador: puerta, telepuerto, por el suelo o por el techo
         */
        Direction getEntry () {  return entry ;  }

        /**
         * Coordenada X del jugador si no entra por una puerta
         */
        int getX () const {  return x ;  }

        /**
         * Coordenada Y del jugador si no entra por una puerta
         */
        int getY () const {  return y ;  }

        /**
         * Coordenada Z del jugador si no entra por una puerta
         */
        int getZ () const {  return z ;  }

        /**
         * Orientación del jugador si no entra por una puerta
         */
        Direction getOrientation () const {  return orientation ;  }

};

/**
 * Objeto-función usado como predicado en la búsqueda de la posición inicial de un jugador
 */
struct EqualPlayerStartPosition : public std::binary_function< PlayerStartPosition, PlayerId, bool >
{
        bool operator() ( const PlayerStartPosition& position, const PlayerId& player ) const ;
};

}

#endif
