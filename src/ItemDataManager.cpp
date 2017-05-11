
#include "ItemDataManager.hpp"
#include "GameManager.hpp"
#include "Exception.hpp"
#include "Ism.hpp"
#include <loadpng.h>
#include <fstream>

# include <xercesc/util/PlatformUtils.hpp>


namespace isomot
{

ItemDataManager::ItemDataManager( const std::string& name )
{
        this->nameOfXMLFile = name;
}

ItemDataManager::~ItemDataManager()
{
        freeItems() ;
}

void ItemDataManager::loadItems ()
{
        xercesc::XMLPlatformUtils::Initialize ();

        // load items from XML file, and transfer this data to the list of items
        try
        {
                std::auto_ptr< ixml::ItemsXML > itemsXML( ixml::items( ( isomot::sharePath() + nameOfXMLFile ) .c_str() ) );

                for ( ixml::ItemsXML::item_const_iterator i = itemsXML->item().begin (); i != itemsXML->item().end (); ++i )
                {
                        std::auto_ptr< ItemData > item( new ItemData );

                        item->label = ( *i ).label();                           // unique label of this item
                        item->directionFrames = ( *i ).directionFrames();       // number of frames for this direction of item
                        item->mortal = ( *i ).mortal();                         // offensive or harmless
                        item->weight = ( *i ).weight();                         // how long, in milliseconds, it falls
                        item->framesDelay = ( *i ).framesDelay();               // delay, in milliseconds, between frames in animation sequence
                        item->speed = ( *i ).speed();                           // how many milliseconds this item moves one single isometric unit
                        item->setNameOfFile( ( *i ).picture().file() );         // name of file with item's graphics
                        item->frameWidth = ( *i ).picture().frameWidth();       // width, in pixels, of frame
                        item->frameHeight = ( *i ).picture().frameHeight();     // height, in pixels, of frame

                        // item's frames are stored in motion vector as long as item isn't door
                        if ( ! ( *i ).door ().present () )
                        {
                                createPictureFrames( item.get(), isomot::GameManager::getInstance()->getChosenGraphicSet() );
                        }

                        // El elemento puede no tener sombra
                        if ( ( *i ).shadow ().present () )
                        {
                                // Ruta a la sombra del elemento
                                item->setNameOfShadowFile( ( *i ).shadow().get().file() );
                                // Anchura en píxeles de un fotograma de sombra
                                item->shadowWidth = ( *i ).shadow().get().shadowWidth();
                                // Altura en píxeles de un fotograma de sombra
                                item->shadowHeight = ( *i ).shadow().get().shadowHeight();
                                // Se almacenan los fotogramas en el vector shadows
                                createShadowFrames( item.get(), isomot::GameManager::getInstance()->getChosenGraphicSet() );
                        }

                        // Sólo unos pocos elementos tienen fotogramas extra
                        if ( ( *i ).extraFrames ().present () )
                        {
                                item->extraFrames = ( *i ).extraFrames().get();
                        }

                        // Secuencia de animación
                        for ( ixml::item::frame_const_iterator j = ( *i ).frame().begin (); j != ( *i ).frame().end (); ++j )
                        {
                                item->frames.push_back( *j );
                        }

                        // Si el elemento es una puerta, tiene tres parámetros que definen sus dimensiones
                        if ( ( *i ).door ().present () )
                        {
                                // Una puerta en realidad son tres elementos distintos: dintel, jamba izquierda y jamba derecha
                                ItemData lintel( *item );
                                ItemData leftJamb( *item );
                                ItemData rightJamb( *item );
                                // Dimensiones de las partes de una puerta
                                DoorMeasures dm;

                                // Anchura espacial en el eje X de las tres partes de la puerta
                                // Se almacenan en el siguiente orden: jamba izquierda, jamba derecha y dintel
                                for ( ixml::item::widthX_const_iterator j = ( *i ).widthX().begin(); j != ( *i ).widthX().end(); ++j )
                                {
                                        if ( dm.leftJambWidthX == 0 )
                                        {
                                                leftJamb.widthX = dm.leftJambWidthX = *j;
                                        }
                                        else if ( dm.rightJambWidthX == 0 )
                                        {
                                                rightJamb.widthX = dm.rightJambWidthX = *j;
                                        }
                                        else if ( dm.lintelWidthX == 0 )
                                        {
                                                lintel.widthX = dm.lintelWidthX = *j;
                                        }
                                }

                                // Anchura espacial en el eje Y de las tres partes de la puerta
                                // Se almacenan en el siguiente orden: jamba izquierda, jamba derecha y dintel
                                for ( ixml::item::widthY_const_iterator j = ( *i ).widthY().begin(); j != ( *i ).widthY().end(); ++j )
                                {
                                        if ( dm.leftJambWidthY == 0 )
                                        {
                                                leftJamb.widthY = dm.leftJambWidthY = *j;
                                        }
                                        else if ( dm.rightJambWidthY == 0 )
                                        {
                                                rightJamb.widthY = dm.rightJambWidthY = *j;
                                        }
                                        else if ( dm.lintelWidthY == 0 )
                                        {
                                                lintel.widthY = dm.lintelWidthY = *j;
                                        }
                                }

                                // Altura espacial de las tres partes de la puerta
                                // Se almacenan en el siguiente orden: jamba izquierda, jamba derecha y dintel
                                for ( ixml::item::height_const_iterator j = ( *i ).height().begin(); j != ( *i ).height().end(); ++j )
                                {
                                        if ( dm.leftJambHeight == 0 )
                                        {
                                                leftJamb.height = dm.leftJambHeight = *j;
                                        }
                                        else if ( dm.rightJambHeight == 0 )
                                        {
                                                rightJamb.height = dm.rightJambHeight = *j;
                                        }
                                        else if ( dm.lintelHeight == 0 )
                                        {
                                                lintel.height = dm.lintelHeight = *j;
                                        }
                                }

                                // load graphics for door as its three parts
                                BITMAP* picture = load_png( ( isomot::sharePath() + isomot::GameManager::getInstance()->getChosenGraphicSet() + "/" + item->getNameOfFile( ) ).c_str () , 0 );
                                if ( picture == 0 )
                                {
                                        std::cerr <<
                                                "picture \"" << item->getNameOfFile( ) <<
                                                "\" at \"" << isomot::GameManager::getInstance()->getChosenGraphicSet() <<
                                                "\" is absent" << std::endl ;
                                        throw "picture " + item->getNameOfFile( ) + " at " + isomot::GameManager::getInstance()->getChosenGraphicSet() + " is absent" ;
                                }

                                // Creación de la jamba izquierda
                                BITMAP* left = cutOutLeftJamb( picture, dm, ( *i ).door().get() );
                                leftJamb.motion.push_back( left );
                                this->itemData.push_back( leftJamb );

                                // Creación de la jamba derecha
                                BITMAP* right = cutOutRightJamb( picture, dm, ( *i ).door().get() );
                                rightJamb.label += 1;
                                rightJamb.motion.push_back( right );
                                this->itemData.push_back( rightJamb );

                                // Creación del dintel
                                BITMAP* top = cutOutLintel( picture, dm, ( *i ).door().get() );
                                lintel.label += 2;
                                lintel.motion.push_back( top );
                                this->itemData.push_back( lintel );

                                // La imagen original no se volverá a utilizar
                                destroy_bitmap( picture ) ;
                        }
                        else
                        {
                                // width at axis X in isometric units
                                item->widthX = *( ( *i ).widthX() ).begin ();
                                // width at axis Y in isometric units
                                item->widthY = *( ( *i ).widthY() ).begin ();
                                // width at axis Z, or height, in isometric units
                                item->height = *( ( *i ).height() ).begin ();

                                // add data to the list
                                this->itemData.push_back( *item.get() );
                        }
                }
        }
        catch ( const xml_schema::exception& e )
        {
                std::cerr << e << std::endl ;
        }
        catch ( const Exception& e )
        {
                std::cerr << e.what () << std::endl ;
        }

        xercesc::XMLPlatformUtils::Terminate ();
}

void ItemDataManager::freeItems ()
{
        std::for_each( itemData.begin(), itemData.end(), destroyItemData );
}

ItemData* ItemDataManager::findItemByLabel( const short label )
{
        std::list< ItemData >::iterator i = std::find_if( itemData.begin (), itemData.end (), std::bind2nd( EqualItemData(), label ) );
        ItemData* data = ( i != itemData.end() ? static_cast< ItemData * >( &( *i ) ) : 0 );

        return data;
}

ItemDataManager::DoorMeasures::DoorMeasures()
{
        lintelWidthX = lintelWidthY = lintelHeight = 0;
        leftJambWidthX = leftJambWidthY = leftJambHeight = 0;
        rightJambWidthX = rightJambWidthY = rightJambHeight = 0;
}

ItemData* ItemDataManager::createPictureFrames( ItemData * itemData, const char* gfxPrefix )
{
        try
        {
                // Datos necesarios
                if ( itemData->getNameOfFile( ).empty() || itemData->frameWidth == 0 || itemData->frameHeight == 0 )
                {
                        throw "either name of file is empty or zero width/height at ItemDataManager::createPictureFrames";
                }

                // Se cargan los gráficos del elemento y si es necesario se crean los distintos fotogramas
                BITMAP* picture = load_png( ( isomot::sharePath() + gfxPrefix + "/" + itemData->getNameOfFile( ) ).c_str () , 0 );
                if ( picture == 0 )
                {
                        std::cerr << "picture \"" << itemData->getNameOfFile( ) << "\" is absent at \"" << gfxPrefix << "\"" << std::endl ;
                        throw "picture " + itemData->getNameOfFile( ) + " is absent at " + gfxPrefix ;
                }

                // Trocea la imagen en los distintos fotogramas y se almacenan en el vector
                for ( int y = 0; y < picture->h / itemData->frameHeight; y++ )
                {
                        for ( int x = 0; x < picture->w / itemData->frameWidth; x++ )
                        {
                                BITMAP* frame = create_bitmap_ex( 32, itemData->frameWidth, itemData->frameHeight );
                                blit( picture, frame, x * itemData->frameWidth, y * itemData->frameHeight, 0, 0, frame->w, frame->h );
                                itemData->motion.push_back( frame );
                        }
                }

                // La imagen original no se volverá a utilizar
                destroy_bitmap( picture ) ;
        }
        catch ( const Exception& e )
        {
                std::cerr << e.what () << std::endl ;
                return 0;
        }

        return itemData;
}

ItemData* ItemDataManager::createShadowFrames( ItemData * itemData, const char* gfxPrefix )
{
        try
        {
                // Datos necesarios
                if ( itemData->getNameOfShadowFile( ).empty() || itemData->shadowWidth == 0 || itemData->shadowHeight == 0 )
                {
                        throw "Insufficient data for ItemDataManager::createShadowFrames";
                }

                // Se cargan los gráficos del elemento y si es necesario se crean los distintos fotogramas
                BITMAP* picture = load_png( ( isomot::sharePath() + gfxPrefix + "/" + itemData->getNameOfShadowFile( ) ).c_str () , 0 );
                if ( picture == 0 )
                {
                        std::cerr << "file of shadows \"" << itemData->getNameOfShadowFile( ) << "\" is absent at \"" << gfxPrefix << "\"" << std::endl ;
                        throw "file of shadows " + itemData->getNameOfShadowFile( ) + " is absent at " + gfxPrefix ;
                }

                // Trocea la imagen en los distintos fotogramas y se almacenan en el vector
                for ( int y = 0; y < picture->h / itemData->shadowHeight; y++ )
                {
                        for ( int x = 0; x < picture->w / itemData->shadowWidth; x++ )
                        {
                                BITMAP* frame = create_bitmap_ex( 32, itemData->shadowWidth, itemData->shadowHeight );
                                blit( picture, frame, x * itemData->shadowWidth, y * itemData->shadowHeight, 0, 0, frame->w, frame->h );
                                itemData->shadows.push_back( frame );
                        }
                }

                // La imagen original no se volverá a utilizar
                destroy_bitmap( picture ) ;
        }
        catch ( const Exception& e )
        {
                std::cerr << e.what () << std::endl ;
                return 0;
        }

        return itemData;
}

BITMAP* ItemDataManager::cutOutLintel( BITMAP* door, const DoorMeasures& dm, const ixml::door::value type )
{
        bool ns = ( type == ixml::door::north || type == ixml::door::south );

        // Parte superior de la puerta
        BITMAP* top = create_bitmap_ex( 32, ( dm.lintelWidthX << 1 ) + ( dm.lintelWidthY << 1 ),
                                              dm.lintelHeight + dm.lintelWidthY + dm.lintelWidthX );
        // El color de fondo es magenta
        clear_to_color( top, makecol( 255, 0, 255 ) );

        // Copia la zona a recortar si la puerta está orientada al norte o al sur
        if ( ns )
        {
                // Copia la zona rectangular
                blit( door, top, 0, 0, 0, 0, top->w, dm.lintelHeight + dm.lintelWidthX );

                int delta = top->w - 1;
                int noPixelIndex = top->w - ( ( dm.rightJambWidthX + dm.rightJambWidthY ) << 1 ) + 1;
                int yStart = noPixelIndex;
                int yEnd = noPixelIndex - 1;

                acquire_bitmap( top );

                for ( int x = dm.lintelHeight + dm.lintelWidthX; x < top->h; x++ )
                {
                        for ( int y = delta; y >= 0; y-- )
                        {
                                if ( x != dm.lintelHeight + dm.lintelWidthX && y == noPixelIndex )
                                {
                                        if ( noPixelIndex > yEnd )
                                        {
                                                noPixelIndex--;
                                        }
                                        else
                                        {
                                                yStart += 2;
                                                noPixelIndex = yStart;
                                        }
                                }
                                else if ( x < dm.lintelHeight + ( dm.lintelWidthX << 1 ) || y < yEnd )
                                {
                                        ( ( int * )top->line[ x ] )[ y ] = ( ( int * )door->line[ x ] )[ y ];
                                }
                        }

                        delta -= 2;
                }

                release_bitmap( top );
        }
        else
        // Copia la zona a recortar si la puerta está orientada al este o al oeste
        {
                // Copia la zona rectangular
                blit( door, top, 0, 0, 0, 0, top->w, dm.lintelHeight + dm.lintelWidthY );

                int delta = 0;
                int noPixelIndex = ( ( dm.leftJambWidthX + dm.leftJambWidthY ) << 1 ) - 2;
                int yStart = noPixelIndex;
                int yEnd = noPixelIndex + 1;

                acquire_bitmap( top );

                for ( int x = dm.lintelHeight + dm.lintelWidthY; x < top->h; x++ )
                {
                        for ( int y = delta; y < top->w; y++ )
                        {
                                if ( x != dm.lintelHeight + dm.lintelWidthY && y == noPixelIndex )
                                {
                                        if ( noPixelIndex < yEnd )
                                        {
                                                noPixelIndex++;
                                        }
                                        else
                                        {
                                                yStart -= 2;
                                                noPixelIndex = yStart;
                                        }
                                }
                                else if ( x < dm.lintelHeight + ( dm.lintelWidthY << 1 ) || y > yEnd )
                                {
                                        ( ( int * )top->line[ x ] )[ y ] = ( ( int * )door->line[ x ] )[ y ];
                                }
                        }

                        delta += 2;
                }

                release_bitmap( top );
        }

        return top;
}

BITMAP* ItemDataManager::cutOutLeftJamb( BITMAP* door, const DoorMeasures& dm, const ixml::door::value type )
{
        bool ns = ( type == ixml::door::north || type == ixml::door::south );
        int fixWidth = ( ns ? 7 : 0 );
        int fixY = ( ns ? -1 : 0 );

        BITMAP* left = create_bitmap_ex ( 32, ( dm.leftJambWidthX << 1 ) + fixWidth + ( dm.leftJambWidthY << 1 ) ,
                                                dm.leftJambHeight + dm.leftJambWidthY + dm.leftJambWidthX ) ;

        // El color de fondo es magenta
        clear_to_color( left, makecol( 255, 0, 255 ) );
        // Copia la zona rectangular
        blit( door, left, fixY, dm.lintelHeight + dm.lintelWidthY - dm.leftJambWidthY + fixY, 0, 0, left->w, left->h );

        return left;
}

BITMAP* ItemDataManager::cutOutRightJamb( BITMAP* door, const DoorMeasures& dm, const ixml::door::value type )
{
        bool ns = (type == ixml::door::north || type == ixml::door::south);
        int fixWidth = (ns ? 0 : 7);
        int fixY = (ns ? 0 : -2);

        BITMAP* right = create_bitmap_ex ( 32, ( dm.rightJambWidthX << 1 ) + fixWidth + ( dm.rightJambWidthY << 1 ) ,
                                                 dm.rightJambHeight + dm.rightJambWidthY + dm.rightJambWidthX ) ;

        // El color de fondo es magenta
        clear_to_color( right, makecol( 255, 0, 255 ) );
        // Copia la zona rectangular
        blit( door, right, door->w - right->w, dm.lintelHeight + dm.lintelWidthX - dm.rightJambWidthY + fixY, 0, 0, right->w, right->h );

        return right;
}

void ItemDataManager::destroyItemData( ItemData& itemData )
{
        itemData.clearNameOfFile ();
        itemData.clearNameOfShadowFile ();
        itemData.frames.clear ();
        std::for_each( itemData.motion.begin(), itemData.motion.end(), destroy_bitmap );
        std::for_each( itemData.shadows.begin(), itemData.shadows.end(), destroy_bitmap );
}

bool EqualItemData::operator() ( const ItemData& itemData, short label ) const
{
        return ( itemData.label == label );
}

}
