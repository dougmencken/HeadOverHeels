// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Drawable_hpp_
#define Drawable_hpp_

#include "WrappersAllegro.hpp"

class Drawable
{

public:

        virtual void draw () = 0 ;

protected:

        virtual void drawOn ( const allegro::Pict & where )
        {
                if ( ! where.isNotNil() ) return ;

                const allegro::Pict& previousWhere = allegro::Pict::getWhereToDraw() ;
                allegro::Pict::setWhereToDraw( where );

                draw ();

                allegro::Pict::setWhereToDraw( previousWhere );
        }

};

#endif
