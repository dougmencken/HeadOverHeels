
#include "WrappersAllegro.hpp"

#include <algorithm>

#include <iostream>

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

#include <allegro5/allegro.h>

#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include <algif5/algif.h>

#include <queue>

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

#include <allegro.h>

#ifdef __WIN32
  #include <winalleg.h>
#endif

#include <loadpng/loadpng.h>

#include <algif/algif.h>

#include <alogg/aloggpth.h>
#include <alogg/aloggint.h>

#endif

#include "Timer.hpp"

#ifdef DEBUG
#  define DEBUG_ALLEGRO_INIT    0
#endif

#define RECORD_EACH_FRAME       0

#if defined( RECORD_EACH_FRAME ) && RECORD_EACH_FRAME
    # include <sstream>
#endif


unsigned char AllegroColor::getRed() const
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        unsigned char red, green, blue, alpha ;
        al_unmap_rgba( color, &red, &green, &blue, &alpha );
        return red ;

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        return getr( color );

#endif
}

unsigned char AllegroColor::getGreen() const
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        unsigned char red, green, blue, alpha ;
        al_unmap_rgba( color, &red, &green, &blue, &alpha );
        return green ;

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        return getg( color );

#endif
}

unsigned char AllegroColor::getBlue() const
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        unsigned char red, green, blue, alpha ;
        al_unmap_rgba( color, &red, &green, &blue, &alpha );
        return blue ;

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        return getb( color );

#endif
}

unsigned char AllegroColor::getAlpha() const
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        unsigned char red, green, blue, alpha ;
        al_unmap_rgba( color, &red, &green, &blue, &alpha );
        return alpha ;

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        return geta( color );

#endif
}

bool AllegroColor::equalsRGBA( const AllegroColor& toCompare ) const
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        unsigned char red, green, blue, alpha ;
        al_unmap_rgba( color, &red, &green, &blue, &alpha );

        unsigned char redToCompare, greenToCompare, blueToCompare, alphaToCompare ;
        al_unmap_rgba( toCompare, &redToCompare, &greenToCompare, &blueToCompare, &alphaToCompare );

        return red == redToCompare && green == greenToCompare && blue == blueToCompare && alpha == alphaToCompare ;

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        return getr( color ) == getr( toCompare ) && getg( color ) == getg( toCompare ) &&
                getb( color ) == getb( toCompare ) && geta( color ) == geta( toCompare ) ;

#endif
}

bool AllegroColor::equalsRGB( const AllegroColor& toCompare ) const
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        unsigned char red, green, blue ;
        al_unmap_rgb( color, &red, &green, &blue );

        unsigned char redToCompare, greenToCompare, blueToCompare ;
        al_unmap_rgb( toCompare, &redToCompare, &greenToCompare, &blueToCompare );

        return red == redToCompare && green == greenToCompare && blue == blueToCompare ;

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        return getr( color ) == getr( toCompare ) &&
                getg( color ) == getg( toCompare ) &&
                getb( color ) == getb( toCompare ) ;

#endif
}

/* static */
AllegroColor AllegroColor::makeColor( unsigned char r, unsigned char g, unsigned char b, unsigned char a )
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        return al_map_rgba( r, g, b, a );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        return makeacol( r, g, b, a );

#endif
}

/* static */
AllegroColor AllegroColor::makeColor( unsigned char r, unsigned char g, unsigned char b )
{
        return AllegroColor::makeColor( r, g, b, 0xff );
}

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

/* static */ unsigned char AllegroColor::redOfKeyColor = 0 ;
/* static */ unsigned char AllegroColor::greenOfKeyColor = 0 ;
/* static */ unsigned char AllegroColor::blueOfKeyColor = 0 ;
/* static */ unsigned char AllegroColor::alphaOfKeyColor =  0 ;

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

/* static */ unsigned char AllegroColor::redOfKeyColor = 255 ;
/* static */ unsigned char AllegroColor::greenOfKeyColor = 0 ;
/* static */ unsigned char AllegroColor::blueOfKeyColor = 255 ;
/* static */ unsigned char AllegroColor::alphaOfKeyColor =  0 ;

#endif

/* static */
AllegroColor AllegroColor::keyColor()
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        return al_map_rgba( redOfKeyColor, greenOfKeyColor, blueOfKeyColor, alphaOfKeyColor ) ;

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        return makeacol( redOfKeyColor, greenOfKeyColor, blueOfKeyColor, alphaOfKeyColor ) ;

#endif
}

bool AllegroColor::isKeyColor () const
{
        return   getRed() == redOfKeyColor   &&
               getGreen() == greenOfKeyColor &&
                getBlue() == blueOfKeyColor  &&
               getAlpha() == alphaOfKeyColor ;
}


namespace allegro
{

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

ALLEGRO_DISPLAY * allegroDisplay ;

#endif

Pict* Pict::whereToDraw = new Pict( NULL );

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

Pict* Pict::globalScreen = new Pict( NULL );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

Pict* Pict::globalScreen = new Pict( /* allegro’s global variable */ screen );

#endif

Pict::Pict( unsigned int w, unsigned int h )
        : shallowCopy( false )
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        al_set_new_bitmap_format( ALLEGRO_PIXEL_FORMAT_RGBA_8888 );
        al_set_new_bitmap_flags( ALLEGRO_MEMORY_BITMAP | ALLEGRO_NO_PREMULTIPLIED_ALPHA | ALLEGRO_ALPHA_TEST | ALLEGRO_NO_PRESERVE_TEXTURE );
        it = al_create_bitmap( w, h );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        it = create_bitmap( w, h );

#endif
}

Pict::~Pict( )
{
        if ( ! isNotNil() ) return ;
        if ( shallowCopy ) return ;

        // nullify it first, then invoke destroy_bitmap
        // to avoid drawing of being-destroyed thing when there’re many threads

        AllegroBitmap* toBin = it;
        it = NULL;

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        al_destroy_bitmap( toBin );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        destroy_bitmap( toBin );

#endif
}

unsigned int Pict::getW() const
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        return ( isNotNil() ) ? al_get_bitmap_width( it ) : 0 ;

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        return ( isNotNil() ) ? it->w : 0 ;

#endif
}

unsigned int Pict::getH() const
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        return ( isNotNil() ) ? al_get_bitmap_height( it ) : 0 ;

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        return ( isNotNil() ) ? it->h : 0 ;

#endif
}

AllegroColor Pict::getPixelAt( int x, int y ) const
{
        if ( ! isNotNil() ) return AllegroColor::keyColor() ;

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        return al_get_pixel( it, x, y ) ;

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        return getpixel( it, x, y ) ;

#endif
}

void Pict::putPixelAt( int x, int y, AllegroColor color ) const
{
        if ( ! isNotNil() ) return ;

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        AllegroBitmap* previous = al_get_target_bitmap() ;
        if ( previous != it ) al_set_target_bitmap( it ) ;
        al_put_pixel( x, y, color );
        if ( previous != it ) al_set_target_bitmap( previous );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        putpixel( it, x, y, color ) ;

#endif
}

void Pict::drawPixelAt( int x, int y, AllegroColor color ) const
{
        if ( ! isNotNil() ) return ;

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        AllegroBitmap* previous = al_get_target_bitmap() ;
        if ( previous != it ) al_set_target_bitmap( it ) ;
        al_draw_filled_rectangle( x, y, x + 1.0, y + 1.0, color );
        ///al_draw_filled_circle( x + 0.5, y + 0.5, 1.0, color );
        if ( previous != it ) al_set_target_bitmap( previous ) ;

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        putpixel( it, x, y, color ) ;

#endif
}

void Pict::clearToColor( AllegroColor color ) const
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        AllegroBitmap* previous = al_get_target_bitmap() ;
        if ( previous != it ) al_set_target_bitmap( it ) ;
        al_clear_to_color( color );
        if ( previous != it ) al_set_target_bitmap( previous );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        clear_to_color( it, color );

#endif
}

void Pict::lockReadOnly() const
{
        if ( ! isNotNil() ) return ;

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        al_lock_bitmap( it, al_get_bitmap_format( it ), ALLEGRO_LOCK_READONLY );

#endif
}

void Pict::lockReadWrite() const
{
        if ( ! isNotNil() ) return ;

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        al_lock_bitmap( it, al_get_bitmap_format( it ), ALLEGRO_LOCK_READWRITE );

#endif
}

void Pict::unlock() const
{
        if ( ! isNotNil() ) return ;

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        al_unlock_bitmap( it );

#endif
}

/* static */ Pict* Pict::asCloneOf( AllegroBitmap* image )
{
        if ( image == NULL ) return nilPict() ;

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        Pict* clone = Pict::newPict( al_clone_bitmap( image ) );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        Pict* clone = Pict::newPict( image->w, image->h );
        blit( image, clone->it, 0, 0, 0, 0, image->w, image->h ) ;

#endif

        return clone ;
}

/* static */ Pict* Pict::mendIntoPict( AllegroBitmap* image )
{
        if ( image == NULL ) return nilPict() ;

        AllegroColor transparent = AllegroColor::keyColor();

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        /* al_convert_mask_to_alpha( image, al_map_rgb( 255, 0, 255 ) ); */

        unsigned int imageWidth = al_get_bitmap_width( image );
        unsigned int imageHeight = al_get_bitmap_height( image );

        Pict* pict = newPict( imageWidth, imageHeight );

        AllegroBitmap* previous = al_get_target_bitmap() ;
        al_set_target_bitmap( pict->it ) ;

        al_lock_bitmap( image, al_get_bitmap_format( image ), ALLEGRO_LOCK_READONLY );
        al_lock_bitmap( pict->it, al_get_bitmap_format( pict->it ), ALLEGRO_LOCK_WRITEONLY );

        for ( unsigned int y = 0; y < imageHeight; y++ ) {
                for ( unsigned int x = 0; x < imageWidth; x++ )
                {
                        AllegroColor pixel = al_get_pixel( image, x, y );

                        unsigned char red, green, blue, alpha ;
                        al_unmap_rgba( pixel, &red, &green, &blue, &alpha );

                        if ( red == 255 && green == 0 && blue == 255 )
                        {
                                al_put_pixel( x, y, transparent );
                        }
                        else /* if ( ! ( red == 255 && green == 0 && blue == 255 ) ) */
                        {
                                al_put_pixel( x, y, al_map_rgba( red, green, blue, alpha ) );
                        }
                }
        }

        al_unlock_bitmap( image );
        al_unlock_bitmap( pict->it );

        al_destroy_bitmap( image );

        al_set_target_bitmap( previous );

        return pict ;

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        unsigned int imageWidth = image->w ;
        unsigned int imageHeight = image->h ;

        Pict* pict = newPict( imageWidth, imageHeight );

        for ( unsigned int y = 0; y < imageHeight; y++ )
        {
                for ( unsigned int x = 0; x < imageWidth; x++ )
                {
                        AllegroColor pixel = getpixel( image, x, y );

                        unsigned char red = getr( pixel );
                        unsigned char green = getg( pixel );
                        unsigned char blue = getb( pixel );

                        if ( red == 255 && green == 0 && blue == 255 )
                                putpixel( pict->it, x, y, transparent );
                        else
                                putpixel( pict->it, x, y, makeacol( red, green, blue, 0xff ) );
                }
        }

        destroy_bitmap( image );

        return pict ;

#endif
}

