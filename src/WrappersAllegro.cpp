
#include "WrappersAllegro.hpp"

#include <string>
#include <iostream>

#include <allegro.h>

#ifdef __WIN32
  #include <winalleg.h>
#endif

#include <loadpng.h>

#include <algif.h>

#include "alogg/aloggpth.h"
#include "alogg/aloggint.h"

#ifdef DEBUG
#  define DEBUG_ALLEGRO_INIT    1
#endif


namespace allegro
{

#if defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

/* static */ Pict* Pict::nil = new Pict( NULL ) ;

/* static */ Pict* Pict::globalScreen = new Pict( /* allegro’s global variable */ screen );

Pict::Pict( unsigned int w, unsigned int h )
        : shallowCopy( false )
{
        it = create_bitmap( w, h );
}

Pict::Pict( unsigned int w, unsigned int h, unsigned int depth )
        : shallowCopy( false )
{
        it = create_bitmap_ex( depth, w, h );
}

Pict::~Pict( )
{
        if ( ! isNotNil() ) return ;
        if ( shallowCopy ) return ;
        if ( it == /* allegro’s global variable */ screen ) return ;

        // nullify it first, then invoke destroy_bitmap
        // to avoid drawing of being-destroyed thing when there’re many threads

        BITMAP* toBin = it;
        it = NULL;

        destroy_bitmap( toBin );
}

unsigned int Pict::getW() const
{
        return ( isNotNil() ) ? it->w : 0 ;
}

unsigned int Pict::getH() const
{
        return ( isNotNil() ) ? it->h : 0 ;
}

int Pict::getPixelAt( int x, int y ) const
{
        return getpixel( it, x, y ) ;
}

void Pict::setPixelAt( int x, int y, int allegroColor ) const
{
        putpixel( it, x, y, allegroColor ) ;
}

unsigned int Pict::getColorDepth () const
{
        return ( isNotNil() ) ? bitmap_color_depth( it ) : 0 ;
}

void Pict::clearToColor( int allegroColor ) const
{
        clear_to_color( it, allegroColor );
}

bool Pict::isInVideoMemory() const
{
        // int is_video_bitmap( BITMAP* bmp )
        // returns TRUE if bmp is the allegro screen bitmap, a video memory bitmap,
        // or a sub-bitmap of either
        return is_video_bitmap( it ) == TRUE ;
}

/* static */ const Pict& Pict::theScreen ()
{
        globalScreen->it = /* allegro’s global variable */ screen ; // update it before returning
        return *globalScreen ;
}


/* static */ Sample* Sample::nil = new Sample( );

Sample::~Sample( )
{
        if ( ! isNotNil() ) return ;
        if ( shallowCopy ) return ;

        stop();

        SAMPLE* toBin = it;
        it = NULL;

        destroy_sample( toBin );
}

unsigned int Sample::getFrequency() const
{
        if ( voice < 0 ) return 0 ;
        int frequency = voice_get_frequency( voice );
        return ( frequency > 0 ) ? frequency : 0 ;
}

short Sample::getPan() const
{
        if ( voice < 0 ) return -1 ;
        return voice_get_pan( voice );
}

void Sample::play( unsigned short pan )
{
        if ( ! isPlaying() )
                voice = play_sample( it, volume, pan, 1000 /* frequency that sample was recorded at */, 0 /* don’t loop */ );
        else
                voice_set_position( voice, 0 );
}

void Sample::loop( unsigned short pan )
{
        if ( ! isPlaying() )
                voice = play_sample( it, volume, pan, 1000 /* frequency that sample was recorded at */, 1 /* loop */ );
        else
                voice_set_position( voice, 0 );
}

void Sample::stop()
{
        if ( ! isNotNil() || voice < 0 ) return ;

        stop_sample( it );
        binVoiceIfNotPlaying();
}

bool Sample::isPlaying() const
{
        if ( voice < 0 ) return false ;

        // int voice_get_position( int voice )
        // returns the current position of voice in sample units or -1 if it has finished playing

        return voice_get_position( voice ) != -1 ;
}

void Sample::binVoiceIfNotPlaying()
{
        if ( voice < 0 || isPlaying() ) return ;
        deallocate_voice( voice );
        voice = -1;
}

/* static */
Sample* Sample::loadFromFile( const std::string& file )
{
        return new Sample( load_sample( file.c_str () ) );
}


namespace ogg
{

void ThreadPlaysStream::play( const std::string& oggFile, bool loop )
{
        if ( isNotNil() ) return ;

        // use buffer of 40 KiB
        const size_t lengthOfbuffer = 40 * 1024;

        stream = alogg_start_streaming( oggFile.c_str(), lengthOfbuffer );

        if ( stream != NULL )
        {
                th = ( loop ) ? alogg_create_thread_which_loops( stream, oggFile.c_str(), lengthOfbuffer )
                                : alogg_create_thread( stream );
        }

        filePlaying = oggFile ;
}

void ThreadPlaysStream::stop()
{
        if ( ! isNotNil() ) return ;

        filePlaying.clear() ;

        alogg_stop_thread( th );
        /// while ( isPlaying() ) ;
        alogg_join_thread( th );

        alogg_thread* toBin = th;
        th = NULL;

        alogg_destroy_thread( toBin );
}

bool ThreadPlaysStream::isPlaying()
{
        return isNotNil() ? th->alive != 0 && th->stop == 0 : false ;
}

}


std::map < int, std::string > scancodesToNames ;

std::map < std::string, int > namesToScancodes ;

bool initialized = false ;

bool audioInitialized = false ;

void init( )
{
        allegro_init();

#ifdef __WIN32
        // when application loses focus, the game will continue in background
        // because there are threads that will continue even when the main thread pauses
        set_display_switch_mode( SWITCH_BACKGROUND );
#endif

        scancodesToNames[ KEY_A ] = "a" ;
        scancodesToNames[ KEY_B ] = "b" ;
        scancodesToNames[ KEY_C ] = "c" ;
        scancodesToNames[ KEY_D ] = "d" ;
        scancodesToNames[ KEY_E ] = "e" ;
        scancodesToNames[ KEY_F ] = "f" ;
        scancodesToNames[ KEY_G ] = "g" ;
        scancodesToNames[ KEY_H ] = "h" ;
        scancodesToNames[ KEY_I ] = "i" ;
        scancodesToNames[ KEY_J ] = "j" ;
        scancodesToNames[ KEY_K ] = "k" ;
        scancodesToNames[ KEY_L ] = "l" ;
        scancodesToNames[ KEY_M ] = "m" ;
        scancodesToNames[ KEY_N ] = "n" ;
        scancodesToNames[ KEY_O ] = "o" ;
        scancodesToNames[ KEY_P ] = "p" ;
        scancodesToNames[ KEY_Q ] = "q" ;
        scancodesToNames[ KEY_R ] = "r" ;
        scancodesToNames[ KEY_S ] = "s" ;
        scancodesToNames[ KEY_T ] = "t" ;
        scancodesToNames[ KEY_U ] = "u" ;
        scancodesToNames[ KEY_V ] = "v" ;
        scancodesToNames[ KEY_W ] = "w" ;
        scancodesToNames[ KEY_X ] = "x" ;
        scancodesToNames[ KEY_Y ] = "y" ;
        scancodesToNames[ KEY_Z ] = "z" ;
        scancodesToNames[ KEY_0 ] = "0" ;
        scancodesToNames[ KEY_1 ] = "1" ;
        scancodesToNames[ KEY_2 ] = "2" ;
        scancodesToNames[ KEY_3 ] = "3" ;
        scancodesToNames[ KEY_4 ] = "4" ;
        scancodesToNames[ KEY_5 ] = "5" ;
        scancodesToNames[ KEY_6 ] = "6" ;
        scancodesToNames[ KEY_7 ] = "7" ;
        scancodesToNames[ KEY_8 ] = "8" ;
        scancodesToNames[ KEY_9 ] = "9" ;
        scancodesToNames[ KEY_ESC ] = "Escape" ;
        scancodesToNames[ KEY_TILDE ] = "Tilde" ;
        scancodesToNames[ KEY_MINUS ] = "Minus" ;
        scancodesToNames[ KEY_EQUALS ] = "Equals" ;
        scancodesToNames[ KEY_BACKSPACE ] = "Backspace" ;
        scancodesToNames[ KEY_TAB ] = "Tab" ;
        scancodesToNames[ KEY_OPENBRACE ] = "OpenBrace" ;
        scancodesToNames[ KEY_CLOSEBRACE ] = "CloseBrace" ;
        scancodesToNames[ KEY_ENTER ] = "Enter" ;
        scancodesToNames[ KEY_COLON ] = "Colon" ;
        scancodesToNames[ KEY_QUOTE ] = "Quote" ;
        scancodesToNames[ KEY_BACKSLASH ] = "Backslash" ;
        scancodesToNames[ KEY_BACKSLASH2 ] = "Backslash2" ;
        scancodesToNames[ KEY_COMMA ] = "Comma" ;
        scancodesToNames[ KEY_STOP ] = "Period" ;
        scancodesToNames[ KEY_SLASH ] = "Slash" ;
        scancodesToNames[ KEY_SPACE ] = "Space" ;
        scancodesToNames[ KEY_LEFT ] = "Left" ;
        scancodesToNames[ KEY_RIGHT ] = "Right" ;
        scancodesToNames[ KEY_UP ] = "Up" ;
        scancodesToNames[ KEY_DOWN ] = "Down" ;
        scancodesToNames[ KEY_INSERT ] = "Insert" ;
        scancodesToNames[ KEY_DEL ] = "Delete" ;
        scancodesToNames[ KEY_HOME ] = "Home" ;
        scancodesToNames[ KEY_END ] = "End" ;
        scancodesToNames[ KEY_PGUP ] = "PageUp" ;
        scancodesToNames[ KEY_PGDN ] = "PageDown" ;
        scancodesToNames[ KEY_F1 ] = "F1" ;
        scancodesToNames[ KEY_F2 ] = "F2" ;
        scancodesToNames[ KEY_F3 ] = "F3" ;
        scancodesToNames[ KEY_F4 ] = "F4" ;
        scancodesToNames[ KEY_F5 ] = "F5" ;
        scancodesToNames[ KEY_F6 ] = "F6" ;
        scancodesToNames[ KEY_F7 ] = "F7" ;
        scancodesToNames[ KEY_F8 ] = "F8" ;
        scancodesToNames[ KEY_F9 ] = "F9" ;
        scancodesToNames[ KEY_F10 ] = "F10" ;
        scancodesToNames[ KEY_F11 ] = "F11" ;
        scancodesToNames[ KEY_F12 ] = "F12" ;
        scancodesToNames[ KEY_PRTSCR ] = "PrintScreen" ;
        scancodesToNames[ KEY_PAUSE ] = "Pause" ;
        scancodesToNames[ KEY_0_PAD ] = "Pad 0" ;
        scancodesToNames[ KEY_1_PAD ] = "Pad 1" ;
        scancodesToNames[ KEY_2_PAD ] = "Pad 2" ;
        scancodesToNames[ KEY_3_PAD ] = "Pad 3" ;
        scancodesToNames[ KEY_4_PAD ] = "Pad 4" ;
        scancodesToNames[ KEY_5_PAD ] = "Pad 5" ;
        scancodesToNames[ KEY_6_PAD ] = "Pad 6" ;
        scancodesToNames[ KEY_7_PAD ] = "Pad 7" ;
        scancodesToNames[ KEY_8_PAD ] = "Pad 8" ;
        scancodesToNames[ KEY_9_PAD ] = "Pad 9" ;
        scancodesToNames[ KEY_SLASH_PAD ] = "Pad /" ;
        scancodesToNames[ KEY_ASTERISK ] = "Pad *" ;
        scancodesToNames[ KEY_MINUS_PAD ] = "Pad -" ;
        scancodesToNames[ KEY_PLUS_PAD ] = "Pad +" ;
        scancodesToNames[ KEY_DEL_PAD ] = "Pad Decimal" ;
        scancodesToNames[ KEY_ENTER_PAD ] = "Pad Enter" ;
        scancodesToNames[ KEY_EQUALS_PAD ] = "Pad =" ;
        scancodesToNames[ KEY_ABNT_C1 ] = "AbntC1" ;
        scancodesToNames[ KEY_YEN ] = "Yen" ;
        scancodesToNames[ KEY_KANA ] = "Kana" ;
        scancodesToNames[ KEY_KANJI ] = "Kanji" ;
        scancodesToNames[ KEY_CONVERT ] = "Convert" ;
        scancodesToNames[ KEY_NOCONVERT ] = "NoConvert" ;
        scancodesToNames[ KEY_AT ] = "At" ;
        scancodesToNames[ KEY_CIRCUMFLEX ] = "Circumflex" ;
        scancodesToNames[ KEY_COLON2 ] = "Colon2" ;
        scancodesToNames[ KEY_BACKQUOTE ] = "Mac Backquote" ;
        scancodesToNames[ KEY_SEMICOLON ] = "Mac Semicolon" ;
        scancodesToNames[ KEY_COMMAND ] = "Mac Command" ;
        scancodesToNames[ 0 ] = "none" ;

        namesToScancodes[ "a" ] = namesToScancodes[ "A" ] = KEY_A ;
        namesToScancodes[ "b" ] = namesToScancodes[ "B" ] = KEY_B ;
        namesToScancodes[ "c" ] = namesToScancodes[ "C" ] = KEY_C ;
        namesToScancodes[ "d" ] = namesToScancodes[ "D" ] = KEY_D ;
        namesToScancodes[ "e" ] = namesToScancodes[ "E" ] = KEY_E ;
        namesToScancodes[ "f" ] = namesToScancodes[ "F" ] = KEY_F ;
        namesToScancodes[ "g" ] = namesToScancodes[ "G" ] = KEY_G ;
        namesToScancodes[ "h" ] = namesToScancodes[ "H" ] = KEY_H ;
        namesToScancodes[ "i" ] = namesToScancodes[ "I" ] = KEY_I ;
        namesToScancodes[ "j" ] = namesToScancodes[ "J" ] = KEY_J ;
        namesToScancodes[ "k" ] = namesToScancodes[ "K" ] = KEY_K ;
        namesToScancodes[ "l" ] = namesToScancodes[ "L" ] = KEY_L ;
        namesToScancodes[ "m" ] = namesToScancodes[ "M" ] = KEY_M ;
        namesToScancodes[ "n" ] = namesToScancodes[ "N" ] = KEY_N ;
        namesToScancodes[ "o" ] = namesToScancodes[ "O" ] = KEY_O ;
        namesToScancodes[ "p" ] = namesToScancodes[ "P" ] = KEY_P ;
        namesToScancodes[ "q" ] = namesToScancodes[ "Q" ] = KEY_Q ;
        namesToScancodes[ "r" ] = namesToScancodes[ "R" ] = KEY_R ;
        namesToScancodes[ "s" ] = namesToScancodes[ "S" ] = KEY_S ;
        namesToScancodes[ "t" ] = namesToScancodes[ "T" ] = KEY_T ;
        namesToScancodes[ "u" ] = namesToScancodes[ "U" ] = KEY_U ;
        namesToScancodes[ "v" ] = namesToScancodes[ "V" ] = KEY_V ;
        namesToScancodes[ "w" ] = namesToScancodes[ "W" ] = KEY_W ;
        namesToScancodes[ "x" ] = namesToScancodes[ "X" ] = KEY_X ;
        namesToScancodes[ "y" ] = namesToScancodes[ "Y" ] = KEY_Y ;
        namesToScancodes[ "z" ] = namesToScancodes[ "Z" ] = KEY_Z ;
        namesToScancodes[ "0" ] = KEY_0 ;
        namesToScancodes[ "1" ] = KEY_1 ;
        namesToScancodes[ "2" ] = KEY_2 ;
        namesToScancodes[ "3" ] = KEY_3 ;
        namesToScancodes[ "4" ] = KEY_4 ;
        namesToScancodes[ "5" ] = KEY_5 ;
        namesToScancodes[ "6" ] = KEY_6 ;
        namesToScancodes[ "7" ] = KEY_7 ;
        namesToScancodes[ "8" ] = KEY_8 ;
        namesToScancodes[ "9" ] = KEY_9 ;
        namesToScancodes[ "escape" ] = namesToScancodes[ "Escape" ] = namesToScancodes[ "ESCAPE" ] = namesToScancodes[ "ESC" ] = namesToScancodes[ "Esc" ] = namesToScancodes[ "esc" ] = KEY_ESC ;
        namesToScancodes[ "~" ] = namesToScancodes[ "tilde" ] = namesToScancodes[ "Tilde" ] = namesToScancodes[ "TILDE" ] = KEY_TILDE ;
        namesToScancodes[ "-" ] = namesToScancodes[ "minus" ] = namesToScancodes[ "Minus" ] = namesToScancodes[ "MINUS" ] = KEY_MINUS ;
        namesToScancodes[ "=" ] = namesToScancodes[ "equals" ] = namesToScancodes[ "Equals" ] = namesToScancodes[ "EQUALS" ] = KEY_EQUALS ;
        namesToScancodes[ "backspace" ] = namesToScancodes[ "Backspace" ] = namesToScancodes[ "BackSpace" ] = namesToScancodes[ "BACKSPACE" ] = KEY_BACKSPACE ;
        namesToScancodes[ "tab" ] = namesToScancodes[ "Tab" ] = namesToScancodes[ "TAB" ] = KEY_TAB ;
        namesToScancodes[ "[" ] = namesToScancodes[ "openbrace" ] = namesToScancodes[ "OpenBrace" ] = namesToScancodes[ "OPENBRACE" ] = KEY_OPENBRACE ;
        namesToScancodes[ "]" ] = namesToScancodes[ "closebrace" ] = namesToScancodes[ "CloseBrace" ] = namesToScancodes[ "CLOSEBRACE" ] = KEY_CLOSEBRACE ;
        namesToScancodes[ "enter" ] = namesToScancodes[ "Enter" ] = namesToScancodes[ "ENTER" ] = KEY_ENTER ;
        namesToScancodes[ ";" ] = namesToScancodes[ ":" ] = namesToScancodes[ "colon" ] = namesToScancodes[ "Colon" ] = namesToScancodes[ "COLON" ] = KEY_COLON ;
        namesToScancodes[ "'" ] = namesToScancodes[ "\"" ] = namesToScancodes[ "quote" ] = namesToScancodes[ "Quote" ] = namesToScancodes[ "QUOTE" ] = KEY_QUOTE ;
        namesToScancodes[ "\\" ] = namesToScancodes[ "backslash" ] = namesToScancodes[ "Backslash" ] = namesToScancodes[ "BackSlash" ] = namesToScancodes[ "BACKSLASH" ] = KEY_BACKSLASH ;
        namesToScancodes[ "backslash2" ] = namesToScancodes[ "Backslash2" ] = namesToScancodes[ "BackSlash2" ] = namesToScancodes[ "BACKSLASH2" ] = KEY_BACKSLASH2 ;
        namesToScancodes[ "," ] = namesToScancodes[ "comma" ] = namesToScancodes[ "Comma" ] = namesToScancodes[ "COMMA" ] = KEY_COMMA ;
        namesToScancodes[ "." ] = namesToScancodes[ "period" ] = namesToScancodes[ "Period" ] = namesToScancodes[ "PERIOD" ] = namesToScancodes[ "stop" ] = namesToScancodes[ "Stop" ] = namesToScancodes[ "STOP" ] = KEY_STOP ;
        namesToScancodes[ "/" ] = namesToScancodes[ "slash" ] = namesToScancodes[ "Slash" ] = namesToScancodes[ "SLASH" ] = KEY_SLASH ;
        namesToScancodes[ " " ] = namesToScancodes[ "space" ] = namesToScancodes[ "Space" ] = namesToScancodes[ "SPACE" ] = KEY_SPACE ;
        namesToScancodes[ "left" ] = namesToScancodes[ "Left" ] = namesToScancodes[ "LEFT" ] = KEY_LEFT ;
        namesToScancodes[ "right" ] = namesToScancodes[ "Right" ] = namesToScancodes[ "RIGHT" ] = KEY_RIGHT ;
        namesToScancodes[ "up" ] = namesToScancodes[ "Up" ] = namesToScancodes[ "UP" ] = KEY_UP ;
        namesToScancodes[ "down" ] = namesToScancodes[ "Down" ] = namesToScancodes[ "DOWN" ] = KEY_DOWN ;
        namesToScancodes[ "insert" ] = namesToScancodes[ "Insert" ] = namesToScancodes[ "INSERT" ] = namesToScancodes[ "INS" ] = namesToScancodes[ "Ins" ] = namesToScancodes[ "ins" ] = KEY_INSERT ;
        namesToScancodes[ "delete" ] = namesToScancodes[ "Delete" ] = namesToScancodes[ "DELETE" ] = namesToScancodes[ "DEL" ] = namesToScancodes[ "Del" ] = namesToScancodes[ "del" ] = KEY_DEL ;
        namesToScancodes[ "home" ] = namesToScancodes[ "Home" ] = namesToScancodes[ "HOME" ] = KEY_HOME ;
        namesToScancodes[ "end" ] = namesToScancodes[ "End" ] = namesToScancodes[ "END" ] = KEY_END ;
        namesToScancodes[ "pageup" ] = namesToScancodes[ "PageUp" ] = namesToScancodes[ "PAGEUP" ] = namesToScancodes[ "PGUP" ] = namesToScancodes[ "PgUp" ] = namesToScancodes[ "pgup" ] = KEY_PGUP ;
        namesToScancodes[ "pagedown" ] = namesToScancodes[ "PageDown" ] = namesToScancodes[ "PAGEDOWN" ] = namesToScancodes[ "PGDN" ] = namesToScancodes[ "PgDn" ] = namesToScancodes[ "pgdn" ] = KEY_PGDN ;
        namesToScancodes[ "F1" ] = namesToScancodes[ "f1" ] = KEY_F1 ;
        namesToScancodes[ "F2" ] = namesToScancodes[ "f2" ] = KEY_F2 ;
        namesToScancodes[ "F3" ] = namesToScancodes[ "f3" ] = KEY_F3 ;
        namesToScancodes[ "F4" ] = namesToScancodes[ "f4" ] = KEY_F4 ;
        namesToScancodes[ "F5" ] = namesToScancodes[ "f5" ] = KEY_F5 ;
        namesToScancodes[ "F6" ] = namesToScancodes[ "f6" ] = KEY_F6 ;
        namesToScancodes[ "F7" ] = namesToScancodes[ "f7" ] = KEY_F7 ;
        namesToScancodes[ "F8" ] = namesToScancodes[ "f8" ] = KEY_F8 ;
        namesToScancodes[ "F9" ] = namesToScancodes[ "f9" ] = KEY_F9 ;
        namesToScancodes[ "F10" ] = namesToScancodes[ "f10" ] = KEY_F10 ;
        namesToScancodes[ "F11" ] = namesToScancodes[ "f11" ] = KEY_F11 ;
        namesToScancodes[ "F12" ] = namesToScancodes[ "f12" ] = KEY_F12 ;
        namesToScancodes[ "printscreen" ] = namesToScancodes[ "PrintScreen" ] = namesToScancodes[ "PRINTSCREEN" ] = namesToScancodes[ "PRTSCR" ] = namesToScancodes[ "PrtScr" ] = namesToScancodes[ "prtscr" ] = KEY_PRTSCR ;
        namesToScancodes[ "pause" ] = namesToScancodes[ "Pause" ] = namesToScancodes[ "PAUSE" ] = KEY_PAUSE ;
        namesToScancodes[ "Pad 0" ] = namesToScancodes[ "pad 0" ] = namesToScancodes[ "PAD 0" ] = KEY_0_PAD ;
        namesToScancodes[ "Pad 1" ] = namesToScancodes[ "pad 1" ] = namesToScancodes[ "PAD 1" ] = KEY_1_PAD ;
        namesToScancodes[ "Pad 2" ] = namesToScancodes[ "pad 2" ] = namesToScancodes[ "PAD 2" ] = KEY_2_PAD ;
        namesToScancodes[ "Pad 3" ] = namesToScancodes[ "pad 3" ] = namesToScancodes[ "PAD 3" ] = KEY_3_PAD ;
        namesToScancodes[ "Pad 4" ] = namesToScancodes[ "pad 4" ] = namesToScancodes[ "PAD 4" ] = KEY_4_PAD ;
        namesToScancodes[ "Pad 5" ] = namesToScancodes[ "pad 5" ] = namesToScancodes[ "PAD 5" ] = KEY_5_PAD ;
        namesToScancodes[ "Pad 6" ] = namesToScancodes[ "pad 6" ] = namesToScancodes[ "PAD 6" ] = KEY_6_PAD ;
        namesToScancodes[ "Pad 7" ] = namesToScancodes[ "pad 7" ] = namesToScancodes[ "PAD 7" ] = KEY_7_PAD ;
        namesToScancodes[ "Pad 8" ] = namesToScancodes[ "pad 8" ] = namesToScancodes[ "PAD 8" ] = KEY_8_PAD ;
        namesToScancodes[ "Pad 9" ] = namesToScancodes[ "pad 9" ] = namesToScancodes[ "PAD 9" ] = KEY_9_PAD ;
        namesToScancodes[ "Pad /" ] = namesToScancodes[ "pad /" ] = namesToScancodes[ "PAD /" ] = KEY_SLASH_PAD ;
        namesToScancodes[ "Pad *" ] = namesToScancodes[ "pad *" ] = namesToScancodes[ "PAD *" ] = KEY_ASTERISK ;
        namesToScancodes[ "Pad -" ] = namesToScancodes[ "pad -" ] = namesToScancodes[ "PAD -" ] = KEY_MINUS_PAD ;
        namesToScancodes[ "Pad +" ] = namesToScancodes[ "pad +" ] = namesToScancodes[ "PAD +" ] = KEY_PLUS_PAD ;
        namesToScancodes[ "Pad Decimal" ] = namesToScancodes[ "pad decimal" ] = namesToScancodes[ "PAD DECIMAL" ] = KEY_DEL_PAD ;
        namesToScancodes[ "Pad Enter" ] = namesToScancodes[ "pad enter" ] = namesToScancodes[ "PAD ENTER" ] = KEY_ENTER_PAD ;
        namesToScancodes[ "Pad =" ] = namesToScancodes[ "pad =" ] = namesToScancodes[ "PAD =" ] = KEY_EQUALS_PAD ; // OS X
        namesToScancodes[ "AbntC1" ] = namesToScancodes[ "ABNT_C1" ] = KEY_ABNT_C1 ; // Japanese
        namesToScancodes[ "yen" ] = namesToScancodes[ "Yen" ] = namesToScancodes[ "YEN" ] = KEY_YEN ; // Japanese
        namesToScancodes[ "kana" ] = namesToScancodes[ "Kana" ] = namesToScancodes[ "KANA" ] = KEY_KANA ; // Japanese
        namesToScancodes[ "kanji" ] = namesToScancodes[ "Kanji" ] = namesToScancodes[ "KANJI" ] = KEY_KANJI ; // Japanese
        namesToScancodes[ "convert" ] = namesToScancodes[ "Convert" ] = namesToScancodes[ "CONVERT" ] = KEY_CONVERT ;
        namesToScancodes[ "noconvert" ] = namesToScancodes[ "NoConvert" ] = namesToScancodes[ "NOCONVERT" ] = KEY_NOCONVERT ;
        namesToScancodes[ "@" ] = namesToScancodes[ "at" ] = namesToScancodes[ "At" ] = namesToScancodes[ "AT" ] = KEY_AT ;
        namesToScancodes[ "^" ] = namesToScancodes[ "circumflex" ] = namesToScancodes[ "Circumflex" ] = namesToScancodes[ "CIRCUMFLEX" ] = KEY_CIRCUMFLEX ;
        namesToScancodes[ "colon2" ] = namesToScancodes[ "Colon2" ] = namesToScancodes[ "COLON2" ] = KEY_COLON2 ;
        namesToScancodes[ "mac backquote" ] = namesToScancodes[ "Mac Backquote" ] = namesToScancodes[ "MAC BACKQUOTE" ] = KEY_BACKQUOTE ;
        namesToScancodes[ "mac semicolon" ] = namesToScancodes[ "Mac Semicolon" ] = namesToScancodes[ "MAC SEMICOLON" ] = KEY_SEMICOLON ;
        namesToScancodes[ "mac command" ] = namesToScancodes[ "Mac Command" ] = namesToScancodes[ "MAC COMMAND" ] = KEY_COMMAND ;
        namesToScancodes[ "none" ] = namesToScancodes[ "None" ] = namesToScancodes[ "NONE" ] = 0 ;

        initialized = true ;

#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        std::cout << "allegro is initialized" << std::endl ;
#endif
}

void setDefaultColorDepth( unsigned int depth )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::setDefaultColorDepth before allegro::init" << std::endl ; return ;  }
#endif

