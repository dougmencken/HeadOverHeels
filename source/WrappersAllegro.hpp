// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
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


#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

#include <allegro5/color.h>
typedef ALLEGRO_COLOR type_of_allegro_color ;

struct ALLEGRO_BITMAP ;
typedef ALLEGRO_BITMAP AllegroBitmap ;

struct ALLEGRO_SAMPLE ;
typedef ALLEGRO_SAMPLE AllegroSample ;

struct ALLEGRO_SAMPLE_INSTANCE ;

struct ALLEGRO_SAMPLE_ID ;

struct ALLEGRO_MIXER ;

struct ALLEGRO_DISPLAY ;

struct ALLEGRO_FONT ;

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

typedef int type_of_allegro_color ;

struct BITMAP ;
typedef BITMAP AllegroBitmap ;

struct SAMPLE ;
typedef SAMPLE AllegroSample ;

struct alogg_stream ;
struct alogg_thread ;

#else

#error I need either Allegro 4 or Allegro 5

#endif


class AllegroColor
{

public:

        AllegroColor ( type_of_allegro_color c ) : color( c ) { }

        operator type_of_allegro_color() const {  return color ;  }

        unsigned char getRed () const ;
        unsigned char getGreen () const ;
        unsigned char getBlue () const ;
        unsigned char getAlpha () const ;

        bool equalsRGBA ( const AllegroColor& toCompare ) const ;
        bool equalsRGB ( const AllegroColor& toCompare ) const ;

        bool isKeyColor () const ;

        static AllegroColor makeColor ( unsigned char r, unsigned char g, unsigned char b, unsigned char a ) ;
        static AllegroColor makeColor ( unsigned char r, unsigned char g, unsigned char b ) ;

        static AllegroColor keyColor () ;

        static unsigned char redOfKeyColor ;
        static unsigned char greenOfKeyColor ;
        static unsigned char blueOfKeyColor ;
        static unsigned char alphaOfKeyColor ;

private:

        type_of_allegro_color color ;

};


namespace allegro
{

class Pict
{

public:

        virtual ~Pict( ) ;

        unsigned int getW () const ;
        unsigned int getH () const ;

        AllegroColor getPixelAt ( int x, int y ) const ;
        void putPixelAt ( int x, int y, AllegroColor color ) const ;
        void drawPixelAt ( int x, int y, AllegroColor color ) const ;

        void clearToColor ( AllegroColor color ) const ;

        void lockReadOnly () const ;
        void lockReadWrite () const ;
        void unlock () const ;

        bool isNotNil () const {  return it != NULL ;  }

        AllegroBitmap* ptr () const {  return it ;  } // instead of just operator AllegroBitmap*() const {  return it ;  }

        bool equals ( const Pict& toWhat ) const {  return it == toWhat.it ;  }

        static Pict* nilPict () {  return new Pict( NULL ) ;  }

        static Pict* newPict ( AllegroBitmap* b ) {  return new Pict( b ) ;  }
        static Pict* newPict ( unsigned int w, unsigned int h ) {  return new Pict( w, h ) ;  }

        static Pict* asCloneOf ( AllegroBitmap* image ) ;

        static Pict* mendIntoPict ( AllegroBitmap* image ) ;

        static Pict* fromPNGFile ( const std::string& file ) ;

        static const Pict& theScreen ();

        static const Pict & getWhereToDraw () ;

        static void setWhereToDraw ( const Pict & where ) ;

        static void resetWhereToDraw () {  setWhereToDraw( theScreen() ) ;  }

private:

        AllegroBitmap* it ;

        bool shallowCopy ;

        static Pict* whereToDraw ;

        static Pict* globalScreen ;

        Pict( AllegroBitmap* p ) : it( p ), shallowCopy( false )  { }
        Pict( unsigned int w, unsigned int h ) ;

        Pict( const Pict & copy ) : it( copy.it ), shallowCopy( true )  { }

        ///bool operator == ( const Pict & ) const = delete ; /// {  return false ;  }
        ///bool operator == ( const void * ) const = delete ; /// {  return false ;  }
        ///bool operator != ( const Pict & ) const = delete ; /// {  return true ;  }
        ///bool operator != ( const void * ) const = delete ; /// {  return true ;  }

};


class Audio
{

public:

        static unsigned short digitalVolume ;
        static unsigned short midiVolume ;

};


class Sample
{

public:

        virtual ~Sample( ) ;

        short getVolume () const {  return volume ;  }
        void setVolume ( unsigned short vol ) {  this->volume = ( vol <= 255 ) ? vol : 255 ;  }

        int getVoice () const {  return voice ;  }
        unsigned int getFrequency () const ;

        void play ( unsigned short pan = 128 /* from 0 as "at left" to 255 as "at right" */ ) ;
        void loop ( unsigned short pan = 128 /* from 0 as "at left" to 255 as "at right" */ ) ;
        void stop () ;

        void neatenIfNotPlaying () ;

        bool isPlaying () const ;

        bool isNotNil () const {  return it != NULL ;  }

