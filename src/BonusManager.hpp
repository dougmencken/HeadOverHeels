// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef BonusManager_hpp_
#define BonusManager_hpp_

#include <list>
#include <string>
#include <algorithm>
#include "csxml/SaveGameXML.hpp"


namespace isomot
{

class BonusItem ;
class BonusRoom ;


/**
 * Manage the presence or absence of bonuses in rooms. Bonuses are elements
 * that provide lives and other powerups to players, in addition to the crowns
 * that are key elements of the game. When a bonus is taken from a particular room,
 * it wouldn’t appear again in the same game
 */

class BonusManager
{

private:

        BonusManager( ) ;

public:

        virtual ~BonusManager( ) ;

       /**
        * Operación de acceso al único objeto de la clase
        * @return Un objeto de esta clase
        */
        static BonusManager * getInstance () ;

       /**
        * Marca en el mapa del juego como ausente el bonus especificado
        * @param fileName Nombre del archivo de la sala donde estaba el bonus
        * @param label Etiqueta del bonus
        */
        void markBonusAsAbsent ( const std::string& fileName, const std::string& label ) ;

       /**
        * Indica si un bonus determinado sigue estando en esta sala
        * @param fileName Nombre del archivo de la sala donde está el bonus
        * @param label Etiqueta del bonus
        */
        bool isPresent ( const std::string& fileName, const std::string& label ) ;

       /**
        * Carga a partir de los datos proporcionados, que han sido extraidos del archivo que
        * guarda la partida en curso, los bonus ausentes en la partida
        * @param roomSequence Estructura de datos empleada por el archivo XML para guardar
        * los datos de los bonus ausentes
        */
        void load ( const sgxml::bonus::room_sequence& roomSequence ) ;

       /**
        * Almacena los bonus ausentes en el archivo que guarda la partida en curso
        * @param roomSequence Estructura de datos empleada por el archivo XML para guardar
        * los datos de los bonus ausentes
        */
        void save ( sgxml::bonus::room_sequence& roomSequence ) ;

       /**
        * Elimina los bonus marcados como ausentes en todas las salas
        */
        void reset () ;

private:

       /**
        * El único objeto de esta clase
        */
        static BonusManager * instance ;

       /**
        * Lista de las salas con bonus ausentes
        */
        std::list < BonusRoom > bonusRooms ;

};


/**
 * Bonus may be absent in the room due to being picked up
 * BonusRoom helps to avoid two equal bonuses in the same room
 */

class BonusRoom
{

public:

       /**
        * Constructor
        * @param name Nombre del archivo de la sala
        */
        BonusRoom( const std::string& name ) ;

        virtual ~BonusRoom( ) ;

       /**
        * Almacena un bonus para no volverse a mostrar nunca en esta sala
        * @param label Etiqueta del bonus
        */
        void markBonusAsAbsent ( const std::string& label ) ;

       /**
        * Indica si un bonus determinado sigue estando en esta sala
        * @param label Etiqueta del bonus
        */
        bool isPresent ( const std::string& label ) ;

       /**
        * Operador de igualdad
        * @return true si el nombre de archivo de la sala de ambos elementos es igual
        */
        bool operator== ( const std::string & fileName ) ;

private:

       /**
        * Nombre del archivo de la sala que hace las veces de identificador
        */
        std::string nameOfFile ;

       /**
        * Bonuses already taken in this room
        */
        std::list < std::string > bonusItems ;

public:

       /**
        * Nombre del archivo de la sala que hace las veces de identificador
        */
        std::string getNameOfFile () const {  return this->nameOfFile ;  }

       /**
        * Bonus que no deben aparecer en la sala
        * @return Una referencia a la lista
        */
        std::list < std::string >& getBonusItems () {  return this->bonusItems ;  }

};

}

#endif
