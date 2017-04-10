// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Widget_hpp_
#define Widget_hpp_

#include <allegro.h>

#include <utility>

namespace gui
{

/**
 * Base para la creación de los elementos existentes en la interfaz gráfica de usuario
 * Cada nuevo elemento debe recodificar la operación 'draw'
 */
class Widget
{

public:

        /**
         * Constructor
         * @param x Coordenada X de pantalla donde situar el elemento
         * @param x Coordenada X de pantalla donde situar el elemento
         */
        Widget( int x, int y ) ;

        virtual ~Widget( ) ;

        /**
         * Dibuja el elemento
         */
        virtual void draw ( BITMAP* where ) = 0 ;

        /**
         * Recibe la pulsación de una tecla. Las subclases pueden responder a dicho evento
         * o comunicárselo al siguiente componente
         */
        virtual void handleKey ( int key ) = 0 ;

        /**
         * Cambia la posición del elemento
         * @param x Coordenada X de pantalla donde situar el elemento
         * @param y Coordenada Y de pantalla donde situar el elemento
         */
        virtual void changePosition ( int x, int y ) ;

private:

        /**
         * first es la coordenada X de pantalla donde situar el elemento
         * second es la coordenada Y de pantalla donde situar el elemento
         */
        std::pair < int, int > xy;

protected:

        void setX ( int x ) {  this->xy.first = x ;  }

        void setY ( int y ) {  this->xy.second = y ;  }

public: // Operaciones de consulta y actualización

        std::pair < int, int > getXY () const {  return this->xy ;  }

        int getX () const {  return this->xy.first ;  }

        int getY () const {  return this->xy.second ;  }

};

}

#endif