        AllegroSample* ptr () const {  return it ;  } // instead of just operator AllegroSample*() const {  return it ;  }

        static Sample* loadFromFile ( const std::string& file ) ;

        static Sample* nilSample () {  return new Sample( ) ;  }

private:

        AllegroSample* it ;
        short volume ; /* from 0 to 255 */
        int voice ;

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        ALLEGRO_SAMPLE_INSTANCE* sampleInstance ;

#endif

        bool shallowCopy ;

        Sample( ) : it( NULL ), volume( 0 ), voice( -1 ), shallowCopy( false )  { }
        Sample( AllegroSample* s ) : it( s ), volume( 128 ), voice( -1 ), shallowCopy( false )  { }

        Sample( const Sample & copy ) : it( copy.it ), volume( copy.volume ), voice( copy.voice ), shallowCopy( true )  { }

        void playMe ( unsigned short volume, unsigned short pan, bool loop ) ;

};


namespace ogg
{

class OggPlayer
{

public:

        OggPlayer( ) :
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5
                oggSample( NULL ), sampleInstance( NULL ), oggMixer( NULL )
#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4
                th( NULL ), stream( NULL )
#endif
        { }

        virtual ~OggPlayer( ) {  stop () ;  }

        void play ( const std::string& oggFile, bool loop = false ) ;
        void stop () ;

        bool isPlaying () ;

        std::string getFilePlaying () const {  return filePlaying ;  }

        static void syncPlayersWithDigitalVolume () ;

private:

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        ALLEGRO_SAMPLE * oggSample ;
        ALLEGRO_SAMPLE_INSTANCE * sampleInstance ;
        ALLEGRO_MIXER * oggMixer ;

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        alogg_thread * th ;
        alogg_stream * stream ;

#endif

        std::string filePlaying ;

        static std::vector < OggPlayer * > everyPlayer ;

};

}


void init ( ) ;

void bye ( ) ;


void update () ;


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


/* Changes the icon associated with the window */

void setAllegroIcon( const Pict & icon ) ;


/* void line( BITMAP* bmp, int x1, int y1, int x2, int y2, int color )
 *
 * Draws a line onto the bitmap, from point (x1, y1) to (x2, y2)
 */

void drawLine ( int xFrom, int yFrom, int xTo, int yTo, AllegroColor color ) ;


/* void circle( BITMAP* bmp, int x, int y, int radius, int color )
 *
 * Draws a circle with the specified centre and radius
 */

void drawCircle ( int x, int y, int radius, AllegroColor color ) ;


/* void circlefill( BITMAP* bmp, int x, int y, int radius, int color )
 *
 * Draws a filled circle with the specified centre and radius
 */

void fillCircle ( int x, int y, int radius, AllegroColor color ) ;


/* void rect( BITMAP* bmp, int x1, int y1, int x2, int y2, int color )
 *
 * Draws an outline rectangle with the two points as its opposite corners
 */

void drawRect ( int x1, int y1, int x2, int y2, AllegroColor color ) ;


/* void rectfill( BITMAP* bmp, int x1, int y1, int x2, int y2, int color )
 *
 * Draws a filled rectangle with the two points as its opposite corners
 */

void fillRect( const Pict & where, int x1, int y1, int x2, int y2, AllegroColor color ) ;

void fillRect ( int x1, int y1, int x2, int y2, AllegroColor color ) ;


void bitBlit ( const Pict & from, int fromX, int fromY, int toX, int toY, unsigned int width, unsigned int height ) ;

void bitBlit ( const Pict& from, int toX, int toY ) ;

void bitBlit ( const Pict & from, const Pict & to, int fromX, int fromY, int toX, int toY, unsigned int width, unsigned int height ) ;

void bitBlit ( const Pict& from, const Pict& to, int toX, int toY ) ;

void bitBlit ( const Pict & from ) ;

void bitBlit ( const Pict & from, const Pict & to ) ;

void stretchBlit ( const Pict & source, int sX, int sY, int sW, int sH, int dX, int dY, int dW, int dH ) ;

void stretchBlit ( const Pict & source, const Pict & dest, int sX, int sY, int sW, int sH, int dX, int dY, int dW, int dH ) ;

void drawSprite ( const Pict & sprite, int x, int y ) ;

void drawSpriteWithTransparency ( const Pict & sprite, int x, int y, unsigned char transparency ) ;


/* void textout_ex( BITMAP* bitmap, const FONT* font, const char* string, int x, int y, int color, int bg )
 *
 * Writes the string onto the bitmap at position x, y, using the specified font, foreground color
 * and background color. If the background color is -1, then the text is written transparently.
 * If the foreground color is -1 and a color font is in use, it will be drawn using the colors
 * from the original font bitmap (the one you imported into the grabber program), which allows
 * multicolored text output. For high and true color fonts, the foreground color is ignored
 * and always treated as -1
 */

void textOut ( const std::string& text, int x, int y, AllegroColor color ) ;


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

bool initAudio( const std::string & audioInterface ) ;

bool initAudio () ;


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

void savePictAsPNG ( const std::string& where, const Pict & what ) ;

}

#endif
