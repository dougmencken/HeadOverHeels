// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Widget_hpp_
#define Widget_hpp_

#include "WrappersAllegro.hpp"

#include "Drawable.hpp"

#include <utility>


namespace gui
{

/**
 * Foundation for creating elements of the user interface
 */

class Widget : public Drawable
{

public:

        Widget( ) ;

        Widget( int x, int y ) ;

        virtual ~Widget( ) ;

        virtual void draw ( allegro::Pict * where ) = 0 ;

        /**
         * Subclasses may respond to this event or notify other component
         */
        virtual void handleKey ( int key ) = 0 ;

        /**
         * Change position of the widget
         */
        virtual void moveTo ( int x, int y ) ;

private:

        /**
         * first es la coordenada X de pantalla donde situar el elemento
         * second es la coordenada Y de pantalla donde situar el elemento
         */
        std::pair < int, int > xy;

        bool onScreen ;

protected:

        void setX ( int x ) {  this->xy.first = x ;  }

        void setY ( int y ) {  this->xy.second = y ;  }

public:

        std::pair < int, int > getXY () const {  return this->xy ;  }

        int getX () const {  return this->xy.first ;  }

        int getY () const {  return this->xy.second ;  }

        bool isOnScreen() const {  return this->onScreen ;  }

        void setOnScreen( bool onscreen ) {  this->onScreen = onscreen ;  }

};

}

#endif
