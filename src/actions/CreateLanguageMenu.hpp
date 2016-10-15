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

#ifndef CREATELANGUAGEMENU_HPP_
#define CREATELANGUAGEMENU_HPP_

#include <string>
#include <list>
#include <functional>
#include <allegro.h>
#include "Action.hpp"
#include "csxml/LanguageXML.hpp"

namespace gui
{

// Declaraciones previas
class LanguageText;

/**
 * Crea el menú de selección de idioma
 */
class CreateLanguageMenu : public Action
{

public:

        /**
         * Constructor
         * @param picture Imagen donde se dibujará la interfaz gráfica
         */
        CreateLanguageMenu( BITMAP* picture ) ;

        ~CreateLanguageMenu( ) ;

        /**
         * Show the language menu
         */
        void doIt () ;

private:

        /**
         * Analiza el archivo XML y extrae las cadenas ligándolas a un identificador textual
         * @param fileName Archivo XML con los textos empleados en el juego
         */
        void parse ( const std::string& fileName ) ;

private:

        /**
         * Idioma seleccinado en la configuración inicial
         */
        std::string language ;

        /**
         * Imagen donde se dibujará la interfaz gráfica
         */
        BITMAP* where ;

        /**
         * Textos empleados en la interfaz de usuario. Cada elemento se compone de un identificador
         * único, su posición en la pantalla y la cadena con el texto
         */
        std::list< LanguageText* > texts ;

};

}

#endif