/* static */ Pict* Pict::fromPNGFile( const std::string& file )
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        AllegroBitmap* image = al_load_bitmap( file.c_str () );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        AllegroBitmap* image = load_png( file.c_str (), NULL );

#endif

        return Pict::mendIntoPict( image ) ;
}

/* static */ const Pict& Pict::theScreen ()
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        globalScreen->it = al_get_backbuffer( allegroDisplay != NULL ? allegroDisplay : al_get_current_display() );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        globalScreen->it = /* allegro’s global variable */ screen ; // update it before returning

#endif

        globalScreen->shallowCopy = true ;
        return *globalScreen ;
}

/* static */ const Pict & Pict::getWhereToDraw ()
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        whereToDraw->it = al_get_target_bitmap() ;

#endif

        whereToDraw->shallowCopy = true ;
        return *whereToDraw ;
}

/* static */ void Pict::setWhereToDraw( const Pict & where )
{
        if ( whereToDraw->it == where.it ) return ;

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        al_set_target_bitmap( where.it ) ;

#endif

        whereToDraw->it = where.it ;
        whereToDraw->shallowCopy = true ;
}


unsigned short Audio::digitalVolume = 200 ;

unsigned short Audio::midiVolume = 200 ;


Sample::~Sample( )
{
        if ( ! isNotNil() ) return ;
        if ( shallowCopy ) return ;

        stop();

        AllegroSample* toBin = it;
        it = NULL;

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        al_destroy_sample( toBin );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        destroy_sample( toBin );

#endif
}

unsigned int Sample::getFrequency() const
{
        if ( voice < 0 ) return 0 ;

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        return al_get_sample_frequency( it );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        int frequency = voice_get_frequency( voice );
        return ( frequency > 0 ) ? frequency : 0 ;

#endif
}

void Sample::play( unsigned short pan )
{
        playMe( this->volume, pan, false );
}

void Sample::loop( unsigned short pan )
{
        playMe( this->volume, pan, true );
}

/* private */ void Sample::playMe( unsigned short volume, unsigned short pan, bool loop )
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        if ( ! isPlaying() )
        {
                sampleInstance = al_create_sample_instance( it );
                al_attach_sample_instance_to_mixer( sampleInstance, al_get_default_mixer() );

                al_set_sample_instance_gain( sampleInstance, volume / 255.0 );
                al_set_sample_instance_pan( sampleInstance, ( pan - 128 ) / 128.0 );
                al_set_sample_instance_speed( sampleInstance, 1.0 /* original frequency */ );
                al_set_sample_instance_playmode( sampleInstance, loop ? ALLEGRO_PLAYMODE_LOOP : ALLEGRO_PLAYMODE_ONCE );

                ///bool okay = al_play_sample( it, volume /* * 0.089 */ / 255.0, ( pan - 128 ) / 128.0, 1.0, ALLEGRO_PLAYMODE_ONCE, sampleID );
                bool okay = al_play_sample_instance( sampleInstance );
                if ( okay ) voice = 1 ;
        }
        else
                al_set_sample_instance_position( sampleInstance, 0 );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        if ( ! isPlaying() )
                voice = play_sample( it, volume, pan, 1000 /* frequency that sample was recorded at */, loop ? 1 : 0 );
        else
                voice_set_position( voice, 0 );

#endif
}

void Sample::stop()
{
        if ( ! isNotNil() || voice < 0 ) return ;

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        if ( sampleInstance == NULL ) return ;

        /* al_stop_sample( sampleID ); */
        al_stop_sample_instance( sampleInstance );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        stop_sample( it );

#endif

        neatenIfNotPlaying();
}

void Sample::neatenIfNotPlaying()
{
        if ( voice < 0 || isPlaying() ) return ;

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        if ( sampleInstance != NULL )
        {
                al_set_sample( sampleInstance, NULL );
                al_destroy_sample_instance( sampleInstance );
                sampleInstance = NULL;
        }

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        deallocate_voice( voice );

#endif

        voice = -1;
}

bool Sample::isPlaying() const
{
        if ( voice < 0 ) return false ;

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        if ( sampleInstance == NULL ) return false ;

        return al_get_sample_instance_playing( sampleInstance );
        /* return al_get_mixer_playing( al_get_default_mixer() ); */

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        // int voice_get_position( int voice )
        // returns the current position of voice in sample units or -1 if it has finished playing

        return voice_get_position( voice ) != -1 ;

#endif
}

/* static */
Sample* Sample::loadFromFile( const std::string& file )
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        return new Sample( al_load_sample( file.c_str () ) );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        return new Sample( load_sample( file.c_str () ) );

#endif
}


namespace ogg
{

/* static */ std::vector< OggPlayer * > OggPlayer::everyPlayer ;

void OggPlayer::play( const std::string& oggFile, bool loop )
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        if ( oggSample != NULL || sampleInstance != NULL || oggMixer != NULL ) return ;

        oggSample = al_load_sample( oggFile.c_str () );
        if ( oggSample == NULL ) return ;

        oggMixer = al_create_mixer (
                /* al_get_sample_frequency( oggSample ) */ al_get_mixer_frequency( al_get_default_mixer() ),
                ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2
        );
        if ( oggMixer == NULL )
        {
                al_destroy_sample( oggSample );
                oggSample = NULL ;
                return ;
        }

        sampleInstance = al_create_sample_instance( oggSample );
        if ( sampleInstance == NULL )
        {
                al_destroy_mixer( oggMixer );
                al_destroy_sample( oggSample );
                oggMixer = NULL ;
                oggSample = NULL ;
                return ;
        }

        al_attach_sample_instance_to_mixer( sampleInstance, oggMixer );
        al_attach_mixer_to_mixer( oggMixer, al_get_default_mixer() );

        al_set_sample_instance_gain( sampleInstance, Audio::digitalVolume / 255.0 );
        al_set_sample_instance_pan( sampleInstance, 0.0 );
        al_set_sample_instance_speed( sampleInstance, 1.0 /* original frequency */ );
        al_set_sample_instance_playmode( sampleInstance, loop ? ALLEGRO_PLAYMODE_LOOP : ALLEGRO_PLAYMODE_ONCE );

        al_play_sample_instance( sampleInstance );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        if ( th != NULL ) return ;

        // use buffer of 40 KiB
        const size_t lengthOfbuffer = 40 * 1024;

        stream = alogg_start_streaming( oggFile.c_str(), lengthOfbuffer );

        if ( stream == NULL ) return ;

        th = ( loop ) ? alogg_create_thread_which_loops( stream, oggFile.c_str(), lengthOfbuffer )
                        : alogg_create_thread( stream );
#endif

        filePlaying = oggFile ;
        everyPlayer.push_back( this );
}

void OggPlayer::stop()
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        if ( sampleInstance != NULL )
        {
                al_stop_sample_instance( sampleInstance );

                al_set_sample( sampleInstance, NULL );
                al_destroy_sample_instance( sampleInstance );

                sampleInstance = NULL;
        }

        if ( oggMixer != NULL )
        {
                al_detach_mixer( oggMixer );
                al_destroy_mixer( oggMixer );
                oggMixer = NULL ;
        }

        if ( oggSample != NULL )
        {
                al_destroy_sample( oggSample );
                oggSample = NULL ;
        }

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        if ( th != NULL )
        {
                alogg_stop_thread( th );
                /// while ( isPlaying() ) ;
                alogg_join_thread( th );

                alogg_thread* toBin = th;
                th = NULL;
                alogg_destroy_thread( toBin );
        }

#endif

        filePlaying.clear() ;
        everyPlayer.erase( std::remove( everyPlayer.begin (), everyPlayer.end (), this ), everyPlayer.end () );
}

bool OggPlayer::isPlaying()
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        return ( oggMixer != NULL ) ? al_get_mixer_playing( oggMixer ) : false ;

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        return ( th != NULL ) ? ( th->alive != 0 && th->stop == 0 ) : false ;

#endif
}

/* static */ void OggPlayer::syncPlayersWithDigitalVolume()
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        for ( std::vector < OggPlayer * >::iterator it = everyPlayer.begin (); it != everyPlayer.end () ; ++ it )
        {
                OggPlayer* player = *it ;
                if ( player != NULL && player->isPlaying() )
                        al_set_sample_instance_gain( player->sampleInstance, Audio::digitalVolume / 255.0 );
        }

#endif
}

}


#if defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