        set_color_depth( depth );
}

bool switchToFullscreenVideo( unsigned int width, unsigned int height )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::switchToFullscreenVideo before allegro::init" << std::endl ; return false ;  }
#endif

        int magicCard = GFX_AUTODETECT_FULLSCREEN ;
        return set_gfx_mode( magicCard, width, height, 0, 0 ) == /* okay */ 0 ;
}

bool switchToFullscreenVideo()
{
        // screen is allegro’s global variable
        return switchToFullscreenVideo( screen->w, screen->h ) ;
}

bool switchToWindowedVideo( unsigned int width, unsigned int height )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::switchToWindowedVideo before allegro::init" << std::endl ; return false ;  }
#endif

        int magicCard = GFX_AUTODETECT_WINDOWED ;
        return set_gfx_mode( magicCard, width, height, 0, 0 ) == /* okay */ 0 ;
}

bool switchToWindowedVideo()
{
        // screen is allegro’s global variable
        return switchToWindowedVideo( screen->w, screen->h ) ;
}

void setTitleOfAllegroWindow( const std::string& title )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::setTitleOfAllegroWindow before allegro::init" << std::endl ; return ;  }
#endif

        set_window_title( title.c_str () );
}

void drawLine( const Pict& where, int xFrom, int yFrom, int xTo, int yTo, int allegroColor )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::drawLine before allegro::init" << std::endl ; return ;  }
#endif

        line( where.ptr (), xFrom, yFrom, xTo, yTo, allegroColor );
}

