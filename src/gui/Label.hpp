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

#include "Gui.hpp"
#include "Widget.hpp"
#include "Font.hpp"


namespace gui
{

class Screen ;
class Action ;

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
        Label( int x, int y, const std::string& text ) ;

        /**
         * Constructor
         * @param text El texto a presentar
         * @param fontName Nombre de la fuente caligráfica usada para representar el texto
         * @param color Color del texto
         * @param spacing Space between characters
         */
        Label( const std::string& text, const std::string& fontName, const std::string& color, int spacing = 0 ) ;

        /**
         * Constructor
         * @param x Coordenada X de pantalla donde situar el elemento
         * @param y Coordenada Y de pantalla donde situar el elemento
         * @param text El texto a presentar
         * @param fontName Nombre de la fuente caligráfica usada para representar el texto
         * @param color Color del texto
         * @param spacing Space between characters
         */
        Label( int x, int y, const std::string& text, const std::string& fontName, const std::string& color, int spacing = 0 ) ;

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

protected:

        /**
         * Create image of label
         * @param text A string of characters in utf-8
         * @param font Font to draw these characters
         * @param color Color of text
         * @return The image with magenta (key color) background and the text
         */
        BITMAP * createBitmapOfLabel ( const std::string& text, const std::string& fontName, const std::string& color ) ;

private:

        std::string text ;

        std::string fontName ;

        std::string color ;

        Font* font ;

        int spacing ;

        /**
         * Image made as composition of characters
         */
        BITMAP* buffer ;

        Action* myAction ;

public:

        std::string getText () const {  return this->text ;  }

        void setText( const std::string& str ) {  this->text = str ;  }

        std::string getFontName () const {  return this->fontName ;  }

        std::string getColor () const {  return this->color ;  }

        Font* getFont () const {  return this->font ;  }

        int getSpacing () const {  return this->spacing ;  }

        void setSpacing ( int newSpacing ) {  this->spacing = newSpacing ;  }

        unsigned int getWidth () const
        {
                // symbols of game fonts are monospaced
                return utf8StringLength( text ) * ( font->getCharWidth() + spacing ) ;
                /* return this->buffer->w ; */
        }

        unsigned int getHeight () const {  return font->getCharHeight() ; /* return this->buffer->h ; */  }

        Action* getAction ( ) const {  return myAction ;  }

        void setAction ( Action* action ) {  myAction = action ;  }

};


struct EqualXYOfLabel : public std::binary_function< Label*, std::pair < int, int >, bool >
{
        bool operator() ( const Label* label, std::pair < int, int > thatXY ) const;
};


struct EqualTextOfLabel : public std::binary_function< Label*, std::string, bool >
{
        bool operator() ( const Label* label, const std::string& text ) const;
};

}

#endif
