// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CreateLanguageMenu_hpp_
#define CreateLanguageMenu_hpp_

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

        const char * getNameOfAction ()  {  return "CreateLanguageMenu" ;  }

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