#define  ALLEGRO_KEY_A                  KEY_A
#define  ALLEGRO_KEY_B                  KEY_B
#define  ALLEGRO_KEY_C                  KEY_C
#define  ALLEGRO_KEY_D                  KEY_D
#define  ALLEGRO_KEY_E                  KEY_E
#define  ALLEGRO_KEY_F                  KEY_F
#define  ALLEGRO_KEY_G                  KEY_G
#define  ALLEGRO_KEY_H                  KEY_H
#define  ALLEGRO_KEY_I                  KEY_I
#define  ALLEGRO_KEY_J                  KEY_J
#define  ALLEGRO_KEY_K                  KEY_K
#define  ALLEGRO_KEY_L                  KEY_L
#define  ALLEGRO_KEY_M                  KEY_M
#define  ALLEGRO_KEY_N                  KEY_N
#define  ALLEGRO_KEY_O                  KEY_O
#define  ALLEGRO_KEY_P                  KEY_P
#define  ALLEGRO_KEY_Q                  KEY_Q
#define  ALLEGRO_KEY_R                  KEY_R
#define  ALLEGRO_KEY_S                  KEY_S
#define  ALLEGRO_KEY_T                  KEY_T
#define  ALLEGRO_KEY_U                  KEY_U
#define  ALLEGRO_KEY_V                  KEY_V
#define  ALLEGRO_KEY_W                  KEY_W
#define  ALLEGRO_KEY_X                  KEY_X
#define  ALLEGRO_KEY_Y                  KEY_Y
#define  ALLEGRO_KEY_Z                  KEY_Z
#define  ALLEGRO_KEY_0                  KEY_0
#define  ALLEGRO_KEY_1                  KEY_1
#define  ALLEGRO_KEY_2                  KEY_2
#define  ALLEGRO_KEY_3                  KEY_3
#define  ALLEGRO_KEY_4                  KEY_4
#define  ALLEGRO_KEY_5                  KEY_5
#define  ALLEGRO_KEY_6                  KEY_6
#define  ALLEGRO_KEY_7                  KEY_7
#define  ALLEGRO_KEY_8                  KEY_8
#define  ALLEGRO_KEY_9                  KEY_9
#define  ALLEGRO_KEY_ESCAPE             KEY_ESC
#define  ALLEGRO_KEY_TILDE              KEY_TILDE
#define  ALLEGRO_KEY_MINUS              KEY_MINUS
#define  ALLEGRO_KEY_EQUALS             KEY_EQUALS
#define  ALLEGRO_KEY_BACKSPACE          KEY_BACKSPACE
#define  ALLEGRO_KEY_TAB                KEY_TAB
#define  ALLEGRO_KEY_OPENBRACE          KEY_OPENBRACE
#define  ALLEGRO_KEY_CLOSEBRACE         KEY_CLOSEBRACE
#define  ALLEGRO_KEY_ENTER              KEY_ENTER
#define  ALLEGRO_KEY_SEMICOLON          KEY_COLON
#define  ALLEGRO_KEY_QUOTE              KEY_QUOTE
#define  ALLEGRO_KEY_BACKSLASH          KEY_BACKSLASH
#define  ALLEGRO_KEY_BACKSLASH2         KEY_BACKSLASH2
#define  ALLEGRO_KEY_COMMA              KEY_COMMA
#define  ALLEGRO_KEY_FULLSTOP           KEY_STOP
#define  ALLEGRO_KEY_SLASH              KEY_SLASH
#define  ALLEGRO_KEY_SPACE              KEY_SPACE
#define  ALLEGRO_KEY_LEFT               KEY_LEFT
#define  ALLEGRO_KEY_RIGHT              KEY_RIGHT
#define  ALLEGRO_KEY_UP                 KEY_UP
#define  ALLEGRO_KEY_DOWN               KEY_DOWN
#define  ALLEGRO_KEY_INSERT             KEY_INSERT
#define  ALLEGRO_KEY_DELETE             KEY_DEL
#define  ALLEGRO_KEY_HOME               KEY_HOME
#define  ALLEGRO_KEY_END                KEY_END
#define  ALLEGRO_KEY_PGUP               KEY_PGUP
#define  ALLEGRO_KEY_PGDN               KEY_PGDN
#define  ALLEGRO_KEY_F1                 KEY_F1
#define  ALLEGRO_KEY_F2                 KEY_F2
#define  ALLEGRO_KEY_F3                 KEY_F3
#define  ALLEGRO_KEY_F4                 KEY_F4
#define  ALLEGRO_KEY_F5                 KEY_F5
#define  ALLEGRO_KEY_F6                 KEY_F6
#define  ALLEGRO_KEY_F7                 KEY_F7
#define  ALLEGRO_KEY_F8                 KEY_F8
#define  ALLEGRO_KEY_F9                 KEY_F9
#define  ALLEGRO_KEY_F10                KEY_F10
#define  ALLEGRO_KEY_F11                KEY_F11
#define  ALLEGRO_KEY_F12                KEY_F12
#define  ALLEGRO_KEY_PRINTSCREEN        KEY_PRTSCR
#define  ALLEGRO_KEY_PAUSE              KEY_PAUSE
#define  ALLEGRO_KEY_PAD_0              KEY_0_PAD
#define  ALLEGRO_KEY_PAD_1              KEY_1_PAD
#define  ALLEGRO_KEY_PAD_2              KEY_2_PAD
#define  ALLEGRO_KEY_PAD_3              KEY_3_PAD
#define  ALLEGRO_KEY_PAD_4              KEY_4_PAD
#define  ALLEGRO_KEY_PAD_5              KEY_5_PAD
#define  ALLEGRO_KEY_PAD_6              KEY_6_PAD
#define  ALLEGRO_KEY_PAD_7              KEY_7_PAD
#define  ALLEGRO_KEY_PAD_8              KEY_8_PAD
#define  ALLEGRO_KEY_PAD_9              KEY_9_PAD
#define  ALLEGRO_KEY_PAD_SLASH          KEY_SLASH_PAD
#define  ALLEGRO_KEY_PAD_ASTERISK       KEY_ASTERISK
#define  ALLEGRO_KEY_PAD_MINUS          KEY_MINUS_PAD
#define  ALLEGRO_KEY_PAD_PLUS           KEY_PLUS_PAD
#define  ALLEGRO_KEY_PAD_DELETE         KEY_DEL_PAD
#define  ALLEGRO_KEY_PAD_ENTER          KEY_ENTER_PAD
#define  ALLEGRO_KEY_PAD_EQUALS         KEY_EQUALS_PAD
#define  ALLEGRO_KEY_ABNT_C1            KEY_ABNT_C1
#define  ALLEGRO_KEY_YEN                KEY_YEN
#define  ALLEGRO_KEY_KANA               KEY_KANA
#define  ALLEGRO_KEY_KANJI              KEY_KANJI
#define  ALLEGRO_KEY_CONVERT            KEY_CONVERT
#define  ALLEGRO_KEY_NOCONVERT          KEY_NOCONVERT
#define  ALLEGRO_KEY_AT                 KEY_AT
#define  ALLEGRO_KEY_CIRCUMFLEX         KEY_CIRCUMFLEX
#define  ALLEGRO_KEY_COLON2             KEY_COLON2
#define  ALLEGRO_KEY_BACKQUOTE          KEY_BACKQUOTE
#define  ALLEGRO_KEY_SEMICOLON2         KEY_SEMICOLON
#define  ALLEGRO_KEY_COMMAND            KEY_COMMAND
#define  ALLEGRO_KEY_UNKNOWN            KEY_UNKNOWN1

#define  ALLEGRO_KEYMOD_SHIFT           KB_SHIFT_FLAG
#define  ALLEGRO_KEYMOD_CTRL            KB_CTRL_FLAG
#define  ALLEGRO_KEYMOD_ALT             KB_ALT_FLAG

#endif

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

#ifndef TRUE
    #define TRUE        -1
    #define FALSE       0
#endif

Timer * keyRepeatTimer ;
const float firstRepeatDelay = 0.5 ;
const float nextRepeatDelay = 0.1 ;
bool firstRepeat ;

ALLEGRO_FONT * allegroFont ;

int key[ ALLEGRO_KEY_MAX ] ;

int key_shifts ;

static std::queue< int > keybuf ;

static ALLEGRO_MUTEX* keybuf_mutex ;

static ALLEGRO_EVENT_QUEUE* event_queue ;

///static void addChar( ALLEGRO_KEYBOARD_EVENT* keyboardEvent )
/* {
        if ( keyboardEvent->unichar == 0 || keyboardEvent->unichar > 255 ) return ;

        al_lock_mutex( keybuf_mutex );
        keybuf.push( keyboardEvent->unichar | ( ( keyboardEvent->keycode << 8 ) & 0xff00 ) );
        al_unlock_mutex( keybuf_mutex );
} */

static void addScancode( int scancode )
{
        al_lock_mutex( keybuf_mutex );
        keybuf.push( ( scancode << 8 ) & 0xff00 );
        al_unlock_mutex( keybuf_mutex );
}

static void peekKey( int scancode, bool repeat, ALLEGRO_KEYBOARD_STATE* ks )
{
        if ( al_key_down( ks, scancode ) )
        {
                if ( key[ scancode ] != TRUE )
                {
                        key[ scancode ] = TRUE ;

                        if ( repeat )
                        {
                                addScancode( scancode );
                                firstRepeat = true ;
                                keyRepeatTimer->go() ;
                        }
                }
                else if ( repeat )
                {
                        if ( ( firstRepeat && keyRepeatTimer->getValue() > firstRepeatDelay ) ||
                                ( ! firstRepeat && keyRepeatTimer->getValue() > nextRepeatDelay ) )
                        {
                                addScancode( scancode );
                                keyRepeatTimer->go() ;
                                firstRepeat = false ;
                        }
                }
        }
        else
        {
                key[ scancode ] = FALSE ;
        }
}

static void peekKeys( bool repeat )
{
        ALLEGRO_KEYBOARD_STATE ks ;
        al_get_keyboard_state( &ks );

        for ( int scancode = ALLEGRO_KEY_A ; scancode < ALLEGRO_KEY_MODIFIERS ; scancode ++ )
        {
                peekKey( scancode, repeat, &ks );
        }
}

void peekKeys()
{
        peekKeys( false );
}

void peekModifiers()
{
        key_shifts = 0;

        ALLEGRO_KEYBOARD_STATE ks ;
        al_get_keyboard_state( &ks );

        if ( al_key_down( &ks, ALLEGRO_KEY_LSHIFT ) || al_key_down( &ks, ALLEGRO_KEY_RSHIFT ) )
        {
                key_shifts |= ALLEGRO_KEYMOD_SHIFT ;
        }

        if ( al_key_down( &ks, ALLEGRO_KEY_LCTRL ) || al_key_down( &ks, ALLEGRO_KEY_RCTRL ) )
        {
                key_shifts |= ALLEGRO_KEYMOD_CTRL ;
        }

        if ( al_key_down( &ks, ALLEGRO_KEY_ALT ) || al_key_down( &ks, ALLEGRO_KEY_ALTGR ) )
        {
                key_shifts |= ALLEGRO_KEYMOD_ALT ;
        }
}

