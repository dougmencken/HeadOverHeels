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

#ifdef __WIN32
  #include <winalleg.h>
#endif

#include <loadpng.h>


namespace allegro
{


typedef BITMAP Pict ;


/* int allegro_init()
 *
 * Macro to initialize Allegro library. Is the same as install_allegro( SYSTEM_AUTODETECT, &errno, atexit )
 */

void init ( ) ;


/* int set_gfx_mode( int card, int w, int h, int v_w, int v_h )
 *
 * Switches into graphics mode. Returns zero on success
 *
 * The v_w and v_h parameters specify the minimum virtual screen size, in case you need a large
 * virtual screen for hardware scrolling or page flipping. Set them to zero if you don't care
 * about size of virtual screen
 */

bool setGraphicsMode ( int magicCard, int w, int h ) ;


/* void blit( BITMAP* source, BITMAP* dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height )
 *
 * Copies a rectangular area of the source bitmap to the destination bitmap. The source_x and source_y
 * parameters are the top left corner of the area to copy from the source bitmap, and dest_x and dest_y
 * are the corresponding position in the destination bitmap
 */

void bitBlit( const Pict* from, Pict* to, int fromX, int fromY, int toX, int toY, unsigned int width, unsigned int height ) ;

void bitBlit( const Pict* from, Pict* to ) ;


/* void draw_sprite( BITMAP* view, BITMAP* sprite, int x, int y )
 *
 * Draws a copy of the sprite onto the view at the specified position. This is almost the same
 * as blit( sprite, view, 0, 0, x, y, sprite->w, sprite->h ), but it uses masked drawing
 * where transparent pixels are skipped
 */

void drawSprite ( Pict* view, const Pict* sprite, int x, int y ) ;


/* void stretch_blit( BITMAP* source, BITMAP* dest, int source_x, source_y, source_width, source_height, int dest_x, dest_y, dest_width, dest_height )
 *
 * Like blit(), except it can scale images (so the source and destination rectangles don’t need
 * to be the same size) and requires the source and destination bitmaps to be of the same color depth
 */

void stretchBlit ( const Pict* source, Pict* dest, int sX, int sY, int sW, int sH, int dX, int dY, int dW, int dH ) ;


/* BITMAP* create_bitmap( int width, int height )
 *
 * Creates a memory bitmap sized width by height. The bitmap will have clipping turned on,
 * and the clipping rectangle set to the full size of the bitmap. The image memory
 * will not be cleared, so it will probably contain garbage: you should clear the bitmap
 * before using it. This routine always uses the global pixel format, as specified
 * by calling set_color_depth()
 */

/* BITMAP* create_bitmap_ex( int color_depth, int width, int height )
 *
 * Creates a bitmap in a specific color depth (8, 15, 16, 24 or 32 bits per pixel). Returns
 * a pointer to the created bitmap, or NULL if the bitmap could not be created
 */

Pict* createPicture ( int width, int height, int depth = 32 ) ;


/* void destroy_bitmap( BITMAP* bitmap )
 *
 * Destroys any type of created bitmap when you are finished with it. Does nothing for nil pointer
 *
 * Wrapper’s note: Pict* as well as any other pointer itself is passed as a copy. To nullify
 * pointer beyond this function, you need to take a reference to it, Pict*&
 */

void binPicture ( Pict*& bitmap ) ;


/* int bitmap_color_depth( BITMAP* bitmap )
 *
 * Returns the color depth of the specified bitmap (8, 15, 16, 24, or 32)
 */

int colorDepthOf ( const Pict* picture ) ;


/* void clear_to_color( BITMAP* bitmap, int color )
 *
 * Clears bitmap to the specified color
 */

void clearToColor ( Pict* bitmap, int allegroColor ) ;


/* int getpixel( BITMAP* bmp, int x, int y )
 *
 * Reads a pixel from point (x, y) in the bitmap. To extract the individual color components,
 * use the getr() / getg() / getb() / geta() family of functions
 */

int getPixel( Pict* picture, int x, int y ) ;


/* void putpixel( BITMAP* bmp, int x, int y, int color )
 *
 * Writes a pixel to the specified position in the bitmap
 */

void putPixel( Pict* picture, int x, int y, int allegroColor ) ;


/* BITMAP* load_png( const char* filename, RGB* pal )
 *
 * Load a PNG from disk
 */

Pict* loadPNG( const char* file ) ;


}

#endif
