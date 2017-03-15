// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef FontManager_hpp_
#define FontManager_hpp_

#include <list>
#include <string>
#include <algorithm>
#include <functional>
#include <allegro.h>


namespace gui
{

class Font;

/**
 * Almacena las fuentes tipográficas usadas en la aplicación
 */

class FontManager
{

private:

        FontManager( ) ;

public:

        virtual ~FontManager( ) ;

       /**
        * Único objeto de esta clase para toda la aplicación
        * @return Un puntero a la clase madre para hacer uso del polimorfismo de clase
        */
        static FontManager* getInstance () ;

       /**
        * Crea una fuente tipográfica en función de los parámetros especificados
        * @param fontName Nombre de la fuente tipográfica empleado como identificador
        * @param fontFile Nombre del archivo de imagen que contienen la fuente tipográfica
        * @param color Color de los caracteres. El color se puede cambiar sólo si los caracteres son blancos
        * @param doubleSize Si vale true se doblará la altura de los caracteres
        */
        void createFont ( const std::string& fontName, const std::string& fontFile, int color, bool doubleSize = false ) ;

       /**
        * Busca una fuente tipográfica previamente creada
        * @param fontName Nombre de la fuente caligráfica
        * @param color Color de los caracteres de la fuente caligráfica
        * @return La fuente ó 0 si no existe
        */
        Font* findFont ( const std::string& font, const std::string& color ) ;

private:

       /**
        * Único objeto de esta clase para toda la aplicación
        */
        static FontManager * instance ;

       /**
        * Conjunto de fuentes disponibles
        */
        std::list < Font * > fonts ;

};

/**
 * Objeto-función usado como predicado en la búsqueda de una fuente tipográfica por su identificador
 */
class EqualFont : public std::binary_function < Font*, std::string, bool >
{

public:
        bool operator() ( const Font* font, const std::string& fontName ) const;

};

}

#endif
