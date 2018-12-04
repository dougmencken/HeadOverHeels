// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef ShadowCaster_hpp_
#define ShadowCaster_hpp_

#include "Ism.hpp"
#include "Picture.hpp"
#include "Item.hpp"
#include "FloorTile.hpp"

namespace iso
{

/**
 * Shades graphics of item or floor tile with shadow of another item
 *
 * @param x Where is shading item on screen
 * @param y Where is shading item on screen
 * @param shadow Image of shadow to project on item
 * @param shading Degree of opacity from 0, without shadows, to 256, fully opaque shadows
 * @param transparency Degree of transparency of shading item
 */

class ShadowCaster
{

public:

        static void castShadowOnItem ( Item & item, int x, int y, const Picture & shadow, unsigned short shading, unsigned char transparency = 0 ) ;

        static void castShadowOnFloor ( FloorTile & tile, int x, int y, const Picture & shadow, unsigned short shading, unsigned char transparency = 0 ) ;

};

}

#endif