void pollEvents()
{
        ALLEGRO_EVENT event;

        while ( al_get_next_event( event_queue, &event ) )
        {
                switch ( event.type )
                {
                        case ALLEGRO_EVENT_KEY_DOWN:
                                /* key[ event.keyboard.keycode ] = TRUE ; */
                                break;

                        case ALLEGRO_EVENT_KEY_UP:
                                /* key[ event.keyboard.keycode ] = FALSE ; */
                                break;

                        case ALLEGRO_EVENT_KEY_CHAR:
                                /* addChar( &event.keyboard ); */
                                break;

                        case ALLEGRO_EVENT_DISPLAY_EXPOSE:
                                break;

                        case ALLEGRO_EVENT_DISPLAY_CLOSE:
                                break;
                }
        }

        peekKeys( true );
        peekModifiers();
}

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

void peekKeys()
{
}

void peekModifiers()
{
}

#endif


std::map < int, std::string > scancodesToNames ;

std::map < std::string, int > namesToScancodes ;

Timer * redrawTimer ;

const float redrawDelay = 0.04 ; /* 25 redraws per second */

bool initialized = false ;

bool audioInitialized = false ;


void init( )
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        al_init();

        allegroDisplay = al_get_current_display();

        al_init_font_addon() ;
        allegroFont = al_create_builtin_font();

        al_init_image_addon();
        al_init_primitives_addon();

        keybuf_mutex = al_create_mutex();

        event_queue = al_create_event_queue() ;
        if ( allegroDisplay != NULL ) al_register_event_source( event_queue, al_get_display_event_source( allegroDisplay ) );

        keyRepeatTimer = new Timer() ;
        keyRepeatTimer->go() ;

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        allegro_init();

        // 8 bits for each of three colors with 8 bits for alpha channel
        set_color_depth( 32 );

#ifdef __WIN32
        // when application loses focus, the game will continue in background
        // because there are threads that will continue even when the main thread pauses
        set_display_switch_mode( SWITCH_BACKGROUND );
#endif

        install_timer() ;