void drawCircle( const Pict& where, int x, int y, int radius, int allegroColor )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::drawCircle before allegro::init" << std::endl ; return ;  }
#endif

        circle( where.ptr (), x, y, radius, allegroColor );
}

void fillCircle( const Pict& where, int x, int y, int radius, int allegroColor )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::fillCircle before allegro::init" << std::endl ; return ;  }
#endif

        circlefill( where.ptr (), x, y, radius, allegroColor );
}

void drawRect( const Pict& where, int x1, int y1, int x2, int y2, int allegroColor )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::drawRect before allegro::init" << std::endl ; return ;  }
#endif

        rect( where.ptr (), x1, y1, x2, y2, allegroColor );
}

void fillRect( const Pict& where, int x1, int y1, int x2, int y2, int allegroColor )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::fillRect before allegro::init" << std::endl ; return ;  }
#endif

        rectfill( where.ptr (), x1, y1, x2, y2, allegroColor );
}

void bitBlit( const Pict& from, const Pict& to, int fromX, int fromY, int toX, int toY, unsigned int width, unsigned int height )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::bitBlit before allegro::init" << std::endl ; return ;  }
#endif

        blit( from.ptr (), to.ptr (), fromX, fromY, toX, toY, width, height );
}

void bitBlit( const Pict& from, const Pict& to )
{
        bitBlit( from, to, 0, 0, 0, 0, from.getW(), from.getH() ) ;
}

