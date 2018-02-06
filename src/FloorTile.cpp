
#include "FloorTile.hpp"
#include "Mediator.hpp"

#include <cmath>


namespace isomot
{

FloorTile::FloorTile( int column, int x, int y, BITMAP* image )
        : Mediated ()
        , column( column )
        , rawImage( image )
        , shadyImage( 0 )
{
        this->coordinates.first = x;
        this->coordinates.second = y;
        this->offset.first = this->offset.second = 0;
        this->shady = NoShadow;
}

FloorTile::~FloorTile()
{
        destroy_bitmap( rawImage );
        destroy_bitmap( shadyImage );
}

void FloorTile::calculateOffset()
{
        if ( mediator != 0 )
        {
                Room* room = mediator->getRoom();
                this->offset.first = room->getX0() + ( ( room->getSizeOfOneTile() * ( coordinates.first - coordinates.second - 1 ) ) << 1 ) + 1;
                this->offset.second = room->getY0() + room->getSizeOfOneTile() * ( coordinates.first + coordinates.second );
        }
}

void FloorTile::draw( BITMAP* where )
{
        if ( shadyImage != 0 )
        {       // draw tile with shadow
                draw_sprite( where, shadyImage, offset.first, offset.second );
        }
        else if ( rawImage != 0 )
        {       // draw tile, just tile
                draw_sprite( where, rawImage, offset.first, offset.second );
        }
}

void FloorTile::requestShadow()
{
        if ( this->rawImage && this->shady == WantShadow )
        {
                mediator->castShadowOnFloor( this );

                // Si no se ha podido sombrear entonces se destruye la imagen de sombreado
                if ( this->shady != AlreadyShady && this->shadyImage )
                {
                        destroy_bitmap( this->shadyImage );
                        this->shadyImage = 0;
                }

                // Reinicia el atributo para el siguiente ciclo
                this->shady = NoShadow;
        }
}

void FloorTile::castShadowImage( int x, int y, BITMAP* shadow, short shadingScale, unsigned char transparency )
{
        // El sombreado se realiza si el elemento que sombrea no es totalmente transparente
        if ( transparency < 100 )
        {
                // Coordenada inicial X
                int inix = x - this->offset.first;
                if ( inix < 0 ) inix = 0;
                // Coordenada inicial Y
                int iniy = y - this->offset.second;
                if ( iniy < 0 ) iniy = 0;
                // Coordenada final X
                int endx = x - this->offset.first + shadow->w;
                if ( endx > this->rawImage->w ) endx = this->rawImage->w;
                // Coordenada final Y
                int endy = y - this->offset.second + shadow->h;
                if ( endy > this->rawImage->h ) endy = this->rawImage->h;

                int iRow = 0;        // row in rows of pixels in the image and shadyImage images of the tile
                int sRow = 0;        // row in rows of pixels in the image shading shadow
                int sPixel = 0;      // pixel in a row of the image shading shadow

                // indices to walk thru color components in a row of image & shadyImage
                int iRpx = 0;        // red
                int iGpx = 0;        // green
                int iBpx = 0;        // blue

                // Las coordenadas iniciales tienen que ser menores a las finales
                if ( iniy < endy && inix < endx )
                {
                        int n2i = inix + this->offset.first - x;

                        // En principio, la imagen de la loseta sombreada es la imagen de la loseta sin sombrear
                        if ( ! this->shadyImage )
                        {
                                this->shadyImage = create_bitmap_ex( bitmap_color_depth( this->rawImage ), this->rawImage->w, this->rawImage->h );
                        }
                        if ( this->shady == WantShadow )
                        {
                                blit( this->rawImage, this->shadyImage, 0, 0, 0, 0, this->rawImage->w, this->rawImage->h );
                                this->shady = AlreadyShady;
                        }

                        // Incremento de los índices iRpx, iGpx e iBpx
                        char iInc = ( bitmap_color_depth( this->rawImage ) == 32 ? 4 : 3 );
                        // Incremento del índice sPixel
                        char sInc = ( bitmap_color_depth( shadow ) == 32 ? 4 : 3 );

                        // Grado de opacidad del sombreado desde 0 a 255, siendo 0 la opacidad total y 255
                        // casi la transparencia total
                        short opacity = short( ( ( 256.0 - shadingScale ) / 100 ) * transparency + shadingScale );

                        endx *= iInc;
                        inix *= iInc;
                        #if IS_BIG_ENDIAN
                                inix += bitmap_color_depth( this->rawImage ) == 32 ? 1 : 0 ;
                        #endif
                        n2i *= sInc;
                        #if IS_BIG_ENDIAN
                                n2i += bitmap_color_depth( shadow ) == 32 ? 1 : 0;
                        #endif

                        // La opacidad es potencia de dos en el intervalo [ 2, 128 ]
                        if ( int( std::pow( 2, std::log10( opacity ) / std::log10( 2 ) ) ) == opacity )
                        {
                                // Divisor del píxel
                                char pxDiv = 7;

                                // En función de la opacidad de la sombra se halla
                                // el valor del divisor del píxel: píxel / 2^pxDiv
                                while ( opacity != 2 )
                                {
                                        opacity = opacity >> 1;
                                        pxDiv--;
                                }

                                // Se recorren las filas de las tres imágenes entre los límites calculados
                                for ( iRow = iniy, sRow = iniy + this->offset.second - y; iRow < endy; iRow++, sRow++ )
                                {
                                        unsigned char* sln = shadow->line[ sRow ];
                                        unsigned char* iln = this->rawImage->line[ iRow ];
                                        unsigned char* rln = this->shadyImage->line[ iRow ];

                                        // Se recorren los píxeles de cada fila según los límites calculados
                                        for ( iRpx = inix, iGpx = inix + 1, iBpx = inix + 2, sPixel = n2i ;
                                                iRpx < endx ;
                                                iRpx += iInc, iGpx += iInc, iBpx += iInc, sPixel += sInc )
                                        {
                                                // Si el píxel de las tres imágenes no tiene el color clave ( 255, 0, 255 )
                                                // entonces el píxel de la imagen resultante se divide entre 2^pxDiv, es decir, se oscurece
                                                if ( ( iln[ iRpx ] < 255 || iln[ iGpx ] || iln[ iBpx ] < 255 )
                                                        && ( iln[ iRpx ] == rln[ iRpx ] && iln[ iGpx ] == rln[ iGpx ] && iln[ iBpx ] == rln[ iBpx ] )
                                                        && ( sln[ sPixel ] < 255 || sln[ sPixel + 1 ] || sln[ sPixel + 2 ] < 255 ) )
                                                {
                                                        rln[ iRpx ] = iln[ iRpx ] >> pxDiv;
                                                        rln[ iGpx ] = iln[ iGpx ] >> pxDiv;
                                                        rln[ iBpx ] = iln[ iBpx ] >> pxDiv;
                                                }
                                        }
                                }
                        }
                        // La opacidad no es potencia de dos
                        else
                        {
                                // La opacidad no es cero, es decir, hay cierta transparencia
                                if ( opacity )
                                {
                                        // Se recorren las filas de las tres imágenes entre los límites calculados
                                        for ( iRow = iniy, sRow = iniy + this->offset.second - y; iRow < endy; iRow++, sRow++ )
                                        {
                                                unsigned short color;
                                                unsigned char* sln = shadow->line[ sRow ];
                                                unsigned char* iln = this->rawImage->line[ iRow ];
                                                unsigned char* rln = this->shadyImage->line[ iRow ];

                                                // Se recorren los píxeles de cada fila según los límites calculados
                                                for ( iRpx = inix, iGpx = inix + 1, iBpx = inix + 2, sPixel = n2i ;
                                                        iRpx < endx ;
                                                        iRpx += iInc, iGpx += iInc, iBpx += iInc, sPixel += sInc )
                                                {
                                                        // Si el píxel de las tres imágenes no tiene el color clave ( 255, 0, 255 )
                                                        // entonces el píxel de la imagen resultante se decrementa su valor para oscurecerlo
                                                        if ( ( iln[ iRpx ] < 255 || iln[ iGpx ] || iln[ iBpx ] < 255 )
                                                                && ( iln[ iRpx ] == rln[ iRpx ] && iln[ iGpx ] == rln[ iGpx ] && iln[ iBpx ] == rln[ iBpx ] )
                                                                && ( sln[ sPixel ] < 255 || sln[ sPixel + 1 ] || sln[ sPixel + 2 ] < 255 ) )
                                                        {
                                                                color = iln[ iRpx ] * opacity;
                                                                rln[ iRpx ] = ( unsigned char )( color >> 8 );
                                                                color = iln[ iGpx ] * opacity;
                                                                rln[ iGpx ] = ( unsigned char )( color >> 8 );
                                                                color = iln[ iBpx ] * opacity;
                                                                rln[ iBpx ] = ( unsigned char )( color >> 8 );
                                                        }
                                                }
                                        }
                                }
                                // La opacidad es cero, es decir, la opacidad es total
                                else
                                {
                                        // Se recorren las filas de las tres imágenes entre los límites calculados
                                        for ( iRow = iniy, sRow = iniy + this->offset.second - y; iRow < endy; iRow++, sRow++ )
                                        {
                                                unsigned char* sln = shadow->line[ sRow ];
                                                unsigned char* iln = this->rawImage->line[ iRow ];
                                                unsigned char* rln = this->shadyImage->line[ iRow ];

                                                // Se recorren los píxeles de cada fila según los límites calculados
                                                for ( iRpx = inix, iGpx = inix + 1, iBpx = inix + 2, sPixel = n2i ;
                                                        iRpx < endx ;
                                                        iRpx += iInc, iGpx += iInc, iBpx += iInc, sPixel += sInc )
                                                {
                                                        // Si el píxel de las tres imágenes no tiene el color clave ( 255, 0, 255 )
                                                        // entonces el píxel de la imagen resultante se cero, totalmente negro
                                                        if ( ( iln[ iRpx ] < 255 || iln[ iGpx ] || iln[ iBpx ] < 255 )
                                                                && ( iln[ iRpx ] == rln[ iRpx ] && iln[ iGpx ] == rln[ iGpx ] && iln[ iBpx ] == rln[ iBpx ] )
                                                                && ( sln[ sPixel ] < 255 || sln[ sPixel + 1 ] || sln[ sPixel + 2 ] < 255 ) )
                                                        {
                                                                rln[ iRpx ] = rln[ iRpx + 1 ] = rln[ iRpx + 2 ] = 0;
                                                        }
                                                }
                                        }
                                }
                        }
                }
        }
}

}
