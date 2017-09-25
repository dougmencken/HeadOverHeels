
#include "FreeItem.hpp"
#include "ItemData.hpp"
#include "Mediator.hpp"


namespace isomot
{

FreeItem::FreeItem( ItemData* itemData, int x, int y, int z, const Direction& direction ) : Item ( itemData, z, direction )
        , myMask ( WantMask )
        , transparency ( 0 )
        , collisionDetector ( true )
        , frozen ( false )
        , shadyImage ( 0 )
{
        this->x = x;
        this->y = y;
        if ( y < 0 ) this->y = 0;
        this->anchor = 0;

        // init frames
        int howManyFrames = ( getDataOfFreeItem ()->motion.size() - getDataOfFreeItem ()->extraFrames ) / getDataOfFreeItem ()->directionFrames;
        int currentFrame = ( getDataOfFreeItem ()->directionFrames > 1 ? getDataOfFreeItem ()->frames[ frameIndex ] + howManyFrames * direction : getDataOfFreeItem ()->frames[ 0 ] );

        this->image = getDataOfFreeItem ()->motion[ currentFrame ];

        // get shadow
        if ( getDataOfFreeItem ()->shadowWidth != 0 && getDataOfFreeItem ()->shadowHeight != 0 )
        {
                this->shadow = getDataOfFreeItem ()->shadows[ currentFrame ];
        }

        ///fprintf( stdout, "meet new free item with graphics from %s\n", getDataOfFreeItem ()->getNameOfFile().c_str () ) ;
}

FreeItem::FreeItem( const FreeItem& freeItem )
: Item( freeItem ),
        myMask( freeItem.myMask ),
        transparency( freeItem.transparency ),
        collisionDetector( freeItem.collisionDetector ),
        frozen( freeItem.frozen ),
        shadyImage( 0 )
{
        if ( freeItem.shadyImage != 0 )
        {
                this->shadyImage = create_bitmap_ex( 32, freeItem.shadyImage->w, freeItem.shadyImage->h );
                blit( freeItem.shadyImage, this->shadyImage, 0, 0, 0, 0, this->shadyImage->w, this->shadyImage->h );
        }
}

FreeItem::~FreeItem()
{
        if ( shadyImage != 0 )
                destroy_bitmap( shadyImage );
}

void FreeItem::draw( BITMAP* where )
{
        // Dibujado del elemento transparente
        if ( this->transparency > 0 && this->transparency < 100 )
        {
                set_trans_blender( 0, 0, 0, static_cast < int > ( 256 - 2.56 * this->transparency ) );

                draw_trans_sprite(
                        where,
                        this->processedImage ? this->processedImage : ( this->shadyImage ? this->shadyImage : this->image ),
                        mediator->getX0 () + this->offset.first,
                        mediator->getY0 () + this->offset.second
                ) ;
        }
        else
        {
                draw_sprite(
                        where,
                        this->processedImage ? this->processedImage : ( this->shadyImage ? this->shadyImage : this->image ),
                        mediator->getX0 () + this->offset.first,
                        mediator->getY0 () + this->offset.second
                ) ;
        }
}

void FreeItem::changeImage( BITMAP* image )
{
        // Si el elemento no tiene ninguna imagen entonces simplemente se asigna. Se entiende que este
        // caso sucede únicamente durante la creación del elemento
        if ( this->image == 0 )
        {
                this->image = image;
        }
        // En caso contrario se realiza el cambio
        else if ( this->image != image )
        {
                // Copia el elemento antes de realizar el cambio
                FreeItem oldFreeItem( *this );

                // Cambio de imagen
                this->image = image;
                this->myShady = WantShadow;
                this->myMask = WantMask;

                // Si la imagen no es nula se recalcula el desplazamiento y se vuelven a crear las
                // imágenes procesadas si éstas tienen un tamaño distinto a la nueva imagen
                if ( this->image )
                {
                        // A cuántos píxeles está la imagen del punto origen de la sala
                        this->offset.first = ( ( this->x - this->y ) << 1 ) + getDataOfFreeItem()->widthX + getDataOfFreeItem()->widthY - ( image->w >> 1 ) - 1;
                        this->offset.second = this->x + this->y + getDataOfFreeItem()->widthX - image->h - this->z;

                        if ( this->processedImage )
                        {
                                if ( this->processedImage->w != this->image->w || this->processedImage->h != this->image->h )
                                {
                                        destroy_bitmap( this->processedImage );
                                        this->processedImage = 0;
                                }
                        }

                        if ( this->shadyImage )
                        {
                                if ( this->shadyImage->w != this->image->w || this->shadyImage->h != this->image->h )
                                {
                                        destroy_bitmap( this->shadyImage );
                                        this->shadyImage = 0;
                                }
                        }
                }
                // Si la imagen es nula se destruyen las imágenes procesadas
                else
                {
                        this->offset.first = this->offset.second = 0;

                        if ( this->processedImage )
                        {
                                destroy_bitmap( this->processedImage );
                                this->processedImage = 0;
                        }

                        if ( this->shadyImage )
                        {
                                destroy_bitmap( this->shadyImage );
                                this->shadyImage = 0;
                        }
                }

                // Se marcan para enmascar los elementos libres afectados por la antigua imagen
                if ( oldFreeItem.getImage() )
                {
                        mediator->markItemsForMasking( &oldFreeItem );
                }

                // Se marcan para enmascar los elementos libres afectados por la nueva imagen
                if ( this->image )
                {
                        mediator->markItemsForMasking( this );
                }
        }
}

void FreeItem::changeShadow( BITMAP* shadow )
{
        // Si el elemento no tiene ninguna sombra entonces simplemente se asigna. Se entiende que este
        // caso sucede únicamente durante la creación del elemento
        if ( this->shadow == NULL )
        {
                // Asignación de la sombra
                this->shadow = shadow;
        }
        // En caso contrario se realiza el cambio
        else if ( this->shadow != shadow )
        {
                // Cambio de la sombra
                this->shadow = shadow;

                // Hay que calcular a qué elementos se sombreará, a no ser que la nueva sombra sea nula
                if ( this->image )
                {
                        // Si las sombras están activas los elementos deben sombrearse
                        if ( mediator->getDegreeOfShading() < 256 )
                        {
                                mediator->markItemsForShady( this );
                        }
                }
        }
}

void FreeItem::requestCastShadow()
{
        if( this->image && this->myShady == WantShadow )
        {
                mediator->castShadowOnFreeItem( this );

                // Si el elemento se ha sombreado se marca para enmascararlo
                if ( this->myShady == AlreadyShady )
                {
                        this->myMask = WantMask;
                }

                // Si no se ha podido sombrear entonces se destruye la imagen de sombreado
                // y se marca el elemento para enmascararlo
                if ( this->myShady == WantShadow && this->shadyImage )
                {
                        destroy_bitmap( this->shadyImage );
                        this->shadyImage = 0;
                        this->myMask = WantMask;
                }
        }
}

void FreeItem::castShadowImage( int x, int y, BITMAP* shadow, short shadingScale, unsigned char transparency )
{
        // El sombreado se realiza si el elemento que sombrea no es totalmente transparente
        if ( transparency < 100 )
        {
                // Anchura del elemento
                int width = ( getDataOfFreeItem()->widthX > getDataOfFreeItem()->widthY ? getDataOfFreeItem()->widthX : getDataOfFreeItem()->widthY );
                // Coordenada inicial X
                int inix = x - this->offset.first;
                if ( inix < 0 ) inix = 0;
                // Coordenada inicial Y
                int iniy = y - this->offset.second;
                if ( iniy < 0 ) iniy = 0;
                // Coordenada final X
                int endx = x - this->offset.first + shadow->w;
                if ( endx > this->image->w ) endx = this->image->w;
                // Coordenada final Y
                int endy = y - this->offset.second + shadow->h;
                if ( endy > this->image->h ) endy = this->image->h;
                // Coordenada intermedia Y
                int my = this->image->h - width - getDataOfFreeItem()->height;
                if ( endy < my ) my = endy;
                if ( endy > my + width ) endy = my + width;

                // Índice para recorrer las filas de píxeles de la imágenes image y shadyImage del elemento
                int iRow = 0;
                // Índice para recorrer las filas de píxeles de la imagen shadow del elemento que sombrea
                int sRow = 0;
                // Índice para recorrer la componente roja de los píxeles de una fila de las imágenes image y shadyImage del elemento
                int iRpx = 0;
                // Índice para recorrer la componente verde de los píxeles de una fila de las imágenes image y shadyImage del elemento
                int iGpx = 0;
                // Índice para recorrer la componente azul de los píxeles de una fila de las imágenes image y shadyImage del elemento
                int iBpx = 0;
                // Índice para recorrer los píxeles de una fila de la imagen shadow del elemento que sombrea
                int sPixel = 0;
                // Primera componente del primer píxel (el situado más a la izquierda) donde comienza el sombreado lateral
                int ltpx = 0;
                // Segunda componente del primer píxel (el situado más a la izquierda) donde comienza el sombreado lateral
                int ltpx1 = 0;
                // Primera componente del último píxel (el situado más a la derecha) donde termina el sombreado lateral
                int rtpx = 0;
                // Segunda componente del último píxel (el situado más a la derecha) donde termina el sombreado lateral
                int rtpx1 = 0;

                // Las coordenadas iniciales tienen que ser menores a las finales
                if ( iniy < endy && inix < endx )
                {
                        int n2i = inix + this->offset.first - x;

                        // En principio, la imagen del elemento sombreado es la imagen del elemento sin sombrear
                        if ( ! this->shadyImage )
                        {
                                this->shadyImage = create_bitmap_ex(bitmap_color_depth(this->image), this->image->w, this->image->h);
                        }
                        if ( this->myShady == WantShadow )
                        {
                                blit( this->image, this->shadyImage, 0, 0, 0, 0, this->image->w, this->image->h );
                                this->myShady = AlreadyShady;
                        }

                        // Incremento de los índices iRpx, iGpx e iBpx
                        char iInc = ( bitmap_color_depth(this->image) == 32 ? 4 : 3 );
                        // Incremento del índice sPixel
                        char sInc = ( bitmap_color_depth(shadow) == 32 ? 4 : 3 );

                        // Grado de opacidad del sombreado desde 0 a 255, siendo 0 la opacidad total y 255
                        // casi la transparencia total
                        short opacity = short( ( ( 256.0 - shadingScale ) / 100 ) * transparency + shadingScale );

                        endx *= iInc;
                        inix *= iInc;
                #if IS_BIG_ENDIAN
                        inix += bitmap_color_depth( this->image ) == 32 ? 1 : 0 ;
                #endif
                        n2i *= sInc;
                #if IS_BIG_ENDIAN
                        n2i += bitmap_color_depth( shadow ) == 32 ? 1 : 0 ;
                #endif

                        // Si la opacidad es potencia de 2 en el intervalo [2,128]
                        if ( int ( pow( 2, log10( opacity ) / log10( 2 ) ) ) == opacity )
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

                                // Sombreado de la superficie del elemento, la parte superior
                                // Se recorren las filas de las tres imágenes entre los límites calculados
                                for ( iRow = iniy, sRow = iniy + this->offset.second - y; iRow < my; iRow++, sRow++ )
                                {
                                        unsigned char* sln = shadow->line[ sRow ];
                                        unsigned char* iln = this->image->line[ iRow ];
                                        unsigned char* rln = this->shadyImage->line[ iRow ];

                                        // Se recorren los píxeles de cada fila según los límites calculados
                                        for ( iRpx = inix, iGpx = inix + 1, iBpx = inix + 2, sPixel = n2i; iRpx < endx; iRpx += iInc, iGpx += iInc, iBpx += iInc, sPixel += sInc )
                                        {
                                                // Si el píxel de las tres imágenes no tiene el color clave (255,0,255)
                                                // entonces el píxel de la imagen resultante se divide entre 2^pxDiv, es decir, se oscurece
                                                if ( ( iln[ iRpx ] < 255 || iln[ iGpx ] || iln[ iBpx ] < 255 ) &&
                                                        ( iln[ iRpx ] == rln[ iRpx ] && iln[ iGpx ] == rln[ iGpx ] && iln[ iBpx ] == rln[ iBpx ] ) &&
                                                        ( sln[ sPixel ] < 255 || sln[ sPixel + 1 ] || sln[ sPixel + 2 ] < 255 ) )
                                                {
                                                        rln[ iRpx ] = iln[ iRpx ] >> pxDiv;
                                                        rln[ iGpx ] = iln[ iGpx ] >> pxDiv;
                                                        rln[ iBpx ] = iln[ iBpx ] >> pxDiv;
                                                }
                                        }
                                }
                                // Hasta aquí el elemento se sombrea de forma horizontal hasta la línea marcada por la variable my

                                // Sombreado de los laterales del elemento
                                ltpx = ( ( this->image->w ) >> 1 ) - ( width << 1 ) + ( getDataOfFreeItem()->widthX - getDataOfFreeItem()->widthY ) + ( ( iRow - my ) << 1 );
                                rtpx = ( ( this->image->w ) >> 1 ) + ( width << 1 ) + ( getDataOfFreeItem()->widthX - getDataOfFreeItem()->widthY ) - ( ( iRow - my ) << 1 ) - 2;
                                ltpx = ltpx * iInc;
                        #if IS_BIG_ENDIAN
                                ltpx += bitmap_color_depth( this->image ) == 32 ? 1 : 0 ;
                        #endif
                                rtpx = rtpx * iInc;
                        #if IS_BIG_ENDIAN
                                rtpx += bitmap_color_depth( this->image ) == 32 ? 1 : 0 ;
                        #endif

                                // Sombrea en escalera isométrica la parte izquierda y derecha del elemento
                                for ( ltpx1 = ltpx + iInc, rtpx1 = rtpx + iInc; iRow < endy; iRow++, sRow++, ltpx += 2 * iInc, ltpx1 += 2 * iInc, rtpx -= 2 * iInc, rtpx1 -= 2 * iInc )
                                {
                                        unsigned char* sln = shadow->line[ sRow ];
                                        unsigned char* iln = this->image->line[ iRow ];
                                        unsigned char* rln = this->shadyImage->line[ iRow ];

                                        if ( inix < ltpx )
                                        {
                                                inix = ltpx;
                                                n2i = inix + ( this->offset.first - x ) * sInc;
                                        }

                                        if ( endx > rtpx + 2 * iInc )
                                        {
                                                endx = rtpx + 2 * iInc;
                                        }

                                        for ( iRpx = inix, iGpx = inix + 1, iBpx = inix + 2, sPixel = n2i; iRpx < endx; iRpx += iInc, iGpx += iInc, iBpx += iInc, sPixel += sInc )
                                        {
                                                if ( sln[ sPixel ] < 255 || sln[ sPixel + 1 ] || sln[ sPixel + 2 ] < 255 )
                                                {
                                                        if ( ( iln[ iRpx ] < 255 || iln[ iGpx ] || iln[ iBpx ] < 255 ) &&
                                                                ( iln[ iRpx ] == rln[ iRpx ] && iln[ iGpx ] == rln[ iGpx ] && iln[ iBpx ] == rln[ iBpx ] ) )
                                                        {
                                                                rln[ iRpx ] = iln[ iRpx ] >> pxDiv;
                                                                rln[ iGpx ] = iln[ iGpx ] >> pxDiv;
                                                                rln[ iBpx ] = iln[ iBpx ] >> pxDiv;
                                                        }

                                                        if ( iRpx == ltpx || iRpx == ltpx1 || iRpx == rtpx || iRpx == rtpx1 )
                                                        {
                                                                for ( int yy = iRow + 1; yy < image->h; yy++ )
                                                                {
                                                                        unsigned char* iln2 = this->image->line[yy];
                                                                        unsigned char* rln2 = this->shadyImage->line[yy];

                                                                        if ( ( iln2[ iRpx ] < 255 || iln2[ iGpx ] || iln2[ iBpx ] < 255 ) &&
                                                                                ( iln2[ iRpx ] == rln2[ iRpx ] && iln2[ iGpx ] == rln2[ iGpx ] && iln2[ iBpx ] == rln2[ iBpx ] ) )
                                                                        {
                                                                                rln2[ iRpx ] = iln2[ iRpx ] >> pxDiv;
                                                                                rln2[ iGpx ] = iln2[ iGpx ] >> pxDiv;
                                                                                rln2[ iBpx ] = iln2[ iBpx ] >> pxDiv;
                                                                        }
                                                                }
                                                        }
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
                                        for ( iRow = iniy, sRow = iniy + this->offset.second - y; iRow < my; iRow++, sRow++ )
                                        {
                                                unsigned short color;
                                                unsigned char* sln = shadow->line[ sRow ];
                                                unsigned char* iln = this->image->line[ iRow ];
                                                unsigned char* rln = this->shadyImage->line[ iRow ];

                                                // Se recorren los píxeles de cada fila según los límites calculados
                                                for ( iRpx = inix, iGpx = inix + 1, iBpx = inix + 2, sPixel = n2i; iRpx < endx; iRpx += iInc, iGpx += iInc, iBpx += iInc, sPixel += sInc )
                                                {
                                                        // Si el píxel de las tres imágenes no tiene el color clave (255,0,255)
                                                        // entonces el píxel de la imagen resultante se decrementa su valor para oscurecerlo
                                                        if ( ( iln[ iRpx ] < 255 || iln[ iGpx ] || iln[ iBpx ] < 255 ) &&
                                                                ( iln[ iRpx ] == rln[ iRpx ] && iln[ iGpx ] == rln[ iGpx ] && iln[ iBpx ] == rln[ iBpx ] ) &&
                                                                ( sln[ sPixel ] < 255 || sln[ sPixel + 1 ] || sln[ sPixel + 2 ] < 255 ) )
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

                                        ltpx = ( ( this->image->w ) >> 1 ) - ( width << 1 ) + ( getDataOfFreeItem()->widthX - getDataOfFreeItem()->widthY ) + ( ( iRow - my ) << 1 );
                                        rtpx = ( ( this->image->w ) >> 1 ) + ( width << 1 ) + ( getDataOfFreeItem()->widthX - getDataOfFreeItem()->widthY ) - ( ( iRow - my ) << 1 ) - 2;
                                        ltpx = ltpx * iInc;
                                #if IS_BIG_ENDIAN
                                        ltpx += bitmap_color_depth( this->image ) == 32 ? 1 : 0 ;
                                #endif
                                        rtpx = rtpx * iInc;
                                #if IS_BIG_ENDIAN
                                        rtpx += bitmap_color_depth( this->image ) == 32 ? 1 : 0 ;
                                #endif

                                        for ( ltpx1 = ltpx + iInc, rtpx1 = rtpx + iInc; iRow < endy; iRow++, sRow++, ltpx += 2 * iInc, ltpx1 += 2 * iInc, rtpx -= 2 * iInc, rtpx1 -= 2 * iInc )
                                        {
                                                unsigned char* sln = shadow->line[ sRow ];
                                                unsigned char* iln = this->image->line[ iRow ];
                                                unsigned char* rln = this->shadyImage->line[ iRow ];

                                                if ( inix < ltpx )
                                                {
                                                        inix = ltpx;
                                                        n2i = inix + ( this->offset.first - x ) * sInc;
                                                }

                                                if ( endx > rtpx + 2 * iInc )
                                                {
                                                        endx = rtpx + 2 * iInc;
                                                }

                                                for ( iRpx = inix, iGpx = inix + 1, iBpx = inix + 2, sPixel = n2i; iRpx < endx; iRpx += iInc, iGpx += iInc, iBpx += iInc, sPixel += sInc )
                                                {
                                                        if ( sln[ sPixel ] < 255 || sln[ sPixel + 1 ] || sln[ sPixel + 2 ] < 255 )
                                                        {
                                                                if ( ( iln[ iRpx ] < 255 || iln[ iGpx ] || iln[ iBpx ] < 255 ) &&
                                                                        ( iln[ iRpx ] == rln[ iRpx ] && iln[ iGpx ] == rln[ iGpx ] && iln[ iBpx ] == rln[ iBpx ] ) )
                                                                {
                                                                        unsigned short color;
                                                                        color = iln[ iRpx ] * opacity;
                                                                        rln[ iRpx ] = ( unsigned char )( color >> 8 );
                                                                        color = iln[ iGpx ] * opacity;
                                                                        rln[ iGpx ] = ( unsigned char )( color >> 8 );
                                                                        color = iln[ iBpx ] * opacity;
                                                                        rln[ iBpx ] = ( unsigned char )( color >> 8 );

                                                                        if ( iRpx == ltpx || iRpx == ltpx1 || iRpx == rtpx || iRpx == rtpx1 )
                                                                        {
                                                                                for ( int yy = iRow + 1; yy < image->h; yy++ )
                                                                                {
                                                                                        unsigned char* iln2 = this->image->line[ yy ];
                                                                                        unsigned char* rln2 = this->shadyImage->line[ yy ];

                                                                                        if ( ( iln2[ iRpx ] < 255 || iln2[ iGpx ] || iln2[ iBpx ] < 255 ) &&
                                                                                                ( iln2[ iRpx ] == rln2[ iRpx ] && iln2[ iGpx ] == rln2[ iGpx ] && iln2[ iBpx ] == rln2[ iBpx ] ) )
                                                                                        {
                                                                                                color = iln2[ iRpx ] * opacity;
                                                                                                rln2[ iRpx ] = ( unsigned char )( color >> 8 );
                                                                                                color = iln2[ iGpx ] * opacity;
                                                                                                rln2[ iGpx ] = ( unsigned char )( color >> 8 );
                                                                                                color = iln2[ iBpx ] * opacity;
                                                                                                rln2[ iBpx ] = ( unsigned char )( color >> 8 );
                                                                                        }
                                                                                }
                                                                        }
                                                                }
                                                        }
                                                }
                                        }
                                }
                                // La opacidad es cero, es decir, la opacidad es total
                                else
                                {
                                        // Se recorren las filas de las tres imágenes entre los límites calculados
                                        for ( iRow = iniy, sRow = iniy + this->offset.second - y; iRow < my; iRow++, sRow++ )
                                        {
                                                unsigned char* sln = shadow->line[ sRow ];
                                                unsigned char* iln = this->image->line[ iRow ];
                                                unsigned char* rln = this->shadyImage->line[ iRow ];

                                                // Se recorren los píxeles de cada fila según los límites calculados
                                                for ( iRpx = inix, iGpx = inix + 1, iBpx = inix + 2, sPixel = n2i; iRpx < endx; iRpx += iInc, iGpx += iInc, iBpx += iInc, sPixel += sInc )
                                                {
                                                        // Si el píxel de las tres imágenes no tiene el color clave (255,0,255)
                                                        // entonces el píxel de la imagen resultante se cero, totalmente negro
                                                        if ( ( iln[ iRpx ] < 255 || iln[ iGpx ] || iln[ iBpx ] < 255 ) &&
                                                                ( iln[ iRpx ] == rln[ iRpx ] && iln[ iGpx ] == rln[ iGpx ] && iln[ iBpx ] == rln[ iBpx ] ) &&
                                                                ( sln[ sPixel ] < 255 || sln[ sPixel + 1 ] || sln[ sPixel + 2 ] < 255 ) )
                                                        {
                                                                rln[ iRpx ] = rln[ iGpx ] = rln[ iBpx ] = 0;
                                                        }
                                                }
                                        }

                                        ltpx = ( ( this->image->w ) >> 1 ) - ( width << 1 ) + ( getDataOfFreeItem()->widthX - getDataOfFreeItem()->widthY ) + ( ( iRow - my ) << 1 );
                                        rtpx = ( ( this->image->w ) >> 1 ) + ( width << 1 ) + ( getDataOfFreeItem()->widthX - getDataOfFreeItem()->widthY ) - ( ( iRow - my ) << 1 ) - 2;
                                        ltpx = ltpx * iInc;
                                #if IS_BIG_ENDIAN
                                        ltpx += bitmap_color_depth( this->image ) == 32 ? 1 : 0 ;
                                #endif
                                        rtpx = rtpx * iInc;
                                #if IS_BIG_ENDIAN
                                        rtpx += bitmap_color_depth( this->image ) == 32 ? 1 : 0 ;
                                #endif

                                        for ( ltpx1 = ltpx + iInc, rtpx1 = rtpx + iInc; iRow < endy; iRow++, sRow++, ltpx += 2 * iInc, ltpx1 += 2 * iInc, rtpx -= 2 * iInc, rtpx1 -= 2 * iInc )
                                        {
                                                unsigned char* sln = shadow->line[ sRow ];
                                                unsigned char* iln = this->image->line[ iRow ];
                                                unsigned char* rln = this->shadyImage->line[ iRow ];

                                                if ( inix < ltpx )
                                                {
                                                        inix = ltpx;
                                                        n2i = inix + ( this->offset.first - x ) * sInc;
                                                }

                                                if ( endx > rtpx + 2 * iInc )
                                                {
                                                        endx = rtpx + 2 * iInc;
                                                }

                                                for ( iRpx = inix, iGpx = inix + 1, iBpx = inix + 2, sPixel = n2i; iRpx < endx; iRpx += iInc, iGpx += iInc, iBpx += iInc, sPixel += sInc )
                                                {
                                                        if ( sln[ sPixel ] < 255 || sln[ sPixel + 1 ] || sln[ sPixel + 2 ] < 255 )
                                                        {
                                                                if ( ( iln[ iRpx ] < 255 || iln[ iGpx ] || iln[ iBpx ] < 255 ) &&
                                                                        ( iln[ iRpx ] == rln[ iRpx ] && iln[ iGpx ] == rln[ iGpx ] && iln[ iBpx ] == rln[ iBpx ] ) )
                                                                {
                                                                        rln[ iRpx ] = rln[ iGpx ] = rln[ iBpx ] = 0;

                                                                        if ( iRpx == ltpx || iRpx == ltpx1 || iRpx == rtpx || iRpx == rtpx1 )
                                                                        {
                                                                                for ( int yy = iRow + 1; yy < this->image->h; yy++ )
                                                                                {
                                                                                        unsigned char* iln2 = this->image->line[ yy ];
                                                                                        unsigned char* rln2 = this->shadyImage->line[ yy ];

                                                                                        if ( ( iln2[ iRpx ] < 255 || iln2[ iGpx ] || iln2[ iBpx ] < 255 ) &&
                                                                                                (iln2[ iRpx ] == rln2[ iRpx ] && iln2[ iGpx ] == rln2[ iGpx ] && iln2[ iBpx ] == rln2[ iBpx ] ) )
                                                                                        {
                                                                                                rln2[ iRpx ] = rln2[ iGpx ] = rln2[ iBpx ] = 0;
                                                                                        }
                                                                                }
                                                                        }
                                                                }
                                                        }
                                                }
                                        }
                                }
                        }
                }
        }
}

void FreeItem::requestMask()
{
        mediator->mask( this );

        // Si no se ha podido enmascarar se destruye la imagen final
        if ( this->myMask == WantMask && this->processedImage )
        {
                destroy_bitmap( this->processedImage );
                this->processedImage = 0;
        }

        // Reinicia los atributos para el siguiente ciclo
        this->myShady = NoShadow;
        this->myMask = NoMask;
}

void FreeItem::maskImage( int x, int y, BITMAP* image )
{
        // Se enmascarará la imagen sombreada. Si el elemento no está sombreado se hará con la imagen normal
        BITMAP* currentImage = ( this->shadyImage ? this->shadyImage : this->image );

        // Coordenada inicial X
        int inix = x - this->offset.first;
        if ( inix < 0 ) inix = 0;
        // Coordenada inicial Y
        int iniy = y - this->offset.second;
        if ( iniy < 0 ) iniy = 0;
        // Coordenada final X
        int endx = x - this->offset.first + image->w;
        if ( endx > currentImage->w ) endx = currentImage->w;
        // Coordenada final Y
        int endy = y - this->offset.second + image->h;
        if ( endy > currentImage->h ) endy = currentImage->h;

        // Índice para recorrer las filas de píxeles de la imagen currentImage
        int cRow = 0;
        // Índice para recorrer las filas de píxeles de la imagen image
        int iRow = 0;
        // Índice para recorrer los píxeles de una fila de la imagen currentImage
        int cPixel = 0;
        // Índice para recorrer los píxeles de una fila de la imagen image
        int iPixel = 0;

        // En principio, la imagen del elemento enmascarado es la imagen del elemento sin enmascarar,
        // sombreada o sin sombrear
        if ( ! this->processedImage )
        {
                this->processedImage = create_bitmap_ex( bitmap_color_depth( currentImage ), currentImage->w, currentImage->h );
        }

        if ( this->myMask == WantMask )
        {
                blit( currentImage, this->processedImage, 0, 0, 0, 0, currentImage->w, currentImage->h );
                this->myMask = AlreadyMasked;
        }

        char increase1 = ( bitmap_color_depth( this->processedImage ) == 32 ? 4 : 3 );
        char increase2 = ( bitmap_color_depth( image ) == 32 ? 4 : 3 );

        int n2i = inix + this->offset.first - x;

        endx *= increase1;
        inix = inix * increase1;
        #if IS_BIG_ENDIAN
                inix += bitmap_color_depth( currentImage ) == 32 ? 1 : 0 ;
        #endif
        n2i = n2i * increase2;
        #if IS_BIG_ENDIAN
                n2i += bitmap_color_depth( image ) == 32 ? 1 : 0;
        #endif

        for ( cRow = iniy, iRow = iniy + this->offset.second - y; cRow < endy; cRow++, iRow++ )
        {
                unsigned char* cln = this->processedImage->line[ cRow ];
                unsigned char* iln = image->line[ iRow ];

                for ( cPixel = inix, iPixel = n2i; cPixel < endx; cPixel += increase1, iPixel += increase2 )
                {
                        if ( iln[ iPixel ] != 255 || iln[ iPixel + 1 ] != 0 || iln[ iPixel + 2 ] != 255 )
                        {
                                cln[ cPixel ] = cln[ cPixel + 2 ] = 255;
                                cln[ cPixel + 1 ] = 0;
                        }
                }
        }
}

bool FreeItem::changeData( int value, int x, int y, int z, const Datum& datum, const WhatToDo& how )
{
        mediator->clearStackOfCollisions( );

        bool collisionFound = false;

        // copy item before making the move
        FreeItem oldFreeItem( *this );

        switch ( datum )
        {
                case CoordinateX:
                        this->x = value + this->x * how;
                        break;

                case CoordinateY:
                        this->y = value + this->y * how;
                        break;

                case CoordinateZ:
                        this->z = value + this->z * how;
                        break;

                case CoordinatesXYZ:
                        this->x = x + this->x * how;
                        this->y = y + this->y * how;
                        this->z = z + this->z * how;
                        break;

                case WidthX:
                        this->dataOfItem->widthX = value + getDataOfFreeItem()->widthX * how;
                        break;

                case WidthY:
                        this->dataOfItem->widthY = value + getDataOfFreeItem()->widthY * how;
                        break;

                case Height:
                        this->dataOfItem->height = value + getDataOfFreeItem()->height * how;
                        break;
        }

        // look for collision with real wall, one which limits the room
        if ( this->x < mediator->getBound( North ) )
        {
                mediator->pushCollision( NorthWall );
        }
        else if ( this->x + getDataOfFreeItem()->widthX > mediator->getBound( South ) )
        {
                mediator->pushCollision( SouthWall );
        }
        if ( this->y >= mediator->getBound( West ) )
        {
                mediator->pushCollision( WestWall );
        }
        else if ( this->y - getDataOfFreeItem()->widthY + 1 < mediator->getBound( East ) )
        {
                mediator->pushCollision( EastWall );
        }

        // look for collision with floor
        if ( this->z < 0 )
        {
                mediator->pushCollision( Floor );
        }

        collisionFound = ! mediator->isStackOfCollisionsEmpty ();
        if ( ! collisionFound )
        {
                // look for collision with other items in room
                collisionFound = mediator->findCollisionWithItem( this );
                if ( ! collisionFound )
                {
                        // for item with image, mark to mask free items whose images overlap with its image
                        if ( this->image )
                        {
                                // get how many pixels is this image from point of room’s origin
                                this->offset.first = ( ( this->x - this->y ) << 1 ) + getDataOfFreeItem()->widthX + getDataOfFreeItem()->widthY - ( image->w >> 1 ) - 1;
                                this->offset.second = this->x + this->y + getDataOfFreeItem()->widthX - image->h - this->z;

                                // for both the previous position and the current position
                                mediator->markItemsForMasking( &oldFreeItem );
                                mediator->markItemsForMasking( this );
                        }
                        else
                        {
                                this->offset.first = this->offset.second = 0;
                        }

                        // reshade items
                        if ( mediator->getDegreeOfShading() < 256 )
                        {
                                // for both the previous position and the current position
                                mediator->markItemsForShady( &oldFreeItem );
                                mediator->markItemsForShady( this );
                        }

                        // reshade and remask
                        this->myShady = WantShadow;
                        this->myMask = WantMask;

                        // rearrange list of free items
                        mediator->activateFreeItemsSorting();
                }
        }

        // restore previous values when there’s a collision
        if ( collisionFound )
        {
                this->x = oldFreeItem.getX();
                this->y = oldFreeItem.getY();
                this->z = oldFreeItem.getZ();

                this->dataOfItem->widthX = oldFreeItem.getWidthX();
                this->dataOfItem->widthY = oldFreeItem.getWidthY();
                this->dataOfItem->height = oldFreeItem.getHeight();

                this->offset = oldFreeItem.getOffset();
        }

        return ! collisionFound;
}

bool FreeItem::addToX ( int value )
{
        return this->changeData( value, 0, 0, 0, CoordinateX, Add );
}

bool FreeItem::addToY ( int value )
{
        return this->changeData( value, 0, 0, 0, CoordinateY, Add );
}

bool FreeItem::addToZ ( int value )
{
        return this->changeData( value, 0, 0, 0, CoordinateZ, Add );
}

bool FreeItem::addToPosition( int x, int y, int z )
{
        return this->changeData( 0, x, y, z, CoordinatesXYZ, Add );
}

bool FreeItem::changeTransparency( unsigned char value, const WhatToDo& how )
{
        bool changed = false;

        // new value of item’s transparency
        unsigned char transpa = value + this->transparency * how;

        if ( transpa <= 100 && transpa != this->transparency )
        {
                // remask item?
                bool mask = this->image && ( this->transparency == 0 || transpa == 0 );

                // update table of transparencies
                mediator->removeFromTableOfTransparencies( this->transparency );
                mediator->addToTableOfTransparencies( transpa );
                this->transparency = transpa;

                // mark to shade items you might have underneath
                mediator->markItemsForShady( this );

                // mark to mask items which overlap
                if ( mask )
                {
                        mediator->markItemsForMasking( this ) ;
                }

                changed = true;
        }

        return changed;
}

}
