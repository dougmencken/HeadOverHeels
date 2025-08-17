// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Automap_hpp_
#define Automap_hpp_

#include "NamedMiniatures.hpp"

class Picture ;


class Automap
{

private :

        Picture * automapImage ;

        unsigned int sizeOfMiniatureTile ;

        NamedMiniatures miniatures ;

public :

        Automap( ) ;

        virtual ~Automap( ) ;

        void updateImage () ;

        void handleKeys () ;

        void drawMiniature () ;

        void drawMiniature ( int screenX, int screenY, unsigned int sizeOfTile ) ;

        Picture * getImage () {  return this->automapImage ;  }


} ;

#endif
