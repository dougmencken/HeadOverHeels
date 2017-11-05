// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Picture_hpp_
#define Picture_hpp_

#include <allegro.h>
#include <string>

#include "Widget.hpp"


namespace gui
{

class Picture : public Widget
{

public:

        static BITMAP * cloneImage ( BITMAP * original ) ;

       /**
        * @param x horizontal offset
        * @param y vertical offset
        */
        Picture( int x, int y, BITMAP * image, std::string name ) ;

        virtual ~Picture( ) ;

       /**
        * Dibuja el elemento
        * @param where Imagen donde será dibujado
        */
        void draw ( BITMAP * where ) ;

        void handleKey ( int rawKey ) {  /* do nothing */  }

        BITMAP * getPicture () const {  return picture ;  }

        unsigned int getWidth () const {  return this->picture->w ;  }

        unsigned int getHeight () const {  return this->picture->h ;  }

private:

        BITMAP * picture ;

        std::string nameOfPicture ;

};

}

#endif
