// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef LanguageManager_hpp_
#define LanguageManager_hpp_

#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <iostream>
#include "csxml/LanguageXML.hpp"
#include "Gui.hpp"


namespace gui
{

class LanguageText;

/**
 * Lee las cadenas de texto usadas en el juego de un archivo XML y las almacena
 * en una estructura de datos a la que acceden otros objetos de la interfaz de usuario
 */

class LanguageManager
{

public:

       /**
        * Constructor
        * @param fileName XML file with localized strings used in the game
        * @param fileWithGuaranteedStrings The file with more strings than in fileName
        */
        LanguageManager( const std::string & fileName, const std::string& fileWithGuaranteedStrings ) ;

        ~LanguageManager( );

        /**
         * Busca una cadena de idioma
         * @param id Cadena identificativa
         * @return La cadena si se encontró ó 0 en caso contrario
         */
        LanguageText * findLanguageString ( const std::string & id ) ;

private:

       /**
        * Analiza el archivo XML y extrae las cadenas ligándolas a un identificador textual
        * @param fileName XML file with localized strings used in the game
        * @param fileWithGuaranteedStrings The file with more strings than in fileName
        */
        void parse ( const std::string & fileName, const std::string& fileWithGuaranteedStrings ) ;

private:

       /**
        * Textos empleados en la interfaz de usuario. Cada elemento se compone de un identificador
        * único, su posición en la pantalla y la cadena con el texto
        */
        std::list< LanguageText * > texts ;

        std::list< LanguageText * > backupTexts ;

};

struct LanguageLine
{

        std::string text ;

        std::string font ;

        std::string color ;

};

class LanguageText
{

public:

        LanguageText( const std::string& id ) ;

        LanguageText( const std::string& id, unsigned int x, unsigned int y ) ;

private:

        std::string id ;

        unsigned int x ;

        unsigned int y ;

        std::vector< LanguageLine > lines ;

public:

        void addLine ( const std::string& text ) ;

        void addLine ( const std::string& text, const std::string& font, const std::string& color ) ;

        std::string getId () const {  return this->id ;  }

        unsigned int getX () const {  return this->x ;  }

        unsigned int getY () const {  return this->y ;  }

        std::string getFirstLineText () const {  return this->lines[ 0 ].text ;  }

        std::string getFirstLineFont () const {  return this->lines[ 0 ].font ;  }

        std::string getFirstLineColor () const {  return this->lines[ 0 ].color ;  }

        std::string getText () const {  return this->getFirstLineText() ;  }

        std::string getFont () const {  return this->getFirstLineFont() ;  }

        std::string getColor () const {  return this->getFirstLineColor() ;  }

        LanguageLine* getFirstLine () {  return &( this->lines[ 0 ] ) ;  }

        LanguageLine* getLine ( size_t number ) {  return &( this->lines[ number ] ) ;  }

        size_t getLinesCount () {  return this->lines.size() ;  }

};

/**
 * Objeto-función usado como predicado en la búsqueda de una cadena de idioma
 */
struct EqualLanguageString : public std::binary_function< LanguageText *, std::string, bool >
{
        bool operator() ( const LanguageText* lang, const std::string& id ) const;
};

}

#endif