#endif

        redrawTimer = new Timer() ;
        redrawTimer->go() ;

        scancodesToNames[ ALLEGRO_KEY_A ] = "a" ;
        scancodesToNames[ ALLEGRO_KEY_B ] = "b" ;
        scancodesToNames[ ALLEGRO_KEY_C ] = "c" ;
        scancodesToNames[ ALLEGRO_KEY_D ] = "d" ;
        scancodesToNames[ ALLEGRO_KEY_E ] = "e" ;
        scancodesToNames[ ALLEGRO_KEY_F ] = "f" ;
        scancodesToNames[ ALLEGRO_KEY_G ] = "g" ;
        scancodesToNames[ ALLEGRO_KEY_H ] = "h" ;
        scancodesToNames[ ALLEGRO_KEY_I ] = "i" ;
        scancodesToNames[ ALLEGRO_KEY_J ] = "j" ;
        scancodesToNames[ ALLEGRO_KEY_K ] = "k" ;
        scancodesToNames[ ALLEGRO_KEY_L ] = "l" ;
        scancodesToNames[ ALLEGRO_KEY_M ] = "m" ;
        scancodesToNames[ ALLEGRO_KEY_N ] = "n" ;
        scancodesToNames[ ALLEGRO_KEY_O ] = "o" ;
        scancodesToNames[ ALLEGRO_KEY_P ] = "p" ;
        scancodesToNames[ ALLEGRO_KEY_Q ] = "q" ;
        scancodesToNames[ ALLEGRO_KEY_R ] = "r" ;
        scancodesToNames[ ALLEGRO_KEY_S ] = "s" ;
        scancodesToNames[ ALLEGRO_KEY_T ] = "t" ;
        scancodesToNames[ ALLEGRO_KEY_U ] = "u" ;
        scancodesToNames[ ALLEGRO_KEY_V ] = "v" ;
        scancodesToNames[ ALLEGRO_KEY_W ] = "w" ;
        scancodesToNames[ ALLEGRO_KEY_X ] = "x" ;
        scancodesToNames[ ALLEGRO_KEY_Y ] = "y" ;
        scancodesToNames[ ALLEGRO_KEY_Z ] = "z" ;
        scancodesToNames[ ALLEGRO_KEY_0 ] = "0" ;
        scancodesToNames[ ALLEGRO_KEY_1 ] = "1" ;
        scancodesToNames[ ALLEGRO_KEY_2 ] = "2" ;
        scancodesToNames[ ALLEGRO_KEY_3 ] = "3" ;
        scancodesToNames[ ALLEGRO_KEY_4 ] = "4" ;
        scancodesToNames[ ALLEGRO_KEY_5 ] = "5" ;
        scancodesToNames[ ALLEGRO_KEY_6 ] = "6" ;
        scancodesToNames[ ALLEGRO_KEY_7 ] = "7" ;
        scancodesToNames[ ALLEGRO_KEY_8 ] = "8" ;
        scancodesToNames[ ALLEGRO_KEY_9 ] = "9" ;
        scancodesToNames[ ALLEGRO_KEY_ESCAPE ] = "Escape" ;
        scancodesToNames[ ALLEGRO_KEY_TILDE ] = "Tilde" ;
        scancodesToNames[ ALLEGRO_KEY_MINUS ] = "Minus" ;
        scancodesToNames[ ALLEGRO_KEY_EQUALS ] = "Equals" ;
        scancodesToNames[ ALLEGRO_KEY_BACKSPACE ] = "Backspace" ;
        scancodesToNames[ ALLEGRO_KEY_TAB ] = "Tab" ;
        scancodesToNames[ ALLEGRO_KEY_OPENBRACE ] = "OpenBrace" ;
        scancodesToNames[ ALLEGRO_KEY_CLOSEBRACE ] = "CloseBrace" ;
        scancodesToNames[ ALLEGRO_KEY_ENTER ] = "Enter" ;
        scancodesToNames[ ALLEGRO_KEY_SEMICOLON ] = "Semicolon" ;
        scancodesToNames[ ALLEGRO_KEY_QUOTE ] = "Quote" ;
        scancodesToNames[ ALLEGRO_KEY_BACKSLASH ] = "Backslash" ;
        scancodesToNames[ ALLEGRO_KEY_BACKSLASH2 ] = "Backslash2" ;
        scancodesToNames[ ALLEGRO_KEY_COMMA ] = "Comma" ;
        scancodesToNames[ ALLEGRO_KEY_FULLSTOP ] = "Period" ;
        scancodesToNames[ ALLEGRO_KEY_SLASH ] = "Slash" ;
        scancodesToNames[ ALLEGRO_KEY_SPACE ] = "Space" ;
        scancodesToNames[ ALLEGRO_KEY_LEFT ] = "Left" ;
        scancodesToNames[ ALLEGRO_KEY_RIGHT ] = "Right" ;
        scancodesToNames[ ALLEGRO_KEY_UP ] = "Up" ;
        scancodesToNames[ ALLEGRO_KEY_DOWN ] = "Down" ;
        scancodesToNames[ ALLEGRO_KEY_INSERT ] = "Insert" ;
        scancodesToNames[ ALLEGRO_KEY_DELETE ] = "Delete" ;
        scancodesToNames[ ALLEGRO_KEY_HOME ] = "Home" ;
        scancodesToNames[ ALLEGRO_KEY_END ] = "End" ;
        scancodesToNames[ ALLEGRO_KEY_PGUP ] = "PageUp" ;
        scancodesToNames[ ALLEGRO_KEY_PGDN ] = "PageDown" ;
        scancodesToNames[ ALLEGRO_KEY_F1 ] = "F1" ;
        scancodesToNames[ ALLEGRO_KEY_F2 ] = "F2" ;
        scancodesToNames[ ALLEGRO_KEY_F3 ] = "F3" ;
        scancodesToNames[ ALLEGRO_KEY_F4 ] = "F4" ;
        scancodesToNames[ ALLEGRO_KEY_F5 ] = "F5" ;
        scancodesToNames[ ALLEGRO_KEY_F6 ] = "F6" ;
        scancodesToNames[ ALLEGRO_KEY_F7 ] = "F7" ;
        scancodesToNames[ ALLEGRO_KEY_F8 ] = "F8" ;
        scancodesToNames[ ALLEGRO_KEY_F9 ] = "F9" ;
        scancodesToNames[ ALLEGRO_KEY_F10 ] = "F10" ;
        scancodesToNames[ ALLEGRO_KEY_F11 ] = "F11" ;
        scancodesToNames[ ALLEGRO_KEY_F12 ] = "F12" ;
        scancodesToNames[ ALLEGRO_KEY_PRINTSCREEN ] = "PrintScreen" ;
        scancodesToNames[ ALLEGRO_KEY_PAUSE ] = "Pause" ;
        scancodesToNames[ ALLEGRO_KEY_PAD_0 ] = "Pad 0" ;
        scancodesToNames[ ALLEGRO_KEY_PAD_1 ] = "Pad 1" ;
        scancodesToNames[ ALLEGRO_KEY_PAD_2 ] = "Pad 2" ;
        scancodesToNames[ ALLEGRO_KEY_PAD_3 ] = "Pad 3" ;
        scancodesToNames[ ALLEGRO_KEY_PAD_4 ] = "Pad 4" ;
        scancodesToNames[ ALLEGRO_KEY_PAD_5 ] = "Pad 5" ;
        scancodesToNames[ ALLEGRO_KEY_PAD_6 ] = "Pad 6" ;
        scancodesToNames[ ALLEGRO_KEY_PAD_7 ] = "Pad 7" ;
        scancodesToNames[ ALLEGRO_KEY_PAD_8 ] = "Pad 8" ;
        scancodesToNames[ ALLEGRO_KEY_PAD_9 ] = "Pad 9" ;
        scancodesToNames[ ALLEGRO_KEY_PAD_SLASH ] = "Pad /" ;
        scancodesToNames[ ALLEGRO_KEY_PAD_ASTERISK ] = "Pad *" ;
        scancodesToNames[ ALLEGRO_KEY_PAD_MINUS ] = "Pad -" ;
        scancodesToNames[ ALLEGRO_KEY_PAD_PLUS ] = "Pad +" ;
        scancodesToNames[ ALLEGRO_KEY_PAD_DELETE ] = "Pad Decimal" ;
        scancodesToNames[ ALLEGRO_KEY_PAD_ENTER ] = "Pad Enter" ;
        scancodesToNames[ ALLEGRO_KEY_PAD_EQUALS ] = "Pad =" ;
        scancodesToNames[ ALLEGRO_KEY_ABNT_C1 ] = "AbntC1" ;
        scancodesToNames[ ALLEGRO_KEY_YEN ] = "Yen" ;
        scancodesToNames[ ALLEGRO_KEY_KANA ] = "Kana" ;
        scancodesToNames[ ALLEGRO_KEY_KANJI ] = "Kanji" ;
        scancodesToNames[ ALLEGRO_KEY_CONVERT ] = "Convert" ;
        scancodesToNames[ ALLEGRO_KEY_NOCONVERT ] = "NoConvert" ;
        scancodesToNames[ ALLEGRO_KEY_AT ] = "At" ;
        scancodesToNames[ ALLEGRO_KEY_CIRCUMFLEX ] = "Circumflex" ;
        scancodesToNames[ ALLEGRO_KEY_COLON2 ] = "Colon2" ;
        scancodesToNames[ ALLEGRO_KEY_BACKQUOTE ] = "Mac Backquote" ;
        scancodesToNames[ ALLEGRO_KEY_SEMICOLON2 ] = "Mac Semicolon" ;
        scancodesToNames[ ALLEGRO_KEY_COMMAND ] = "Mac Command" ;
        scancodesToNames[ 0 ] = "none" ;

        namesToScancodes[ "a" ] = namesToScancodes[ "A" ] = ALLEGRO_KEY_A ;
        namesToScancodes[ "b" ] = namesToScancodes[ "B" ] = ALLEGRO_KEY_B ;
        namesToScancodes[ "c" ] = namesToScancodes[ "C" ] = ALLEGRO_KEY_C ;
        namesToScancodes[ "d" ] = namesToScancodes[ "D" ] = ALLEGRO_KEY_D ;
        namesToScancodes[ "e" ] = namesToScancodes[ "E" ] = ALLEGRO_KEY_E ;
        namesToScancodes[ "f" ] = namesToScancodes[ "F" ] = ALLEGRO_KEY_F ;
        namesToScancodes[ "g" ] = namesToScancodes[ "G" ] = ALLEGRO_KEY_G ;
        namesToScancodes[ "h" ] = namesToScancodes[ "H" ] = ALLEGRO_KEY_H ;
        namesToScancodes[ "i" ] = namesToScancodes[ "I" ] = ALLEGRO_KEY_I ;
        namesToScancodes[ "j" ] = namesToScancodes[ "J" ] = ALLEGRO_KEY_J ;
        namesToScancodes[ "k" ] = namesToScancodes[ "K" ] = ALLEGRO_KEY_K ;
        namesToScancodes[ "l" ] = namesToScancodes[ "L" ] = ALLEGRO_KEY_L ;
        namesToScancodes[ "m" ] = namesToScancodes[ "M" ] = ALLEGRO_KEY_M ;
        namesToScancodes[ "n" ] = namesToScancodes[ "N" ] = ALLEGRO_KEY_N ;
        namesToScancodes[ "o" ] = namesToScancodes[ "O" ] = ALLEGRO_KEY_O ;
        namesToScancodes[ "p" ] = namesToScancodes[ "P" ] = ALLEGRO_KEY_P ;
        namesToScancodes[ "q" ] = namesToScancodes[ "Q" ] = ALLEGRO_KEY_Q ;
        namesToScancodes[ "r" ] = namesToScancodes[ "R" ] = ALLEGRO_KEY_R ;
        namesToScancodes[ "s" ] = namesToScancodes[ "S" ] = ALLEGRO_KEY_S ;
        namesToScancodes[ "t" ] = namesToScancodes[ "T" ] = ALLEGRO_KEY_T ;
        namesToScancodes[ "u" ] = namesToScancodes[ "U" ] = ALLEGRO_KEY_U ;
        namesToScancodes[ "v" ] = namesToScancodes[ "V" ] = ALLEGRO_KEY_V ;
        namesToScancodes[ "w" ] = namesToScancodes[ "W" ] = ALLEGRO_KEY_W ;
        namesToScancodes[ "x" ] = namesToScancodes[ "X" ] = ALLEGRO_KEY_X ;
        namesToScancodes[ "y" ] = namesToScancodes[ "Y" ] = ALLEGRO_KEY_Y ;
        namesToScancodes[ "z" ] = namesToScancodes[ "Z" ] = ALLEGRO_KEY_Z ;
        namesToScancodes[ "0" ] = ALLEGRO_KEY_0 ;
        namesToScancodes[ "1" ] = ALLEGRO_KEY_1 ;
        namesToScancodes[ "2" ] = ALLEGRO_KEY_2 ;
        namesToScancodes[ "3" ] = ALLEGRO_KEY_3 ;
        namesToScancodes[ "4" ] = ALLEGRO_KEY_4 ;
        namesToScancodes[ "5" ] = ALLEGRO_KEY_5 ;
        namesToScancodes[ "6" ] = ALLEGRO_KEY_6 ;
        namesToScancodes[ "7" ] = ALLEGRO_KEY_7 ;
        namesToScancodes[ "8" ] = ALLEGRO_KEY_8 ;
        namesToScancodes[ "9" ] = ALLEGRO_KEY_9 ;
        namesToScancodes[ "escape" ] = namesToScancodes[ "Escape" ] = namesToScancodes[ "ESCAPE" ] = namesToScancodes[ "ESC" ] = namesToScancodes[ "Esc" ] = namesToScancodes[ "esc" ] = ALLEGRO_KEY_ESCAPE ;
        namesToScancodes[ "~" ] = namesToScancodes[ "tilde" ] = namesToScancodes[ "Tilde" ] = namesToScancodes[ "TILDE" ] = ALLEGRO_KEY_TILDE ;
        namesToScancodes[ "-" ] = namesToScancodes[ "minus" ] = namesToScancodes[ "Minus" ] = namesToScancodes[ "MINUS" ] = ALLEGRO_KEY_MINUS ;
        namesToScancodes[ "=" ] = namesToScancodes[ "equals" ] = namesToScancodes[ "Equals" ] = namesToScancodes[ "EQUALS" ] = ALLEGRO_KEY_EQUALS ;
        namesToScancodes[ "backspace" ] = namesToScancodes[ "Backspace" ] = namesToScancodes[ "BACKSPACE" ] = ALLEGRO_KEY_BACKSPACE ;
        namesToScancodes[ "tab" ] = namesToScancodes[ "Tab" ] = namesToScancodes[ "TAB" ] = ALLEGRO_KEY_TAB ;
        namesToScancodes[ "[" ] = namesToScancodes[ "openbrace" ] = namesToScancodes[ "OpenBrace" ] = namesToScancodes[ "OPENBRACE" ] = ALLEGRO_KEY_OPENBRACE ;
        namesToScancodes[ "]" ] = namesToScancodes[ "closebrace" ] = namesToScancodes[ "CloseBrace" ] = namesToScancodes[ "CLOSEBRACE" ] = ALLEGRO_KEY_CLOSEBRACE ;
        namesToScancodes[ "enter" ] = namesToScancodes[ "Enter" ] = namesToScancodes[ "ENTER" ] = ALLEGRO_KEY_ENTER ;
        namesToScancodes[ ";" ] = namesToScancodes[ "semicolon" ] = namesToScancodes[ "Semicolon" ] = namesToScancodes[ "SEMICOLON" ] = ALLEGRO_KEY_SEMICOLON ;
        namesToScancodes[ "'" ] = namesToScancodes[ "\"" ] = namesToScancodes[ "quote" ] = namesToScancodes[ "Quote" ] = namesToScancodes[ "QUOTE" ] = ALLEGRO_KEY_QUOTE ;
        namesToScancodes[ "\\" ] = namesToScancodes[ "backslash" ] = namesToScancodes[ "Backslash" ] = namesToScancodes[ "BackSlash" ] = namesToScancodes[ "BACKSLASH" ] = ALLEGRO_KEY_BACKSLASH ;
        namesToScancodes[ "backslash2" ] = namesToScancodes[ "Backslash2" ] = namesToScancodes[ "BackSlash2" ] = namesToScancodes[ "BACKSLASH2" ] = ALLEGRO_KEY_BACKSLASH2 ;
        namesToScancodes[ "," ] = namesToScancodes[ "comma" ] = namesToScancodes[ "Comma" ] = namesToScancodes[ "COMMA" ] = ALLEGRO_KEY_COMMA ;
        namesToScancodes[ "." ] = namesToScancodes[ "period" ] = namesToScancodes[ "Period" ] = namesToScancodes[ "PERIOD" ] = ALLEGRO_KEY_FULLSTOP ;
        namesToScancodes[ "/" ] = namesToScancodes[ "slash" ] = namesToScancodes[ "Slash" ] = namesToScancodes[ "SLASH" ] = ALLEGRO_KEY_SLASH ;
        namesToScancodes[ " " ] = namesToScancodes[ "space" ] = namesToScancodes[ "Space" ] = namesToScancodes[ "SPACE" ] = ALLEGRO_KEY_SPACE ;
        namesToScancodes[ "left" ] = namesToScancodes[ "Left" ] = namesToScancodes[ "LEFT" ] = ALLEGRO_KEY_LEFT ;
        namesToScancodes[ "right" ] = namesToScancodes[ "Right" ] = namesToScancodes[ "RIGHT" ] = ALLEGRO_KEY_RIGHT ;
        namesToScancodes[ "up" ] = namesToScancodes[ "Up" ] = namesToScancodes[ "UP" ] = ALLEGRO_KEY_UP ;
        namesToScancodes[ "down" ] = namesToScancodes[ "Down" ] = namesToScancodes[ "DOWN" ] = ALLEGRO_KEY_DOWN ;
        namesToScancodes[ "insert" ] = namesToScancodes[ "Insert" ] = namesToScancodes[ "INSERT" ] = namesToScancodes[ "INS" ] = namesToScancodes[ "Ins" ] = namesToScancodes[ "ins" ] = ALLEGRO_KEY_INSERT ;
        namesToScancodes[ "delete" ] = namesToScancodes[ "Delete" ] = namesToScancodes[ "DELETE" ] = namesToScancodes[ "DEL" ] = namesToScancodes[ "Del" ] = namesToScancodes[ "del" ] = ALLEGRO_KEY_DELETE ;
        namesToScancodes[ "home" ] = namesToScancodes[ "Home" ] = namesToScancodes[ "HOME" ] = ALLEGRO_KEY_HOME ;
        namesToScancodes[ "end" ] = namesToScancodes[ "End" ] = namesToScancodes[ "END" ] = ALLEGRO_KEY_END ;
        namesToScancodes[ "pageup" ] = namesToScancodes[ "PageUp" ] = namesToScancodes[ "PAGEUP" ] = namesToScancodes[ "PGUP" ] = namesToScancodes[ "PgUp" ] = namesToScancodes[ "pgup" ] = ALLEGRO_KEY_PGUP ;
        namesToScancodes[ "pagedown" ] = namesToScancodes[ "PageDown" ] = namesToScancodes[ "PAGEDOWN" ] = namesToScancodes[ "PGDN" ] = namesToScancodes[ "PgDn" ] = namesToScancodes[ "pgdn" ] = ALLEGRO_KEY_PGDN ;
        namesToScancodes[ "F1" ] = namesToScancodes[ "f1" ] = ALLEGRO_KEY_F1 ;
        namesToScancodes[ "F2" ] = namesToScancodes[ "f2" ] = ALLEGRO_KEY_F2 ;
        namesToScancodes[ "F3" ] = namesToScancodes[ "f3" ] = ALLEGRO_KEY_F3 ;
        namesToScancodes[ "F4" ] = namesToScancodes[ "f4" ] = ALLEGRO_KEY_F4 ;
        namesToScancodes[ "F5" ] = namesToScancodes[ "f5" ] = ALLEGRO_KEY_F5 ;
        namesToScancodes[ "F6" ] = namesToScancodes[ "f6" ] = ALLEGRO_KEY_F6 ;
        namesToScancodes[ "F7" ] = namesToScancodes[ "f7" ] = ALLEGRO_KEY_F7 ;
        namesToScancodes[ "F8" ] = namesToScancodes[ "f8" ] = ALLEGRO_KEY_F8 ;
        namesToScancodes[ "F9" ] = namesToScancodes[ "f9" ] = ALLEGRO_KEY_F9 ;
        namesToScancodes[ "F10" ] = namesToScancodes[ "f10" ] = ALLEGRO_KEY_F10 ;
        namesToScancodes[ "F11" ] = namesToScancodes[ "f11" ] = ALLEGRO_KEY_F11 ;
        namesToScancodes[ "F12" ] = namesToScancodes[ "f12" ] = ALLEGRO_KEY_F12 ;
        namesToScancodes[ "printscreen" ] = namesToScancodes[ "PrintScreen" ] = namesToScancodes[ "PRINTSCREEN" ] = namesToScancodes[ "PRTSCR" ] = namesToScancodes[ "PrtScr" ] = namesToScancodes[ "prtscr" ] = ALLEGRO_KEY_PRINTSCREEN ;
        namesToScancodes[ "pause" ] = namesToScancodes[ "Pause" ] = namesToScancodes[ "PAUSE" ] = ALLEGRO_KEY_PAUSE ;
        namesToScancodes[ "Pad 0" ] = namesToScancodes[ "pad 0" ] = namesToScancodes[ "PAD 0" ] = ALLEGRO_KEY_PAD_0 ;
        namesToScancodes[ "Pad 1" ] = namesToScancodes[ "pad 1" ] = namesToScancodes[ "PAD 1" ] = ALLEGRO_KEY_PAD_1 ;
        namesToScancodes[ "Pad 2" ] = namesToScancodes[ "pad 2" ] = namesToScancodes[ "PAD 2" ] = ALLEGRO_KEY_PAD_2 ;
        namesToScancodes[ "Pad 3" ] = namesToScancodes[ "pad 3" ] = namesToScancodes[ "PAD 3" ] = ALLEGRO_KEY_PAD_3 ;
        namesToScancodes[ "Pad 4" ] = namesToScancodes[ "pad 4" ] = namesToScancodes[ "PAD 4" ] = ALLEGRO_KEY_PAD_4 ;
        namesToScancodes[ "Pad 5" ] = namesToScancodes[ "pad 5" ] = namesToScancodes[ "PAD 5" ] = ALLEGRO_KEY_PAD_5 ;
        namesToScancodes[ "Pad 6" ] = namesToScancodes[ "pad 6" ] = namesToScancodes[ "PAD 6" ] = ALLEGRO_KEY_PAD_6 ;
        namesToScancodes[ "Pad 7" ] = namesToScancodes[ "pad 7" ] = namesToScancodes[ "PAD 7" ] = ALLEGRO_KEY_PAD_7 ;
        namesToScancodes[ "Pad 8" ] = namesToScancodes[ "pad 8" ] = namesToScancodes[ "PAD 8" ] = ALLEGRO_KEY_PAD_8 ;
        namesToScancodes[ "Pad 9" ] = namesToScancodes[ "pad 9" ] = namesToScancodes[ "PAD 9" ] = ALLEGRO_KEY_PAD_9 ;
        namesToScancodes[ "Pad /" ] = namesToScancodes[ "pad /" ] = namesToScancodes[ "PAD /" ] = ALLEGRO_KEY_PAD_SLASH ;
        namesToScancodes[ "Pad *" ] = namesToScancodes[ "pad *" ] = namesToScancodes[ "PAD *" ] = ALLEGRO_KEY_PAD_ASTERISK ;
        namesToScancodes[ "Pad -" ] = namesToScancodes[ "pad -" ] = namesToScancodes[ "PAD -" ] = ALLEGRO_KEY_PAD_MINUS ;
        namesToScancodes[ "Pad +" ] = namesToScancodes[ "pad +" ] = namesToScancodes[ "PAD +" ] = ALLEGRO_KEY_PAD_PLUS ;
        namesToScancodes[ "Pad Decimal" ] = namesToScancodes[ "pad decimal" ] = namesToScancodes[ "PAD DECIMAL" ] = ALLEGRO_KEY_PAD_DELETE ;
        namesToScancodes[ "Pad Enter" ] = namesToScancodes[ "pad enter" ] = namesToScancodes[ "PAD ENTER" ] = ALLEGRO_KEY_PAD_ENTER ;
        namesToScancodes[ "Pad =" ] = namesToScancodes[ "pad =" ] = namesToScancodes[ "PAD =" ] = ALLEGRO_KEY_PAD_EQUALS ; // OS X
        namesToScancodes[ "AbntC1" ] = namesToScancodes[ "ABNT_C1" ] = ALLEGRO_KEY_ABNT_C1 ; // Japanese
        namesToScancodes[ "yen" ] = namesToScancodes[ "Yen" ] = namesToScancodes[ "YEN" ] = ALLEGRO_KEY_YEN ; // Japanese
        namesToScancodes[ "kana" ] = namesToScancodes[ "Kana" ] = namesToScancodes[ "KANA" ] = ALLEGRO_KEY_KANA ; // Japanese
        namesToScancodes[ "kanji" ] = namesToScancodes[ "Kanji" ] = namesToScancodes[ "KANJI" ] = ALLEGRO_KEY_KANJI ; // Japanese
        namesToScancodes[ "convert" ] = namesToScancodes[ "Convert" ] = namesToScancodes[ "CONVERT" ] = ALLEGRO_KEY_CONVERT ;
        namesToScancodes[ "noconvert" ] = namesToScancodes[ "NoConvert" ] = namesToScancodes[ "NOCONVERT" ] = ALLEGRO_KEY_NOCONVERT ;
        namesToScancodes[ "@" ] = namesToScancodes[ "at" ] = namesToScancodes[ "At" ] = namesToScancodes[ "AT" ] = ALLEGRO_KEY_AT ;
        namesToScancodes[ "^" ] = namesToScancodes[ "circumflex" ] = namesToScancodes[ "Circumflex" ] = namesToScancodes[ "CIRCUMFLEX" ] = ALLEGRO_KEY_CIRCUMFLEX ;
        namesToScancodes[ "colon2" ] = namesToScancodes[ "Colon2" ] = namesToScancodes[ "COLON2" ] = ALLEGRO_KEY_COLON2 ;
        namesToScancodes[ "mac backquote" ] = namesToScancodes[ "Mac Backquote" ] = namesToScancodes[ "MAC BACKQUOTE" ] = ALLEGRO_KEY_BACKQUOTE ;
        namesToScancodes[ "mac semicolon" ] = namesToScancodes[ "Mac Semicolon" ] = namesToScancodes[ "MAC SEMICOLON" ] = ALLEGRO_KEY_SEMICOLON2 ;
        namesToScancodes[ "mac command" ] = namesToScancodes[ "Mac Command" ] = namesToScancodes[ "MAC COMMAND" ] = ALLEGRO_KEY_COMMAND ;
        namesToScancodes[ "none" ] = namesToScancodes[ "None" ] = namesToScancodes[ "NONE" ] = 0 ;

        initialized = true ;

