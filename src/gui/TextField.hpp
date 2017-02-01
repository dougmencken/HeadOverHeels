// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef TextField_hpp_
#define TextField_hpp_

#include <list>
#include <algorithm>
#include <functional>
#include <string>
#include <memory>

#include <allegro.h>

#include "Widget.hpp"


namespace gui
{

// Declaraciones adelantadas
class Label;

enum Alignment
{
        LeftAlignment,
        CenterAlignment,
        RightAlignment
};

/**
 * Almacena texto en varias líneas que puede alinearse a la izquierda, centro y derecha
 */
class TextField : public Widget
{

public:

        /**
        * Constructor
        * @param x Coordenada X de pantalla donde situar el elemento
        * @param x Coordenada X de pantalla donde situar el elemento
        * @param width Anchura del campo de texto
        * @param height Altura del campo de texto
        * @param alignment Alineación horizontal del texto: izquierda, centro o derecha
        */
        TextField( unsigned int x, unsigned int y, unsigned int width, unsigned int height, const Alignment& alignment ) ;

        virtual ~TextField( ) ;

        /**
        * Dibuja el elemento
        */
        virtual void draw ( BITMAP * where ) ;

        /**
        * Añade una nueva línea al campo de texto
        */
        void addLine ( const std::string& text, const std::string& font, const std::string& color ) ;

        /**
        * Cambia la posición del elemento
        * @param x Coordenada X de pantalla donde situar el elemento
        * @param y Coordenada Y de pantalla donde situar el elemento
        */
        void changePosition ( unsigned int x, unsigned int y ) ;

private:

        /**
        * Anchura del campo de texto
        */
        unsigned int width ;

        /**
        * Altura del campo de texto
        */
        unsigned int height ;

        /**
        * Alineación horizontal del texto: izquierda, centro o derecha
        */
        Alignment alignment ;

        /**
        * Distancia de interlineado respecto a la última línea añadida
        */
        unsigned int delta ;

        /**
        * El campo de texto se compone de etiquetas
        */
        std::list < Label * > lines ;

};

}

#endif
