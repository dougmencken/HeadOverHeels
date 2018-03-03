
#include "FreeItem.hpp"
#include "ItemData.hpp"
#include "Mediator.hpp"


namespace isomot
{

FreeItem::FreeItem( ItemData* itemData, int x, int y, int z, const Way& way )
        : Item ( itemData, z, way )
        , myMask ( WantMask )
        , transparency ( 0 )
        , collisionDetector ( true )
        , frozen ( false )
        , shadyImage ( nilPointer )
{
        this->x = x;
        this->y = y;
        if ( y < 0 ) this->y = 0;

        // init frames
        int howManyFrames = ( getDataOfItem()->howManyMotions() - getDataOfItem()->howManyExtraFrames() ) / getDataOfItem()->howManyFramesForOrientations() ;
        unsigned int orientation = ( way.getIntegerOfWay() == Nowhere ? 0 : way.getIntegerOfWay() );
        int currentFrame = ( getDataOfItem()->howManyFramesForOrientations() > 1 ?
                                        getDataOfItem()->getFrameAt( getIndexOfFrame() ) + howManyFrames * orientation :
                                        getDataOfItem()->getFrameAt( 0 ) );

        this->rawImage = getDataOfItem()->getMotionAt( currentFrame );

        // get shadow
        if ( getDataOfItem()->getWidthOfShadow() > 0 && getDataOfItem()->getHeightOfShadow() > 0 )
        {
                this->shadow = getDataOfItem()->getShadowAt( currentFrame );
        }
}

FreeItem::FreeItem( const FreeItem& freeItem )
        : Item( freeItem )
        , myMask( freeItem.myMask )
        , transparency( freeItem.transparency )
        , collisionDetector( freeItem.collisionDetector )
        , frozen( freeItem.frozen )
        , shadyImage( nilPointer )
{
        if ( freeItem.shadyImage != nilPointer )
        {
                this->shadyImage = create_bitmap_ex( 32, freeItem.shadyImage->w, freeItem.shadyImage->h );
                blit( freeItem.shadyImage, this->shadyImage, 0, 0, 0, 0, this->shadyImage->w, this->shadyImage->h );
        }
}

FreeItem::~FreeItem()
{
        if ( shadyImage != nilPointer )
                destroy_bitmap( shadyImage );
}

void FreeItem::draw( BITMAP* where )
{
        // draw item with transparency
        if ( this->transparency > 0 && this->transparency < 100 )
        {
                set_trans_blender( 0, 0, 0, static_cast < int > ( 256 - 2.56 * this->transparency ) );

                draw_trans_sprite(
                        where,
                        this->processedImage ? this->processedImage : ( this->shadyImage ? this->shadyImage : this->rawImage ),
                        mediator->getRoom()->getX0 () + this->offset.first,
                        mediator->getRoom()->getY0 () + this->offset.second
                ) ;
        }
        else
        {
                draw_sprite(
                        where,
                        this->processedImage ? this->processedImage : ( this->shadyImage ? this->shadyImage : this->rawImage ),
                        mediator->getRoom()->getX0 () + this->offset.first,
                        mediator->getRoom()->getY0 () + this->offset.second
                ) ;
        }
}

void FreeItem::binProcessedImages()
{
        if ( this->processedImage != nilPointer )
        {
                destroy_bitmap( this->processedImage );
                this->processedImage = nilPointer;
        }

        if ( this->shadyImage != nilPointer )
        {
                destroy_bitmap( this->shadyImage );
                this->shadyImage = nilPointer;
        }
}

void FreeItem::changeImage( BITMAP* image )
{
        if ( image == nilPointer )
        {
                std::cout << "nil image at FreeItem.changeImage" << std::endl ;
        }

        if ( this->rawImage == nilPointer )
        {
                this->rawImage = image;
        }
        else if ( this->rawImage != image )
        {
                FreeItem oldFreeItem( *this );

                this->rawImage = nilPointer;

                // recalculate displacement, it is how many pixels is this image from point of room’s origin
                if ( image != nilPointer )
                {
                        this->offset.first = ( ( this->x - this->y ) << 1 ) + static_cast< int >( getDataOfItem()->getWidthX() + getDataOfItem()->getWidthY() ) - ( image->w >> 1 ) - 1;
                        this->offset.second = this->x + this->y + static_cast< int >( getDataOfItem()->getWidthX() ) - image->h - this->z;
                }
                else
                {
                        this->offset.first = this->offset.second = 0;
                }

                binProcessedImages() ;

                this->rawImage = image;
                this->myShady = WantShadow;
                this->myMask = WantMask;

                // remask with old image
                if ( oldFreeItem.getRawImage() != nilPointer )
                {
                        mediator->remaskFreeItem( &oldFreeItem );
                }

                // remask with new image
                if ( image != nilPointer )
                {
                        mediator->remaskFreeItem( this );
                }
        }
}

void FreeItem::changeShadow( BITMAP* shadow )
{
        if ( this->shadow == nilPointer )
        {
                this->shadow = shadow;
        }
        else if ( this->shadow != shadow )
        {
                this->shadow = shadow;

                // reshade items
                if ( this->rawImage )
                {
                        if ( mediator->getDegreeOfShading() < 256 )
                        {
                                mediator->reshadeFreeItem( this );
                        }
                }
        }
}

void FreeItem::requestCastShadow()
{
        if( this->rawImage && this->myShady == WantShadow )
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
                        this->shadyImage = nilPointer;
                        this->myMask = WantMask;
                }
        }
}

