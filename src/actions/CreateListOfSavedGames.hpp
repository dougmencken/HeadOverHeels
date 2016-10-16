// Head over Heels (A remake by helmántika.org)
//
// © Copyright 2008 Jorge Rodríguez Santos <jorge@helmantika.org>
// © Copyright 1987 Ocean Software Ltd. (Original game)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

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

};

}

#endif
