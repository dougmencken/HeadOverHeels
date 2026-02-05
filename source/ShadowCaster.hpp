// The free and open source remake of Head over Heels
//
// Copyright © 2026 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef ShadowCaster_hpp_
#define ShadowCaster_hpp_

#include "Picture.hpp"

class DescribedItem ;
class FloorTile ;


/**
 * Shades the graphics of an item or a floor tile with the shadow from another item
 *
 * @param x Where is the shading item on screen
 * @param y Where’s the shading item on screen
 * @param shadow The image of shadow to project on an item or a floor tile
 * @param shading The degree of opacity from 0, without shadows, to 256, fully opaque shadows
 * @param transparency The degree of transparency of the shading item
 */

class ShadowCaster
{

public:

        static void castShadowOnItem ( DescribedItem & item, int x, int y, const Picture & shadow, unsigned short shading, unsigned char transparency = 0 ) ;

        static void castShadowOnFloor ( FloorTile & tile, int x, int y, const Picture & shadow, unsigned short shading, unsigned char transparency = 0 ) ;

} ;

#endif
