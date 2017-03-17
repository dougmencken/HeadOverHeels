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
         * @param picture Imagen donde se dibujará la interfaz gráfica
         * @param isLoadMenu true para cargar partidas o false para grabarlas
         */
        CreateListOfSavedGames( BITMAP* picture, bool isLoadMenu ) ;

        /**
         * Show the saved games
         */
        void doIt () ;

        const char * getNameOfAction ()  {  return "CreateListOfSavedGames" ;  }

private:

        /**
         * Recupera la información esencial de un archivo para mostrarla en pantalla posteriormente
         * @param fileName Nombre del archivo XML que contiene los datos de la partida
         * @param rooms Devuelve el número de salas visitadas
         * @param planets Devuelve el número de planetas liberados
         */
        void recoverFileInfo( const std::string& fileName, short* rooms, short* planets ) ;

private:

        /**
         * Imagen donde se dibujará la interfaz gráfica
         */
        BITMAP* where ;

        /**
         * Indica si se presenta el menú para cargar (true) o grabar partidas (false)
         */
        bool loadMenu ;

        static const unsigned int howManySaves = 10 ;

};

}

#endif
