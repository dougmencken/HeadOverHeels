// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Label_hpp_
#define Label_hpp_

#include <string>
#include <cassert>
#include <allegro.h>
#include "Widget.hpp"

namespace gui
{

class Screen;
class Font;
class Action;

/**
 * Un texto fijo presente en la pantalla
 */
class Label : public Widget
{

public:

        /**
         * Constructor. La posición de la etiqueta se establece en ( 0, 0 ), se usa la fuente
         * por defecto y el texto es de color blanco
         * @param text El texto a presentar
         */
        Label( const std::string& text ) ;

        /**
         * Constructor. Se usa la fuente por defecto y el texto es de color blanco
         * @param x Coordenada X de pantalla donde situar el elemento
         * @param y Coordenada Y de pantalla donde situar el elemento
         * @param text El texto a presentar
         */
        Label( unsigned int x, unsigned int y, const std::string& text ) ;

        /**
         * Constructor
         * @param text El texto a presentar
         * @param fontName Nombre de la fuente caligráfica usada para representar el texto
         * @param color Color del texto
         * @param spacing Espaciado entre caracteres, suma o resta tantos píxeles como se indiquen
         */
        Label( const std::string& text, const std::string& fontName, const std::string& color, int spacing = 0 ) ;

        /**
         * Constructor
         * @param x Coordenada X de pantalla donde situar el elemento
         * @param y Coordenada Y de pantalla donde situar el elemento
         * @param text El texto a presentar
         * @param fontName Nombre de la fuente caligráfica usada para representar el texto
         * @param color Color del texto
         * @param spacing Espaciado entre caracteres, suma o resta tantos píxeles como se indiquen
         */
        Label( unsigned int x, unsigned int y, const std::string& text, const std::string& fontName, const std::string& color, int spacing = 0 ) ;

        virtual ~Label( ) ;

        /**
         * Cambia el tipo de letra del texto
         * @param fontName Nombre de la fuente caligráfica
         * @param color Color del texto
         */
        void changeFontAndColor ( const std::string& fontName, const std::string& color ) ;

        /**
         * Dibuja el elemento
         * @param where Imagen donde será dibujado
         */
        void draw ( BITMAP* where ) ;

        /**
         * Responde a la pulsación de una tecla
         */
        void handleKey ( int key ) ;

private:

        /**
         * Crea la etiqueta sobre una imagen descodificando una cadena UTF-8
         * @param text Una cadena de caracteres UTF-8
         * @param font Fuente caligráfica empleada para dibujar el texto
         * @param color Color del texto
         * @param spacing Espaciado entre caracteres, suma o resta tantos píxeles como se indiquen
         * @return La imagen con fondo magenta (el color clave) y el texto
         */
        BITMAP* createBitmapLabel ( const std::string& text, const std::string& fontName, const std::string& color, int spacing = 0 ) ;

protected:

        /**
         * El texto de la etiqueta
         */
        std::string text ;

        /**
         * Nombre de la fuente caligráfica de la etiqueta
         */
        std::string fontName ;

        /**
         * Color del texto
         */
        std::string color ;

        /**
         * Fuente caligráfica de un color específico empleada para representar el texto
         */
        Font* font ;

        /**
         * El texto de una etiqueta en pantalla se compone de muchas imágenes, tantos como
         * caracteres tenga. Esta imagen almacena la composición para evitar volverla a
         * realizar si el texto no cambia
         */
        BITMAP* buffer ;

        Action* myAction ;

public: // Operaciones de consulta y actualización

        /**
         * Devuelve el texto de la etiqueta
         * @param Una cadena de caracteres
         */
        std::string getText () const {  return this->text ;  }

        void setText( const std::string& str ) {  this->text = str ;  }

        /**
         * Devuelve la fuente tipográfica usada en la etiqueta
         * @param Un tipo de letra
         */
        std::string getFontName () const {  return this->fontName ;  }

        /**
         * Devuelve el color del texto
         * @param Un color
         */
        std::string getColor () const {  return this->color ;  }

        /**
         * Fuente caligráfica de un color específico empleada para representar el texto
         */
        Font* getFont () const {  return this->font ;  }

        /**
         * Devuelve la anchura de la etiqueta
         */
        int getWidth () const {  return this->buffer->w ;  }

        /**
         * Devuelve la altura de la etiqueta
         */
        int getHeight () const {  return this->buffer->h ;  }

        Action* getAction ( ) const {  return myAction ;  }

        void setAction ( Action* action ) {  myAction = action ;  }

};


struct EqualXYOfLabel : public std::binary_function< Label*, std::pair < unsigned int, unsigned int >, bool >
{
        bool operator() ( const Label* label, std::pair < unsigned int, unsigned int > thatXY ) const;
};


/**
 * Objeto-función usado como predicado en la búsqueda de una etiqueta
 */
struct EqualTextOfLabel : public std::binary_function< Label*, std::string, bool >
{
        bool operator() ( const Label* label, const std::string& text ) const;
};

}

#endif