#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        std::cout << "allegro is initialized" << std::endl ;
#endif
}

void bye()
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        al_destroy_mutex( keybuf_mutex );
        keybuf_mutex = NULL ;

        if ( event_queue != NULL ) al_destroy_event_queue( event_queue );
        event_queue = NULL ;

        if ( audioInitialized ) al_uninstall_audio() ;

        if ( allegroDisplay != NULL ) al_destroy_display( allegroDisplay );

        delete keyRepeatTimer ;

#endif

        delete redrawTimer ;

        initialized = false ;
}

void update()
{
        if ( redrawTimer->getValue() > redrawDelay )
        {
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5
                al_flip_display() ;
#endif

#if defined( RECORD_EACH_FRAME ) && RECORD_EACH_FRAME
                static int number = 0;
                savePictAsPCX( "frame" + static_cast< std::ostringstream & >( std::ostringstream() << std::dec << ( ++ number ) ).str (), Pict::theScreen() );
#endif

                redrawTimer->go ();
        }
}

bool switchToFullscreenVideo( unsigned int width, unsigned int height )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::switchToFullscreenVideo before allegro::init" << std::endl ; return false ;  }
#endif

        bool result = false ;

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        if ( allegroDisplay == NULL )
        {
                al_set_new_display_flags( ALLEGRO_FULLSCREEN /* ALLEGRO_FULLSCREEN_WINDOW */ | ALLEGRO_GENERATE_EXPOSE_EVENTS );
                ALLEGRO_DISPLAY* newDisplay = al_create_display( width, height );
                if ( newDisplay == NULL ) return false ;
                al_register_event_source( event_queue, al_get_display_event_source( newDisplay ) );
                allegroDisplay = newDisplay ;
                result = true ;
        }
        else
        {
                al_resize_display( allegroDisplay, width, height );
                result = al_set_display_flag( allegroDisplay, ALLEGRO_FULLSCREEN_WINDOW, true );
        }

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        int magicCard = GFX_AUTODETECT_FULLSCREEN ;
        result = set_gfx_mode( magicCard, width, height, 0, 0 ) == /* okay */ 0 ;

