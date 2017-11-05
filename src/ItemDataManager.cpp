
#include "ItemDataManager.hpp"
#include "GameManager.hpp"
#include "Exception.hpp"
#include "Ism.hpp"
#include <loadpng.h>
#include <fstream>

# include <xercesc/util/PlatformUtils.hpp>

#ifdef DEBUG
#  define DEBUG_ITEM_DATA         1
#endif


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
        freeItems() ;

        xercesc::XMLPlatformUtils::Initialize ();

        // load items from XML file, and transfer this data to the list of items
        try
        {
                std::auto_ptr< ixml::ItemsXML > itemsXML( ixml::items( ( isomot::sharePath() + nameOfXMLFile ) .c_str() ) );

                for ( ixml::ItemsXML::item_const_iterator i = itemsXML->item().begin (); i != itemsXML->item().end (); ++i )
                {
                        ItemData* item = new ItemData ();

                        item->label = ( *i ).label();                           // unique name of this item
                        item->orientationFrames = ( *i ).directionFrames();     // number of frames for orientations of this item
                        item->mortal = ( *i ).mortal();                         // offensive or harmless
                        item->weight = ( *i ).weight();                         // how long, in milliseconds, it falls
                        item->delayBetweenFrames = ( *i ).framesDelay();        // delay, in milliseconds, between frames in animation sequence
                        item->speed = ( *i ).speed();                           // how many milliseconds this item moves one single isometric unit
                        item->setNameOfFile( ( *i ).picture().file() );         // name of file with item's graphics
                        item->widthOfFrame = ( *i ).picture().frameWidth();     // width, in pixels, of frame
                        item->heightOfFrame = ( *i ).picture().frameHeight();   // height, in pixels, of frame

                        // store item’s frames using motion vector, if item isn’t door
                        if ( ! ( *i ).door ().present () )
                        {
                        # if  defined( DEBUG_ITEM_DATA )  &&  DEBUG_ITEM_DATA
                                std::cout << "got item \"" << item->label << "\"" ;
                        # endif

                                createPictureFrames( item, isomot::GameManager::getInstance()->getChosenGraphicSet() );
                        }

                        // element may have no shadow
                        if ( ( *i ).shadow ().present () )
                        {
                                item->setNameOfShadowFile( ( *i ).shadow().get().file() );
                                item->widthOfShadow = ( *i ).shadow().get().shadowWidth();
                                item->heightOfShadow = ( *i ).shadow().get().shadowHeight();

                                // frames of shadow
                                createShadowFrames( item, isomot::GameManager::getInstance()->getChosenGraphicSet() );
                        }

                        // only few items have extra frames
                        if ( ( *i ).extraFrames ().present () )
                        {
                                item->extraFrames = ( *i ).extraFrames().get () ;
                        }

                        // sequence of animation
                        for ( ixml::item::frame_const_iterator j = ( *i ).frame().begin (); j != ( *i ).frame().end (); ++j )
                        {
                                item->frames.push_back( *j );
                        }

                        // door has three parameters which define its dimensions
                        if ( ( *i ).door ().present () )
                        {
                        # if  defined( DEBUG_ITEM_DATA )  &&  DEBUG_ITEM_DATA
                                std::cout << "got door \"" << item->label << "\"" ;
                        # endif

                                // door is actually three items: lintel, left jamb and right jamb
                                ItemData* lintel = ItemData::clone( item ) ;
                                ItemData* leftJamb = ItemData::clone( item ) ;
                                ItemData* rightJamb = ItemData::clone( item ) ;

                                DoorMeasures dm;

                                // width at axis X of three parts ( left jamb, right jamb and lintel ) of door
                                for ( ixml::item::widthX_const_iterator j = ( *i ).widthX().begin(); j != ( *i ).widthX().end(); ++j )
                                {
                                        if ( dm.leftJambWidthX == 0 )
                                        {
                                                leftJamb->widthX = dm.leftJambWidthX = *j;
                                        }
                                        else if ( dm.rightJambWidthX == 0 )
                                        {
                                                rightJamb->widthX = dm.rightJambWidthX = *j;
                                        }
                                        else if ( dm.lintelWidthX == 0 )
                                        {
                                                lintel->widthX = dm.lintelWidthX = *j;
                                        }
                                }

                                // width at axis Y of three parts ( left jamb, right jamb and lintel ) of door
                                for ( ixml::item::widthY_const_iterator j = ( *i ).widthY().begin(); j != ( *i ).widthY().end(); ++j )
                                {
                                        if ( dm.leftJambWidthY == 0 )
                                        {
                                                leftJamb->widthY = dm.leftJambWidthY = *j;
                                        }
                                        else if ( dm.rightJambWidthY == 0 )
                                        {
                                                rightJamb->widthY = dm.rightJambWidthY = *j;
                                        }
                                        else if ( dm.lintelWidthY == 0 )
                                        {
                                                lintel->widthY = dm.lintelWidthY = *j;
                                        }
                                }

                                // height of three parts ( left jamb, right jamb and lintel ) of door
                                for ( ixml::item::height_const_iterator j = ( *i ).height().begin(); j != ( *i ).height().end(); ++j )
                                {
                                        if ( dm.leftJambHeight == 0 )
                                        {
                                                leftJamb->height = dm.leftJambHeight = *j;
                                        }
                                        else if ( dm.rightJambHeight == 0 )
                                        {
                                                rightJamb->height = dm.rightJambHeight = *j;
                                        }
                                        else if ( dm.lintelHeight == 0 )
                                        {
                                                lintel->height = dm.lintelHeight = *j;
                                        }
                                }

                                // load graphics for door
                                BITMAP* picture = load_png( isomot::pathToFile( isomot::sharePath() + isomot::GameManager::getInstance()->getChosenGraphicSet() + pathSeparator + item->getNameOfFile( ) ), 0 );
                                if ( picture == 0 )
                                {
                                        std::cerr <<
                                                "picture \"" << item->getNameOfFile( ) <<
                                                "\" at \"" << isomot::GameManager::getInstance()->getChosenGraphicSet() <<
                                                "\" is absent" << std::endl ;
                                        throw "picture " + item->getNameOfFile( ) + " at " + isomot::GameManager::getInstance()->getChosenGraphicSet() + " is absent" ;
                                }

                                // cut out left jamb
                                BITMAP* leftJambImage = cutOutLeftJamb( picture, dm, ( *i ).door().get() );
                                leftJamb->label += "~leftjamb";
                                leftJamb->motion.push_back( leftJambImage );
                                this->itemData.push_back( leftJamb );

                                // cut out right jamb
                                BITMAP* rightJambImage = cutOutRightJamb( picture, dm, ( *i ).door().get() );
                                rightJamb->label += "~rightjamb";
                                rightJamb->motion.push_back( rightJambImage );
                                this->itemData.push_back( rightJamb );

                                // cut out lintel
                                BITMAP* lintelImage = cutOutLintel( picture, dm, ( *i ).door().get() );
                                lintel->label += "~lintel";
                                lintel->motion.push_back( lintelImage );
                                this->itemData.push_back( lintel );

                        # if  defined( DEBUG_ITEM_DATA )  &&  DEBUG_ITEM_DATA
                                std::cout << ", three parts are \"" <<
                                                leftJamb->label << "\" \"" << rightJamb->label << "\" \"" << lintel->label << "\""
                                                << std::endl ;
                        # endif

                                // bin original image
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
                                this->itemData.push_back( item );
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
        std::for_each( itemData.begin(), itemData.end(), finalizeData );
}

ItemData* ItemDataManager::findItemByLabel( const std::string& label )
{
        for ( std::list< ItemData * >::iterator i = this->itemData.begin (); i != this->itemData.end (); ++i )
        {
                if ( ( *i )->getLabel() == label )
                {
                        return *i ;
                }
        }

        std::cerr << "item with label \"" << label << "\" is absent" << std::endl;
        return 0;
}

ItemDataManager::DoorMeasures::DoorMeasures()
        :  lintelWidthX( 0 ), lintelWidthY( 0 ), lintelHeight( 0 )
        ,  leftJambWidthX( 0 ), leftJambWidthY( 0 ), leftJambHeight( 0 )
        ,  rightJambWidthX( 0 ), rightJambWidthY( 0 ), rightJambHeight( 0 )
{

}

ItemData* ItemDataManager::createPictureFrames( ItemData * itemData, const char* gfxPrefix )
{
        try
        {
                if ( itemData->getNameOfFile( ).empty() || itemData->getWidthOfFrame() == 0 || itemData->getHeightOfFrame() == 0 )
                {
                        throw "either name of file is empty or zero width / height at ItemDataManager::createPictureFrames";
                }

                // load graphics for item
                BITMAP* picture = load_png( isomot::pathToFile( isomot::sharePath() + gfxPrefix + pathSeparator + itemData->getNameOfFile( ) ), 0 );
                if ( picture == 0 )
                {
                        std::cerr << "picture \"" << itemData->getNameOfFile( ) << "\" is absent at \"" << gfxPrefix << "\"" << std::endl ;
                        throw "picture " + itemData->getNameOfFile( ) + " is absent at " + gfxPrefix ;
                }

                // decompose image into frames and store them in vector
                for ( unsigned int y = 0; y < picture->h / itemData->getHeightOfFrame(); y++ )
                {
                        for ( unsigned int x = 0; x < picture->w / itemData->getWidthOfFrame(); x++ )
                        {
                                BITMAP* frame = create_bitmap_ex( 32, itemData->getWidthOfFrame(), itemData->getHeightOfFrame() );
                                blit( picture, frame, x * itemData->getWidthOfFrame(), y * itemData->getHeightOfFrame(), 0, 0, frame->w, frame->h );
                                itemData->motion.push_back( frame );
                        }
                }

                # if  defined( DEBUG_ITEM_DATA )  &&  DEBUG_ITEM_DATA
                        size_t howManyFrames = itemData->motion.size();
                        std::cout << " with " << howManyFrames << ( howManyFrames == 1 ? " frame" : " frames" ) << std::endl ;
                # endif

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
                if ( itemData->getNameOfShadowFile( ).empty() || itemData->getWidthOfShadow() == 0 || itemData->getHeightOfShadow() == 0 )
                {
                        throw "either name of file is empty or zero width / height at ItemDataManager::createShadowFrames";
                }

                // load graphics for shadow
                BITMAP* picture = load_png( isomot::pathToFile( isomot::sharePath() + gfxPrefix + pathSeparator + itemData->getNameOfShadowFile( ) ), 0 );
                if ( picture == 0 )
                {
                        std::cerr << "file of shadows \"" << itemData->getNameOfShadowFile( ) << "\" is absent at \"" << gfxPrefix << "\"" << std::endl ;
                        throw "file of shadows " + itemData->getNameOfShadowFile( ) + " is absent at " + gfxPrefix ;
                }

                // decompose image into frames
                for ( unsigned int y = 0; y < picture->h / itemData->getHeightOfShadow(); y++ )
                {
                        for ( unsigned int x = 0; x < picture->w / itemData->getWidthOfShadow(); x++ )
                        {
                                BITMAP* frame = create_bitmap_ex( 32, itemData->getWidthOfShadow(), itemData->getHeightOfShadow() );
                                blit( picture, frame, x * itemData->getWidthOfShadow(), y * itemData->getHeightOfShadow(), 0, 0, frame->w, frame->h );
                                itemData->shadows.push_back( frame );
                        }
                }

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

        // top of door
        BITMAP* top = create_bitmap_ex( 32, ( dm.lintelWidthX << 1 ) + ( dm.lintelWidthY << 1 ),
                                              dm.lintelHeight + dm.lintelWidthY + dm.lintelWidthX );

        clear_to_color( top, makecol( 255, 0, 255 ) ); // color of transparency

        if ( ns )
        {
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
        {
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

        clear_to_color( left, makecol( 255, 0, 255 ) ); // color of transparency

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

        clear_to_color( right, makecol( 255, 0, 255 ) );  // color of transparency

        blit( door, right, door->w - right->w, dm.lintelHeight + dm.lintelWidthX - dm.rightJambWidthY + fixY, 0, 0, right->w, right->h );

        return right;
}

void ItemDataManager::finalizeData( ItemData* itemData )
{
        if ( itemData != 0 )
        {
                itemData->clearNameOfFile ();
                itemData->clearNameOfShadowFile ();
                itemData->frames.clear ();

                std::for_each( itemData->motion.begin(), itemData->motion.end(), destroy_bitmap );
                std::for_each( itemData->shadows.begin(), itemData->shadows.end(), destroy_bitmap );
        }
}

}
