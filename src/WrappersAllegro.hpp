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

#include <cstddef>
#include <string>
#include <vector>
#include <map>


#if defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

struct BITMAP ;
struct SAMPLE ;

struct alogg_stream ;
struct alogg_thread ;

#endif


namespace allegro
{

#if defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

class Pict
{

public:

        virtual ~Pict( ) ;

        unsigned int getW () const ;
        unsigned int getH () const ;

        int getPixelAt ( int x, int y ) const ;
        void setPixelAt ( int x, int y, int allegroColor ) const ;

        /* Returns the color depth of picture
         * 8, 15, 16, 24, or 32 */
        unsigned int getColorDepth () const ;

        void clearToColor ( int allegroColor ) const ;

        bool isInVideoMemory () const ;

        bool isNotNil () const {  return it != NULL ;  }

        BITMAP* ptr () const {  return it ;  } // instead of just operator BITMAP*() const {  return it ;  }

        bool equals ( const Pict& toWhat ) const {  return it == toWhat.it ;  }

        static Pict* nilPict () {  return nil ;  }

        static Pict* newPict ( BITMAP* b ) {  return new Pict( b ) ;  }
        static Pict* newPict ( unsigned int w, unsigned int h ) {  return new Pict( w, h ) ;  }
        static Pict* newPict ( unsigned int w, unsigned int h, unsigned int depth ) {  return new Pict( w, h, depth ) ;  }

        static const Pict& theScreen ();

private:

        BITMAP* it ;

        bool shallowCopy ;

        static Pict* nil ;
        static Pict* globalScreen ;

        Pict( BITMAP* p ) : it( p ), shallowCopy( false )  { }
        Pict( unsigned int w, unsigned int h ) ;
        Pict( unsigned int w, unsigned int h, unsigned int depth ) ;

        Pict( const Pict & copy ) : it( copy.it ), shallowCopy( true )  { }

        bool operator == ( const Pict & ) {  return false ;  }
        bool operator == ( void * ) {  return false ;  }
        bool operator != ( const Pict & ) {  return true ;  }
        bool operator != ( void * ) {  return true ;  }

};


class Sample
{

public:

        virtual ~Sample( ) ;

        short getVolume () const {  return volume ;  }
        void setVolume ( unsigned short vol ) {  this->volume = ( vol <= 255 ) ? vol : 255 ;  }

        int getVoice () const {  return voice ;  }
        unsigned int getFrequency () const ;
        short getPan () const ;

        void play ( unsigned short pan = 128 /* from 0 as "at left" to 255 as "at right" */ ) ;
        void loop ( unsigned short pan = 128 /* from 0 as "at left" to 255 as "at right" */ ) ;
        void stop () ;

        bool isPlaying () const ;
        void binVoiceIfNotPlaying () ;

        bool isNotNil () const {  return it != NULL ;  }

        SAMPLE* ptr () const {  return it ;  } // instead of just operator SAMPLE*() const {  return it ;  }

        static Sample* loadFromFile ( const std::string& file ) ;

        static Sample* nilSample () {  return nil ;  }

private:

        SAMPLE* it ;
        short volume ; /* from 0 to 255 */
        int voice ;

        bool shallowCopy ;

        static Sample* nil ;

        Sample( ) : it( NULL ), volume( 0 ), voice( -1 ), shallowCopy( false )  { }
        Sample( SAMPLE* s ) : it( s ), volume( 128 ), voice( -1 ), shallowCopy( false )  { }

        Sample( const Sample & copy ) : it( copy.it ), volume( copy.volume ), voice( copy.voice ), shallowCopy( true )  { }

};


namespace ogg
{

class ThreadPlaysStream
{

public:

        ThreadPlaysStream( ) : th( NULL ), stream( NULL )  { }
        virtual ~ThreadPlaysStream( )  {  stop () ;  }

        void play ( const std::string& oggFile, bool loop = false ) ;
        void stop () ;

