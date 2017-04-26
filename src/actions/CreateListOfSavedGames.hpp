// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CreateListOfSavedGames_hpp_
#define CreateListOfSavedGames_hpp_

#include <sstream>
#include <allegro.h>
#include "Action.hpp"
#include "csxml/SaveGameXML.hpp"


namespace gui
{

/**
 * Crea el menú para seleccionar la partida a cargar
 */

class CreateListOfSavedGames : public gui::Action
{

public:

        /**
         * Constructor
         * @param picture Image where to draw
         * @param isLoadMenu true for "load game", false for "save game"
         */
        CreateListOfSavedGames( BITMAP* picture, bool isLoadMenu ) ;

        /**
         * Show the saved games
         */
        void doIt () ;

        std::string getNameOfAction ()  {  return "CreateListOfSavedGames" ;  }

        bool isLoadMenu () {  return this->isMenuForLoad ;  }

private:

        /**
         * Read some information from a file of saved game to display it
         * @param fileName Nombre del archivo XML que contiene los datos de la partida
         * @param rooms Devuelve el número de salas visitadas
         * @param planets Devuelve el número de planetas liberados
         */
        void readSomeInfoFromGamefile( const std::string& fileName, short* rooms, short* planets ) ;

private:

        BITMAP* where ;

        bool isMenuForLoad ;

        static const unsigned int howManySaves = 10 ;

};

}

#endif