void drawSprite( const Pict& view, const Pict& sprite, int x, int y )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::drawSprite before allegro::init" << std::endl ; return ;  }
#endif

        draw_sprite( view.ptr (), sprite.ptr (), x, y );
}

void drawSpriteWithTransparencyBlender( const Pict& view, const Pict& sprite, int x, int y, int r, int g, int b, int a )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::drawSpriteWithTransparencyBlender before allegro::init" << std::endl ; return ;  }
#endif

        set_trans_blender( r, g, b, a );
        draw_trans_sprite( view.ptr (), sprite.ptr (), x, y );
}

void stretchBlit( const Pict& source, const Pict& dest, int sX, int sY, int sW, int sH, int dX, int dY, int dW, int dH )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::stretchBlit before allegro::init" << std::endl ; return ;  }
#endif

        stretch_blit( source.ptr (), dest.ptr (), sX, sY, sW, sH, dX, dY, dW, dH );
}

int getRed ( int color ) {  return getr( color );  }

int getGreen ( int color ) {  return getg( color ); }

int getBlue ( int color ) {  return getb( color ); }

int getAlpha ( int color ) {  return geta( color );  }

int makeColor( int r, int g, int b )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::makeColor before allegro::init" << std::endl ; return -1 ;  }
#endif

        return makecol( r, g, b );
}

