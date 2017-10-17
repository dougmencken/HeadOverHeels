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

#include "Ism.hpp"
#include "Way.hpp"


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
         */
        std::string findConnectedRoom ( const Way& wayOfExit, Way* wayOfEntry ) const ;

        /**
         * Comprueba que la entrada a la sala es correcta y la corrige si no lo es
         * Cuando se accede a una sala triple o cuádruple desde una simple o doble
         * la entrada no es correcta porque por la estructura del mapa se devuelve
         * un punto cardinal simple y las puertas de las salas triple o cuádruples
         * no están situadas en esos puntos cardinales
         * @param wayOfEntry Dirección de entrada a comprobar
         * @param previousRoom Nombre de la sala desde la cual se ha entrado a ésta
         */
        void adjustEntry ( Way* wayOfEntry, const std::string& previousRoom ) ;

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
         * File for room to the north of this room, empty string if no room there
         */
        std::string north ;

        /**
         * File for room to the south of this room, empty string if no room there
         */
        std::string south ;

        /**
         * File for room to the east of this room, empty string if no room there
         */
        std::string east ;

        /**
         * File for room to the west of this room, empty string if no room there
         */
        std::string west ;

        /**
         * File for room below this room, empty string if no room there
         */
        std::string floor ;

        /**
         * File for room above this room, empty string if no room there
         */
        std::string roof ;

        /**
         * File for room to teleport from this room, empty string when no room to teleport to
         */
        std::string teleport ;

        std::string teleport2 ;

        /**
         * File for room located at north-east for triple or quadruple rooms, or empty string
         */
        std::string northEast ;

        /**
         * File for room located at north-west for triple or quadruple rooms, or empty string
         */
        std::string northWest ;

        /**
         * File for room located at south-east for triple or quadruple rooms, or empty string
         */
        std::string southEast ;

        /**
         * File for room located at south-west for triple or quadruple rooms, or empty string
         */
        std::string southWest ;

        /**
         * File for room located at east-north for triple or quadruple rooms, or empty string
         */
        std::string eastNorth ;

        /**
         * File for room located at east-south for triple or quadruple rooms, or empty string
         */
        std::string eastSouth ;

        /**
         * File for room located at west-north for triple or quadruple rooms, or empty string
         */
        std::string westNorth ;

        /**
         * File for room located at west-south for triple or quadruple rooms, or empty string
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
