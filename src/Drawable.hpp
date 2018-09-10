// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
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

        Drawable() { }

        virtual ~Drawable() { }

        virtual void draw( allegro::Pict * where ) = 0;

};

#endif
