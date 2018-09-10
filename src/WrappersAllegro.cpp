
#include "WrappersAllegro.hpp"

#include <string>
#include <iostream>

#ifdef DEBUG
#  define CHECK_ALLEGRO_INITED  0
#endif


namespace allegro
{

bool initialized = false ;

void init( )
{
        allegro_init();

        initialized = true;
}

bool setGraphicsMode( int magicCard, int w, int h )
{
#if defined( CHECK_ALLEGRO_INITED ) && CHECK_ALLEGRO_INITED
        if ( ! initialized ) {  std::cerr << "allegro::setGraphicsMode before allegro::init" << std::endl ; return false ;  }
#endif

        return ( set_gfx_mode( magicCard, w, h, 0, 0 ) == /* okay */ 0 );
}

void bitBlit( const Pict* from, Pict* to, int fromX, int fromY, int toX, int toY, unsigned int width, unsigned int height )
{
#if defined( CHECK_ALLEGRO_INITED ) && CHECK_ALLEGRO_INITED
        if ( ! initialized ) {  std::cerr << "allegro::bitBlit before allegro::init" << std::endl ; return ;  }
#endif

        blit( const_cast< Pict* >( from ), to, fromX, fromY, toX, toY, width, height );
}

void bitBlit( const Pict* from, Pict* to )
{
        return bitBlit( from, to, 0, 0, 0, 0, from->w, from->h );
}

void drawSprite( Pict* view, const Pict* sprite, int x, int y )
{
#if defined( CHECK_ALLEGRO_INITED ) && CHECK_ALLEGRO_INITED
        if ( ! initialized ) {  std::cerr << "allegro::drawSprite before allegro::init" << std::endl ; return ;  }
#endif

        if ( sprite == NULL || view == NULL ) return ;

        draw_sprite( view, const_cast< Pict* >( sprite ), x, y );
}

void stretchBlit( const Pict* source, Pict* dest, int sX, int sY, int sW, int sH, int dX, int dY, int dW, int dH )
{
#if defined( CHECK_ALLEGRO_INITED ) && CHECK_ALLEGRO_INITED
        if ( ! initialized ) {  std::cerr << "allegro::stretchBlit before allegro::init" << std::endl ; return ;  }
#endif

        stretch_blit( const_cast< Pict* >( source ), dest, sX, sY, sW, sH, dX, dY, dW, dH );
}

Pict* createPicture( int width, int height, int depth )
{
#if defined( CHECK_ALLEGRO_INITED ) && CHECK_ALLEGRO_INITED
        if ( ! initialized ) {  std::cerr << "allegro::createPicture before allegro::init" << std::endl ; return NULL ;  }
#endif

        return create_bitmap_ex( depth, width, height );
}

void binPicture( Pict*& picture )
{
#if defined( CHECK_ALLEGRO_INITED ) && CHECK_ALLEGRO_INITED
        if ( ! initialized ) {  std::cerr << "allegro::binPicture before allegro::init" << std::endl ; return ;  }
#endif

        if ( picture == NULL ) return ;

        // nullify it first, then invoke destroy_bitmap
        // to avoid drawing of being-destroyed thing when there’re many threads

        Pict* toBin = picture;
        picture = NULL ;

        destroy_bitmap( toBin );
}

int colorDepthOf( const Pict* picture )
{
#if defined( CHECK_ALLEGRO_INITED ) && CHECK_ALLEGRO_INITED
        if ( ! initialized ) {  std::cerr << "allegro::colorDepthOf before allegro::init" << std::endl ; return 0 ;  }
#endif

        if ( picture == NULL ) return 0 ;

        return bitmap_color_depth( const_cast< Pict* >( picture ) );
}

void clearToColor( Pict* picture, int allegroColor )
{
#if defined( CHECK_ALLEGRO_INITED ) && CHECK_ALLEGRO_INITED
        if ( ! initialized ) {  std::cerr << "allegro::clearToColor before allegro::init" << std::endl ; return ;  }
#endif

        if ( picture == NULL ) return ;

        clear_to_color( picture, allegroColor );
}

int getPixel( Pict* picture, int x, int y )
{
#if defined( CHECK_ALLEGRO_INITED ) && CHECK_ALLEGRO_INITED
        if ( ! initialized ) {  std::cerr << "allegro::getPixel before allegro::init" << std::endl ; return -1 ;  }
#endif

        return getpixel( picture, x, y );
}

void putPixel( Pict* picture, int x, int y, int allegroColor )
{
#if defined( CHECK_ALLEGRO_INITED ) && CHECK_ALLEGRO_INITED
        if ( ! initialized ) {  std::cerr << "allegro::putPixel before allegro::init" << std::endl ; return ;  }
#endif

        if ( picture == NULL ) return ;

        putpixel( picture, x, y, allegroColor );
}

Pict* loadPNG( const char* file )
{
#if defined( CHECK_ALLEGRO_INITED ) && CHECK_ALLEGRO_INITED
        if ( ! initialized ) {  std::cerr << "allegro::loadPNG before allegro::init" << std::endl ; return NULL ;  }
#endif

        return load_png( file, NULL );
}

}