#endif

        if ( result ) Pict::resetWhereToDraw();

        return result ;
}

bool switchToFullscreenVideo()
{
        return switchToFullscreenVideo( Pict::theScreen().getW(), Pict::theScreen().getH() ) ;
}

bool switchToWindowedVideo( unsigned int width, unsigned int height )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::switchToWindowedVideo before allegro::init" << std::endl ; return false ;  }
#endif

        bool result = false ;

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        if ( allegroDisplay == NULL )
        {
                al_set_new_display_flags( ALLEGRO_WINDOWED | ALLEGRO_GENERATE_EXPOSE_EVENTS );
                ALLEGRO_DISPLAY* newDisplay = al_create_display( width, height );
                if ( newDisplay == NULL ) return false ;
                al_register_event_source( event_queue, al_get_display_event_source( newDisplay ) );
                allegroDisplay = newDisplay ;
                result = true ;
        }
        else
        {
                al_resize_display( allegroDisplay, width, height );
                result = al_set_display_flag( allegroDisplay, ALLEGRO_FULLSCREEN_WINDOW, false );
        }

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        int magicCard = GFX_AUTODETECT_WINDOWED ;
        result = set_gfx_mode( magicCard, width, height, 0, 0 ) == /* okay */ 0 ;

#endif

        if ( result ) Pict::resetWhereToDraw();

        return result ;
}

bool switchToWindowedVideo()
{
        return switchToWindowedVideo( Pict::theScreen().getW(), Pict::theScreen().getH() ) ;
}

void setTitleOfAllegroWindow( const std::string& title )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::setTitleOfAllegroWindow before allegro::init" << std::endl ; return ;  }
#endif

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        if ( allegroDisplay != NULL ) al_set_window_title( allegroDisplay, title.c_str () );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        set_window_title( title.c_str () );

#endif
}

void setAllegroIcon( const Pict & icon )
{
        if ( ! icon.isNotNil() ) return ;

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        al_set_display_icon( allegroDisplay, icon.ptr () );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        // do nothing for allegro 4

#endif
}

void drawLine( int xFrom, int yFrom, int xTo, int yTo, AllegroColor color )
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        al_draw_line( xFrom + 0.5, yFrom + 0.5, xTo + 0.5, yTo + 0.5, color, /* thickness */ 1.0 );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        line( Pict::getWhereToDraw().ptr (), xFrom, yFrom, xTo, yTo, color );

#endif
}

void drawCircle( int x, int y, int radius, AllegroColor color )
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        al_draw_circle( x + 0.5, y + 0.5, radius, color, /* thickness */ 1.0 );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        circle( Pict::getWhereToDraw().ptr (), x, y, radius, color );

#endif
}

void fillCircle( int x, int y, int radius, AllegroColor color )
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        al_draw_filled_circle( x + 0.5, y + 0.5, radius, color );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        circlefill( Pict::getWhereToDraw().ptr (), x, y, radius, color );

#endif
}

void drawRect( int x1, int y1, int x2, int y2, AllegroColor color )
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        al_draw_rectangle( x1 + 0.5, y1 + 0.5, x2 + 0.5, y2 + 0.5, color, /* thickness */ 1.0 );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        rect( Pict::getWhereToDraw().ptr (), x1, y1, x2, y2, color );

#endif
}

void fillRect( const Pict & where, int x1, int y1, int x2, int y2, AllegroColor color )
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        AllegroBitmap* previous = al_get_target_bitmap () ;
        if ( previous != where.ptr() ) al_set_target_bitmap( where.ptr() ) ;
        al_draw_filled_rectangle( x1 + 0.5, y1 + 0.5, x2 + 0.5, y2 + 0.5, color );
        if ( previous != where.ptr() ) al_set_target_bitmap( previous ) ;

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        rectfill( where.ptr (), x1, y1, x2, y2, color );

#endif
}

void fillRect( int x1, int y1, int x2, int y2, AllegroColor color )
{
        fillRect( Pict::getWhereToDraw(), x1, y1, x2, y2, color );
}

void bitBlit( const Pict& from, int fromX, int fromY, int toX, int toY, unsigned int width, unsigned int height )
{
        if ( ! ( from.isNotNil() && Pict::getWhereToDraw().isNotNil() ) ) return ;

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        al_draw_bitmap_region( from.ptr(), fromX, fromY, width, height, toX, toY, /* flipping */ 0 );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        blit( from.ptr (), Pict::getWhereToDraw().ptr (), fromX, fromY, toX, toY, width, height );

#endif
}

void bitBlit( const Pict& from, int toX, int toY )
{
        if ( ! ( from.isNotNil() && Pict::getWhereToDraw().isNotNil() ) ) return ;

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        al_draw_bitmap( from.ptr(), toX, toY, /* flipping */ 0 );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        bitBlit( from, 0, 0, toX, toY, from.getW(), from.getH() ) ;

#endif
}

void bitBlit( const Pict& from, const Pict& to, int fromX, int fromY, int toX, int toY, unsigned int width, unsigned int height )
{
        if ( ! ( from.isNotNil() && to.isNotNil() ) ) return ;

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        AllegroBitmap* previous = al_get_target_bitmap() ;
        if ( previous != to.ptr() ) al_set_target_bitmap( to.ptr() ) ;
        al_draw_bitmap_region( from.ptr(), fromX, fromY, width, height, toX, toY, /* flipping */ 0 );
        if ( previous != to.ptr() ) al_set_target_bitmap( previous ) ;

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        blit( from.ptr (), to.ptr (), fromX, fromY, toX, toY, width, height );

#endif
}

void bitBlit( const Pict& from, const Pict& to, int toX, int toY )
{
        if ( ! ( from.isNotNil() && to.isNotNil() ) ) return ;

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        AllegroBitmap* previous = al_get_target_bitmap() ;
        if ( previous != to.ptr() ) al_set_target_bitmap( to.ptr() ) ;
        al_draw_bitmap( from.ptr(), toX, toY, /* flipping */ 0 );
        if ( previous != to.ptr() ) al_set_target_bitmap( previous ) ;

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        bitBlit( from, to, 0, 0, toX, toY, from.getW(), from.getH() ) ;

#endif
}

void bitBlit ( const Pict & from ) {  bitBlit( from, 0, 0 ) ;  }

void bitBlit ( const Pict & from, const Pict & to ) {  bitBlit( from, to, 0, 0 ) ;  }

void stretchBlit( const Pict& source, int sX, int sY, int sW, int sH, int dX, int dY, int dW, int dH )
{
        if ( ! ( source.isNotNil() && Pict::getWhereToDraw().isNotNil() ) ) return ;

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        al_draw_scaled_bitmap( source.ptr (), sX, sY, sW, sH, dX, dY, dW, dH, /* flipping */ 0 );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        stretch_blit( source.ptr (), Pict::getWhereToDraw().ptr (), sX, sY, sW, sH, dX, dY, dW, dH );

#endif
}

void stretchBlit( const Pict& source, const Pict& dest, int sX, int sY, int sW, int sH, int dX, int dY, int dW, int dH )
{
        if ( ! ( source.isNotNil() && dest.isNotNil() ) ) return ;

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        AllegroBitmap* previous = al_get_target_bitmap() ;
        if ( previous != dest.ptr() ) al_set_target_bitmap( dest.ptr () ) ;
        al_draw_scaled_bitmap( source.ptr (), sX, sY, sW, sH, dX, dY, dW, dH, /* flipping */ 0 );
        if ( previous != dest.ptr() ) al_set_target_bitmap( previous ) ;

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        stretch_blit( source.ptr (), dest.ptr (), sX, sY, sW, sH, dX, dY, dW, dH );

#endif
}

void drawSprite( const Pict& sprite, int x, int y )
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        bitBlit( sprite, x, y ) ;

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        if ( sprite.isNotNil() && Pict::getWhereToDraw().isNotNil() )
                draw_sprite( Pict::getWhereToDraw().ptr (), sprite.ptr (), x, y );

#endif
}

void drawSpriteWithTransparency( const Pict& sprite, int x, int y, unsigned char transparency )
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        al_draw_tinted_bitmap( sprite.ptr(), al_map_rgba( 0, 0, 0, 255 - transparency ), x, y, /* flipping */ 0 );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        set_trans_blender( 0, 0, 0, transparency );
        draw_trans_sprite( Pict::getWhereToDraw().ptr (), sprite.ptr (), x, y );

#endif
}

void textOut( const std::string& text, int x, int y, AllegroColor color )
{
#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        al_draw_text( allegroFont, color, x, y, ALLEGRO_ALIGN_LEFT, text.c_str() );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        textout_ex( Pict::getWhereToDraw().ptr (), /* allegro’s global */ font, text.c_str(), x, y, color, -1 );

#endif
}

#if defined( USE_ALLEGRO4 ) && USE_ALLEGRO4
std::string allegro4AudioDriverCodeToName( int code )
{
        if ( code == DIGI_AUTODETECT ) return "DIGI_AUTODETECT" ;

        if ( code == DIGI_ALSA ) return "DIGI_ALSA" ;
        if ( code == DIGI_OSS ) return "DIGI_OSS" ;
        if ( code == DIGI_ESD ) return "DIGI_ESD" ;
        if ( code == DIGI_ARTS ) return "DIGI_ARTS" ;
        if ( code == DIGI_JACK ) return "DIGI_JACK" ;

        if ( code == DIGI_NONE ) return "DIGI_NONE" ;

        return "unknown" ;
}
#endif

bool initAudio( const std::string & audioInterface )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::initAudio before allegro::init" << std::endl ; return false ;  }
#endif

        if ( audioInitialized ) return true ; // don't do this again

        int result = -2 ; // "not tried"

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        ( void ) audioInterface ; // unused with allegro 5

#if defined( DARWIN ) && DARWIN
        al_set_config_value( al_get_system_config(), "audio", "driver", "openal" );
