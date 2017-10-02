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

/**
 * Container for information of a map room read from a file with data for this room
 */

class MapRoomData
{

public:

        /**
         * Constructor
         * @param Full path and name of file with data for this room
         */
        MapRoomData( const std::string& room ) ;

        virtual ~MapRoomData( ) ;

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

public:

        /**
         * Name of the file with data for this room
         */
        std::string getNameOfRoomFile () const {  return this->room ;  }

        void setVisited ( bool visited ) {  this->visited = visited ;  }

        bool isVisited () const {  return this->visited ;  }

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

}

#endif