void textOut( const std::string& text, const Pict& where, int x, int y, int allegroColor )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::textOut before allegro::init" << std::endl ; return ;  }
#endif

        textout_ex( where.ptr (), /* allegro’s global */ font, text.c_str(), x, y, allegroColor, -1 );
}

void acquirePict( const Pict& what )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::acquirePict before allegro::init" << std::endl ; return ;  }
#endif

        acquire_bitmap( what.ptr () );
}

void releasePict( const Pict& what )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::releasePict before allegro::init" << std::endl ; return ;  }
#endif

        release_bitmap( what.ptr () );
}

void initTimers()
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::initTimer before allegro::init" << std::endl ; return ;  }
#endif

        install_timer() ;
}

void initAudio()
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::initAudio before allegro::init" << std::endl ; return ;  }
#endif

        alogg_init();

        allegro::initTimers();

        int digitalDrivers[] = {  DIGI_AUTODETECT,
        # ifdef __gnu_linux__
                                  DIGI_ALSA,
                                  DIGI_OSS,
                                  DIGI_ESD,
                                  DIGI_ARTS,
                                  DIGI_JACK,
        # endif
                                  DIGI_NONE  };

        int index = 0;
        int result = -1;

        while ( result != 0 )
        {
                result = install_sound( digitalDrivers[ index++ ], MIDI_NONE, NULL );
        }

        audioInitialized = true ;

