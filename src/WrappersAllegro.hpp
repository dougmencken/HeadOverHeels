// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef WrappersAllegro_hpp_
#define WrappersAllegro_hpp_

#include <allegro.h>


namespace allegro
{


/* int allegro_init()
 *
 * Macro to initialize Allegro library. Is the same as install_allegro( SYSTEM_AUTODETECT, &errno, atexit )
 */
void init ( /*~ const std::string& printMe = "" ~*/ ) ;


/* void draw_sprite( BITMAP* view, BITMAP* sprite, int x, int y )
 *
 * Draws a copy of the sprite onto the view at the specified position. This is almost the same
 * as blit( sprite, view, 0, 0, x, y, sprite->w, sprite->h ), but it uses masked drawing
 * where transparent pixels are skipped
 */
void drawSprite ( BITMAP* view, BITMAP* sprite, int x, int y /*~ const std::string& printMe = "" ~*/ ) ;


/* void destroy_bitmap( BITMAP* bitmap )
 *
 * Destroys any type of created bitmap when you are finished with it. Does nothing for nil pointer
 *
 * Wrapper’s note: BITMAP* as well as any other pointer itself is passed as a copy. To nullify
 * pointer beyond this function, you need to take a reference to it, BITMAP*&
 */
void destroyBitmap( BITMAP*& bitmap /*~ , const std::string& printMe = "" ~*/ ) ;


}

#endif