        bool isPlaying () ;

        bool isNotNil () const {  return th != NULL ;  }

        std::string getFilePlaying () const {  return filePlaying ;  }

private:

        alogg_thread * th ;
        alogg_stream * stream ;

        std::string filePlaying ;

        ThreadPlaysStream( const ThreadPlaysStream & copy ) : th( copy.th ), stream( copy.stream ), filePlaying( copy.filePlaying )  { }

};

}


/* int allegro_init()
 *
 * Macro to initialize Allegro library. Is the same as install_allegro( SYSTEM_AUTODETECT, &errno, atexit )
 */

void init ( ) ;


/* void set_color_depth( int depth )
 *
 * Sets the pixel format to be used by subsequent calls to set_gfx_mode() and create_bitmap().
 * Valid depths are 8 (the default), 15, 16, 24, and 32 bits
 */

void setDefaultColorDepth ( unsigned int depth ) ;


/* int set_gfx_mode( int card, int w, int h, int v_w, int v_h )
 *
 * Switches into graphics mode. Returns zero on success
 *
 * The v_w and v_h parameters specify the minimum virtual screen size, in case you need a large
 * virtual screen for hardware scrolling or page flipping. Set them to zero if you don't care
 * about size of virtual screen
 */

bool switchToFullscreenVideo ( unsigned int width, unsigned int height ) ;

bool switchToFullscreenVideo () ;

bool switchToWindowedVideo ( unsigned int width, unsigned int height ) ;

bool switchToWindowedVideo () ;


/* void set_window_title( const char* name )
 *
 * Alters the window title for your Allegro program on platforms that are capable of it
 */

void setTitleOfAllegroWindow ( const std::string& title ) ;


/* void line( BITMAP* bmp, int x1, int y1, int x2, int y2, int color )
 *
 * Draws a line onto the bitmap, from point (x1, y1) to (x2, y2)
 */

void drawLine ( const Pict & where, int xFrom, int yFrom, int xTo, int yTo, int allegroColor ) ;


/* void circle( BITMAP* bmp, int x, int y, int radius, int color )
 *
 * Draws a circle with the specified centre and radius
 */

void drawCircle ( const Pict & where, int x, int y, int radius, int allegroColor ) ;


/* void circlefill( BITMAP* bmp, int x, int y, int radius, int color )
 *
 * Draws a filled circle with the specified centre and radius
 */

void fillCircle ( const Pict & where, int x, int y, int radius, int allegroColor ) ;


/* void rect( BITMAP* bmp, int x1, int y1, int x2, int y2, int color )
 *
 * Draws an outline rectangle with the two points as its opposite corners
 */

void drawRect ( const Pict & where, int x1, int y1, int x2, int y2, int allegroColor ) ;


/* void rectfill( BITMAP* bmp, int x1, int y1, int x2, int y2, int color )
 *
 * Draws a filled rectangle with the two points as its opposite corners
 */

void fillRect ( const Pict & where, int x1, int y1, int x2, int y2, int allegroColor ) ;


/* void blit( BITMAP* source, BITMAP* dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height )
 *
 * Copies a rectangular area of the source bitmap to the destination bitmap. The source_x and source_y
 * parameters are the top left corner of the area to copy from the source bitmap, and dest_x and dest_y
 * are the corresponding position in the destination bitmap
 */

void bitBlit( const Pict & from, const Pict & to, int fromX, int fromY, int toX, int toY, unsigned int width, unsigned int height ) ;

void bitBlit( const Pict & from, const Pict & to ) ;


/* void draw_sprite( BITMAP* view, BITMAP* sprite, int x, int y )
 *
 * Draws a copy of the sprite onto the view at the specified position. This is almost the same
 * as blit( sprite, view, 0, 0, x, y, sprite->w, sprite->h ), but it uses masked drawing
 * where transparent pixels are skipped
 */

void drawSprite ( const Pict & view, const Pict & sprite, int x, int y ) ;


/* void draw_trans_sprite( BITMAP* bmp, BITMAP* sprite, int x, int y )
 *
 * Uses the global color_map table or truecolor blender functions to overlay the sprite on top
 * of the existing image
 */
/* void set_trans_blender( int r, int g, int b, int a )
 *
 * Enables a linear interpolator blender mode for combining translucent or lit truecolor pixels
 */

void drawSpriteWithTransparencyBlender ( const Pict & view, const Pict & sprite, int x, int y, int r, int g, int b, int a ) ;


/* void stretch_blit( BITMAP* source, BITMAP* dest, int source_x, source_y, source_width, source_height, int dest_x, dest_y, dest_width, dest_height )
 *
 * Like blit(), except it can scale images (so the source and destination rectangles don’t need
 * to be the same size) and requires the source and destination bitmaps to be of the same color depth
 */

void stretchBlit ( const Pict & source, const Pict & dest, int sX, int sY, int sW, int sH, int dX, int dY, int dW, int dH ) ;


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

/* Pict createPict ( int width, int height, int depth = 32 ) ; */


/* int getpixel( BITMAP* bmp, int x, int y )
 *
 * Reads a pixel from point (x, y) in the bitmap. To extract the individual color components,
 * use the getr() / getg() / getb() / geta() family of functions
 */

/* int getPixel ( const Pict & picture, int x, int y ) ; */

int getRed ( int color ) ;
int getGreen ( int color ) ;
int getBlue ( int color ) ;
int getAlpha ( int color ) ;


/* void putpixel( BITMAP* bmp, int x, int y, int color )
 *
 * Writes a pixel to the specified position in the bitmap
 */

/* void putPixel ( const Pict & picture, int x, int y, int allegroColor ) ; */


/* int makecol( int r, int g, int b )
 *
 * Converts colors from a hardware independent format (red, green, and blue values ranging 0..255)
 * to the pixel format required by the current video mode, calling the preceding 8, 15, 16, 24,
 * or 32-bit makecol functions as appropriate
 */

int makeColor ( int r, int g, int b ) ;


/* void textout_ex( BITMAP* bitmap, const FONT* font, const char* string, int x, int y, int color, int bg )
 *
 * Writes the string onto the bitmap at position x, y, using the specified font, foreground color
 * and background color. If the background color is -1, then the text is written transparently.
 * If the foreground color is -1 and a color font is in use, it will be drawn using the colors
 * from the original font bitmap (the one you imported into the grabber program), which allows
 * multicolored text output. For high and true color fonts, the foreground color is ignored
 * and always treated as -1
 */

void textOut ( const std::string& text, const Pict & where, int x, int y, int allegroColor ) ;


/* void acquire_screen()
 *
 * Shortcut of acquire_bitmap( screen )
 */
/* void acquire_bitmap( BITMAP* bmp )
 *
 * Locks the bitmap before drawing onto it. You never need to call the function explicitly as it
 * is low level, and will only give you a speed up if you know what you are doing. Using it wrongly
 * may cause slowdown, or even lock up your program
 */

void acquirePict ( const Pict & what ) ;


/* void release_screen()
 *
 * Shortcut of release_bitmap( screen )
 */
/* void release_bitmap( BITMAP* bmp )
 *
 * Releases a bitmap that was previously locked by acquire_bitmap. If the bitmap was locked
 * multiple times, you must release it the same number of times before it will truly be unlocked
 */

void releasePict ( const Pict & what ) ;


/* int install_timer()
 *
 * Installs the Allegro timer interrupt handler. Do this before installing any timer routines, and
 * also before displaying a mouse pointer, playing animations or music, and using any of GUI routines
 *
 * Returns zero on success, or a negative number on failure which is very unlikely
 */

void initTimers () ;


/* int install_sound( int digi, int midi, const char* cfg_path )
 *
 * Initialises the sound module. Pass DIGI_AUTODETECT and MIDI_AUTODETECT as the driver parameters
 * to let Allegro will read hardware settings from current configuration. Alternatively, see
 * platform specific documentation for a list of the available drivers. The cfg_path parameter
 * is only present for compatibility with previous versions, and has no effect on anything
 *
 * Returns zero if the sound is successfully installed, and -1 on failure. When it fails, it stores
 * description of the problem in allegro_error
 */

void initAudio () ;


/* void set_volume( int digi_volume, int midi_volume )
 *
 * Alters the global sound output volume. Specify volumes for both digital samples and MIDI playback
 * as integers from 0 to 255, or pass a negative value to leave one of the settings unchanged.
 * Values bigger than 255 will be reduced to 255
 */

void setDigitalVolume ( unsigned short volume ) ;

void setMIDIVolume ( unsigned short volume ) ;


/* int install_keyboard()
 *
 * Installs the Allegro keyboard handler. Call this before using any of keyboard input routines.
 * Once you have set up the Allegro handler, you can no longer use operating system calls
 * or C library functions to access the keyboard
 *
 * Note that on some platforms the keyboard won't work unless you have set a graphics mode,
 * even if this function returns a success value before calling set_gfx_mode. This can happen
 * in environments with graphic windowed modes, since Allegro usually reads the keyboard through
 * the graphical window (which appears after the set_gfx_mode call)
 *
 * Returns zero on success, or a negative number on failure which is very unlikely
 */

void initKeyboardHandler () ;


/* const char* scancode_to_name( int scancode )
 *
 * Converts scancode to name of key with that scancode
 */

std::string scancodeToNameOfKey ( int scancode ) ;

int nameOfKeyToScancode ( const std::string& name ) ;

bool isKeyPushed ( const std::string& name ) ;

bool isShiftKeyPushed () ;

bool isControlKeyPushed () ;

bool isAltKeyPushed () ;

void releaseKey ( const std::string& name ) ;


/* int keypressed()
 *
 * Returns TRUE if there are keypresses waiting in the input buffer. You can use this to see
 * if the next call to readkey() is going to block or to simply wait for the user to push
 * a key while you still update the screen possibly drawing some animation
 */

bool areKeypushesWaiting () ;


/* int readkey()
 *
 * Returns the next character from the keyboard buffer, in ASCII format. If the buffer is empty,
 * it waits until a key is pressed. You can see if there are queued keypresses via keypressed()
 *
 * This function cannot return character values greater than 255. If you need Unicode input,
 * use ureadkey() instead
 */
 //     int key = readkey();
 //     if ( ( key & 0xff ) == 'd' ) /* by ASCII code */
 //             printf( "You typed 'd'\n" );
 //     if ( ( key >> 8 ) == KEY_SPACE ) /* by scancode */
 //             printf( "You typed Space\n" );

std::string nextKey () ;


/* void clear_keybuf()
 *
 * Empties the keyboard buffer. Usually you want to use this in your program before reading keys
 * to avoid previously buffered keys to be returned by calls to readkey() or ureadkey()
 */

void emptyKeyboardBuffer () ;


/* BITMAP* load_png( const char* filename, RGB* pal )
 *
 * Load a PNG from disk
 */

Pict* loadPNG ( const std::string& file ) ;


/* int algif_load_animation( const char* filename, BITMAP*** frames, int** durations )
 *
 * Returns number of frames
 */

int loadGIFAnimation ( const std::string& gifFile, std::vector< Pict * >& frames, std::vector< int >& durations ) ;


/* int save_bitmap( const char* filename, BITMAP* bmp, const RGB* pal )
 *
 * Writes a bitmap into a file, using the specified palette. The output format is determined
 * from the filename extension: at present this function supports BMP, PCX and TGA formats.
 * Returns non-zero on error
 */

void savePictAsPCX ( const std::string& where, const Pict & what ) ;

#endif

}

#endif