void FreeItem::castShadowImage( int x, int y, BITMAP* shadow, short shadingScale, unsigned char transparency )
{
        // is item not fully transparent
        if ( transparency < 100 )
        {
                int width = ( getDataOfItem()->getWidthX() > getDataOfItem()->getWidthY() ? getDataOfItem()->getWidthX() : getDataOfItem()->getWidthY() );
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
                // Coordenada intermedia Y
                int my = this->rawImage->h - width - getDataOfItem()->getHeight ();
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
                                this->shadyImage = create_bitmap_ex( bitmap_color_depth( this->rawImage ), this->rawImage->w, this->rawImage->h );
                        }
                        if ( this->myShady == WantShadow )
                        {
                                blit( this->rawImage, this->shadyImage, 0, 0, 0, 0, this->rawImage->w, this->rawImage->h );
                                this->myShady = AlreadyShady;
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
                                        unsigned char* iln = this->rawImage->line[ iRow ];
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
                                ltpx = ( ( this->rawImage->w ) >> 1 ) - ( width << 1 ) + ( getDataOfItem()->getWidthX() - getDataOfItem()->getWidthY() ) + ( ( iRow - my ) << 1 );
                                rtpx = ( ( this->rawImage->w ) >> 1 ) + ( width << 1 ) + ( getDataOfItem()->getWidthX() - getDataOfItem()->getWidthY() ) - ( ( iRow - my ) << 1 ) - 2;
                                ltpx = ltpx * iInc;
                        #if IS_BIG_ENDIAN
                                ltpx += bitmap_color_depth( this->rawImage ) == 32 ? 1 : 0 ;
                        #endif
                                rtpx = rtpx * iInc;
                        #if IS_BIG_ENDIAN
                                rtpx += bitmap_color_depth( this->rawImage ) == 32 ? 1 : 0 ;
                        #endif

                                // Sombrea en escalera isométrica la parte izquierda y derecha del elemento
                                for ( ltpx1 = ltpx + iInc, rtpx1 = rtpx + iInc; iRow < endy; iRow++, sRow++, ltpx += 2 * iInc, ltpx1 += 2 * iInc, rtpx -= 2 * iInc, rtpx1 -= 2 * iInc )
                                {
                                        unsigned char* sln = shadow->line[ sRow ];
                                        unsigned char* iln = this->rawImage->line[ iRow ];
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
                                                                for ( int yy = iRow + 1; yy < this->rawImage->h; yy++ )
                                                                {
                                                                        unsigned char* iln2 = this->rawImage->line[yy];
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
                                                unsigned char* iln = this->rawImage->line[ iRow ];
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

                                        ltpx = ( ( this->rawImage->w ) >> 1 ) - ( width << 1 ) + ( getDataOfItem()->getWidthX() - getDataOfItem()->getWidthY() ) + ( ( iRow - my ) << 1 );
                                        rtpx = ( ( this->rawImage->w ) >> 1 ) + ( width << 1 ) + ( getDataOfItem()->getWidthX() - getDataOfItem()->getWidthY() ) - ( ( iRow - my ) << 1 ) - 2;
                                        ltpx = ltpx * iInc;
                                #if IS_BIG_ENDIAN
                                        ltpx += bitmap_color_depth( this->rawImage ) == 32 ? 1 : 0 ;
                                #endif
                                        rtpx = rtpx * iInc;
                                #if IS_BIG_ENDIAN
                                        rtpx += bitmap_color_depth( this->rawImage ) == 32 ? 1 : 0 ;
                                #endif

                                        for ( ltpx1 = ltpx + iInc, rtpx1 = rtpx + iInc; iRow < endy; iRow++, sRow++, ltpx += 2 * iInc, ltpx1 += 2 * iInc, rtpx -= 2 * iInc, rtpx1 -= 2 * iInc )
                                        {
                                                unsigned char* sln = shadow->line[ sRow ];
                                                unsigned char* iln = this->rawImage->line[ iRow ];
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
                                                                                for ( int yy = iRow + 1; yy < this->rawImage->h; yy++ )
                                                                                {
                                                                                        unsigned char* iln2 = this->rawImage->line[ yy ];
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
                                                unsigned char* iln = this->rawImage->line[ iRow ];
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

                                        ltpx = ( ( this->rawImage->w ) >> 1 ) - ( width << 1 ) + ( getDataOfItem()->getWidthX() - getDataOfItem()->getWidthY() ) + ( ( iRow - my ) << 1 );
                                        rtpx = ( ( this->rawImage->w ) >> 1 ) + ( width << 1 ) + ( getDataOfItem()->getWidthX() - getDataOfItem()->getWidthY() ) - ( ( iRow - my ) << 1 ) - 2;
                                        ltpx = ltpx * iInc;
                                #if IS_BIG_ENDIAN
                                        ltpx += bitmap_color_depth( this->rawImage ) == 32 ? 1 : 0 ;
                                #endif
                                        rtpx = rtpx * iInc;
                                #if IS_BIG_ENDIAN
                                        rtpx += bitmap_color_depth( this->rawImage ) == 32 ? 1 : 0 ;
                                #endif

                                        for ( ltpx1 = ltpx + iInc, rtpx1 = rtpx + iInc; iRow < endy; iRow++, sRow++, ltpx += 2 * iInc, ltpx1 += 2 * iInc, rtpx -= 2 * iInc, rtpx1 -= 2 * iInc )
                                        {
                                                unsigned char* sln = shadow->line[ sRow ];
                                                unsigned char* iln = this->rawImage->line[ iRow ];
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
                                                                                for ( int yy = iRow + 1; yy < this->rawImage->h; yy++ )
                                                                                {
                                                                                        unsigned char* iln2 = this->rawImage->line[ yy ];
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

        if ( this->myMask == WantMask && this->processedImage )
        {
                destroy_bitmap( this->processedImage );
                this->processedImage = nilPointer;
        }

        this->myShady = NoShadow;
        this->myMask = NoMask;
}

void FreeItem::maskImage( int x, int y, BITMAP* image )
{
        assert( image != nilPointer );

        // mask shaded image or raw image when item is not yet shaded
        BITMAP* currentImage = ( this->shadyImage != nilPointer ? this->shadyImage : this->rawImage );
        assert( currentImage != nilPointer );

        int inix = x - this->offset.first;                      // initial X
        if ( inix < 0 ) inix = 0;

        int iniy = y - this->offset.second;                     // initial Y
        if ( iniy < 0 ) iniy = 0;

        int endx = x - this->offset.first + image->w;           // final X
        if ( endx > currentImage->w ) endx = currentImage->w;

        int endy = y - this->offset.second + image->h;          // final Y
        if ( endy > currentImage->h ) endy = currentImage->h;

        // in principle, image of masked item is image of unmasked item, shaded or unshaded
        if ( this->processedImage == nilPointer )
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
        inix *= increase1;
        n2i *= increase2;
        #if IS_BIG_ENDIAN
                inix += bitmap_color_depth( currentImage ) == 32 ? 1 : 0 ;
                n2i += bitmap_color_depth( image ) == 32 ? 1 : 0;
        #endif

        int cRow = 0;           // row of pixels in currentImage
        int iRow = 0;           // row of pixels in image
        int cPixel = 0;         // pixel in row of currentImage
        int iPixel = 0;         // pixel in row of image

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

bool FreeItem::updatePosition( int newX, int newY, int newZ, const Coordinate& whatToChange, const ChangeOrAdd& how )
{
        mediator->clearStackOfCollisions( );

        bool collisionFound = false;

        // copy item before making the move
        FreeItem oldFreeItem( *this );

        if ( whatToChange & CoordinateX )
        {
                this->x = newX + this->x * how;
        }
        if ( whatToChange & CoordinateY )
        {
                this->y = newY + this->y * how;
        }
        if ( whatToChange & CoordinateZ )
        {
                this->z = newZ + this->z * how;
        }

        // look for collision with real wall, one which limits the room
        if ( this->x < mediator->getRoom()->getLimitAt( Way( "north" ) ) )
        {
                mediator->pushCollision( NorthWall );
        }
        else if ( this->x + static_cast< int >( getDataOfItem()->getWidthX() ) > mediator->getRoom()->getLimitAt( Way( "south" ) ) )
        {
                mediator->pushCollision( SouthWall );
        }
        if ( this->y >= mediator->getRoom()->getLimitAt( Way( "west" ) ) )
        {
                mediator->pushCollision( WestWall );
        }
        else if ( this->y - static_cast< int >( getDataOfItem()->getWidthY() ) + 1 < mediator->getRoom()->getLimitAt( Way( "east" ) ) )
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
                        if ( this->rawImage )
                        {
                                // get how many pixels is this image from point of room’s origin
                                this->offset.first = ( ( this->x - this->y ) << 1 ) + getDataOfItem()->getWidthX() + getDataOfItem()->getWidthY() - ( this->rawImage->w >> 1 ) - 1;
                                this->offset.second = this->x + this->y + getDataOfItem()->getWidthX() - this->rawImage->h - this->z;

                                // for both the previous position and the current position
                                mediator->remaskFreeItem( &oldFreeItem );
                                mediator->remaskFreeItem( this );
                        }
                        else
                        {
                                this->offset.first = this->offset.second = 0;
                        }

                        // reshade items
                        if ( mediator->getDegreeOfShading() < 256 )
                        {
                                // for both the previous position and the current position
                                mediator->reshadeFreeItem( &oldFreeItem );
                                mediator->reshadeFreeItem( this );
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

                this->offset = oldFreeItem.getOffset();
        }

        return ! collisionFound;
}

bool FreeItem::addToX ( int value )
{
        return this->updatePosition( value, 0, 0, CoordinateX, Add );
}

bool FreeItem::addToY ( int value )
{
        return this->updatePosition( 0, value, 0, CoordinateY, Add );
}

bool FreeItem::addToZ ( int value )
{
        return this->updatePosition( 0, 0, value, CoordinateZ, Add );
}

bool FreeItem::addToPosition( int x, int y, int z )
{
        return this->updatePosition( x, y, z, CoordinatesXYZ, Add );
}

}