#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        std::cout << "allegro audio is initialized" << std::endl ;
#endif
}

void setDigitalVolume( unsigned short volume )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! audioInitialized ) {  std::cerr << "allegro::setDigitalVolume before allegro::initAudio" << std::endl ; return ;  }
#endif

        set_volume( ( volume <= 255 ) ? volume : 255, -1 ) ;
}

void setMIDIVolume( unsigned short volume )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! audioInitialized ) {  std::cerr << "allegro::setMIDIVolume before allegro::initAudio" << std::endl ; return ;  }
#endif

        set_volume( -1, ( volume <= 255 ) ? volume : 255 ) ;
}

void initKeyboardHandler()
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::initKeyboardHandler before allegro::init" << std::endl ; return ;  }
#endif

        install_keyboard() ;
}

std::string scancodeToNameOfKey( int scancode )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::scancodeToNameOfKey before allegro::init" << std::endl ; return "no key" ;  }
#endif

        if ( scancodesToNames.find( scancode ) != scancodesToNames.end () )
                return scancodesToNames[ scancode ];

        return "unknown";
}

int nameOfKeyToScancode( const std::string& name )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::nameOfKeyToScancode before allegro::init" << std::endl ; return 0 ;  }
#endif

        if ( namesToScancodes.find( name ) != namesToScancodes.end () )
                return namesToScancodes[ name ];

        return KEY_UNKNOWN5 /* 111 */ ;
}