#endif

        al_set_config_value( al_get_system_config(), "audio", "default_mixer_quality", /* "point" */ "linear" /* "cubic" */ );

        bool installed = al_install_audio() ;

        if ( installed )
        {
                al_init_acodec_addon() ;

                al_reserve_samples( 256 ) ;

                result = 0 ; // no error
        } else
                result = -1 ;

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        alogg_init();

        std::vector< int > digitalDrivers ;
        bool noAudio = false ;

        if (( audioInterface[ 0 ] == 'n' || audioInterface[ 0 ] == 'N' )
                        && ( audioInterface[ 1 ] == 'o' || audioInterface[ 1 ] == 'O' ))
                noAudio = true ; // "no" "none" "nope"

# ifdef __gnu_linux__
        else
         if (( audioInterface[ 0 ] == 'a' || audioInterface[ 0 ] == 'A' )
                && ( audioInterface[ 1 ] == 'l' || audioInterface[ 1 ] == 'L' )
                        && ( audioInterface[ 2 ] == 's' || audioInterface[ 2 ] == 'S' )
                                && ( audioInterface[ 3 ] == 'a' || audioInterface[ 3 ] == 'A' ))
                digitalDrivers.push_back( DIGI_ALSA );
        else
         if (( audioInterface[ 0 ] == 'o' || audioInterface[ 0 ] == 'O' )
                && ( audioInterface[ 1 ] == 's' || audioInterface[ 1 ] == 'S' )
                        && ( audioInterface[ 2 ] == 's' || audioInterface[ 2 ] == 'S' ))
                digitalDrivers.push_back( DIGI_OSS );
        else
         if (( audioInterface[ 0 ] == 'j' || audioInterface[ 0 ] == 'J' )
                && ( audioInterface[ 1 ] == 'a' || audioInterface[ 1 ] == 'A' )
                        && ( audioInterface[ 2 ] == 'c' || audioInterface[ 2 ] == 'C' )
                                && ( audioInterface[ 3 ] == 'k' || audioInterface[ 3 ] == 'K' ))
                digitalDrivers.push_back( DIGI_JACK );
        else
         if (( audioInterface[ 0 ] == 'e' || audioInterface[ 0 ] == 'E' )
                && ( audioInterface[ 1 ] == 's' || audioInterface[ 1 ] == 'S' )
                        && ( audioInterface[ 2 ] == 'd' || audioInterface[ 2 ] == 'D' ))
                digitalDrivers.push_back( DIGI_ESD );
        else
         if (( audioInterface[ 0 ] == 'a' || audioInterface[ 0 ] == 'A' )
                && ( audioInterface[ 1 ] == 'r' || audioInterface[ 1 ] == 'R' )
                        && ( audioInterface[ 2 ] == 't' || audioInterface[ 2 ] == 'T' )
                                && ( audioInterface[ 3 ] == 's' || audioInterface[ 3 ] == 'S' ))
                digitalDrivers.push_back( DIGI_ARTS );
# endif

        if ( ! noAudio && digitalDrivers.size () == 0 ) {
                // try auto audio
                digitalDrivers.push_back( DIGI_AUTODETECT );

        # ifdef __gnu_linux__
                digitalDrivers.push_back( DIGI_ALSA );
                digitalDrivers.push_back( DIGI_OSS );
                digitalDrivers.push_back( DIGI_JACK );
                digitalDrivers.push_back( DIGI_ESD );
                digitalDrivers.push_back( DIGI_ARTS );
        # endif
        }

        if ( ! noAudio )
        {
                for ( unsigned int index = 0 ; index < digitalDrivers.size () ; index ++ )
                {
        #if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
                        std::cout << "trying to install_sound( " << allegro4AudioDriverCodeToName( digitalDrivers[ index ] ) << " ) ...." ;
        #endif
                        result = install_sound( digitalDrivers[ index ], MIDI_NONE, NULL );
                        // install_sound returns zero on success, and -1 on failure

                        if ( result == 0 )
                        {
        #if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
                                std::cout << " okay" << std::endl ;
        #endif
                                break ;
                        } else
                        {
        #if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
                                std::cout << " oops" << std::endl ;
        #endif
                                std::string allegroError( allegro_error ) ;
                                std::cout << "install_sound( " << allegro4AudioDriverCodeToName( digitalDrivers[ index ] )
                                                << " ) returned error \"" << allegroError << "\"" << std::endl ;
                        }
                }
        }

        // if "no" audio or when nothing works, try DIGI_NONE
        if ( result != 0 )
                result = install_sound( DIGI_NONE, MIDI_NONE, NULL );
#endif

        if ( result == 0 ) {
                audioInitialized = true ;

        #if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
                std::cout << "allegro audio is initialized" << std::endl ;
        #endif
        }

        return audioInitialized ;
}

bool initAudio ()
{
        return initAudio( "auto" ) ;
}

void setDigitalVolume( unsigned short volume )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! audioInitialized ) {  std::cerr << "allegro::setDigitalVolume before allegro::initAudio" << std::endl ; return ;  }
#endif

        Audio::digitalVolume = ( volume <= 255 ) ? volume : 255 ;
        ogg::OggPlayer::syncPlayersWithDigitalVolume ();

#if defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        set_volume( Audio::digitalVolume, -1 ) ;

#endif
}

void setMIDIVolume( unsigned short volume )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! audioInitialized ) {  std::cerr << "allegro::setMIDIVolume before allegro::initAudio" << std::endl ; return ;  }
#endif

        Audio::midiVolume = ( volume <= 255 ) ? volume : 255 ;

#if defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        set_volume( -1, Audio::midiVolume ) ;

#endif
}

void initKeyboardHandler()
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::initKeyboardHandler before allegro::init" << std::endl ; return ;  }
#endif

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        al_install_keyboard() ;

        al_register_event_source( event_queue, al_get_keyboard_event_source() ) ;

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        install_keyboard() ;

#endif
}

std::string scancodeToNameOfKey( int scancode )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::scancodeToNameOfKey before allegro::init" << std::endl ; return "no key" ;  }
#endif

        if ( scancodesToNames.find( scancode ) != scancodesToNames.end () )
                return scancodesToNames[ scancode ];

        return "unknown" ;
}

int nameOfKeyToScancode( const std::string& name )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::nameOfKeyToScancode before allegro::init" << std::endl ; return 0 ;  }
#endif

        if ( namesToScancodes.find( name ) != namesToScancodes.end () )
                return namesToScancodes[ name ];

        return ALLEGRO_KEY_UNKNOWN ;
}

bool isKeyPushed( const std::string& name )
{
        peekKeys();

        // key is allegro4’s global array
        return key[ nameOfKeyToScancode( name ) ] == TRUE ;
}

bool isShiftKeyPushed()
{
        peekModifiers();
        return ( key_shifts & ALLEGRO_KEYMOD_SHIFT ) != 0 ;
}

bool isControlKeyPushed()
{
        peekModifiers();
        return ( key_shifts & ALLEGRO_KEYMOD_CTRL ) != 0 ;
}

bool isAltKeyPushed()
{
        peekModifiers();
        return ( key_shifts & ALLEGRO_KEYMOD_ALT ) != 0 ;
}

void releaseKey( const std::string& name )
{
        // key is allegro4’s global
        key[ nameOfKeyToScancode( name ) ] = FALSE ;
}

bool areKeypushesWaiting()
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::areKeypushesWaiting before allegro::init" << std::endl ; return false ;  }
#endif

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        pollEvents() ;
        return ! keybuf.empty() ;

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        return keypressed() == TRUE ;

#endif
}

std::string nextKey()
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::nextKey before allegro::init" << std::endl ; return "no key" ;  }
#endif

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        int keycode = 0 ;
        pollEvents();

        if ( ! keybuf.empty() )
        {
                al_lock_mutex( keybuf_mutex );
                keycode = keybuf.front();
                keybuf.pop();
                al_unlock_mutex( keybuf_mutex );
        }

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        int keycode = readkey() ;

#endif

        return scancodeToNameOfKey( keycode >> 8 ) ;
}

void emptyKeyboardBuffer ()
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::emptyKeyboardBuffer before allegro::init" << std::endl ; return ;  }
#endif

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        al_lock_mutex( keybuf_mutex );
        while ( ! keybuf.empty() ) keybuf.pop();
        al_unlock_mutex( keybuf_mutex );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        clear_keybuf();

#endif
}

int loadGIFAnimation( const std::string& gifFile, std::vector< Pict* >& frames, std::vector< int >& durations )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::loadGIFAnimation before allegro::init" << std::endl ; return 0 ;  }
#endif

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        int howManyFrames = 0;
        ALGIF_ANIMATION* gifAnimation = algif_load_animation( gifFile.c_str () );
        if ( gifAnimation != NULL )
        {
                howManyFrames = gifAnimation->frames_count ;

                if ( howManyFrames > 0 )
                {
                        for ( int i = 0; i < howManyFrames; i++ )
                        {
                                AllegroBitmap* frame = algif_get_frame_bitmap( gifAnimation, i );
                                frames.push_back( Pict::mendIntoPict( frame ) );

                                durations.push_back( algif_get_frame_duration( gifAnimation, i ) );
                        }
                }
        }

        return howManyFrames ;

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        AllegroBitmap** theseFrames = NULL;
        int* theseDurations = NULL;
        int howManyFrames = algif_load_animation( gifFile.c_str (), &theseFrames, &theseDurations );

        if ( howManyFrames > 0 )
        {
                for ( int i = 0; i < howManyFrames; i++ )
                {
                        frames.push_back( Pict::mendIntoPict( theseFrames[ i ] ) );

                        durations.push_back( theseDurations[ i ] );
                }

                free( theseDurations );
        }

        return howManyFrames;

#endif
}

void savePictAsPCX( const std::string& where, const Pict& what )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::savePictAsPCX before allegro::init" << std::endl ; return ;  }
#endif

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        al_save_bitmap( ( where + ".pcx" ).c_str (), what.ptr () );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        save_bitmap( ( where + ".pcx" ).c_str (), what.ptr (), NULL );

#endif
}

void savePictAsPNG( const std::string& where, const Pict& what )
{
#if defined( DEBUG_ALLEGRO_INIT ) && DEBUG_ALLEGRO_INIT
        if ( ! initialized ) {  std::cerr << "allegro::savePictAsPNG before allegro::init" << std::endl ; return ;  }
#endif

#if defined( USE_ALLEGRO5 ) && USE_ALLEGRO5

        al_save_bitmap( ( where + ".png" ).c_str (), what.ptr () );

#elif defined( USE_ALLEGRO4 ) && USE_ALLEGRO4

        save_png( ( where + ".png" ).c_str (), what.ptr (), NULL );

#endif
}

}