bool isKeyPushed( const std::string& name )
{
        // key is allegro’s global array
        return key[ nameOfKeyToScancode( name ) ] == TRUE ;
}

bool isShiftKeyPushed()
{
        return ( key_shifts & KB_SHIFT_FLAG ) != 0 ;
}

bool isControlKeyPushed()
{
        return ( key_shifts & KB_CTRL_FLAG ) != 0 ;
}

bool isAltKeyPushed()
{
        return ( key_shifts & KB_ALT_FLAG ) != 0 ;
}

void releaseKey( const std::string& name )
{
        // key is allegro’s global
        key[ nameOfKeyToScancode( name ) ] = FALSE ;
}

bool areKeypushesWaiting()
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::areKeypushesWaiting before allegro::init" << std::endl ; return false ;  }
#endif

        return keypressed() == TRUE ;
}

std::string nextKey()
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::nextKey before allegro::init" << std::endl ; return "no key" ;  }
#endif

        int keycode = readkey() >> 8 ;
        return scancodeToNameOfKey( keycode );
}

void emptyKeyboardBuffer ()
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::emptyKeyboardBuffer before allegro::init" << std::endl ; return ;  }
#endif

        clear_keybuf();
}

Pict* loadPNG( const std::string& file )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::loadPNG before allegro::init" << std::endl ; return Pict::nilPict() ;  }
#endif

        return Pict::newPict( load_png( file.c_str (), NULL ) );
}

int loadGIFAnimation( const std::string& gifFile, std::vector< Pict* >& frames, std::vector< int >& durations )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::loadGIFAnimation before allegro::init" << std::endl ; return 0 ;  }
#endif

        BITMAP** theseFrames = NULL;
        int* theseDurations = NULL;
        int howManyFrames = algif_load_animation( gifFile.c_str (), &theseFrames, &theseDurations );

        if ( howManyFrames > 0 )
        {
                for ( int i = 0; i < howManyFrames; i++ )
                {
                        frames.push_back( Pict::newPict( theseFrames[ i ] ) );
                        durations.push_back( theseDurations[ i ] );
                }

                free( theseDurations );
        }

        return howManyFrames;
}

void savePictAsPCX ( const std::string& where, const Pict& what )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::savePictAsPCX before allegro::init" << std::endl ; return ;  }
#endif

        save_bitmap( ( where + ".pcx" ).c_str (), what.ptr (), NULL );
}

#endif

}
